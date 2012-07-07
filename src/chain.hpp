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
        NODE_SET_PROTOTYPE_METHOD(constructor_template, "flow", Flow);
        
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
    
    static Handle<Value> Flow(const Arguments &args) {
        HandleScope scope;
        Chain *c = ObjectWrap::Unwrap<Chain>(args.This());
        void *data = (void *) c->chain;
        eio_custom(eio_flow, EIO_PRI_DEFAULT, eio_flow_after, data);
        ev_ref(EV_DEFAULT_UC);
        c->Ref();
        return Undefined();
    }
    
    static void eio_flow (eio_req *ereq) {
        sox_effects_chain_t *chain = (sox_effects_chain_t *) ereq->data;
        sox_flow_effects(chain, NULL, NULL);
        sox_delete_effects_chain(chain);
    }
    
    static int eio_flow_after (eio_req *ereq) {
        HandleScope scope;
        ev_unref(EV_DEFAULT_UC);
        return 0;
    }
};

Persistent<FunctionTemplate> Chain::constructor_template;

#endif
