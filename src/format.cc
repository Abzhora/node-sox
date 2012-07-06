#ifndef NODE_SOX_FORMAT
#define NODE_SOX_FORMAT

#include <node.h>
#include <v8.h>
#include <node_buffer.h>

using namespace v8;
using namespace node;

#include <sox.h>
#include "signal.hpp"

class Format : public ObjectWrap {
public:
    static Persistent<FunctionTemplate> constructor_template;
    sox_format_t *format;
    
    static void Initialize (Handle<Object> target) {
        Local<FunctionTemplate> t = FunctionTemplate::New(New);
        constructor_template = Persistent<FunctionTemplate>::New(t);
        constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
        constructor_template->SetClassName(String::NewSymbol("Format"));
        
        NODE_SET_PROTOTYPE_METHOD(
            constructor_template, "readFile", ReadFile
        );
        NODE_SET_PROTOTYPE_METHOD(
            constructor_template, "getSignal", GetSignal
        );
        NODE_SET_PROTOTYPE_METHOD(
            constructor_template, "writeFile", WriteFile
        );
        NODE_SET_PROTOTYPE_METHOD(constructor_template, "play", Play);
        NODE_SET_PROTOTYPE_METHOD(
            constructor_template, "memstreamWrite", MemstreamWrite
        );
        
        target->Set(
            String::NewSymbol("Format"),
            constructor_template->GetFunction()
        );
    }
    
    static Handle<Value> New(const Arguments &args) {
        HandleScope scope;
        Format *format = new Format();
        format->Wrap(args.This());
        return args.This();
    }
    
    static Handle<Value> ReadFile(const Arguments &args) {
        HandleScope scope;
        Format *fmt = ObjectWrap::Unwrap<Format>(args.This());
        String::Utf8Value path(args[0]);
        fmt->format = sox_open_read(*path, NULL, NULL, NULL);
        fmt->Ref();
        
        return Undefined();
    }
    
    static Handle<Value> GetSignal(const Arguments &args) {
        HandleScope scope;
        Format *fmt = ObjectWrap::Unwrap<Format>(args.This());
        return Signal::Properties(fmt->format->signal);
    }
    
    static Handle<Value> WriteFile(const Arguments &args) {
        HandleScope scope;
        Format *fmt = ObjectWrap::Unwrap<Format>(args.This());
        
        String::Utf8Value path(args[0]);
        Signal *signal = ObjectWrap::Unwrap<Signal>(
            Handle<Object>::Cast(args[1])
        );
        
        fmt->format = sox_open_write(
            *path, signal->signal, NULL, NULL, NULL, NULL
        );
        fmt->Ref();
        
        return Undefined();
    }
    
    static Handle<Value> Play(const Arguments &args) {
        HandleScope scope;
        Format *fmt = ObjectWrap::Unwrap<Format>(args.This());
        
        Signal *signal = ObjectWrap::Unwrap<Signal>(
            Handle<Object>::Cast(args[0])
        );
        const char * dev;
        if (args[1]->IsUndefined()) {
            dev = "alsa";
        }
        else {
            dev = *String::Utf8Value(args[1]);
        }
        
        fmt->format = sox_open_write(
            "default", signal->signal, NULL, dev, NULL, NULL
        );
        fmt->Ref();
        
        return Undefined();
    }
    
    static Handle<Value> MemstreamWrite (const Arguments &args) {
        return Undefined();
    }
};

Persistent<FunctionTemplate> Format::constructor_template;

void init(Handle<Object> target) {
    HandleScope scope;
    Format::Initialize(target);
    Signal::Initialize(target);
}

NODE_MODULE(format, init);

#endif
