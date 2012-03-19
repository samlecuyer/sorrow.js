#include "sorrow.h"

namespace sorrow {
	using namespace v8;
    
    /**
     *  File functions for A/0
     */
    
	JS_FUNCTN(OpenRaw) {
		if (args.Length() < 2) return ThrowException(String::New("This method requires 2 parameters"));
		return ThrowException(String::New("Unimplemented"));
	}
	
    // only implements part 1
	JS_FUNCTN(Move) {
		HandleScope scope;
		if (args.Length() != 2) return ThrowException(String::New("This method must take 2 arguments"));
		String::Utf8Value source(args[0]);
        String::Utf8Value target(args[1]);
		int mv = rename(*source, *target);
		if (mv != 0) {
			return ThrowException(String::New("File could not be moved"));
		}
		return Undefined();
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
		return ThrowException(String::New("Unimplemented"));
	}
    
    
    /**
     *  Directory functions for A/0
     */
    
	JS_FUNCTN(MakeDirectory) {
		return ThrowException(String::New("Unimplemented"));
	}
	
	JS_FUNCTN(RemoveDirectory) {
		return ThrowException(String::New("Unimplemented"));
	}
	
	
    /**
     *  Path functions for A/0
     */
	JS_FUNCTN(Canonical) {
		return ThrowException(String::New("Unimplemented"));
	}
	
    // WorkingDirectory functions are implemented as an rw
    // variable and any wrappers can be done in js
    
    // workingDirectory
	JS_GETTER(CwdGetter) {
        char *path = NULL;
        size_t size;
        path = getcwd(path, size);
        return String::New(path);
    }
    
    // changeWorkingDirectory
    JS_SETTER(CwdSetter) {
        HandleScope scope;
        String::Utf8Value path(value);
        int err = chdir(*path);
        if (err) {
            ThrowException(String::New("Could not set working directory"));
            return;
        }
    }
    
    
    /**
     *  Security functions for A/0
     */
    
	JS_FUNCTN(Owner) {
		return ThrowException(String::New("Unimplemented"));
	}
    JS_FUNCTN(ChangeOwner) {
		return ThrowException(String::New("Unimplemented"));
	}
	
	JS_FUNCTN(Group) {
		return ThrowException(String::New("Unimplemented"));
	}
    JS_FUNCTN(ChangeGroup) {
		return ThrowException(String::New("Unimplemented"));
	}
    
    JS_FUNCTN(Permissions) {
		return ThrowException(String::New("Unimplemented"));
	}
    JS_FUNCTN(ChangePermissions) {
		return ThrowException(String::New("Unimplemented"));
	}
    
    
    /**
     *  Link functions for A/0
     */
    
	JS_FUNCTN(SymbolicLink) {
		return ThrowException(String::New("Unimplemented"));
	}
	
	JS_FUNCTN(HardLink) {
		return ThrowException(String::New("Unimplemented"));
	}
    
    JS_FUNCTN(ReadLink) {
		return ThrowException(String::New("Unimplemented"));
	}
    
    
    /**
     *  Test functions for A/0
     */
    
	JS_FUNCTN(Exists) {
		return ThrowException(String::New("Unimplemented"));
	}
	
	JS_FUNCTN(IsFile) {
		return ThrowException(String::New("Unimplemented"));
	}
    
    JS_FUNCTN(IsDirectory) {
		return ThrowException(String::New("Unimplemented"));
	}
    
    JS_FUNCTN(IsLink) {
		return ThrowException(String::New("Unimplemented"));
	}
    
    JS_FUNCTN(IsReadable) {
		return ThrowException(String::New("Unimplemented"));
	}
    
    JS_FUNCTN(IsWriteable) {
		return ThrowException(String::New("Unimplemented"));
	}
    
    JS_FUNCTN(Same) {
		return ThrowException(String::New("Unimplemented"));
	}
    
    JS_FUNCTN(SameFilesystem) {
		return ThrowException(String::New("Unimplemented"));
	}
    
    
    /**
     *  Attribute functions for A/0
     */
    
	JS_FUNCTN(Size) {
		return ThrowException(String::New("Unimplemented"));
	}
	
	JS_FUNCTN(LastModified) {
		return ThrowException(String::New("Unimplemented"));
	}
    
    
    /**
     *  Listing functions for A/0
     */
    
	JS_FUNCTN(List) {
		return ThrowException(String::New("Unimplemented"));
	}
	
	JS_FUNCTN(Iterate) {
		return ThrowException(String::New("Unimplemented"));
	}
    
    JS_FUNCTN(Next) {
		return ThrowException(String::New("Unimplemented"));
	}
    
    JS_FUNCTN(Iterator) {
		return ThrowException(String::New("Unimplemented"));
	}
    
    JS_FUNCTN(Close) {
		return ThrowException(String::New("Unimplemented"));
	}
    
    
        
    void SetupFS(Handle<Object> internals) {
		HandleScope scope;
        
        internals->SetAccessor(String::New("cwd"), CwdGetter, CwdSetter);
		
		SET_METHOD(internals, "remove", Remove)
    }
	
} // namespce sorrow