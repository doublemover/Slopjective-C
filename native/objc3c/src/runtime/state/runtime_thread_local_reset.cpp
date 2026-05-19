#include "runtime/state/runtime_thread_local_reset.h"

#include "runtime/blocks/block_runtime_state.h"
#include "runtime/concurrency/runtime_debug.h"
#include "runtime/errors/error_bridge.h"
#include "runtime/memory/arc_debug_state.h"
#include "runtime/memory/autorelease_pool.h"

namespace objc3c::runtime {

void ResetRuntimeThreadLocalDebugStateForTesting() {
  ResetRuntimeAutoreleasepoolStateForTesting();
  ResetRuntimeArcDebugStateForTesting();
  ResetRuntimeBlockDebugStateForTesting();
  ResetRuntimeErrorBridgeStateForTesting();
  ResetRuntimeConcurrencyDebugStateForTesting();
}

}  // namespace objc3c::runtime
