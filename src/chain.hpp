#ifndef NODE_SOX_CHAIN
#define NODE_SOX_CHAIN

#include <node.h>
#include <v8.h>
#include <node_buffer.h>

using namespace v8;
using namespace node;

#include <sox.h>
#include "format.hpp"

class Chain : public ObjectWrap {
public:
    static Persistent<FunctionTemplate> constructor_template;
    sox_effects_chain_t *chain;
    
    static void Initialize (Handle<Object> target) {
        Local<FunctionTemplate> t = FunctionTemplate::New(New);
        constructor_template = Persistent<FunctionTemplate>::New(t);
        constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
        constructor_template->SetClassName(String::NewSymbol("Chain"));
        NODE_SET_PROTOTYPE_METHOD(constructor_template, "addEffect", AddEffect);
        
        target->Set(
            String::NewSymbol("Chain"),
            constructor_template->GetFunction()
        );
    }
    
    static Handle<Value> New(const Arguments &args) {
        HandleScope scope;
        
        Format *in, *out;
        in = ObjectWrap::Unwrap<Format>(Handle<Object>::Cast(args[0]));
        out = ObjectWrap::Unwrap<Format>(Handle<Object>::Cast(args[1]));
        
        Chain *c = new Chain();
        c->chain = sox_create_effects_chain(
            &(in->format->encoding), &(out->format->encoding)
        );
        
        c->Wrap(args.This());
        return args.This();
    }
    
    static Handle<Value> AddEffect(const Arguments &args) {
        HandleScope scope;
        Chain *c = ObjectWrap::Unwrap<Chain>(args.This());
        Effect *e = ObjectWrap::Unwrap<Effect>(Handle<Object>::Cast(args[0]));
        
        Format *in, *out;
        in = ObjectWrap::Unwrap<Format>(Handle<Object>::Cast(args[1]));
        out = ObjectWrap::Unwrap<Format>(Handle<Object>::Cast(args[2]));
        
        assert(sox_add_effect(
            c->chain, e->effect, &in->format->signal, &out->format->signal
        ) == SOX_SUCCESS);
        return args.This();
    }
};

Persistent<FunctionTemplate> Chain::constructor_template;

#endif
