// largely derived from the sox examples

#include <node.h>
#include <v8.h>
#include <node_buffer.h>

using namespace v8;
using namespace node;

#include <sox.h>
#include <assert.h>

struct node_sox_request {
    sox_format_t *in, *out;
    sox_effects_chain_t *chain;
    Persistent<Function> ondata;
    Persistent<Function> onend;
    char *buffer;
    size_t buffer_size;
};
 
class Player : public ObjectWrap {
    static Persistent<FunctionTemplate> constructor_template;
    node_sox_request *request;
    
public:
    static void Initialize(Handle<Object> target) {
        Local<FunctionTemplate> t = FunctionTemplate::New(New);
        constructor_template = Persistent<FunctionTemplate>::New(t);
        constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
        constructor_template->SetClassName(String::NewSymbol("Player"));
        
        NODE_SET_PROTOTYPE_METHOD(constructor_template, "play", play);
        NODE_SET_PROTOTYPE_METHOD(constructor_template, "stop", stop);
        
        target->Set(
            String::NewSymbol("Player"),
            constructor_template->GetFunction()
        );
    }
    
    static Handle<Value> New(const Arguments &args) {
        HandleScope scope;
        Player *player = new Player();
        player->Wrap(args.This());
        return args.This();
    }
    
    static Handle<Value> stop(const Arguments &args) {
        HandleScope scope;
        Player *player = ObjectWrap::Unwrap<Player>(args.This());
        node_sox_request *req = player->request;
        
        eio_cancel((eio_req *) req);
        sox_delete_effects_chain(req->chain);
        sox_close(req->out);
        sox_close(req->in);
        
        return Undefined();
    }
    
    static Handle<Value> play(const Arguments &arguments) {
        HandleScope scope;
        Player *player = ObjectWrap::Unwrap<Player>(arguments.This());
        
        node_sox_request *req;
        req = (node_sox_request *) malloc(sizeof(node_sox_request));
        player->request = req;
        
        Local<Object> opts = Local<Object>::Cast(arguments[0]);
        req->onend = Persistent<Function>::New(
            Local<Function>::Cast(
                opts->Get(String::NewSymbol("onend"))
            )
        );
        
        sox_format_t *in, *out;
        
        in = sox_open_read("test.wav", NULL, NULL, NULL);
        assert(in);
        req->in = in;
        
        Local<Function> ondata_ = Local<Function>::Cast(
            opts->Get(String::NewSymbol("ondata"))
        );
        
        if (ondata_->IsUndefined()) {
            out = sox_open_write(
                "default", &in->signal, NULL, "alsa", NULL, NULL
            );
        }
        else {
            req->ondata = Persistent<Function>::New(ondata_);
            req->buffer = (char *) malloc(4096);
            req->buffer_size = 4096;
            
            out = sox_open_memstream_write(
                &req->buffer, &req->buffer_size, &in->signal,
                NULL, "sox", NULL
            );
        }
        
        assert(out);
        req->out = out;
        
        sox_effects_chain_t *chain;
        chain = sox_create_effects_chain(&in->encoding, &out->encoding);
        req->chain = chain;
        
        sox_effect_t *e;
        char * args[10];
        
        e = sox_create_effect(sox_find_effect("input"));
        args[0] = (char *) in;
        assert(sox_effect_options(e, 1, args) == SOX_SUCCESS);
        assert(sox_add_effect(chain, e, &in->signal, &in->signal) == SOX_SUCCESS);
        free(e);
        
        if (in->signal.rate != out->signal.rate) {
            e = sox_create_effect(sox_find_effect("rate"));
            assert(sox_effect_options(e, 0, NULL) == SOX_SUCCESS);
            assert(
                sox_add_effect(chain, e, &in->signal, &out->signal)
                == SOX_SUCCESS
            );
            free(e);
        }
        
        if (in->signal.channels != out->signal.channels) {
            e = sox_create_effect(sox_find_effect("channels"));
            assert(sox_effect_options(e, 0, NULL) == SOX_SUCCESS);
            assert(sox_add_effect(chain, e, &in->signal, &out->signal) == SOX_SUCCESS);
            free(e);
        }
        
        e = sox_create_effect(sox_find_effect("output"));
        args[0] = (char *) out;
        assert(sox_effect_options(e, 1, args) == SOX_SUCCESS);
        assert(sox_add_effect(chain, e, &in->signal, &out->signal) == SOX_SUCCESS);
        free(e);
        
        if (ondata_->IsUndefined()) {
            eio_custom(eio_flow, EIO_PRI_DEFAULT, eio_flow_after, req);
        }
        else {
            eio_custom(eio_rw, EIO_PRI_DEFAULT, eio_rw_after, req);
        }
        
        ev_ref(EV_DEFAULT_UC);
        player->Ref();
        
        return scope.Close(arguments.This());
    }
    
    static void eio_rw (eio_req *ereq) {
        sox_sample_t samples[4096];
        node_sox_request *req = (node_sox_request *) ereq->data;
        
        size_t number_read = sox_read(req->in, samples, (size_t) 4096);
        //req->buffer_size = number_read;
        
        if (number_read) {
            req->buffer_size = number_read;
            assert(sox_write(req->out, samples, number_read) == number_read);
        }
    }
    
    static int eio_rw_after (eio_req *ereq) {
        node_sox_request *req = (node_sox_request *) ereq->data;
        
        HandleScope scope;
        Local<Value> *argv;
        Buffer * buf;
        
        if (req->buffer_size == 0) {
            req->onend->Call(Context::GetCurrent()->Global(), 1, argv);
            
            req->onend.Dispose();
            free(req);
        }
        else {
            buf = Buffer::New(req->buffer, req->buffer_size);
            argv = (Local<Value> *) malloc(sizeof(Local<Value>));
            
            argv[1] = buf->handle_->ToObject();
            req->ondata->Call(Context::GetCurrent()->Global(), 2, argv);
            eio_custom(eio_rw, EIO_PRI_DEFAULT, eio_rw_after, req);
        }
        
        return 0;
    }
    
    static void eio_flow (eio_req *ereq) {
        node_sox_request *req = (node_sox_request *) ereq->data;
        
        sox_flow_effects(req->chain, NULL, NULL);
        sox_delete_effects_chain(req->chain);
        sox_close(req->out);
        sox_close(req->in);
    }
    
    static int eio_flow_after (eio_req *ereq) {
        ev_unref(EV_DEFAULT_UC);
        node_sox_request *req = (node_sox_request *) ereq->data;
        
        HandleScope scope;
        Local<Value> argv[0];
        
        req->onend->Call(Context::GetCurrent()->Global(), 1, argv);
        
        req->onend.Dispose();
        free(req);
        return 0;
    }
};

Persistent<FunctionTemplate> Player::constructor_template;

void init(Handle<Object> target) {
    sox_init();
    HandleScope scope;
    Player::Initialize(target);
}

NODE_MODULE(sox, init);
