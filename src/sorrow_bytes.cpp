#include "sorrow.h"
#include "sorrow_bytes.h"

#include <iconv.h>
#include <errno.h>

namespace sorrow {
    using namespace v8;

    Bytes::Bytes(): 
        len(0), bytes(NULL) {}

    Bytes::Bytes(size_t l): 
        len(0), bytes(NULL) {
        resize(l, true);
    }
    
    Bytes::Bytes(size_t l, uint8_t *data): 
        len(0), bytes(NULL) {
            //printf("constructor: %s", (char*)data);
            resize(l, false);
            memcpy(this->bytes, data, l);
    }
    
    Bytes::Bytes(Bytes *b): 
        len(0), bytes(NULL) {
            resize(b->getLength(), false);
            memcpy(this->bytes, b->getBytes(), this->len);
    }
    
    Bytes::Bytes(Handle<Array> array): 
        len(0), bytes(NULL) {
            resize(array->Length(), false);
            for (int i = 0; i < array->Length(); i++) {
                uint32_t val = array->Get(i)->IntegerValue();
                if (!IS_BYTE(val)) throw "NonByte";
                this->setByteAt(i, val);
            }
    }

    Bytes::~Bytes() {
        free(this->bytes);
    }
    
    uint8_t *Bytes::getBytes() {
        return this->bytes;
    }

    size_t Bytes::getLength() {
        return this->len;
    }

    uint8_t Bytes::getByteAt(size_t i) {
        return this->bytes[i];
    }

    void Bytes::setByteAt(size_t i, uint8_t b) {
        this->bytes[i] = b;
    }

    void Bytes::resize(size_t nl, bool zero) {
        if (!nl) {
            free(this->bytes);
            this->bytes = NULL;
        } else {
            this->bytes = (uint8_t*)realloc(this->bytes, nl);
            if (zero && nl > this->len) { 
                memset(this->bytes + this->len, 0, nl-this->len);
            }
        }
        this->len = nl;
    }
    
    
    Bytes *Bytes::concat(const Arguments &args) {
        size_t totalSize = this->len;
        for (int i = 0; i < args.Length(); i++) {
            if (IS_BINARY(args[i])) {
                totalSize += BYTES_FROM_BIN(Object::Cast(*args[i]))->getLength();
            } else if (args[0]->IsArray()){
                totalSize += Array::Cast(*args[i])->Length();
            }
        }
        uint8_t *newBytes = (uint8_t*)malloc(totalSize);
        
        memcpy(newBytes, this->bytes, this->len);
        uint8_t *nca = newBytes + this->len;
        for (int i = 0; i < args.Length(); i++) {
            if (IS_BINARY(args[i])) {
                Bytes *toCopy = BYTES_FROM_BIN(Object::Cast(*args[i]));
                memcpy(nca, toCopy->getBytes(), toCopy->getLength());
                nca += toCopy->getLength();
            } else if (args[i]->IsArray()){
                Local<Array> array = Array::Cast(*args[i]);
                for (int i = 0; i < array->Length(); i++) {
                    uint32_t val = array->Get(i)->Uint32Value();
                    if (!IS_BYTE(val)) throw "NonByte";
                    *(nca++) = val;
                }
            }
        }
        Bytes *ret = new Bytes(totalSize, newBytes);
        free(newBytes);
        return ret;
    }
    
#define CLEANUP_CONVERSION(b,c,m) free(b); iconv_close(c); throw m;
    Bytes *Bytes::transcode(const char* source, const char* target) {
        if (this->len == 0) {
            return new Bytes();
        }
        if ((source == target) || strcasecmp(source, target) == 0) {
            return new Bytes(this);
        }
        iconv_t converter;
        converter = iconv_open(source, target);
        if (converter == iconv_t(-1)) {
            throw "Unsupported charset";
        }
        
        size_t size = this->len;
        uint8_t *newBytes = (uint8_t*)malloc(size);
        
        size_t inBytesLeft = this->len;
        size_t outBytesLeft = size;
        char *inBuf = (char *)this->bytes;
        char *outBuf = (char *)newBytes;
        
        size_t result = 0;
        do {
            result = iconv(converter, &inBuf, &inBytesLeft, &outBuf, &outBytesLeft);
            if (result == (size_t)(-1)) {
                switch (errno) {
                    case EILSEQ:
                        CLEANUP_CONVERSION(newBytes, converter, 
                        "Transcoding stopped due to an input byte that does not belong to the input codeset.")
                    case EINVAL:
                        CLEANUP_CONVERSION(newBytes, converter, 
                        "Transcoding stopped due to an incomplete character or shift sequence at the end of the input buffer.")
                    case E2BIG: {
                        size_t newSize = size + (this->len)/4;
                        uint8_t *newerBytes = (uint8_t*)realloc(newBytes, newSize);
                        if (newerBytes == NULL) {
                            CLEANUP_CONVERSION(newBytes, converter, 
                            "Could not allocate enough memory to perform transcoding")
                        }
                        outBuf = (char*)newerBytes + (outBuf - (char*)newBytes);
                        newBytes = newerBytes;
                        outBytesLeft += newSize - size;
                        size = newSize;
                    } break;
                    default:
                        CLEANUP_CONVERSION(newBytes, converter, "Conversion failed")
                }
            }
        } while (result == -1);
        iconv_close(converter);
        
        size_t len = outBuf - (char *)newBytes;
        Bytes * bs = new Bytes(len, newBytes);
        
        free(newBytes);
        return bs;
    }
    
    Handle<Array>Bytes::toArray() {
        HandleScope scope;
        Handle<Array> array = Array::New(this->len);
        for (int i = 0; i < this->len; i++) {
            uint32_t val = array->Set(i, Integer::New(this->getByteAt(i)));
        }
        return scope.Close(array);
    }
}
