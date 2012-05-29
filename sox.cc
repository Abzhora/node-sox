#include <node.h>
#include <v8.h>
using namespace v8;

#include <sox.h>

Handle<Value> hello(const Arguments& args) {
    HandleScope scope;
    
    sox_format_t *sfh = sox_open_read("test.wav", NULL, NULL, NULL);
    //sox_write(sfh, set_default_device);
    
    return scope.Close(String::New("world"));
}

void init(Handle<Object> target) {
    sox_format_init();
    NODE_SET_METHOD(target, "hello", hello);
}

NODE_MODULE(sox, init);
