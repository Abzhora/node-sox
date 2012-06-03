// largely derived from the sox examples

#include <node.h>
#include <v8.h>
using namespace v8;

#include <sox.h>
#include <assert.h>

struct node_sox_request {
    sox_format_t *in, *out;
    sox_effects_chain_t *chain;
    Persistent<Function> onend;
};

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

Handle<Value> play(const Arguments& arguments) {
    HandleScope scope;
    
    node_sox_request *req;
    req = (node_sox_request *) malloc(sizeof(node_sox_request));
    
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
    
    out = sox_open_write("default", &in->signal, NULL, "alsa", NULL, NULL);
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
    
    eio_custom(eio_flow, EIO_PRI_DEFAULT, eio_flow_after, req);
    ev_ref(EV_DEFAULT_UC);
    
    Persistent<Object> player = Persistent<Object>::New(Object::New());
    player->Set(String::New("x"), Integer::New(5));
    return scope.Close(player);
}

void init(Handle<Object> target) {
    sox_init();
    NODE_SET_METHOD(target, "play", play);
}

NODE_MODULE(sox, init);
