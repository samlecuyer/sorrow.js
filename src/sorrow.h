// copyright 2012 sam l'ecuyer

#ifndef _SORROW_H_
#define _SORROW_H_

#include <v8.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NULL_STREAM_EXCEPTION(file, message) \
    if (file == NULL) { \
        return ThrowException(String::New(#message)); \
    }

#define JS_SETTER(name) void name(Local<String> property, Local<Value> value, const AccessorInfo& info)
#define JS_GETTER(name) Handle<Value> name(Local<String> property, const AccessorInfo& info)
#define JS_FUNCTN(name) Handle<Value> name(const Arguments& args)
#define FN_OF_TMPLT(name) FunctionTemplate::New(name)->GetFunction()
#define SET_METHOD(obj,name,method) obj->Set(String::New(name), FunctionTemplate::New(method)->GetFunction());

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
    void SetupIOStreams(Handle<Object> internals);
    // Can only be used after streams are setup
    extern Persistent<Function> rawStream_f;
    extern Persistent<Function> textStream_f;
    
    
    /**
     * sorrow_binary.cpp
     */
    void SetupBinaryTypes(Handle<Object> internals);
    // Can only be used after types are setup
    extern Persistent<Function> byteString_f;
    extern Persistent<Function> byteArray_f;
    
    /**
     * sorrow_io.cpp
     */
    void SetupFS(Handle<Object> internals);
    
}

#endif // _SORROW_H_
