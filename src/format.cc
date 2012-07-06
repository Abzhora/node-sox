#include <node.h>
#include <v8.h>
#include <node_buffer.h>

using namespace v8;
using namespace node;

#include <sox.h>

class Format : public ObjectWrap {
public:
    static Persistent<FunctionTemplate> constructor_template;
    sox_format_t *format;
    
    static void Initialize (Handle<Object> target) {
        Local<FunctionTemplate> t = FunctionTemplate::New(New);
        constructor_template = Persistent<FunctionTemplate>::New(t);
        constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
        constructor_template->SetClassName(String::NewSymbol("Format"));
        
        target->Set(
            String::NewSymbol("Format"),
            constructor_template->GetFunction()
        );
        
        NODE_SET_METHOD(target, "openRead", OpenRead);
    }
    
    static Handle<Value> New(const Arguments &args) {
        HandleScope scope;
        Format *format = new Format();
        format->Wrap(args.This());
        return args.This();
    }
    
    static Handle<Value> OpenRead(const Arguments &args) {
        HandleScope scope;
        Handle<Object> h = constructor_template->GetFunction()->New();
        Format *obj = ObjectWrap::Unwrap<Format>(h);
        
        String::Utf8Value path(args[0]);
        obj->format = sox_open_read(*path, NULL, NULL, NULL);
        
        return h;
    }
};

Persistent<FunctionTemplate> Format::constructor_template;

void init(Handle<Object> target) {
    HandleScope scope;
    Format::Initialize(target);
}

NODE_MODULE(format, init);
