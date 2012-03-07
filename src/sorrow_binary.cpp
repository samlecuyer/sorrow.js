/*
 copyright 2012 sam l'ecuyer
 */


#include <v8.h>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>

namespace sorrow {
	using namespace v8;
	
	Handle<Value> BinaryFunction(const Arguments& args) {
		return ThrowException(String::New("Binary is non-instantiable"));
	}
	
	void ExternalArrayWeakCallback(Persistent<Value> object, void* data) {
		free(data);
		object.Dispose();
	}
	
	const char isByteArrayPropName[] = "_is_byte_array_";
	const char isByteStringPropName[] = "_is_byte_string_";
	
	Handle<Value> ByteArray(const Arguments& args) {
		Handle<Object> array = args.This();
		
		// ByteArray()
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
			size = args[0]->ToObject()->GetIndexedPropertiesExternalArrayDataLength();
			void *other_data = args[0]->ToObject()->GetIndexedPropertiesExternalArrayData();
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
		persistent_array.MakeWeak(data, ExternalArrayWeakCallback);
		persistent_array.MarkIndependent();
		
		array->SetIndexedPropertiesToExternalArrayData(data, kExternalUnsignedByteArray, size);
		array->Set(String::New(isByteArrayPropName), True(), ReadOnly);
		array->Set(String::New("length"), Integer::New(size), ReadOnly);
		
		return array;
	}	
	
	void BinaryTypes(Handle<Object> internals) {
		HandleScope scope;
		
		Local<FunctionTemplate> binary_t = FunctionTemplate::New(BinaryFunction);
		internals->Set(String::New("Binary"), binary_t->GetFunction());
		
		Local<FunctionTemplate> byteArray_t = FunctionTemplate::New(ByteArray);
		byteArray_t->Inherit(binary_t);
		
		internals->Set(String::New("ByteArray"), byteArray_t->GetFunction());
		
	}
	
}
