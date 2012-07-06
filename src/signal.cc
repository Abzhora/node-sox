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
        
        sig->signal->rate = (double) args[0]->NumberValue();
        sig->signal->channels = (unsigned) args[1]->Int32Value();
        sig->signal->precision = (unsigned) args[2]->Int32Value();
        sig->signal->length = (size_t) args[3]->Int32Value();
        
        if (args[4]->IsUndefined()) {
            sig->signal->mult = NULL;
        }
        else {
            double * mult = (double *) malloc(sizeof(double));
            *mult = args[4]->NumberValue();
            sig->signal->mult = mult;
        }
        
        sig->Wrap(args.This());
        return args.This();
    }
    
};

Persistent<FunctionTemplate> Signal::constructor_template;

#endif
