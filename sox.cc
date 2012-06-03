// largely derived from the sox examples

#include <node.h>
#include <v8.h>
using namespace v8;

#include <sox.h>
#include <assert.h>

Handle<Value> hello(const Arguments& arguments) {
    HandleScope scope;
    
    sox_format_t *in, *out;
    in = sox_open_read("test.wav", NULL, NULL, NULL);
    assert(in);
    
    out = sox_open_write("default", &in->signal, NULL, "alsa", NULL, NULL);
    assert(out);
    
    sox_effects_chain_t *chain;
    chain = sox_create_effects_chain(&in->encoding, &out->encoding);
    
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
    
    sox_flow_effects(chain, NULL, NULL);
    sox_delete_effects_chain(chain);
    sox_close(out);
    sox_close(in);
    
    return scope.Close(String::New("world"));
}

void init(Handle<Object> target) {
    sox_init();
    NODE_SET_METHOD(target, "hello", hello);
}

NODE_MODULE(sox, init);
