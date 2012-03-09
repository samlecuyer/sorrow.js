/*
 copyright 2012 sam l'ecuyer
 */

#include "sorrow.h"

namespace sorrow {
	using namespace v8;
	
    Persistent<Function> byteString_f;
    Persistent<Function> byteArray_f;

    
	Handle<Value> BinaryFunction(const Arguments& args) {
		return ThrowException(String::New("Binary is non-instantiable"));
	}
    
	void ExternalWeakCallback(Persistent<Value> object, void* data) {
		free(data);
		object.Dispose();
	}
	
	const char isByteArrayPropName[] = "_is_byte_array_";
	const char isByteStringPropName[] = "_is_byte_string_";
    
    Handle<Value> ByteStringCodeAt(const Arguments& args) {
        HandleScope scope;
        Local<Object> byteString = args.This();
        uint64_t index = args[0]->IntegerValue();
        uint64_t byteString_len = byteString->Get(String::New("length"))->IntegerValue();
        if (index < 0 || index >= byteString_len) {
            return Undefined();
        }
        uint8_t *data = (uint8_t*)byteString->GetPointerFromInternalField(0);
        return Number::New(data[index]);
    }
    
    Handle<Value> ByteStringIndexedGetter(uint32_t index, const AccessorInfo &info) {
        HandleScope scope;
        Handle<Object> byteString = info.This();
        uint64_t byteString_len = byteString->Get(String::New("length"))->IntegerValue();
        if (index < 0 || index >= byteString_len) {
            return Undefined();
        }
        
        Local<Object> clone = byteString->Clone();
        int size = 1;
        uint8_t *data = new uint8_t[size];
        
        uint8_t *orig_data = (uint8_t*)byteString->GetPointerFromInternalField(0);
        data[0] = orig_data[index];
        
        Persistent<Object> persistent_string = Persistent<Object>::New(clone);
		persistent_string.MakeWeak(data, ExternalWeakCallback);
		persistent_string.MarkIndependent();
		
		clone->SetPointerInInternalField(0, data);
        clone->ForceSet(String::New("length"), Integer::New(size), ReadOnly);
		return scope.Close(clone);
    }

    
    Handle<Value> ByteString(const Arguments& args) {
        Handle<Object> string = args.This();
		uint64_t size;
		void *data;
		
		if (args.Length() == 0) {
			// ByteString()
			size = 0;
			data = calloc(size, sizeof(uint8_t));
			
		} else if (args[0]->IsExternal() &&
                   args[1]->IsNumber()) {
			// ByteString([buffer], size)
			size = args[1]->IntegerValue();
            uint8_t *other_data = (uint8_t*)External::Unwrap(args[0]);
			data = calloc(size, sizeof(uint8_t));
			memcpy(data, other_data, size);
            
        } else if (args[0]->IsNumber()) {
			// ByteString(length)
			size = args[0]->IntegerValue();
			data = calloc(size, sizeof(uint8_t));
			
		} else if (args[0]->IsObject() &&
				   args[0]->ToObject()->Get(String::New(isByteArrayPropName))->IsTrue()) {
			// ByteString(byteArray)
			size = args[0]->ToObject()->GetIndexedPropertiesExternalArrayDataLength();
			void *other_data = args[0]->ToObject()->GetIndexedPropertiesExternalArrayData();
			data = calloc(size, sizeof(uint8_t));
			memcpy(data, other_data, size);
            
		} else if (args[0]->IsObject() &&
				   args[0]->ToObject()->Get(String::New(isByteStringPropName))->IsTrue()) {
			// ByteString(byteString)
			size = args[0]->ToObject()->Get(String::New("length"))->IntegerValue();
			void *other_data = args[0]->ToObject()->GetPointerFromInternalField(0);
			data = calloc(size, sizeof(uint8_t));
			memcpy(data, other_data, size);
			
		} else if (args[0]->IsArray()) {
			// ByteString(arrayOfBytes)
			HandleScope scope;
			Handle<Array> arrayOfBytes = Handle<Array>::Cast(args[0]);
			size = arrayOfBytes->Length();
			data = calloc(size, sizeof(uint8_t));
			for (int32_t i = 0; i < size; i++) {
				Local<Value> elem = arrayOfBytes->Get(i);
				if (*elem == NULL) {
					return ThrowException(String::New("Error copying array"));
				}
				if (!elem->IsNumber()) {
					return ThrowException(String::New("All elements must be a number"));
				}
				int val = elem->Int32Value();
				if (val > 255 || val < 0) {
					return ThrowException(String::New("All elements must be a number between 0 and 255"));
				}
				((uint8_t*)data)[i] = val;
			}
			
		} else if (args.Length() == 2 && args[0]->IsString() && args[1]->IsString()) {
			// ByteString(string, charset)
			HandleScope scope;
			if (args[1]->Equals(String::New("us-ascii"))) {
				String::AsciiValue val(args[0]);
				size = val.length();
				data = calloc(size, sizeof(uint8_t));
				memcpy(data, *val, size);
			} else if (args[1]->Equals(String::New("utf-8"))) {
				String::Utf8Value val(args[0]);
				size = val.length();
				data = calloc(size, sizeof(uint8_t));
				memcpy(data, *val, size);
			} else {
				return ThrowException(String::New("Unsupported charset"));
			}
		} else {
			return ThrowException(String::New("This is not a valid constructor call"));
		}
		
		Persistent<Object> persistent_string = Persistent<Object>::New(string);
		persistent_string.MakeWeak(data, ExternalWeakCallback);
		persistent_string.MarkIndependent();
		
		string->SetPointerInInternalField(0, data);
        string->ForceSet(String::New("length"), Integer::New(size), ReadOnly);
		string->ForceSet(String::New(isByteStringPropName), True(), ReadOnly);
		return string;
    }
	
	Handle<Value> ByteArray(const Arguments& args) {
		Handle<Object> array = args.This();
		int64_t size;
		void *data;
		
		if (args.Length() == 0) {
			// ByteArray()
			size = 0;
			data = calloc(size, sizeof(uint8_t));
			
		} else if (args[0]->IsNumber()) {
			// ByteArray(length)
			size = args[0]->IntegerValue();
			data = calloc(size, sizeof(uint8_t));
			
		} else if (args[0]->IsObject() &&
				   args[0]->ToObject()->Get(String::New(isByteArrayPropName))->IsTrue()) {
			// ByteArray(byteArray)
			size = args[0]->ToObject()->GetIndexedPropertiesExternalArrayDataLength();
			void *other_data = args[0]->ToObject()->GetIndexedPropertiesExternalArrayData();
			data = calloc(size, sizeof(uint8_t));
			
			memcpy(data, other_data, size);
		} else if (args[0]->IsObject() &&
				   args[0]->ToObject()->Get(String::New(isByteStringPropName))->IsTrue()) {
			// ByteArray(byteString)
			size = args[0]->ToObject()->Get(String::New("length"))->IntegerValue();
			void *other_data = args[0]->ToObject()->GetPointerFromInternalField(0);
			data = calloc(size, sizeof(uint8_t));
			memcpy(data, other_data, size);
			
		} else if (args[0]->IsArray()) {
			// ByteArray(arrayOfBytes)
			HandleScope scope;
			Handle<Array> arrayOfBytes = Handle<Array>::Cast(args[0]);
			size = arrayOfBytes->Length();
			data = calloc(size, sizeof(uint8_t));
			for (int32_t i = 0; i < size; i++) {
				Local<Value> elem = arrayOfBytes->Get(i);
				if (*elem == NULL) {
					return ThrowException(String::New("Error copying array"));
				}
				if (!elem->IsNumber()) {
					return ThrowException(String::New("All elements must be a number"));
				}
				int val = elem->Int32Value();
				if (val > 255 || val < 0) {
					return ThrowException(String::New("All elements must be a number between 0 and 255"));
				}
				((uint8_t*)data)[i] = val;
			}
			
		} else if (args.Length() == 2 && args[0]->IsString() && args[1]->IsString()) {
			// ByteArray(string, charset)
			HandleScope scope;
			if (args[1]->Equals(String::New("us-ascii"))) {
				String::AsciiValue val(args[0]);
				size = val.length();
				data = calloc(size, sizeof(uint8_t));
				memcpy(data, *val, size);
			} else if (args[1]->Equals(String::New("utf-8"))) {
				String::Utf8Value val(args[0]);
				size = val.length();
				data = calloc(size, sizeof(uint8_t));
				memcpy(data, *val, size);
			} else {
				return ThrowException(String::New("Unsupported charset"));
			}
		} else {
			return ThrowException(String::New("This is not a valid constructor call"));
		}

		
		Persistent<Object> persistent_array = Persistent<Object>::New(array);
		persistent_array.MakeWeak(data, ExternalWeakCallback);
		persistent_array.MarkIndependent();
		
		array->SetIndexedPropertiesToExternalArrayData(data, kExternalUnsignedByteArray, size);
		array->Set(String::New(isByteArrayPropName), True(), ReadOnly);
		array->Set(String::New("length"), Integer::New(size), ReadOnly);
		return array;
	}	
	
	void SetupBinaryTypes(Handle<Object> internals) {
		HandleScope scope;
		// function Binary
		Local<FunctionTemplate> binary_t = FunctionTemplate::New(BinaryFunction);
		internals->Set(String::New("Binary"), binary_t->GetFunction());
		
        // function ByteArray
		Local<FunctionTemplate> byteArray_t = FunctionTemplate::New(ByteArray);
		byteArray_t->Inherit(binary_t);
		
        byteArray_f = Persistent<Function>::New(byteArray_t->GetFunction());
		internals->Set(String::New("ByteArray"), byteArray_f);
        
        // function ByteString
        Local<FunctionTemplate> byteString_t = FunctionTemplate::New(ByteString);
        Local<ObjectTemplate> byteString_ot = byteString_t->InstanceTemplate();
		byteString_t->Inherit(binary_t);
        
        byteString_ot->SetIndexedPropertyHandler(ByteStringIndexedGetter);
        byteString_ot->Set(String::New("codeAt"), FunctionTemplate::New(ByteStringCodeAt)->GetFunction());
        byteString_ot->SetInternalFieldCount(1);
		
        byteString_f = Persistent<Function>::New(byteString_t->GetFunction());
		internals->Set(String::New("ByteString"), byteString_f);
		
	}
	
}
