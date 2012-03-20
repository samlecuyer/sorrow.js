// copyright 2012 sam l'ecuyer
#include <v8.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "sorrow.h"
#include "sorrow_natives.h"

namespace sorrow {
	using namespace v8;

	static Persistent<Object> internals;
	
	const char* ToCString(const String::Utf8Value& value) {
		return *value ? *value : "<string conversion failed>";
	} // ToCString
	
	JS_FUNCTN(Quit) {
		int exit_code = args[0]->Int32Value();
		exit(exit_code);
		return Undefined();
	} // Quit
	
	JS_FUNCTN(Version) {
		return String::New(V8::GetVersion());
	} // Version
	
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
    
    JS_FUNCTN(CompileScript) {
		HandleScope scope;
        Local<String> script = args[0]->ToString();
		Local<String> source = args[1]->ToString();
		Local<Value> result = ExecuteString(script, source);
		return scope.Close(result);
	} // CompileScript
    
    Handle<Value> EnvGetter(Local<String> name, const AccessorInfo &info) {
        HandleScope scope;
        String::Utf8Value nameVal(name);
        char *env = getenv(*nameVal);
        if (env == NULL) {
            return Undefined();
        }
        Local<String> ret = String::New(env);
        return scope.Close(ret);
    }
	
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
		
		SET_METHOD(internals, "quit",    Quit)
		SET_METHOD(internals, "version", Version)
        SET_METHOD(internals, "compile", CompileScript)
        
		Local<Array> lineArgs = Array::New(argc-1);
		for (int i = 0; i +1 < argc; i++) {
			lineArgs->Set(Integer::New(i), String::New(argv[i+1]));
		}
		internals->Set(String::New("args"), lineArgs);
		
		Handle<Object> libsObject = Object::New();
		LoadNativeLibraries(libsObject);
		internals->Set(String::New("stdlib"), libsObject);
        
        Handle<ObjectTemplate> env = ObjectTemplate::New();
        env->SetNamedPropertyHandler(EnvGetter);
        internals->Set(String::New("env"), env->NewInstance());
		
		SetupBinaryTypes(internals);
		SetupIOStreams(internals);
        SetupFS(internals);
		
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
			
            InitV8Arrays(highlander->Global());
			Handle<Object> internals = SetupInternals(argc, argv);
			Load(internals);
			
			//RunArgs(argc, argv);
			
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
