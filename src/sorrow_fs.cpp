#include "sorrow.h"

namespace sorrow {
	using namespace v8;

    
    JS_GETTER(CwdGetter) {
        char *path = NULL;
        size_t size;
        path = getcwd(path, size);
        return String::New(path);
    }
    
    JS_SETTER(CwdSetter) {
        HandleScope scope;
        String::Utf8Value path(value);
        int err = chdir(*path);
        if (err) {
            ThrowException(String::New("Could not set working directory"));
            return;
        }
    }
    
    void SetupFS(Handle<Object> internals) {
		HandleScope scope;
        
        internals->SetAccessor(String::New("cwd"), CwdGetter, CwdSetter);
    }
	
} // namespce sorrow