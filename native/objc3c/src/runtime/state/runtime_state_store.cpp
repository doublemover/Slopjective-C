#include "runtime/state/runtime_state_store.h"

namespace objc3c::runtime {

RuntimeState &ProcessRuntimeState() {
  static RuntimeState state;
  return state;
}

}  // namespace objc3c::runtime
