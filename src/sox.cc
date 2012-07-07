#include <node.h>
#include <v8.h>
#include <node_buffer.h>

using namespace v8;
using namespace node;

#include <sox.h>
#include "format.hpp"
#include "signal.hpp"
#include "effect.hpp"
#include "chain.hpp"

void init(Handle<Object> target) {
    assert(sox_init() == SOX_SUCCESS);
    HandleScope scope;
    Format::Initialize(target);
    Signal::Initialize(target);
    Effect::Initialize(target);
    Chain::Initialize(target);
}

NODE_MODULE(sox, init);
