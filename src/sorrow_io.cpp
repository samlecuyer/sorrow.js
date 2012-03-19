/*
 copyright 2012 sam l'ecuyer
 */

#include "sorrow.h"

namespace sorrow {
	using namespace v8;
    
    Persistent<Function> rawStream_f;
    Persistent<Function> textStream_f;
    
	void ExternalWeakIOCallback(Persistent<Value> object, void* file) {
        fclose((FILE*)file);
		object.Dispose();
	}
    
    const char isRawStreamPropName[] = "_is_raw_stream_";
    const char isTextStreamPropName[] = "_is_text_stream_";

    // args[0] - file name
    // args[1] - {app, ate, binary, read, write, trunc}
	JS_FUNCTN(RawIOStream) {
        HandleScope scope;
		Handle<Object> rawStream = args.This();
        
        FILE *file;
        if (args.Length() == 2 && args[0]->IsString() && args[1]->IsString()) {
            String::Utf8Value name(args[0]);
            String::Utf8Value mode(args[1]);
            
            file = fopen(*name, *mode);
            NULL_STREAM_EXCEPTION(file, "Could not open stream")
        } else if (args[0]->IsExternal()) {
			// ByteString([buffer], size)
            file = (FILE*)External::Unwrap(args[0]);
        } else {
            return ThrowException(String::New("This is not a valid constructor call"));
        }
		Persistent<Object> persistent_stream = Persistent<Object>::New(rawStream);
		persistent_stream.MakeWeak(file, ExternalWeakIOCallback);
		persistent_stream.MarkIndependent();
		
		rawStream->SetPointerInInternalField(0, file);
		rawStream->Set(String::New(isRawStreamPropName), True(), ReadOnly);
		return rawStream;
	}
    
    JS_GETTER(RawStreamPositionGetter) {
        FILE *file = (FILE*)info.This()->ToObject()->GetPointerFromInternalField(0);
        NULL_STREAM_EXCEPTION(file, "Could not get position")
        int pos = ftell(file);
        return Integer::New(pos);
    }
    
    JS_SETTER(RawStreamPositionSetter) {
        FILE *file = (FILE*)info.This()->ToObject()->GetPointerFromInternalField(0);
        int pos = value->IntegerValue();
        fseek(file, pos, SEEK_SET);
        if (ferror(file)) {
            clearerr(file);
            ThrowException(String::New("Could not set stream position"));
            return;
        }
    }
	
    JS_FUNCTN(RawIOStreamClose) {
        FILE *file = (FILE*)args.This()->ToObject()->GetPointerFromInternalField(0);
        NULL_STREAM_EXCEPTION(file, "Could not close stream")
        fclose(file);
        if (ferror(file)) {
            clearerr(file);
            return ThrowException(String::New("Could not close stream"));
        }
        return Undefined();
	}	
	
    JS_FUNCTN(RawIOStreamFlush) {
        FILE *file = (FILE*)args.This()->ToObject()->GetPointerFromInternalField(0);
        NULL_STREAM_EXCEPTION(file, "Could not flush stream")
        fflush(file);
        if (ferror(file)) {
            clearerr(file);
            return ThrowException(String::New("Could not flush stream"));
        }
        return Undefined();
	}	
    
    JS_FUNCTN(RawIOStreamSkip) {
        FILE *file = (FILE*)args.This()->ToObject()->GetPointerFromInternalField(0);
        int dist = args[0]->IntegerValue();
        NULL_STREAM_EXCEPTION(file, "Could not skip on stream")
        fseek(file, dist, SEEK_CUR);
        if (ferror(file)) {
            clearerr(file);
            return ThrowException(String::New("Could not skip stream"));
        }
        return Undefined();
	}	
    
    JS_FUNCTN(RawIOStreamRead) {
        HandleScope scope;
        FILE *file = (FILE*)args.This()->ToObject()->GetPointerFromInternalField(0);
        NULL_STREAM_EXCEPTION(file, "Could not read from stream")
        int n;
        uint8_t *buffer;
        if (feof(file)) {
            Local<Value> bs = byteString_f->NewInstance();
            return scope.Close(bs);
        }
        if (args.Length() == 0) {
            int start = ftell(file);
            fseek(file, 0, SEEK_END);
            n = ftell(file);
            fseek(file, start, SEEK_SET);
            buffer = new uint8_t[n-start];
        } else if (args.Length() == 1) {
            if (args[0]->IsNull()) {
                n = 1024;
            } else if (args[0]->IsNumber()) {
                n = args[0]->IntegerValue();
            } else {
                return ThrowException(String::New("Not a valid invocation"));
            }
            buffer = new uint8_t[n];
        } else {
            return ThrowException(String::New("Not a valid invocation"));
        }
        int readBytes = fread(buffer, sizeof(uint8_t), n, file);
        if (ferror(file)) {
            clearerr(file);
            delete[] buffer;
            return ThrowException(String::New("Stream is in a bad state"));
        }
		Local<Value> bsArgs[2] = { External::New(buffer), Integer::New(readBytes) };
		Local<Value> bs = byteString_f->NewInstance(2, bsArgs);
        delete[] buffer;
        return scope.Close(bs);
	}
	
    JS_FUNCTN(RawIOStreamWrite) {
        HandleScope scope;
        FILE *file = (FILE*)args.This()->ToObject()->GetPointerFromInternalField(0);
        NULL_STREAM_EXCEPTION(file, "Could not write to stream")
        const void *data;
        int size;
        
        if (args.Length() == 0) {
            return ThrowException(String::New("This requires at least one parameter"));
        } else if (args.Length() == 1) {
            size = args[0]->ToObject()->Get(String::New("length"))->IntegerValue();
            data = args[0]->ToObject()->GetPointerFromInternalField(0);
        } else {
            return ThrowException(String::New("Not currently supported."));
        }
        int wrote = fwrite(data, sizeof(uint8_t), size, file);
        if (ferror(file) || wrote != size) {
            clearerr(file);
            return ThrowException(String::New("Could not write to stream"));
        }
        return Integer::New(wrote);
	}
    
    // args[0] - raw stream
    // args[1] - {app, ate, binary, read, write, trunc}
	JS_FUNCTN(TextIOStream) {
        HandleScope scope;
		Handle<Object> textStream = args.This();
        
        if (args.Length() == 2 && args[0]->IsObject() && args[1]->IsObject()) {
            textStream->Set(String::New(isRawStreamPropName), True(), ReadOnly);
            textStream->Set(String::New("raw"), args[0]->ToObject(), ReadOnly);
        } else {
            return ThrowException(String::New("This is not a valid constructor call"));
        }
		return textStream;
	}
    
    JS_FUNCTN(TextIOStreamReadLine) {
        HandleScope scope;
        FILE *file = (FILE*)args.This()->Get(String::New("raw"))
                        ->ToObject()->GetPointerFromInternalField(0);
        NULL_STREAM_EXCEPTION(file, "Could not read from stream")
        int n = 4096;
        char *buffer;
        if (feof(file)) {
            return ThrowException(String::New("EOF"));
        }
        buffer = new char[n+1];
        buffer[n] = '\0';
        fgets(buffer, n, file);
        if (ferror(file)) {
            clearerr(file);
            delete[] buffer;
            return ThrowException(String::New("Error reading line"));
        }
        int length = strlen(buffer);
		Handle<String> stringVal = String::New(buffer, length);
        delete[] buffer;
        return stringVal;
    }
    
    JS_FUNCTN(TextIOStreamWriteLine) {
        HandleScope scope;
        FILE *file = (FILE*)args.This()->Get(String::New("raw"))
                ->ToObject()->GetPointerFromInternalField(0);
        NULL_STREAM_EXCEPTION(file, "Could not read from stream")
        String::Utf8Value val(args[0]);
        fwrite(*val, sizeof(char), val.length(), file);
        if (!args[1]->IsTrue()) {
            fwrite("\n", 1, 1, file);
        }
        if (ferror(file)) {
            clearerr(file);
            return ThrowException(String::New("Error writing line"));
        }
        return Undefined();
    }
	
	void SetupIOStreams(Handle<Object> internals) {
		HandleScope scope;
		
        // Create RawStream type
		Local<FunctionTemplate> rawStream_t = FunctionTemplate::New(RawIOStream);
        Local<ObjectTemplate> rawStream_ot = rawStream_t->InstanceTemplate();
        
        rawStream_ot->SetAccessor(String::New("position"), 
                                  RawStreamPositionGetter, 
                                  RawStreamPositionSetter);
        SET_METHOD(rawStream_ot, "close", RawIOStreamClose)
        SET_METHOD(rawStream_ot, "flush", RawIOStreamFlush)
        SET_METHOD(rawStream_ot, "skip",  RawIOStreamSkip)
        SET_METHOD(rawStream_ot, "read",  RawIOStreamRead)
        SET_METHOD(rawStream_ot, "write", RawIOStreamWrite)
        rawStream_ot->SetInternalFieldCount(1);
		
        rawStream_f = Persistent<Function>::New(rawStream_t->GetFunction());
		internals->Set(String::New("RawStream"), rawStream_f);
        
        // Create TextStreamType
        Local<FunctionTemplate> textStream_t = FunctionTemplate::New(TextIOStream);
        Local<ObjectTemplate> textStream_ot = textStream_t->InstanceTemplate();
		
        SET_METHOD(textStream_ot, "readLine",  TextIOStreamReadLine)
        SET_METHOD(textStream_ot, "writeLine", TextIOStreamWriteLine)
        
        textStream_f = Persistent<Function>::New(textStream_t->GetFunction());
		internals->Set(String::New("TextStream"), textStream_f);
        
        // seup standard streams
        Local<Value> rawInArgs[1] = { External::New(stdin) };
        Local<Object> rawIn = rawStream_f->NewInstance(1, rawInArgs);
        Local<Value> stdinArgs[2] = { rawIn, Object::New() };
		internals->Set(String::New("stdin"), textStream_f->NewInstance(2, stdinArgs));
        
        Local<Value> rawOutArgs[1] = { External::New(stdout) };
        Local<Object> rawOut = rawStream_f->NewInstance(1, rawOutArgs);
        Local<Value> stdoutArgs[2] = { rawOut, Object::New() };
		internals->Set(String::New("stdout"), textStream_f->NewInstance(2, stdoutArgs));
	}
	
}
