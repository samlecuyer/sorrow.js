/*
 * copyright 2012 sam l'ecuyer
 */

#include <utime.h>
#include <sys/stat.h>

#include "sorrow.h"

namespace sorrow {
	using namespace v8;
    
    /**
     *  File functions for A/0
     */
    
	JS_FUNCTN(OpenRaw) {
		return EXCEPTION("Unimplemented")
	}
	
    // only implements part 1
	JS_FUNCTN(Move) {
		HandleScope scope;
		if (args.Length() != 2) return EXCEPTION("This method must take 2 arguments")
		String::Utf8Value source(args[0]);
        String::Utf8Value target(args[1]);
		int mv = rename(*source, *target);
		if (mv != 0) {
			return EXCEPTION("File could not be moved")
		}
		return Undefined();
	}
	
	JS_FUNCTN(Remove) {
		HandleScope scope;
		if (args.Length() != 1) return EXCEPTION("This method must take 1 argument")
		String::Utf8Value path(args[0]);
		int rm = remove(*path);
		if (rm != 0) {
			return EXCEPTION("File could not be removed")
		}
		return Undefined();
	}
	
    // TODO: This does not work if the file doesn't exist
	JS_FUNCTN(Touch) {
        HandleScope scope;
        if (args.Length() < 1) {
            return EXCEPTION("This method must take at least 1 argument")
        } 
        if (args.Length() == 2 && !args[1]->IsDate()) {
            return EXCEPTION("Second arg must be a Date")
        } 
        char *path;
        struct utimbuf ubuf;
        
        int touch;
        String::Utf8Value pathString(args[0]->ToString());
        path = *pathString;
        
        if (args.Length() == 1) {
            touch = utime(path, NULL);
        } else  {
            Local<Date> date = Date::Cast(*args[1]);
            // I wouldn't count on this being portable
            uint64_t modtime = static_cast<uint64_t>(date->NumberValue());
            ubuf.actime = ubuf.modtime = modtime/1000;
            touch = utime(path, &ubuf); 
        }
		if (touch != 0) {
            return EXCEPTION("File could not be touched")
        }
        return Undefined();
	}
    
    
    /**
     *  Directory functions for A/0
     */
    
	JS_FUNCTN(MakeDirectory) {
		return EXCEPTION("Unimplemented")
	}
	
	JS_FUNCTN(RemoveDirectory) {
		return EXCEPTION("Unimplemented")
	}
	
	
    /**
     *  Path functions for A/0
     */
	JS_FUNCTN(Canonical) {
		HandleScope scope;
		if (args.Length() != 1) return EXCEPTION("This method must take 1 argument")
        String::Utf8Value path(args[0]->ToString());
		char *fullPath = realpath(*path, NULL);
		if (fullPath == 0) {
			return Undefined();
		}
		return String::New(fullPath);
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
            EXCEPTION("Could not set working directory")
            return;
        }
    }
    
    
    /**
     *  Security functions for A/0
     */
    
	JS_FUNCTN(Owner) {
		return EXCEPTION("Unimplemented")
	}
    JS_FUNCTN(ChangeOwner) {
		return EXCEPTION("Unimplemented")
	}
	
	JS_FUNCTN(Group) {
		return EXCEPTION("Unimplemented")
	}
    JS_FUNCTN(ChangeGroup) {
		return EXCEPTION("Unimplemented")
	}
    
    JS_FUNCTN(Permissions) {
		return EXCEPTION("Unimplemented")
	}
    JS_FUNCTN(ChangePermissions) {
		return EXCEPTION("Unimplemented")
	}
    
    
    /**
     *  Link functions for A/0
     */
    
	JS_FUNCTN(SymbolicLink) {
		return EXCEPTION("Unimplemented")
	}
	
	JS_FUNCTN(HardLink) {
		return EXCEPTION("Unimplemented")
	}
    
    JS_FUNCTN(ReadLink) {
		return EXCEPTION("Unimplemented")
	}
    
    
    /**
     *  Test functions for A/0
     */
    
	JS_FUNCTN(Exists) {
		return EXCEPTION("Unimplemented")
	}
	
	JS_FUNCTN(IsFile) {
		return EXCEPTION("Unimplemented")
	}
    
    JS_FUNCTN(IsDirectory) {
		return EXCEPTION("Unimplemented")
	}
    
    JS_FUNCTN(IsLink) {
		return EXCEPTION("Unimplemented")
	}
    
    JS_FUNCTN(IsReadable) {
		return EXCEPTION("Unimplemented")
	}
    
    JS_FUNCTN(IsWriteable) {
		return EXCEPTION("Unimplemented")
	}
    
    JS_FUNCTN(Same) {
		return EXCEPTION("Unimplemented")
	}
    
    JS_FUNCTN(SameFilesystem) {
		return EXCEPTION("Unimplemented")
	}
    
    
    /**
     *  Attribute functions for A/0
     */
    
	JS_FUNCTN(Size) {
		return EXCEPTION("Unimplemented")
	}
	
	JS_FUNCTN(LastModified) {
		return EXCEPTION("Unimplemented")
	}
    
    
    /**
     *  Listing functions for A/0
     */
    
	JS_FUNCTN(List) {
		return EXCEPTION("Unimplemented")
	}
	
	JS_FUNCTN(Iterate) {
		return EXCEPTION("Unimplemented")
	}
    
    JS_FUNCTN(Next) {
		return EXCEPTION("Unimplemented")
	}
    
    JS_FUNCTN(Iterator) {
		return EXCEPTION("Unimplemented")
	}
    
    JS_FUNCTN(Close) {
		return EXCEPTION("Unimplemented")
	}
    
    
        
    void SetupFS(Handle<Object> internals) {
		HandleScope scope;
        Local<Object> fsObj = Object::New();
        
        fsObj->SetAccessor(String::New("cwd"), CwdGetter, CwdSetter);
		
		SET_METHOD(fsObj, "openRaw",    OpenRaw)
        SET_METHOD(fsObj, "move",       Move)
        SET_METHOD(fsObj, "remove",     Remove)
        SET_METHOD(fsObj, "touch",      Touch)
        SET_METHOD(fsObj, "mkdir",      MakeDirectory)
        SET_METHOD(fsObj, "rmdir",      RemoveDirectory)
        SET_METHOD(fsObj, "canonical",  Canonical)
        SET_METHOD(fsObj, "owner",      Owner)
        SET_METHOD(fsObj, "chown",      ChangeOwner)
        SET_METHOD(fsObj, "group",      Group)
        SET_METHOD(fsObj, "chgroup",    ChangeGroup)
        SET_METHOD(fsObj, "perm",       Permissions)
        SET_METHOD(fsObj, "chperm",     ChangePermissions)
        
        internals->Set(String::New("fs"), fsObj);
    }
	
} // namespce sorrow