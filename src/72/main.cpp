// build with:
// g++ -dynamiclib -undefined suppress -flat_namespace \
   -I../../deps/v8/include/ -arch i386  main.cpp -o seventy2.dylib

#include "../sorrow_ext.cpp"

namespace seventy {
	using namespace v8;

	JS_FUNCTN(SeventyTwo) {
		return Integer::New(72);
	}

	extern "C" Handle<Object> Initialize() {
		HandleScope scope;
		Local<Object> sevenObj = Object::New();
	
		SET_METHOD(sevenObj, "getNum", SeventyTwo)
		
		return sevenObj;
	}

	extern "C" void Teardown(Handle<Object> o) {
		//nothing really to do here
	}

}