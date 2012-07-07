#ifndef NODE_SOX_EFFECT
#define NODE_SOX_EFFECT

#include <node.h>
#include <v8.h>
#include <node_buffer.h>

using namespace v8;
using namespace node;

#include <sox.h>

class Effect : public ObjectWrap {
public:
    static Persistent<FunctionTemplate> constructor_template;
    sox_effect_t *effect;
    
    static void Initialize (Handle<Object> target) {
        Local<FunctionTemplate> t = FunctionTemplate::New(New);
        constructor_template = Persistent<FunctionTemplate>::New(t);
        constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
        constructor_template->SetClassName(String::NewSymbol("Effect"));
        
        target->Set(
            String::NewSymbol("Effect"),
            constructor_template->GetFunction()
        );
    }
    
    static Handle<Value> New(const Arguments &args) {
        HandleScope scope;
        Effect *e = new Effect();
        String::Utf8Value name(args[0]);
        e->effect = sox_create_effect(sox_find_effect(*name));
        
        Local<Array> xs;
        char * argv[10];
        
        if (args[1]->IsArray()) {
            xs = Local<Array>::Cast(args[1]);
        }
        else {
            xs = Array::New();
        }
        
        for (unsigned int i = 0; i < 10 && i < xs->Length(); i++) {
            argv[i] = *String::Utf8Value(xs->Get(Integer::New(i)));
        }
        
        sox_effect_options(e->effect, xs->Length(), argv);
        
        e->Wrap(args.This());
        return args.This();
    }
};

Persistent<FunctionTemplate> Effect::constructor_template;

#endif
