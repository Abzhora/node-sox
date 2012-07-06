#ifndef NODE_SOX_SIGNAL
#define NODE_SOX_SIGNAL

#include <node.h>
#include <v8.h>
#include <node_buffer.h>

using namespace v8;
using namespace node;

#include <sox.h>

class Signal : public ObjectWrap {
public:
    static Persistent<FunctionTemplate> constructor_template;
    sox_signalinfo_t *signal;
    
    static void Initialize (Handle<Object> target) {
        Local<FunctionTemplate> t = FunctionTemplate::New(New);
        constructor_template = Persistent<FunctionTemplate>::New(t);
        constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
        constructor_template->SetClassName(String::NewSymbol("Signal"));
        
        target->Set(
            String::NewSymbol("Signal"),
            constructor_template->GetFunction()
        );
    }
    
    static Handle<Value> New(const Arguments &args) {
        HandleScope scope;
        Signal *sig = new Signal();
        sig->signal = (sox_signalinfo_t *) malloc(sizeof(sox_signalinfo_t));
        
        Handle<Object> opts = Handle<Object>::Cast(args[0]);
        
        sig->signal->rate = (double)
            opts->Get(String::NewSymbol("rate"))->NumberValue();
        sig->signal->channels = (unsigned)
            opts->Get(String::NewSymbol("channels"))->Int32Value();
        sig->signal->precision = (unsigned)
            opts->Get(String::NewSymbol("precision"))->Int32Value();
        sig->signal->length = (size_t)
            opts->Get(String::NewSymbol("length"))->Int32Value();
        
        sig->signal->mult = NULL;
        if (!opts->Get(String::NewSymbol("mult"))->IsUndefined()) {
            double * mult = (double *) malloc(sizeof(double));
            *mult = opts->Get(String::NewSymbol("mult"))->NumberValue();
            sig->signal->mult = mult;
        }
        
        sig->Wrap(args.This());
        return args.This();
    }
    
    static Handle<Value> Properties(sox_signalinfo_t sig) {
        Handle<Object> target = Object::New();
        target->Set(String::NewSymbol("rate"), Number::New(sig.rate));
        target->Set(String::NewSymbol("channels"), Int32::New(sig.channels));
        target->Set(String::NewSymbol("precision"), Int32::New(sig.precision));
        target->Set(String::NewSymbol("length"), Int32::New(sig.length));
        
        if (sig.mult == NULL) {
            target->Set(String::NewSymbol("mult"), Undefined());
        }
        else {
            target->Set(String::NewSymbol("mult"), Number::New(*sig.mult));
        }
        return target;
    }
};

Persistent<FunctionTemplate> Signal::constructor_template;

#endif
