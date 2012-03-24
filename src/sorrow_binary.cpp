/*
 copyright 2012 sam l'ecuyer
 */

#include "sorrow.h"
#include "sorrow_bytes.h"

namespace sorrow {
	using namespace v8;
	
    Persistent<Function> byteString;
    Persistent<Function> byteArray;
    Persistent<FunctionTemplate> byteString_t;
    Persistent<FunctionTemplate> byteArray_t;
    
	JS_FUNCTN(BinaryFunction) {
		return ThrowException(JS_STR("Binary is non-instantiable"));
	}
    
    JS_GETTER(BinaryLengthGetter) {
        return Integer::New(BYTES_FROM_BIN(info.This())->getLength());
    }
    
	void ExternalWeakCallback(Persistent<Value> object, void* data) {
        delete reinterpret_cast<Bytes*>(data);
		object.Dispose();
	}
    
    JS_FUNCTN(BinaryToArray) {
        return BYTES_FROM_BIN(args.This())->toArray();
    }
    
    JS_FUNCTN(ByteStringCodeAt) {
        HandleScope scope;
        Local<Object> byteString = args.This();
        uint64_t index = args[0]->IntegerValue();
        uint64_t code = BYTES_FROM_BIN(args.This())->getByteAt(index);
        return Number::New(code);
    }

    
    Handle<Value> ByteStringIndexedGetter(uint32_t index, const AccessorInfo &info) {
        HandleScope scope;
        uint8_t byte[1] = { BYTES_FROM_BIN(info.This())->getByteAt(index) };
        Bytes *bytes = new Bytes(1, byte);
		Local<Value> bsArgs[1] = { External::New((void*)bytes) };
		Local<Value> bs = byteString->NewInstance(1, bsArgs);
        
        return scope.Close(bs);
    }
    
    JS_FUNCTN(ByteStringDecodeToString) {
        HandleScope scope;
        Bytes *bytes = BYTES_FROM_BIN(args.This());
        return String::New((const char*)bytes->getBytes(), bytes->getLength());
    }

    
    JS_FUNCTN(ByteString) {
        HandleScope scope;
        Handle<Object> string = args.This();
		Bytes *bytes;
        try {
		int argsLength = args.Length();
        switch (argsLength) {
            case 0: 
                // ByteString()
                bytes = new Bytes();
                break;
            case 1:
                if (args[0]->IsArray()) {
                    // ByteString(arrayOfNumbers)
                    Local<Array> array =  Array::Cast(*args[0]);
                    bytes = new Bytes(array);
                } else if (IS_BINARY(args[0])) {
                    // ByteString(byteString|byteArray)
                    Local<Object> obj = Object::Cast(*args[0]);
                    bytes = new Bytes(BYTES_FROM_BIN(obj));
                } else if (args[0]->IsExternal()) {
                    Bytes *otherBytes = reinterpret_cast<Bytes*>(External::Unwrap(args[0]));
                    bytes = new Bytes(otherBytes);
                } else{
                    return EXCEPTION("Not valid parameters -- bytestring")
                }
                break;
            case 2:
                // ByteString(string, charset)
                String::Utf8Value str(args[0]->ToString());
                String::Utf8Value charset(args[1]->ToString());
                bytes = new Bytes(str.length(), (uint8_t*)(*str));
                break;
        }
		} catch ( char *e) {
            return EXCEPTION(e)
        }
		Persistent<Object> persistent_string = Persistent<Object>::New(string);
		persistent_string.MakeWeak(bytes, ExternalWeakCallback);
		persistent_string.MarkIndependent();
		
		string->SetPointerInInternalField(0, bytes);
		return string;
    }
	
    Handle<Value> ByteArrayIndexedGetter(uint32_t index, const AccessorInfo &info) {
        assert(index >= 0 && index < BYTES_FROM_BIN(info.This())->getLength());
        return Integer::New(BYTES_FROM_BIN(info.This())->getByteAt(index));
    }
    Handle< Value >ByteArrayIndexedSetter(uint32_t index, Local< Value > value, const AccessorInfo &info) {
        uint32_t val = value->Uint32Value();
        if (!IS_BYTE(val)) {
            EXCEPTION("must be a byte value")
        }
        BYTES_FROM_BIN(info.This())->setByteAt(index, val);
        return Undefined();
    }
    
	JS_FUNCTN(ByteArray) {
        HandleScope scope;
        Handle<Object> string = args.This();
		Bytes *bytes;
		int argsLength = args.Length();
        switch (argsLength) {
            case 0: 
                // ByteString()
                bytes = new Bytes();
                break;
            case 1:
                if (args[0]->IsNumber()) {
                    // ByteString(arrayOfNumbers)
                    bytes = new Bytes(args[0]->Uint32Value());
                } else if (args[0]->IsArray()) {
                    // ByteString(arrayOfNumbers)
                    Local<Array> array =  Array::Cast(*args[0]);
                    bytes = new Bytes(array);
                } else if (IS_BINARY(args[0])) {
                    // ByteString(byteString|byteArray)
                    Local<Object> obj = Object::Cast(*args[0]);
                    bytes = new Bytes(BYTES_FROM_BIN(obj));
                } else if (args[0]->IsExternal()) {
                    Bytes *otherBytes = reinterpret_cast<Bytes*>(External::Unwrap(args[0]));
                    bytes = new Bytes(otherBytes);
                } else{
                    return EXCEPTION("Not valid parameters")
                }
                break;
            case 2:
                // ByteString(string, charset)
                String::Utf8Value str(args[0]->ToString());
                String::Utf8Value charset(args[1]->ToString());
                bytes = new Bytes(str.length(), (uint8_t*)*str);
                break;
        }
        
		Persistent<Object> persistent_string = Persistent<Object>::New(string);
		persistent_string.MakeWeak(bytes, ExternalWeakCallback);
		persistent_string.MarkIndependent();
		
		string->SetPointerInInternalField(0, bytes);
		return string;
	}	
	
    namespace BinaryTypes {
	void Initialize(Handle<Object> internals) {
		HandleScope scope;
        
		// function Binary
		Local<FunctionTemplate> binary_t = FunctionTemplate::New(BinaryFunction);
        Local<ObjectTemplate> binary_ot = binary_t->PrototypeTemplate();
        binary_ot->SetAccessor(JS_STR("length"), BinaryLengthGetter);
        SET_METHOD(binary_ot, "toArray", BinaryToArray);
		internals->Set(JS_STR("Binary"), binary_t->GetFunction());
		
        // function ByteArray
		byteArray_t = Persistent<FunctionTemplate>::New(FunctionTemplate::New(ByteArray));
        Local<ObjectTemplate> byteArray_ot = byteArray_t->InstanceTemplate();
		byteArray_t->Inherit(binary_t);
        
        byteArray_ot->SetIndexedPropertyHandler(ByteArrayIndexedGetter, ByteArrayIndexedSetter);
        
        byteArray_ot->SetInternalFieldCount(1);
		
        byteArray = Persistent<Function>::New(byteArray_t->GetFunction());
		internals->Set(JS_STR("ByteArray"), byteArray);
        
        // function ByteString
        byteString_t = Persistent<FunctionTemplate>::New(FunctionTemplate::New(ByteString));
        Local<ObjectTemplate> byteString_ot = byteString_t->InstanceTemplate();
		byteString_t->Inherit(binary_t);
        
        byteString_ot->SetIndexedPropertyHandler(ByteStringIndexedGetter);
        SET_METHOD(byteString_ot, "codeAt", ByteStringCodeAt)
        SET_METHOD(byteString_ot, "decodeToString", ByteStringDecodeToString)
        byteString_ot->SetInternalFieldCount(1);
		
        byteString = Persistent<Function>::New(byteString_t->GetFunction());
		internals->Set(JS_STR("ByteString"), byteString);
		
	}
    }
	
}
