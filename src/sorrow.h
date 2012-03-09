// copyright 2012 sam l'ecuyer

#ifndef _SORROW_H_
#define _SORROW_H_

#include <v8.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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
	
    /**
     * sorrow_io.cpp
     */
    void SetupIOStreams(Handle<Object> internals);
    
    
    /**
     * sorrow_binary.cpp
     */
    void SetupBinaryTypes(Handle<Object> internals);
    // Can only be used after types are setup
    extern Persistent<Function> byteString_f;
    extern Persistent<Function> byteArray_f;
    
}

#endif // _SORROW_H_
