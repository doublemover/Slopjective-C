#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"
#include "runtime/strings/borrowed_string.h"

#include <cstdint>
#include <mutex>

extern "C" int objc3_runtime_copy_dispatch_state_for_testing(
    objc3_runtime_dispatch_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  snapshot->cache_entry_count =
      static_cast<std::uint64_t>(state.method_cache.size());
  snapshot->fast_path_seed_count = state.fast_path_seed_count;
  snapshot->fast_path_hit_count = state.fast_path_hit_count;
  snapshot->live_dispatch_count = state.live_dispatch_count;
  snapshot->strict_dispatch_error_count = state.strict_dispatch_error_count;
  snapshot->last_selector_stable_id = state.last_dispatch_selector_stable_id;
  snapshot->last_normalized_receiver_identity =
      state.last_dispatch_normalized_receiver_identity;
  snapshot->last_resolved_parameter_count = state.last_dispatch_parameter_count;
  snapshot->last_property_base_identity =
      state.last_dispatch_property_base_identity;
  snapshot->last_property_slot_index = state.last_dispatch_property_slot_index;
  snapshot->last_dispatch_used_cache = state.last_dispatch_used_cache ? 1 : 0;
  snapshot->last_dispatch_used_fast_path =
      state.last_dispatch_used_fast_path ? 1 : 0;
  snapshot->last_dispatch_resolved_live_method =
      state.last_dispatch_resolved_live_method ? 1 : 0;
  snapshot->last_dispatch_strict_error =
      state.last_dispatch_strict_error ? 1 : 0;
  snapshot->last_effective_direct_dispatch =
      state.last_dispatch_effective_direct_dispatch ? 1 : 0;
  snapshot->last_used_builtin = state.last_dispatch_used_builtin ? 1 : 0;
  snapshot->last_dispatch_status_code = state.last_dispatch_status_code;
  snapshot->last_selector =
      objc3c::runtime::BorrowRuntimeCString(state.last_dispatch_selector);
  snapshot->last_fast_path_reason =
      objc3c::runtime::BorrowRuntimeCString(state.last_fast_path_reason);
  snapshot->last_dispatch_path =
      objc3c::runtime::BorrowRuntimeCString(state.last_dispatch_path);
  snapshot->last_implementation_kind =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_dispatch_implementation_kind);
  snapshot->last_return_kind =
      objc3c::runtime::BorrowRuntimeCString(state.last_dispatch_return_kind);
  snapshot->last_diagnostic_code =
      objc3c::runtime::BorrowRuntimeCString(state.last_dispatch_diagnostic_code);
  snapshot->last_diagnostic_message =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_dispatch_diagnostic_message);
  snapshot->last_result_contract =
      objc3c::runtime::BorrowRuntimeCString(state.last_dispatch_result_contract);
  snapshot->last_property_name =
      objc3c::runtime::BorrowRuntimeCString(state.last_dispatch_property_name);
  snapshot->last_resolved_class_name =
      objc3c::runtime::BorrowRuntimeCString(state.last_resolved_class_name);
  snapshot->last_resolved_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(state.last_resolved_owner_identity);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}
