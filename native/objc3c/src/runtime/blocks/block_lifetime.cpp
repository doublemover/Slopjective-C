#include "runtime/blocks/block_lifetime.h"

#include "runtime/blocks/block_lifetime_records.h"
#include "runtime/state/runtime_state_records.h"

namespace objc3c::runtime {

bool RetainRuntimeBlockHandleUnlocked(RuntimeState &state, int block_handle) {
  return RetainRuntimeBlockRecord(
      FindRuntimeBlockRecordUnlocked(state, block_handle));
}

bool ReleaseRuntimeBlockHandleUnlocked(
    RuntimeState &state, int block_handle,
    std::vector<RuntimeBlockRecord> *records_to_dispose) {
  return ReleaseRuntimeBlockRecordUnlocked(state, block_handle,
                                           records_to_dispose);
}

}  // namespace objc3c::runtime
