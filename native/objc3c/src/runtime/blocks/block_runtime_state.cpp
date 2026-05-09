#include "runtime/blocks/block_runtime_state.h"

#include "runtime/blocks/block_debug_snapshot.h"

namespace objc3c::runtime {

RuntimeBlockDebugState &RuntimeBlockDebugStateForCurrentThread() {
  thread_local RuntimeBlockDebugState state;
  return state;
}

void ResetRuntimeBlockDebugStateForTesting() {
  ResetRuntimeBlockDebugState(RuntimeBlockDebugStateForCurrentThread());
}

void CopyRuntimeBlockFieldsToBlockArcSnapshot(
    objc3_runtime_block_arc_runtime_abi_snapshot *snapshot) {
  CopyRuntimeBlockDebugStateToBlockArcSnapshot(
      RuntimeBlockDebugStateForCurrentThread(), snapshot);
}

}  // namespace objc3c::runtime
