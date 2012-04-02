/*
 * copyright 2012 sam l'ecuyer
 */

#include <dlfcn.h>

#include "sorrow.h"

namespace sorrow {
	using namespace v8;
	
	void PluginWeakCallback(Persistent<Value> object, void* handle) {
		sp_destroy *destructor = (sp_destroy*)dlsym(handle, "Teardown");
		if (destructor == NULL) {
			dlclose(handle);
		}
		destructor();
        dlclose(handle);
		object.Dispose();
	}
	
	/** 
     * Plugin functions
     */
    
	JS_FUNCTN(SorrowPlugin) {
        HandleScope scope;
		if (args.Length() != 1) return EXCEPTION("This method must take 1 argument")
        Handle<Object> plugin = args.This();
		plugin->SetHiddenValue(JS_STR("lib"), args[0]);
		return plugin;
	}	
	
	
	JS_FUNCTN(LoadExtension) {
		HandleScope scope;
		Handle<Object> plugin = args.This();
		String::Utf8Value source(plugin->GetHiddenValue(JS_STR("lib")));
		void *sp_library = dlopen(*source, RTLD_LAZY);
		if (sp_library == NULL) {
			return EXCEPTION(dlerror());
		}
		
		sp_init *initializer = (sp_init*)dlsym(sp_library, "Initialize");
		if (initializer == NULL) {
			dlclose(sp_library);
			return EXCEPTION(dlerror());
		}
		
		Persistent<Object> persistent_plugin = Persistent<Object>::New(plugin);
		persistent_plugin.MakeWeak(sp_library, PluginWeakCallback);
		persistent_plugin.MarkIndependent();
		
		plugin->SetPointerInInternalField(0, sp_library);
		Handle<Object> ret = initializer();
		return scope.Close(ret);
	}
	
	JS_FUNCTN(CloseExtension) {
		HandleScope scope;
		Handle<Object> plugin = args.This();
		void *sp_library = plugin->GetPointerFromInternalField(0);
		sp_destroy *destructor = (sp_destroy*)dlsym(sp_library, "Teardown");
		if (destructor == NULL) {
			dlclose(sp_library);
		}
		destructor();
        dlclose(sp_library);
		return Undefined();
	}
    
    namespace Extensions {    
		void Initialize(Handle<Object> internals) {
			HandleScope scope;
			Local<Object> extObj = Object::New();
			
			Local<FunctionTemplate> plugin_t = FunctionTemplate::New(SorrowPlugin);
			Local<ObjectTemplate> plugin_ot = plugin_t->PrototypeTemplate();
			SET_METHOD(plugin_ot, "load",	LoadExtension)
			SET_METHOD(plugin_ot, "close",	CloseExtension)
			plugin_ot->SetInternalFieldCount(1);
			extObj->Set(JS_STR("Plugin"), plugin_t->GetFunction());
			
			internals->Set(String::New("ext"), extObj);
		}
    }
	
} // namespce sorrow