#include "runtime/memory/arc_value_lifetime.h"

#include "runtime/blocks/block_lifetime.h"
#include "runtime/memory/runtime_instance_lifetime.h"
#include "runtime/state/runtime_state_records.h"

namespace objc3c::runtime {

bool RuntimeArcValueIsRetainable(int value) {
  return value != 0;
}

void RetainRuntimeValueUnlocked(RuntimeState &state, int value) {
  if (!RuntimeArcValueIsRetainable(value)) {
    return;
  }
  const auto instance_it = state.runtime_instances_by_receiver.find(value);
  if (instance_it != state.runtime_instances_by_receiver.end()) {
    ++instance_it->second.retain_count;
    return;
  }
  (void)RetainRuntimeBlockHandleUnlocked(state, value);
}

void ReleaseRuntimeValueUnlocked(
    RuntimeState &state, int value,
    std::vector<RuntimeBlockRecord> *records_to_dispose) {
  if (!RuntimeArcValueIsRetainable(value)) {
    return;
  }
  const auto instance_it = state.runtime_instances_by_receiver.find(value);
  if (instance_it != state.runtime_instances_by_receiver.end()) {
    if (instance_it->second.retain_count > 1u) {
      --instance_it->second.retain_count;
      return;
    }
    DestroyRuntimeInstanceUnlocked(state, value, records_to_dispose);
    return;
  }
  (void)ReleaseRuntimeBlockHandleUnlocked(state, value, records_to_dispose);
}

}  // namespace objc3c::runtime
