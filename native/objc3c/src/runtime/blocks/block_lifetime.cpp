#include "runtime/blocks/block_lifetime.h"

#include "runtime/blocks/block_lifetime_records.h"
#include "runtime/state/runtime_state_records.h"

namespace objc3c::runtime {

bool RetainRuntimeBlockHandleUnlocked(RuntimeState &state, int block_handle) {
  return RetainRuntimeBlockRecord(
      FindRuntimeBlockRecordUnlocked(state, block_handle));
}

bool ReleaseRuntimeBlockHandleUnlocked(RuntimeState &state, int block_handle) {
  return ReleaseRuntimeBlockRecordUnlocked(state, block_handle);
}

}  // namespace objc3c::runtime
