#include <node.h>
#include <v8.h>
using namespace v8;

#include <sox.h>

Handle<Value> hello(const Arguments& args) {
    HandleScope scope;
    
    sox_format_t *rh = sox_open_read("test.wav", NULL, NULL, NULL);
    sox_signalinfo_t signal;
    signal.rate = 48000;
    signal.channels = 2;
    signal.precision = 16;
    signal.length = 0;
    signal.mult = NULL;
    
    sox_encodinginfo_t ei;
    ei.encoding = SOX_ENCODING_SIGN2;
    ei.bits_per_sample = 0;
    ei.compression = 0;
    ei.reverse_bytes = SOX_OPTION_DEFAULT;
    ei.reverse_nibbles = SOX_OPTION_DEFAULT;
    ei.reverse_bits = SOX_OPTION_DEFAULT;
    ei.opposite_endian = sox_false;
    
    sox_format_t *wh = sox_open_write("alsa", &signal, &ei, "wav", NULL, false);
    
    return scope.Close(String::New("world"));
}

void init(Handle<Object> target) {
    sox_format_init();
    NODE_SET_METHOD(target, "hello", hello);
}

NODE_MODULE(sox, init);
