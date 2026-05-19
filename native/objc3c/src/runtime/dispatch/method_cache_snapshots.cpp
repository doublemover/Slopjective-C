#include "runtime/dispatch/method_cache.h"

#include "runtime/classes/receiver_identity.h"
#include "runtime/dispatch/dispatch_family.h"
#include "runtime/dispatch/runtime_resolution_records.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"
#include "runtime/strings/borrowed_string.h"

#include <cstdint>
#include <mutex>

extern "C" int objc3_runtime_copy_method_cache_state_for_testing(
    objc3_runtime_method_cache_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  snapshot->cache_entry_count =
      static_cast<std::uint64_t>(state.method_cache.size());
  snapshot->cache_hit_count = state.method_cache_hit_count;
  snapshot->cache_miss_count = state.method_cache_miss_count;
  snapshot->slow_path_lookup_count = state.slow_path_lookup_count;
  snapshot->live_dispatch_count = state.live_dispatch_count;
  snapshot->strict_dispatch_error_count = state.strict_dispatch_error_count;
  snapshot->fast_path_seed_count = state.fast_path_seed_count;
  snapshot->fast_path_hit_count = state.fast_path_hit_count;
  snapshot->class_graph_generation = state.class_graph_generation;
  snapshot->category_attachment_generation =
      state.category_attachment_generation;
  snapshot->protocol_declaration_generation =
      state.protocol_declaration_generation;
  snapshot->storage_surface_generation = state.storage_surface_generation;
  snapshot->method_surface_generation = state.method_surface_generation;
  snapshot->last_selector_stable_id = state.last_dispatch_selector_stable_id;
  snapshot->last_normalized_receiver_identity =
      state.last_dispatch_normalized_receiver_identity;
  snapshot->last_category_probe_count = state.last_category_probe_count;
  snapshot->last_protocol_probe_count = state.last_protocol_probe_count;
  snapshot->last_dispatch_used_cache = state.last_dispatch_used_cache ? 1 : 0;
  snapshot->last_dispatch_used_fast_path =
      state.last_dispatch_used_fast_path ? 1 : 0;
  snapshot->last_dispatch_resolved_live_method =
      state.last_dispatch_resolved_live_method ? 1 : 0;
  snapshot->last_dispatch_strict_error =
      state.last_dispatch_strict_error ? 1 : 0;
  snapshot->last_selector =
      objc3c::runtime::BorrowRuntimeCString(state.last_dispatch_selector);
  snapshot->last_fast_path_reason =
      objc3c::runtime::BorrowRuntimeCString(state.last_fast_path_reason);
  snapshot->last_resolved_class_name =
      objc3c::runtime::BorrowRuntimeCString(state.last_resolved_class_name);
  snapshot->last_resolved_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(state.last_resolved_owner_identity);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

extern "C" int objc3_runtime_copy_method_cache_entry_for_testing(
    int receiver,
    const char *selector,
    objc3_runtime_method_cache_entry_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->found = 0;
  snapshot->resolved = 0;
  snapshot->dispatch_family_is_class = 0;
  snapshot->lookup_start_base_identity = 0;
  snapshot->normalized_receiver_identity = 0;
  snapshot->selector_stable_id = 0;
  snapshot->parameter_count = 0;
  snapshot->category_probe_count = 0;
  snapshot->protocol_probe_count = 0;
  snapshot->cache_class_graph_generation = 0;
  snapshot->cache_category_attachment_generation = 0;
  snapshot->cache_protocol_declaration_generation = 0;
  snapshot->cache_storage_surface_generation = 0;
  snapshot->cache_method_surface_generation = 0;
  snapshot->fast_path_seeded = 0;
  snapshot->effective_direct_dispatch = 0;
  snapshot->objc_final_declared = 0;
  snapshot->objc_sealed_declared = 0;
  snapshot->selector = nullptr;
  snapshot->fast_path_reason = nullptr;
  snapshot->resolved_class_name = nullptr;
  snapshot->resolved_owner_identity = nullptr;

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  std::uint64_t base_identity = 0;
  std::uint64_t normalized_receiver_identity = 0;
  objc3c::runtime::DispatchFamily family =
      objc3c::runtime::DispatchFamily::Invalid;
  if (!objc3c::runtime::DecodeReceiverIdentity(
          state, receiver, base_identity, family,
          normalized_receiver_identity)) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }
  if (selector == nullptr || selector[0] == '\0') {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }
  const auto selector_it = state.selector_index_by_name.find(selector);
  if (selector_it == state.selector_index_by_name.end()) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }
  const objc3c::runtime::MethodCacheKey key{
      base_identity,
      normalized_receiver_identity,
      state.selector_slots[selector_it->second].handle.stable_id};
  const auto cache_it = state.method_cache.find(key);
  if (cache_it == state.method_cache.end()) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }
  const objc3c::runtime::MethodCacheEntry &entry = cache_it->second;
  snapshot->found = 1;
  snapshot->resolved = entry.resolved ? 1 : 0;
  snapshot->dispatch_family_is_class =
      entry.dispatch_family_is_class ? 1 : 0;
  snapshot->lookup_start_base_identity = entry.lookup_start_base_identity;
  snapshot->normalized_receiver_identity = entry.normalized_receiver_identity;
  snapshot->selector_stable_id = entry.selector_stable_id;
  snapshot->parameter_count = entry.parameter_count;
  snapshot->category_probe_count = entry.category_probe_count;
  snapshot->protocol_probe_count = entry.protocol_probe_count;
  snapshot->cache_class_graph_generation =
      entry.cache_class_graph_generation;
  snapshot->cache_category_attachment_generation =
      entry.cache_category_attachment_generation;
  snapshot->cache_protocol_declaration_generation =
      entry.cache_protocol_declaration_generation;
  snapshot->cache_storage_surface_generation =
      entry.cache_storage_surface_generation;
  snapshot->cache_method_surface_generation =
      entry.cache_method_surface_generation;
  snapshot->fast_path_seeded = entry.fast_path_seeded ? 1 : 0;
  snapshot->effective_direct_dispatch =
      entry.effective_direct_dispatch ? 1 : 0;
  snapshot->objc_final_declared = entry.objc_final_declared ? 1 : 0;
  snapshot->objc_sealed_declared = entry.objc_sealed_declared ? 1 : 0;
  snapshot->selector =
      objc3c::runtime::BorrowRuntimeCString(entry.selector_storage);
  snapshot->fast_path_reason =
      objc3c::runtime::BorrowRuntimeCString(entry.fast_path_reason);
  snapshot->resolved_class_name =
      objc3c::runtime::BorrowRuntimeCString(entry.class_name);
  snapshot->resolved_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(entry.owner_identity);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}
