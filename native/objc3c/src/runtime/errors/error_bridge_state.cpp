#include "runtime/errors/error_bridge_state.h"

namespace objc3c::runtime {

RuntimeErrorBridgeState &RuntimeErrorBridgeThreadState() {
  thread_local RuntimeErrorBridgeState state;
  return state;
}

void ResetRuntimeErrorBridgeThreadState() {
  RuntimeErrorBridgeThreadState() = RuntimeErrorBridgeState{};
}

}  // namespace objc3c::runtime
