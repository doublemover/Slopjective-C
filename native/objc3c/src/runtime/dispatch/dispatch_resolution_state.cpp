#include "runtime/dispatch/dispatch_resolution_state.h"

#include "runtime/dispatch/dispatch_result_state.h"
#include "runtime/dispatch/runtime_resolution_records.h"
#include "runtime/state/runtime_state_records.h"

namespace objc3c::runtime {

namespace {

void PublishRuntimePropertyAccessorStateUnlocked(
    RuntimeState &state,
    const RealizedPropertyAccessor *runtime_property_accessor,
    std::uint64_t receiver_base_identity) {
  if (runtime_property_accessor == nullptr ||
      runtime_property_accessor->property_descriptor == nullptr) {
    return;
  }
  state.last_dispatch_property_name =
      runtime_property_accessor->property_descriptor->property_name != nullptr
          ? runtime_property_accessor->property_descriptor->property_name
          : "";
  state.last_dispatch_property_base_identity = receiver_base_identity;
  state.last_dispatch_property_slot_index =
      runtime_property_accessor->ivar_descriptor != nullptr
          ? runtime_property_accessor->ivar_descriptor->slot_index
          : runtime_property_accessor->property_descriptor
                ->ivar_layout_slot_index;
}

}  // namespace

void ResetRuntimeDispatchStateUnlocked(
    RuntimeState &state, const char *selector,
    const objc3_runtime_selector_handle *selector_handle,
    objc3_runtime_dispatch_status_code initial_status) {
  state.last_dispatch_selector = selector != nullptr ? selector : "";
  state.last_dispatch_selector_stable_id =
      selector_handle != nullptr ? selector_handle->stable_id : 0;
  state.last_dispatch_normalized_receiver_identity = 0;
  state.last_category_probe_count = 0;
  state.last_protocol_probe_count = 0;
  state.last_dispatch_used_cache = false;
  state.last_dispatch_used_fast_path = false;
  state.last_dispatch_resolved_live_method = false;
  state.last_dispatch_strict_error = false;
  state.last_dispatch_effective_direct_dispatch = false;
  state.last_dispatch_used_builtin = false;
  state.last_dispatch_status_code = initial_status;
  state.last_fast_path_reason.clear();
  state.last_dispatch_path.clear();
  state.last_dispatch_implementation_kind.clear();
  state.last_dispatch_return_kind.clear();
  state.last_dispatch_diagnostic_code.clear();
  state.last_dispatch_diagnostic_message.clear();
  state.last_dispatch_result_contract.clear();
  state.last_dispatch_property_name.clear();
  state.last_resolved_class_name.clear();
  state.last_resolved_owner_identity.clear();
  state.last_dispatch_parameter_count = 0;
  state.last_dispatch_property_base_identity = 0;
  state.last_dispatch_property_slot_index = 0;
  StoreDispatchResultContractUnlocked(
      state, initial_status, RuntimeMethodReturnKind::Unsupported);
}

void PublishMethodCacheEntryStateUnlocked(
    RuntimeState &state, const MethodCacheEntry &entry,
    std::uint64_t receiver_base_identity) {
  state.last_dispatch_used_cache = true;
  state.last_dispatch_used_fast_path = entry.fast_path_seeded;
  state.last_dispatch_resolved_live_method = entry.resolved;
  state.last_dispatch_strict_error = !entry.resolved;
  state.last_dispatch_effective_direct_dispatch =
      entry.effective_direct_dispatch;
  state.last_category_probe_count = entry.category_probe_count;
  state.last_protocol_probe_count = entry.protocol_probe_count;
  state.last_fast_path_reason = entry.fast_path_reason;
  state.last_resolved_class_name = entry.class_name;
  state.last_resolved_owner_identity = entry.owner_identity;
  state.last_dispatch_parameter_count = entry.parameter_count;
  PublishRuntimePropertyAccessorStateUnlocked(
      state, entry.runtime_property_accessor, receiver_base_identity);
}

void PublishSlowPathResolutionStateUnlocked(
    RuntimeState &state, const SlowPathResolution &resolution,
    std::uint64_t receiver_base_identity) {
  state.last_dispatch_resolved_live_method = resolution.resolved;
  state.last_dispatch_strict_error = !resolution.resolved;
  state.last_dispatch_effective_direct_dispatch =
      resolution.effective_direct_dispatch;
  state.last_category_probe_count = resolution.category_probe_count;
  state.last_protocol_probe_count = resolution.protocol_probe_count;
  state.last_fast_path_reason = resolution.fast_path_reason;
  state.last_resolved_class_name = resolution.class_name;
  state.last_resolved_owner_identity = resolution.owner_identity;
  state.last_dispatch_parameter_count = resolution.parameter_count;
  PublishRuntimePropertyAccessorStateUnlocked(
      state, resolution.runtime_property_accessor, receiver_base_identity);
}

RuntimeDispatchTarget PublishStrictDispatchErrorTargetUnlocked(
    RuntimeState &state, objc3_runtime_dispatch_status_code status_code,
    const char *dispatch_path) {
  RuntimeDispatchTarget target;
  target.dispatch_status = status_code;
  ++state.strict_dispatch_error_count;
  state.last_dispatch_strict_error = true;
  state.last_dispatch_resolved_live_method = false;
  state.last_dispatch_path = dispatch_path != nullptr ? dispatch_path : "";
  state.last_dispatch_implementation_kind = "strict-dispatch-error";
  StoreDispatchResultContractUnlocked(
      state, status_code, RuntimeMethodReturnKind::Unsupported);
  return target;
}

}  // namespace objc3c::runtime
