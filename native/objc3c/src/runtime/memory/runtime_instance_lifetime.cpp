#include "runtime/memory/runtime_instance_lifetime.h"

#include "runtime/classes/receiver_identity.h"
#include "runtime/memory/arc_value_lifetime.h"
#include "runtime/memory/runtime_instance_destroy_plan.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/storage/instance_storage.h"
#include "runtime/storage/runtime_instance_records.h"
#include "runtime/storage/weak_slots.h"

#include <cstdint>
#include <utility>
#include <vector>

namespace objc3c::runtime {

int AllocateRuntimeInstanceUnlocked(RuntimeState &state,
                                    std::uint64_t base_identity) {
  const RealizedClassNode *node =
      FindRealizedClassNodeByBaseIdentityUnlocked(state, base_identity);
  if (node == nullptr) {
    return 0;
  }

  int receiver_identity = state.next_runtime_instance_receiver;
  while (receiver_identity <= 0 ||
         state.runtime_instances_by_receiver.find(receiver_identity) !=
             state.runtime_instances_by_receiver.end()) {
    ++receiver_identity;
  }
  state.next_runtime_instance_receiver = receiver_identity + 1;

  RuntimeInstanceRecord instance;
  instance.receiver_identity = static_cast<std::uint64_t>(receiver_identity);
  instance.base_identity = base_identity;
  instance.allocation_ordinal = state.next_runtime_instance_allocation_ordinal++;
  instance.class_name = node->class_name;
  instance.instance_size_bytes =
      RuntimeInstanceStorageSize(node->runtime_instance_size_bytes);
  const std::uint64_t instance_size_bytes =
      static_cast<std::uint64_t>(instance.instance_size_bytes);
  instance.storage_bytes.assign(instance.instance_size_bytes, 0u);
  instance.retain_count = 1u;

  state.last_allocated_runtime_instance_receiver =
      static_cast<std::uint64_t>(receiver_identity);
  state.last_allocated_runtime_instance_base_identity = base_identity;
  state.last_allocated_runtime_instance_size_bytes = instance_size_bytes;
  state.last_allocated_runtime_instance_allocation_ordinal =
      instance.allocation_ordinal;
  state.last_allocated_runtime_instance_class_name = node->class_name;
  state.runtime_instances_by_receiver.emplace(receiver_identity,
                                             std::move(instance));
  state.live_runtime_instance_count =
      static_cast<std::uint64_t>(state.runtime_instances_by_receiver.size());
  return receiver_identity;
}

void DestroyRuntimeInstanceUnlocked(RuntimeState &state, int receiver) {
  const auto instance_it = state.runtime_instances_by_receiver.find(receiver);
  if (instance_it == state.runtime_instances_by_receiver.end()) {
    return;
  }
  RuntimeInstanceRecord instance = std::move(instance_it->second);
  state.runtime_instances_by_receiver.erase(instance_it);
  state.live_runtime_instance_count =
      static_cast<std::uint64_t>(state.runtime_instances_by_receiver.size());

  ZeroWeakSlotRefsForTargetUnlocked(state, receiver);
  RemoveWeakSlotRefsOwnedByReceiverUnlocked(state, receiver);

  const std::vector<int> owned_values_to_release =
      RuntimeInstanceOwnedValuesToReleaseUnlocked(state, instance);
  for (int stored_value : owned_values_to_release) {
    ReleaseRuntimeValueUnlocked(state, stored_value);
  }
}

}  // namespace objc3c::runtime
