/*
 * copyright 2012 sam l'ecuyer
 */

#include <utime.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>

#include "sorrow.h"

namespace sorrow {
	using namespace v8;
    
    /**
     *  File functions for A/0
     */
    
	V8_FUNCTN(OpenRaw) {
		return THROW(ERR(V8_STR("Unimplemented")))
	}
	
    // only implements part 1
	V8_FUNCTN(Move) {
		HandleScope scope;
		if (args.Length() != 2) {
            return THROW(ERR(V8_STR("This method must take two arguments")))
        }
		String::Utf8Value source(args[0]);
        String::Utf8Value target(args[1]);
		int mv = rename(*source, *target);
		if (mv != 0) {
			return THROW(ERR(V8_STR("Could not move file")))
		}
		return Undefined();
	}
	
	V8_FUNCTN(Remove) {
		HandleScope scope;
		if (args.Length() != 1) {
            return THROW(ERR(V8_STR("This method must take one argument")))
        }
        String::Utf8Value path(args[0]);
		int rm = remove(*path);
		if (rm != 0) {
			return THROW(ERR(V8_STR("Could not remove file")))
		}
		return Undefined();
	}
	
	V8_FUNCTN(Touch) {
        HandleScope scope;
        if (args.Length() < 1) {
            return THROW(ERR(V8_STR("This method must take at least one argument")))
        } 
        if (args.Length() == 2 && !args[1]->IsDate()) {
            return THROW(TYPE_ERR(V8_STR("Second arg must be a Date")))
        } 
        
        String::Utf8Value path(args[0]->ToString());
        struct utimbuf ubuf;
        int touch;
        
        struct stat buffer;
        int status = stat(*path, &buffer);
        if (status != 0) {
            status = creat(*path, 0777);
            if (status != 0) {
               return THROW(ERR(V8_STR("Could not touch file"))) 
            }
        }
        
        if (args.Length() == 1) {
            touch = utime(*path, NULL);
        } else  if (args.Length() == 2) {
            Local<Date> date = Date::Cast(*args[1]);
            // I wouldn't count on this being portable
            uint64_t modtime = static_cast<uint64_t>(date->NumberValue());
            ubuf.actime = ubuf.modtime = modtime/1000;
            touch = utime(*path, &ubuf); 
        }
        if (touch != 0) {
            return THROW(ERR(V8_STR("Could not touch file")))
        }
        return Undefined();
	}
    
    
    /**
     *  Directory functions for A/0
     */
    
	V8_FUNCTN(MakeDirectory) {
        HandleScope scope;
		if (args.Length() < 1) {
            return THROW(ERR(V8_STR("This method must take at least one argument")))
        }
        String::Utf8Value path(args[0]->ToString());
        mode_t mod = 0777;
        if (args.Length() == 2 && args[1]->IsUint32()) {
            mod = args[1]->Uint32Value();
        }
        int status = mkdir(*path, mod);
        if (status != 0) {
            return THROW(ERR(V8_STR("Could not create directory")))
        }
        return Undefined();
	}
	
	V8_FUNCTN(RemoveDirectory) {
		HandleScope scope;
		if (args.Length() != 1) {
            return THROW(ERR(V8_STR("This method must take one argument")))
        }
        String::Utf8Value path(args[0]->ToString());
		int status = rmdir(*path);
        if (status != 0) {
            return THROW(ERR(V8_STR("Could not remove directory")))
        }
        return Undefined();
	}
	
	
    /**
     *  Path functions for A/0
     */
	V8_FUNCTN(Canonical) {
		HandleScope scope;
		if (args.Length() != 1) {
            return THROW(ERR(V8_STR("This method must take one argument")))
        }
        String::Utf8Value path(args[0]->ToString());
		char *buffer = new char[PATH_MAX];
		realpath(*path, buffer);
		if (buffer == 0) {
			delete buffer;
			return Undefined();
		}
		Local<String> ret = V8_STR(buffer);
		delete buffer;
		return scope.Close(ret);
	}
    
    // workingDirectory
	V8_FUNCTN(WorkingDirectory) {
        char *path = NULL;
        size_t size;
        path = getcwd(path, size);
        return V8_STR(path);
    }
    
    // changeWorkingDirectory
    V8_FUNCTN(ChangeWorkingDirectory) {
        HandleScope scope;
        String::Utf8Value path(args[0]);
        int err = chdir(*path);
        if (err) {
            return THROW(ERR(V8_STR("Could not change working directory")))
        }
    }
    
    
    /**
     *  Security functions for A/0
     */
    
	V8_FUNCTN(Owner) {
		HandleScope scope;
		if (args.Length() != 1) {
            return THROW(ERR(V8_STR("This method must take one argument")))
        }
        String::Utf8Value path(args[0]->ToString());
        struct stat buffer;
        struct passwd *pwd_buf;
		int status = stat(*path, &buffer);
        if (status != 0) return Undefined();
        pwd_buf = getpwuid(buffer.st_uid);
        return V8_STR(pwd_buf->pw_name);
	}
    V8_FUNCTN(ChangeOwner) {
		HandleScope scope;
		if (args.Length() != 2) {
            return THROW(ERR(V8_STR("This method must take two arguments")))
        }
        String::Utf8Value path(args[0]->ToString());
        String::Utf8Value user(args[1]->ToString());
        struct stat buffer;
		int status = stat(*path, &buffer);
        if (status != 0) {
            return THROW(ERR(V8_STR("Could not change owner")))
        }
        struct passwd *pwd_buf = getpwnam(*user);
        status = chown(*path, pwd_buf->pw_uid, buffer.st_gid);
        if (status != 0) {
            return THROW(ERR(V8_STR("Could not change owner")))
        }
        return Undefined();
	}
	
	V8_FUNCTN(Group) {
		HandleScope scope;
		if (args.Length() != 1) {
            return THROW(ERR(V8_STR("This method must take one argument")))
        }
        String::Utf8Value path(args[0]->ToString());
        struct stat buffer;
        struct group *grp_buf;
		int status = stat(*path, &buffer);
        if (status != 0) return Undefined();
        grp_buf = getgrgid(buffer.st_gid);
        return V8_STR(grp_buf->gr_name);
	}
    V8_FUNCTN(ChangeGroup) {
		HandleScope scope;
		if (args.Length() != 2) {
            return THROW(ERR(V8_STR("This method must take two arguments")))
        }
        String::Utf8Value path(args[0]->ToString());
        String::Utf8Value name(args[1]->ToString());
        struct stat buffer;
		int status = stat(*path, &buffer);
        if (status != 0) {
            return THROW(ERR(V8_STR("Could not change group")))
        }
        struct group *grp_buf = getgrnam(*name);
        status = chown(*path, buffer.st_uid, grp_buf->gr_gid);
        if (status != 0) {
            return THROW(ERR(V8_STR("Could not change group")))
        }
        return Undefined();
	}
    
    V8_FUNCTN(Permissions) {
		HandleScope scope;
		if (args.Length() != 1) {
            return THROW(ERR(V8_STR("This method must take one argument")))
        }
        String::Utf8Value path(args[0]->ToString());
        struct stat buffer;
		int status = stat(*path, &buffer);
        if (status != 0) return Undefined();
        return Integer::New(buffer.st_mode);
	}
    V8_FUNCTN(ChangePermissions) {
		HandleScope scope;
		if (args.Length() != 2) {
            return THROW(ERR(V8_STR("This method must take two arguments")))
        }
        String::Utf8Value path(args[0]->ToString());
        mode_t mod = args[1]->Uint32Value();
		int status = chmod(*path, mod);
        if (status != 0) {
            return THROW(ERR(V8_STR("Could not change permissions")))
        }
        return Undefined();

	}
    
    
    /**
     *  Link functions for A/0
     */
    
	V8_FUNCTN(SymbolicLink) {
		HandleScope scope;
		if (args.Length() != 2) {
            return THROW(ERR(V8_STR("This method must take two arguments")))
        }
        String::Utf8Value path1(args[0]->ToString());
        String::Utf8Value path2(args[1]->ToString());
        int status = symlink(*path1, *path2);
        if (status != 0) {
            return THROW(ERR(V8_STR("Could not create symlink")))
        }
        return Undefined();
	}
	
	V8_FUNCTN(HardLink) {
		HandleScope scope;
		if (args.Length() != 2) {
            return THROW(ERR(V8_STR("This method must take two arguments")))
        }
        String::Utf8Value path1(args[0]->ToString());
        String::Utf8Value path2(args[1]->ToString());
        int status = link(*path1, *path2);
        if (status != 0) {
            return THROW(ERR(V8_STR("Could not create link")))
        }
        return Undefined();
	}
    
    V8_FUNCTN(ReadLink) {
		HandleScope scope;
		if (args.Length() != 1) {
            return THROW(ERR(V8_STR("This method must take one argument")))
        }
        String::Utf8Value path(args[0]->ToString());
        char *buffer = new char[PATH_MAX];
		int read = readlink(*path, buffer, PATH_MAX);
        if (read == -1) {
            delete buffer;
            return THROW(ERR(V8_STR("Could not read link")))
        }
        Local<String> target = String::New(buffer, read);
        delete buffer;
        return scope.Close(target);
	}
    
    
    /**
     *  Test functions for A/0
     */
    
	V8_FUNCTN(Exists) {
		HandleScope scope;
		if (args.Length() != 1) {
            return THROW(ERR(V8_STR("This method must take one argument")))
        }
        String::Utf8Value path(args[0]->ToString());
        struct stat buffer;
		int status = stat(*path, &buffer);
        return Boolean::New(status == 0);
	}
	
	V8_FUNCTN(IsFile) {
		HandleScope scope;
		if (args.Length() != 1) {
            return THROW(ERR(V8_STR("This method must take one argument")))
        }
        String::Utf8Value path(args[0]->ToString());
        struct stat buffer;
		int status = stat(*path, &buffer);
        if (status != 0) return False();
        return Boolean::New(S_ISREG(buffer.st_mode));
	}
    
    V8_FUNCTN(IsDirectory) {
		HandleScope scope;
		if (args.Length() != 1) {
            return THROW(ERR(V8_STR("This method must take one argument")))
        }
        String::Utf8Value path(args[0]->ToString());
        struct stat buffer;
		int status = stat(*path, &buffer);
        if (status != 0) return False();
        return Boolean::New(S_ISDIR(buffer.st_mode));
	}
    
    V8_FUNCTN(IsLink) {
		HandleScope scope;
		if (args.Length() != 1) {
            return THROW(ERR(V8_STR("This method must take one argument")))
        }
        String::Utf8Value path(args[0]->ToString());
        struct stat buffer;
		int status = lstat(*path, &buffer);
        if (status != 0) return False();
        return Boolean::New(S_ISLNK(buffer.st_mode));
	}
    
    V8_FUNCTN(IsReadable) {
		HandleScope scope;
		if (args.Length() != 1) {
            return THROW(ERR(V8_STR("This method must take one argument")))
        }
        String::Utf8Value path(args[0]->ToString());
        int status = access(*path, R_OK);
        return Boolean::New(status == 0);
	}
    
    V8_FUNCTN(IsWriteable) {
        HandleScope scope;
		if (args.Length() != 1) {
            return THROW(ERR(V8_STR("This method must take one argument")))
        }
        String::Utf8Value path(args[0]->ToString());
		int status = access(*path, W_OK);
        return Boolean::New(status == 0);
	}
    
    V8_FUNCTN(Same) {
		HandleScope scope;
		if (args.Length() != 2) {
            return THROW(ERR(V8_STR("This method must take two arguments")))
        }
        String::Utf8Value path1(args[0]->ToString());
        String::Utf8Value path2(args[1]->ToString());
        struct stat buffer1;
        int         status1;
        struct stat buffer2;
        int         status2;
		status1 = stat(*path1, &buffer1);
        if (status1 != 0) return False();
        status2 = stat(*path2, &buffer2);
        if (status2 != 0) return False();
        return Boolean::New(buffer1.st_ino == buffer2.st_ino);
	}
    
    V8_FUNCTN(SameFilesystem) {
		return THROW(ERR(V8_STR("Unimplemented")))
	}
    
    
    /**
     *  Attribute functions for A/0
     */
    
	V8_FUNCTN(Size) {
		HandleScope scope;
		if (args.Length() != 1) {
            return THROW(ERR(V8_STR("This method must take one argument")))
        }
        String::Utf8Value path(args[0]->ToString());
        struct stat buffer;
        int status = stat(*path, &buffer);
        if (status != 0) {
            return THROW(ERR(V8_STR("Could not get size")))
        }
        return Number::New(buffer.st_size);
	}
	
	V8_FUNCTN(LastModified) {
		HandleScope scope;
		if (args.Length() != 1) {
            return THROW(ERR(V8_STR("This method must take one argument")))
        }
        String::Utf8Value path(args[0]->ToString());
        struct stat buffer;
        int status = stat(*path, &buffer);
        if (status != 0) {
            return THROW(ERR(V8_STR("Could not get last modified")))
        }
        // don't count on portability :(
        return Date::New(static_cast<double>(buffer.st_mtime)*1000);
	}
    
    
    /**
     *  Listing functions for A/0
     */
    
	V8_FUNCTN(List) {
		HandleScope scope;
		if (args.Length() != 1) {
            return THROW(ERR(V8_STR("This method must take one argument")))
        }
        String::Utf8Value path(args[0]->ToString());
		DIR *dir;
        struct dirent *listing;
		dir = opendir(*path);
        if (dir == NULL) {
            return THROW(ERR(V8_STR("Could not read directory")))
        }
        // seekdir doesn't seem to have an end like fseek does, so 
        // we have to iterate it twice.  sub-optimal.
        int count = 0;
        while (dir) {
            if (readdir(dir) != NULL) {
                count++;
            } else {
                break;
            }
        }
        rewinddir(dir);
        Local<Array> list = Array::New(count);
        count = 0;
        while (dir) {
            if ((listing = readdir(dir)) != NULL) {
                list->Set(Integer::New(count++), V8_STR(listing->d_name));
            } else {
                closedir(dir);
                break;
            }
        }
        return scope.Close(list);
	}
	
	V8_FUNCTN(Iterate) {
		return THROW(ERR(V8_STR("Unimplemented")))
	}
    
    V8_FUNCTN(Next) {
		return THROW(ERR(V8_STR("Unimplemented")))
	}
    
    V8_FUNCTN(Iterator) {
		return THROW(ERR(V8_STR("Unimplemented")))
	}
    
    V8_FUNCTN(Close) {
		return THROW(ERR(V8_STR("Unimplemented")))
	}
    
    
    namespace Filesystem {    
    void Initialize(Handle<Object> internals) {
		HandleScope scope;
        Local<Object> fsObj = Object::New();
        
        SET_METHOD(fsObj, "workingDirectory",    WorkingDirectory)
        SET_METHOD(fsObj, "changeWorkingDirectory",    ChangeWorkingDirectory)
		
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
        
        SET_METHOD(fsObj, "slink",      SymbolicLink)
        SET_METHOD(fsObj, "hlink",      HardLink)
        SET_METHOD(fsObj, "rlink",      ReadLink)
        
        SET_METHOD(fsObj, "exists",     Exists)
        SET_METHOD(fsObj, "isfile",     IsFile)
        SET_METHOD(fsObj, "isdir",      IsDirectory)
        SET_METHOD(fsObj, "islink",     IsLink)
        SET_METHOD(fsObj, "isreadable", IsReadable)
        SET_METHOD(fsObj, "iswriteable",IsWriteable)
        SET_METHOD(fsObj, "same",       Same)
        SET_METHOD(fsObj, "samefs",     SameFilesystem)
        
        SET_METHOD(fsObj, "size",       Size)
        SET_METHOD(fsObj, "lastmod",    LastModified)
        
        SET_METHOD(fsObj, "list",       List)
        
        internals->Set(V8_STR("fs"), fsObj);
    }
    }
	
} // namespce sorrow