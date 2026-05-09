#include "runtime/memory/autorelease_pool_state.h"

namespace objc3c::runtime {

RuntimeAutoreleasePoolState &RuntimeAutoreleasePoolStateForCurrentThread() {
  thread_local RuntimeAutoreleasePoolState state;
  return state;
}

}  // namespace objc3c::runtime
