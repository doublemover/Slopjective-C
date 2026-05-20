#include "runtime/memory/runtime_instance_lifetime.h"

#include "runtime/classes/receiver_identity.h"
#include "runtime/memory/arc_value_lifetime.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/storage/instance_storage.h"
#include "runtime/storage/property_accessors.h"
#include "runtime/storage/runtime_instance_records.h"
#include "runtime/storage/weak_slots.h"

#include <algorithm>
#include <cstdint>
#include <utility>
#include <vector>

namespace objc3c::runtime {

namespace {

std::size_t RuntimeInstanceStorageFloorForClassUnlocked(
    const RuntimeState &state,
    const RealizedClassNode &node) {
  std::size_t storage_size_bytes =
      node.runtime_layout_ready ? node.runtime_instance_size_bytes : 0u;
  const RealizedClassNode *cursor = &node;
  std::size_t visited_count = 0;
  while (cursor->has_super_node &&
         cursor->super_node_index < state.realized_class_nodes.size() &&
         visited_count < state.realized_class_nodes.size()) {
    const RealizedClassNode &super_node =
        state.realized_class_nodes[cursor->super_node_index];
    if (super_node.runtime_layout_ready) {
      storage_size_bytes =
          std::max(storage_size_bytes, super_node.runtime_instance_size_bytes);
    }
    cursor = &super_node;
    ++visited_count;
  }
  return storage_size_bytes;
}

void AppendRuntimeInstanceOwnedValuesForNodeUnlocked(
    const RuntimeInstanceRecord &instance,
    const RealizedClassNode &node,
    std::vector<std::pair<std::size_t, std::size_t>> &visited_spans,
    std::vector<int> &owned_values_to_release) {
  if (!node.runtime_layout_ready) {
    return;
  }
  for (const RealizedPropertyAccessor &accessor :
       node.runtime_property_accessors) {
    if (!UsesStrongOwnedRuntimeHooks(accessor)) {
      continue;
    }
    const RuntimeIvarStorageSpan span =
        ResolveRuntimeIvarStorageSpan(instance, accessor);
    if (!span.addressable) {
      continue;
    }
    const std::pair<std::size_t, std::size_t> key{span.offset, span.size};
    const auto already_visited =
        std::find(visited_spans.begin(), visited_spans.end(), key);
    if (already_visited != visited_spans.end()) {
      continue;
    }
    visited_spans.push_back(key);
    int stored_value = 0;
    if (ReadRuntimeManagedPropertyValueRaw(instance, accessor, stored_value) &&
        stored_value != 0) {
      owned_values_to_release.push_back(stored_value);
    }
  }
}

std::vector<int> RuntimeInstanceOwnedValuesToReleaseForTeardownUnlocked(
    const RuntimeState &state,
    const RuntimeInstanceRecord &instance) {
  std::vector<int> owned_values_to_release;
  const RealizedClassNode *node =
      FindRealizedClassNodeByBaseIdentityUnlocked(state,
                                                  instance.base_identity);
  if (node == nullptr) {
    return owned_values_to_release;
  }
  std::vector<std::pair<std::size_t, std::size_t>> visited_spans;
  const RealizedClassNode *cursor = node;
  std::size_t visited_count = 0;
  while (cursor != nullptr && visited_count < state.realized_class_nodes.size()) {
    AppendRuntimeInstanceOwnedValuesForNodeUnlocked(
        instance, *cursor, visited_spans, owned_values_to_release);
    if (!cursor->has_super_node ||
        cursor->super_node_index >= state.realized_class_nodes.size()) {
      break;
    }
    cursor = &state.realized_class_nodes[cursor->super_node_index];
    ++visited_count;
  }
  return owned_values_to_release;
}

}  // namespace

int AllocateRuntimeInstanceUnlocked(RuntimeState &state,
                                    std::uint64_t base_identity,
                                    bool initialized) {
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
  instance.normalized_receiver_identity =
      BuildInstanceReceiverIdentity(base_identity);
  instance.class_receiver_identity = BuildClassReceiverIdentity(base_identity);
  instance.allocation_ordinal = state.next_runtime_instance_allocation_ordinal++;
  instance.class_name = node->class_name;
  instance.class_owner_identity = node->class_owner_identity;
  instance.metaclass_owner_identity = node->metaclass_owner_identity;
  instance.instance_isa_owner_identity = node->class_owner_identity;
  instance.class_object_isa_owner_identity = node->metaclass_owner_identity;
  instance.instance_size_bytes =
      RuntimeInstanceStorageSize(
          RuntimeInstanceStorageFloorForClassUnlocked(state, *node));
  const std::uint64_t instance_size_bytes =
      static_cast<std::uint64_t>(instance.instance_size_bytes);
  instance.storage_bytes.assign(instance.instance_size_bytes, 0u);
  instance.retain_count = 1u;
  instance.initialized = initialized;
  if (initialized) {
    instance.initialization_ordinal =
        state.next_runtime_instance_initialization_ordinal++;
  }

  state.last_runtime_instance_lifecycle_failure_reason.clear();
  state.last_allocated_runtime_instance_receiver =
      static_cast<std::uint64_t>(receiver_identity);
  state.last_allocated_runtime_instance_base_identity = base_identity;
  state.last_allocated_runtime_instance_size_bytes = instance_size_bytes;
  state.last_allocated_runtime_instance_allocation_ordinal =
      instance.allocation_ordinal;
  state.last_allocated_runtime_instance_class_name = node->class_name;
  if (initialized) {
    state.last_initialized_runtime_instance_receiver =
        static_cast<std::uint64_t>(receiver_identity);
    state.last_initialized_runtime_instance_initialization_ordinal =
        instance.initialization_ordinal;
  }
  state.runtime_instances_by_receiver.emplace(receiver_identity,
                                             std::move(instance));
  state.live_runtime_instance_count =
      static_cast<std::uint64_t>(state.runtime_instances_by_receiver.size());
  return receiver_identity;
}

bool InitializeRuntimeInstanceUnlocked(RuntimeState &state, int receiver) {
  const auto instance_it = state.runtime_instances_by_receiver.find(receiver);
  if (instance_it == state.runtime_instances_by_receiver.end()) {
    state.last_runtime_instance_lifecycle_failure_reason =
        "init target is not a live runtime instance";
    return false;
  }
  RuntimeInstanceRecord &instance = instance_it->second;
  if (instance.initialized) {
    state.last_runtime_instance_lifecycle_failure_reason =
        "runtime instance already initialized";
    return false;
  }
  instance.initialized = true;
  instance.initialization_ordinal =
      state.next_runtime_instance_initialization_ordinal++;
  state.last_initialized_runtime_instance_receiver =
      static_cast<std::uint64_t>(receiver);
  state.last_initialized_runtime_instance_initialization_ordinal =
      instance.initialization_ordinal;
  state.last_runtime_instance_lifecycle_failure_reason.clear();
  return true;
}

void DestroyRuntimeInstanceUnlocked(
    RuntimeState &state, int receiver,
    std::vector<RuntimeBlockRecord> *records_to_dispose) {
  const auto instance_it = state.runtime_instances_by_receiver.find(receiver);
  if (instance_it == state.runtime_instances_by_receiver.end()) {
    return;
  }
  const std::vector<int> owned_values_to_release =
      RuntimeInstanceOwnedValuesToReleaseForTeardownUnlocked(
          state, instance_it->second);

  ZeroWeakSlotRefsForTargetUnlocked(state, receiver);
  RemoveWeakSlotRefsOwnedByReceiverUnlocked(state, receiver);

  state.runtime_instances_by_receiver.erase(instance_it);
  state.live_runtime_instance_count =
      static_cast<std::uint64_t>(state.runtime_instances_by_receiver.size());

  for (int stored_value : owned_values_to_release) {
    ReleaseRuntimeValueUnlocked(state, stored_value, records_to_dispose);
  }
}

}  // namespace objc3c::runtime
