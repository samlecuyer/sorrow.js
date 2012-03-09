/*
 copyright 2012 sam l'ecuyer
 */

#include "sorrow.h"

namespace sorrow {
	using namespace v8;
    
	void ExternalWeakIOCallback(Persistent<Value> object, void* file) {
        fclose((FILE*)file);
		object.Dispose();
	}
    
    const char isRawStreamPropName[] = "_is_raw_stream_";
    
    Handle<Value> RawStreamPositionGetter(Local<String> property, const AccessorInfo& info) {
        FILE *file = (FILE*)info.This()->ToObject()->GetPointerFromInternalField(0);
        if (file == NULL) {
            return ThrowException(String::New("Could not get stream position"));
        }
        int pos = ftell(file);
        return Integer::New(pos);
    }
    
    void RawStreamPositionSetter(Local<String> property, Local<Value> value, const AccessorInfo& info) {
        FILE *file = (FILE*)info.This()->ToObject()->GetPointerFromInternalField(0);
        int pos = value->IntegerValue();
        fseek(file, pos, SEEK_CUR);
    }
	
    // args[0] - file name
    // args[1] - {app, ate, binary, read, write, trunc}
	Handle<Value> RawIOStream(const Arguments& args) {
        HandleScope scope;
		Handle<Object> rawStream = args.This();
        
        FILE *file;
        if (args.Length() == 2 && args[0]->IsString() && args[1]->IsString()) {
            String::Utf8Value name(args[0]);
            String::Utf8Value mode(args[1]);
            
            file = fopen(*name, *mode);
            if (file == NULL) {
                return ThrowException(String::New("Could not open stream"));
            }
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
	
    Handle<Value> RawIOStreamClose(const Arguments& args) {
        FILE *file = (FILE*)args.This()->ToObject()->GetPointerFromInternalField(0);
        if (file == NULL) {
            return ThrowException(String::New("Could not close stream"));
        }
        fclose(file);
        if (ferror(file)) {
            return ThrowException(String::New("Could not close stream"));
        }
        return Undefined();
	}	
	
    Handle<Value> RawIOStreamFlush(const Arguments& args) {
        FILE *file = (FILE*)args.This()->ToObject()->GetPointerFromInternalField(0);
        if (file == NULL) {
            return ThrowException(String::New("Could not flush stream"));
        }
        fflush(file);
        if (ferror(file)) {
            return ThrowException(String::New("Could not flush stream"));
        }
        return Undefined();
	}	
    
    Handle<Value> RawIOStreamRead(const Arguments& args) {
        HandleScope scope;
        FILE *file = (FILE*)args.This()->ToObject()->GetPointerFromInternalField(0);
        int n;
        uint8_t *buffer;
        if (ferror(file)) {
            return ThrowException(String::New("Stream is in a bad state"));
        }
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
		Local<Value> bsArgs[2] = { External::New(buffer), Integer::New(readBytes) };
		Local<Value> bs = byteString_f->NewInstance(2, bsArgs);
        delete[] buffer;
        return scope.Close(bs);
	}	
	
	void SetupIOStreams(Handle<Object> internals) {
		HandleScope scope;
		
		Local<FunctionTemplate> rawStream_t = FunctionTemplate::New(RawIOStream);
        Local<ObjectTemplate> rawStream_ot = rawStream_t->InstanceTemplate();
        
        rawStream_ot->SetAccessor(String::New("position"), 
                                  RawStreamPositionGetter, 
                                  RawStreamPositionSetter);
        rawStream_ot->Set(String::New("close"), FunctionTemplate::New(RawIOStreamClose)->GetFunction());
        rawStream_ot->Set(String::New("flush"), FunctionTemplate::New(RawIOStreamFlush)->GetFunction());
        rawStream_ot->Set(String::New("read"), FunctionTemplate::New(RawIOStreamRead)->GetFunction());
        rawStream_ot->SetInternalFieldCount(1);
		
		internals->Set(String::New("RawStream"), rawStream_t->GetFunction());
	}
	
}
