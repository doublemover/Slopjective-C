#include "runtime/state/runtime_instance_state_clear.h"

#include "runtime/state/runtime_state_records.h"

namespace objc3c::runtime {

void ClearRuntimeInstanceStateUnlocked(RuntimeState &state) {
  state.runtime_instances_by_receiver.clear();
  state.runtime_blocks_by_handle.clear();
  state.weak_slot_refs_by_target_receiver.clear();
  state.next_runtime_instance_receiver = 0x100000;
  state.next_runtime_block_handle = 0x200000;
  state.live_runtime_instance_count = 0;
  state.last_allocated_runtime_instance_receiver = 0;
  state.last_allocated_runtime_instance_base_identity = 0;
  state.last_allocated_runtime_instance_size_bytes = 0;
  state.last_allocated_runtime_instance_class_name.clear();
}

}  // namespace objc3c::runtime
