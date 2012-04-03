// copyright 2012 sam l'ecuyer

#ifndef _SORROW_H_
#define _SORROW_H_

#include <v8.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <sys/param.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#define NULL_STREAM_EXCEPTION(file, message) \
    if (file == NULL) { \
        return ThrowException(String::New(#message)); \
    }

#define JS_SETTER(name) void name(Local<String> property, Local<Value> value, const AccessorInfo& info)
#define JS_GETTER(name) Handle<Value> name(Local<String> property, const AccessorInfo& info)
#define JS_FUNCTN(name) Handle<Value> name(const Arguments& args)
#define JS_STR(lit)     String::New(lit)
#define FN_OF_TMPLT(name) FunctionTemplate::New(name)->GetFunction()
#define SET_METHOD(obj,name,method) obj->Set(JS_STR(name), FN_OF_TMPLT(method));
#define EXCEPTION(message) ThrowException(JS_STR(message));

#define IS_BINARY(obj) (byteString_t->HasInstance(obj) || byteArray_t->HasInstance(obj))
#define BYTES_FROM_BIN(obj) reinterpret_cast<Bytes*>(obj->GetPointerFromInternalField(0))

namespace sorrow {
	using namespace v8;
	
	Local<Value> ExecuteString(Handle<String> source, Handle<Value> filename);
	JS_FUNCTN(Quit);
	JS_FUNCTN(Version);
	void ReportException(TryCatch* handler);
	void LoadNativeLibraries(Handle<Object> natives);
	
    void InitV8Arrays(Handle<Object> target);
    
    /**
     * sorrow_io.cpp
     */
    namespace IOStreams {
        void Initialize(Handle<Object> internals);
    }
    // Can only be used after streams are setup
    extern Persistent<Function> rawStream_f;
    extern Persistent<Function> textStream_f;
    
    
    /**
     * sorrow_binary.cpp
     */
    namespace BinaryTypes {
        void Initialize(Handle<Object> internals);
    }
    // Can only be used after types are setup
    extern Persistent<Function> byteString;
    extern Persistent<Function> byteArray;
    extern Persistent<FunctionTemplate> byteString_t;
    extern Persistent<FunctionTemplate> byteArray_t;
    
    /**
     * sorrow_fs.cpp
     */
    namespace Filesystem {
        void Initialize(Handle<Object> internals);
    }
	
	/**
     * sorrow_ext.cpp
     */
    namespace Extensions {
        void Initialize(Handle<Object> internals);
    }
	
	typedef Handle<Object> sp_init();
	typedef void *sp_destroy();

    
}

#endif // _SORROW_H_
