#include "runtime/memory/arc.h"

#include "runtime/memory/arc_debug_state.h"
#include "runtime/memory/autorelease_pool.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state.h"
#include "runtime/storage/property_accessors.h"
#include "runtime/storage/weak_slots.h"

#include <mutex>
#include <utility>
#include <vector>

namespace objc3c::runtime {

namespace {

const RealizedClassNode *FindArcClassNodeByBaseIdentityUnlocked(
    const RuntimeState &state,
    std::uint64_t base_identity) {
  const auto class_name_it =
      state.realized_class_name_by_base_identity.find(base_identity);
  if (class_name_it == state.realized_class_name_by_base_identity.end()) {
    return nullptr;
  }
  const auto node_indexes_it =
      state.realized_class_node_indices_by_name.find(class_name_it->second);
  if (node_indexes_it == state.realized_class_node_indices_by_name.end()) {
    return nullptr;
  }
  for (const std::size_t node_index : node_indexes_it->second) {
    if (node_index >= state.realized_class_nodes.size()) {
      continue;
    }
    const RealizedClassNode &node = state.realized_class_nodes[node_index];
    if (node.base_identity == base_identity) {
      return &node;
    }
  }
  return nullptr;
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

  const RealizedClassNode *node =
      FindArcClassNodeByBaseIdentityUnlocked(state, instance.base_identity);
  std::vector<int> owned_values_to_release;
  if (node != nullptr && node->runtime_layout_ready) {
    owned_values_to_release.reserve(node->runtime_property_accessors.size());
    for (const RealizedPropertyAccessor &accessor :
         node->runtime_property_accessors) {
      if (!UsesStrongOwnedRuntimeHooks(accessor)) {
        continue;
      }
      int stored_value = 0;
      if (ReadRuntimeManagedPropertyValueRaw(instance, accessor,
                                             stored_value) &&
          stored_value != 0) {
        owned_values_to_release.push_back(stored_value);
      }
    }
  }
  for (int stored_value : owned_values_to_release) {
    ReleaseRuntimeValueUnlocked(state, stored_value);
  }
}

}  // namespace

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
  const auto block_it = state.runtime_blocks_by_handle.find(value);
  if (block_it == state.runtime_blocks_by_handle.end()) {
    return;
  }
  ++block_it->second.retain_count;
}

void ReleaseRuntimeValueUnlocked(RuntimeState &state, int value) {
  if (!RuntimeArcValueIsRetainable(value)) {
    return;
  }
  const auto instance_it = state.runtime_instances_by_receiver.find(value);
  if (instance_it != state.runtime_instances_by_receiver.end()) {
    if (instance_it->second.retain_count > 1u) {
      --instance_it->second.retain_count;
      return;
    }
    DestroyRuntimeInstanceUnlocked(state, value);
    return;
  }
  const auto block_it = state.runtime_blocks_by_handle.find(value);
  if (block_it == state.runtime_blocks_by_handle.end()) {
    return;
  }
  if (block_it->second.retain_count > 1u) {
    --block_it->second.retain_count;
    return;
  }
  // Final block-handle release owns copy/dispose teardown in the ARC module.
  if (block_it->second.dispose_helper != nullptr &&
      !block_it->second.storage_words.empty()) {
    block_it->second.dispose_helper(block_it->second.storage_words.data());
  }
  state.runtime_blocks_by_handle.erase(block_it);
}

}  // namespace objc3c::runtime

extern "C" int objc3_runtime_load_weak_current_property_i32(void) {
  objc3c::runtime::RuntimeArcDebugState &arc_debug =
      objc3c::runtime::RuntimeArcDebugStateForCurrentThread();
  ++arc_debug.weak_current_property_load_count;
  const int result = objc3_runtime_read_current_property_i32();
  arc_debug.last_weak_loaded_value = result;
  return result;
}

extern "C" void objc3_runtime_store_weak_current_property_i32(int value) {
  objc3c::runtime::RuntimeArcDebugState &arc_debug =
      objc3c::runtime::RuntimeArcDebugStateForCurrentThread();
  ++arc_debug.weak_current_property_store_count;
  arc_debug.last_weak_stored_value = value;
  objc3_runtime_write_current_property_i32(value);
}

extern "C" int objc3_runtime_retain_i32(int value) {
  objc3c::runtime::RuntimeArcDebugState &arc_debug =
      objc3c::runtime::RuntimeArcDebugStateForCurrentThread();
  ++arc_debug.retain_call_count;
  arc_debug.last_retain_value = value;
  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  objc3c::runtime::RetainRuntimeValueUnlocked(state, value);
  return value;
}

extern "C" int objc3_runtime_release_i32(int value) {
  objc3c::runtime::RuntimeArcDebugState &arc_debug =
      objc3c::runtime::RuntimeArcDebugStateForCurrentThread();
  ++arc_debug.release_call_count;
  arc_debug.last_release_value = value;
  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  objc3c::runtime::ReleaseRuntimeValueUnlocked(state, value);
  return value;
}

extern "C" int objc3_runtime_autorelease_i32(int value) {
  objc3c::runtime::RuntimeArcDebugState &arc_debug =
      objc3c::runtime::RuntimeArcDebugStateForCurrentThread();
  ++arc_debug.autorelease_call_count;
  arc_debug.last_autorelease_value = value;
  if (objc3c::runtime::RuntimeAutoreleasePoolCanEnqueue(value)) {
    objc3c::runtime::EnqueueAutoreleaseValue(value);
  }
  return value;
}

extern "C" void objc3_runtime_push_autoreleasepool_scope(void) {
  objc3c::runtime::PushRuntimeAutoreleasePoolFrame();
  objc3c::runtime::RuntimeArcDebugState &arc_debug =
      objc3c::runtime::RuntimeArcDebugStateForCurrentThread();
  ++arc_debug.autoreleasepool_push_count;
}

extern "C" void objc3_runtime_pop_autoreleasepool_scope(void) {
  objc3c::runtime::RuntimeArcDebugState &arc_debug =
      objc3c::runtime::RuntimeArcDebugStateForCurrentThread();
  ++arc_debug.autoreleasepool_pop_count;
  const std::vector<int> values =
      objc3c::runtime::PopRuntimeAutoreleasePoolFrameValues();
  if (values.empty()) {
    return;
  }
  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  for (auto it = values.rbegin(); it != values.rend(); ++it) {
    objc3c::runtime::RecordRuntimeAutoreleasePoolDrainedValue(*it);
    objc3c::runtime::ReleaseRuntimeValueUnlocked(state, *it);
  }
}
