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
        
        NODE_SET_METHOD(target, "read", Read);
    }
    
    static Handle<Value> New(const Arguments &args) {
        HandleScope scope;
        Format *format = new Format();
        format->Wrap(args.This());
        return args.This();
    }
    
    static Handle<Value> Read(const Arguments &args) {
        HandleScope scope;
        Handle<Value> format = constructor_template->GetFunction()->New();
        return format;
    }
};

Persistent<FunctionTemplate> Format::constructor_template;

void init(Handle<Object> target) {
    HandleScope scope;
    Format::Initialize(target);
}

NODE_MODULE(format, init);
