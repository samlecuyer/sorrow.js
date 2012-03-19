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
	
	JS_FUNCTN(OpenRaw) {
		if (args.Length() < 2) return ThrowException(String::New("This method requires 2 parameters"));
		
	}
	
	JS_FUNCTN(Move) {
		
	}
	
	JS_FUNCTN(Remove) {
		HandleScope scope;
		if (args.Length() != 1) return ThrowException(String::New("This method must take 1 argument"));
		String::Utf8Value path(args[0]);
		int rm = remove(*path);
		if (rm != 0) {
			return ThrowException(String::New("File could not be removed"));
		}
		return Undefined();
	}
	
	JS_FUNCTN(Touch) {
		
	}	
    
    void SetupFS(Handle<Object> internals) {
		HandleScope scope;
        
        internals->SetAccessor(String::New("cwd"), CwdGetter, CwdSetter);
		
		SET_METHOD(internals, "remove", Remove)
    }
	
} // namespce sorrow