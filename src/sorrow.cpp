// copyright 2012 the V8 project authors & sam l'ecuyer
#include <v8.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "gen/sorrow_natives.h"

namespace sorrow {
	using namespace v8;
	
	Local<Value> ExecuteString(Handle<String> source, Handle<Value> filename);
	Handle<Value> LoadFile(const Arguments& args);
	Handle<Value> Print(const Arguments& args);
	Handle<Value> Read(const Arguments& args);
	Handle<Value> Quit(const Arguments& args);
	Handle<Value> Version(const Arguments& args);
	Handle<String> ReadFile(const char* name);
	void ReportException(TryCatch* handler);
	void LoadNativeLibraries(Handle<Object> natives);
	void RunArgs(int argc, char* argv[]);
	
	void BinaryTypes(Handle<Object> internals);

	static Persistent<Object> internals;
	
	const char* ToCString(const String::Utf8Value& value) {
		return *value ? *value : "<string conversion failed>";
	} // ToCString
	
	Handle<Value> Print(const Arguments& args) {
		bool first = true;
		for (int i = 0; i < args.Length(); i++) {
			HandleScope handle_scope;
			if (first) {
				first = false;
			} else {
				printf(" ");
			}
			String::Utf8Value str(args[i]);
			const char* cstr = ToCString(str);
			printf("%s", cstr);
		}
		printf("\n");
		fflush(stdout);
		return Undefined();
	} // Print
	
	Handle<Value> Read(const Arguments& args) {
		if (args.Length() != 1) {
			return ThrowException(String::New("Bad parameters"));
		}
		String::Utf8Value file(args[0]);
		if (*file == NULL) {
			return ThrowException(String::New("Error loading file"));
		}
		Handle<String> source = ReadFile(*file);
		if (source.IsEmpty()) {
			return ThrowException(String::New("Error loading file"));
		}
		return source;
	} // Read
	
	Handle<Value> Quit(const Arguments& args) {
		int exit_code = args[0]->Int32Value();
		exit(exit_code);
		return Undefined();
	} // Quit
	
	Handle<Value> Version(const Arguments& args) {
		return String::New(V8::GetVersion());
	} // Version
	
	Handle<String> ReadFile(const char* name) {
		FILE* file = fopen(name, "rb");
		if (file == NULL) return Handle<String>();
		
		fseek(file, 0, SEEK_END);
		int size = ftell(file);
		rewind(file);
		
		char* chars = new char[size + 1];
		chars[size] = '\0';
		for (int i = 0; i < size;) {
			int read = fread(&chars[i], 1, size - i, file);
			i += read;
		}
		fclose(file);
		Handle<String> result = String::New(chars, size);
		delete[] chars;
		return result;
	} // ReadFile
	
	void ReportException(TryCatch* try_catch) {
		HandleScope handle_scope;
		String::Utf8Value exception(try_catch->Exception());
		const char* exception_string = ToCString(exception);
		Handle<Message> message = try_catch->Message();
		if (message.IsEmpty()) {
			// V8 didn't provide any extra information about this error; just
			// print the exception.
			printf("%s\n", exception_string);
		} else {
			// Print (filename):(line number): (message).
			String::Utf8Value filename(message->GetScriptResourceName());
			const char* filename_string = ToCString(filename);
			int linenum = message->GetLineNumber();
			printf("%s:%i: %s\n", filename_string, linenum, exception_string);
			// Print line of source code.
			String::Utf8Value sourceline(message->GetSourceLine());
			const char* sourceline_string = ToCString(sourceline);
			printf("%s\n", sourceline_string);
			// Print wavy underline (GetUnderline is deprecated).
			int start = message->GetStartColumn();
			for (int i = 0; i < start; i++) {
				printf(" ");
			}
			int end = message->GetEndColumn();
			for (int i = start; i < end; i++) {
				printf("^");
			}
			printf("\n");
			String::Utf8Value stack_trace(try_catch->StackTrace());
			if (stack_trace.length() > 0) {
				const char* stack_trace_string = ToCString(stack_trace);
				printf("%s\n", stack_trace_string);
			}
		}
	} // ReportException
	
	Handle<Value> LoadFile(const Arguments& args) {
		for (int i = 0; i < args.Length(); i++) {
			HandleScope handle_scope;
			String::Utf8Value file(args[i]);
			if (*file == NULL) {
				return ThrowException(String::New("Error loading file"));
			}
			Handle<String> source = ReadFile(*file);
			if (source.IsEmpty()) {
				return ThrowException(String::New("Error loading file"));
			}
			return ExecuteString(source, String::New(*file));
		}
		return Undefined();
	} // LoadFile
	
	void RunArgs(int argc, char* argv[]) {
		V8::SetFlagsFromCommandLine(&argc, argv, true);
		for (int i = 1; i < argc; i++) {
			const char* str = argv[i];
			if (strcmp(str, "-e") == 0 && i + 1 < argc) {
				// Execute argument given to -e option directly
				HandleScope handle_scope;
				Handle<String> file_name = String::New("unnamed");
				Handle<String> source = String::New(argv[i + 1]);
				ExecuteString(source, file_name);
				i++;
			} else {
				// Use all other arguments as names of files to load and run.
				HandleScope handle_scope;
				Handle<String> file_name = String::New(str);
				Handle<String> source = ReadFile(str);
				if (source.IsEmpty()) {
					printf("Error reading '%s'\n", str);
				}
				ExecuteString(source, file_name);
			}
		}
	} // RunArgs
	
	Local<Value> ExecuteString(Handle<String> source, Handle<Value> filename) {
		HandleScope scope;
		TryCatch tryCatch;
		
		Local<Script> script = Script::Compile(source, filename);
		if (script.IsEmpty()) {
			ReportException(&tryCatch);
			exit(1);
		}
		
		Local<Value> result = script->Run();
		if (result.IsEmpty()) {
			ReportException(&tryCatch);
			exit(1);
		}
		
		return scope.Close(result);
	} // ExecuteString
	
	void Load(Handle<Object> internals) {
		TryCatch tryCatch;
		
		Local<Value> func = ExecuteString(String::New(sorrow_native), String::New("sorrow.js"));
		
		if (tryCatch.HasCaught()) {
			ReportException(&tryCatch);
			exit(10);
		}
		assert(func->IsFunction());
		Local<Function> f = Local<Function>::Cast(func);
		
		Local<Object> global = Context::GetCurrent()->Global();
		Local<Value> args[1] = { Local<Value>::New(internals) };
		
		f->Call(global, 1, args);
		
		if (tryCatch.HasCaught())  {
			ReportException(&tryCatch);
			exit(11);
		}
	} // Load
	
	Handle<Object> SetupInternals(int argc, char *argv[]) {
		HandleScope scope;
		Local<FunctionTemplate> internals_template = FunctionTemplate::New();
		internals = Persistent<Object>::New(internals_template->GetFunction()->NewInstance());
		
		internals->Set(String::New("print"), FunctionTemplate::New(Print)->GetFunction());
		internals->Set(String::New("read"), FunctionTemplate::New(Read)->GetFunction());
		internals->Set(String::New("load"), FunctionTemplate::New(LoadFile)->GetFunction());
		internals->Set(String::New("quit"), FunctionTemplate::New(Quit)->GetFunction());
		internals->Set(String::New("version"), FunctionTemplate::New(Version)->GetFunction());
		
		Handle<Object> libsObject = Object::New();
		LoadNativeLibraries(libsObject);
		internals->Set(String::New("stdlib"), libsObject);
		
		BinaryTypes(internals);
		
		return internals;
	} // SetupInternals
	
	int Main(int argc, char *argv[]) {
		V8::Initialize();
		Persistent<Context> context;
		{
			Locker locker;
			HandleScope handle_scope;
			
			Persistent<Context> highlander = Context::New(); // there can only be one!
			Context::Scope context_scope(highlander);
			
			Handle<Object> internals = SetupInternals(argc, argv);
			Load(internals);
			
			RunArgs(argc, argv);
			
			highlander.Dispose();
		}
		V8::Dispose();
	} // Main
	
	void LoadNativeLibraries(Handle<Object> libs) {
		HandleScope scope;
		for (int i = 1; natives[i].name; i++) {
			Local<String> name = String::New(natives[i].name);
			Handle<String> source = String::New(natives[i].source, natives[i].source_len);
			libs->Set(name, source);
		}
	} // LoadNativeLibraries
	
} // namespce sorrow

int main(int argc, char *argv[]) {
	return sorrow::Main(argc, argv);
}
