#include "runtime/dispatch/dispatch_api.h"

#include "runtime/dispatch/builtin_methods.h"
#include "runtime/dispatch/dispatch_result_state.h"
#include "runtime/dispatch/dispatch_target_resolution.h"
#include "runtime/dispatch/method_invocation.h"
#include "runtime/dispatch/typed_dispatch_result.h"
#include "runtime/memory/arc_value_lifetime.h"
#include "runtime/memory/dispatch_frame_state.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/public/objc3_runtime_api.h"
#include "runtime/public/objc3_runtime_result_contract.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"
#include "runtime/strings/borrowed_string.h"

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <mutex>
#include <utility>
#include <vector>

namespace objc3c::runtime {

objc3_runtime_dispatch_status_code RuntimeStrictDispatchStatus(
    bool resolved, bool ambiguous,
    objc3_runtime_dispatch_status_code unresolved_status) {
  if (resolved) {
    return OBJC3_RUNTIME_DISPATCH_STATUS_OK;
  }
  if (ambiguous) {
    return OBJC3_RUNTIME_DISPATCH_STATUS_CATEGORY_CONFLICT;
  }
  return unresolved_status;
}

bool RuntimeDispatchStatusIsSuccess(
    objc3_runtime_dispatch_status_code status_code) {
  return status_code == OBJC3_RUNTIME_DISPATCH_STATUS_OK;
}

}  // namespace objc3c::runtime

extern "C" objc3_runtime_dispatch_i32_result objc3_runtime_dispatch_i32_checked(
    int receiver, const char *selector, int a0, int a1, int a2, int a3) {
  using objc3c::runtime::InvokeRuntimeBuiltinMethod;
  using objc3c::runtime::InvokeRuntimeMethodImplementation;
  using objc3c::runtime::PopRuntimeDispatchFrameAutoreleaseValues;
  using objc3c::runtime::ProcessRuntimeState;
  using objc3c::runtime::PushRuntimeDispatchFrame;
  using objc3c::runtime::RecordPostResolutionStrictDispatchFailure;
  using objc3c::runtime::RecordTypedDispatchSuccess;
  using objc3c::runtime::ReleaseRuntimeValueUnlocked;
  using objc3c::runtime::RuntimeBuiltinKind;
  using objc3c::runtime::RuntimeDispatchTarget;
  using objc3c::runtime::RuntimeDispatchStatusIsSuccess;
  using objc3c::runtime::RuntimeState;
  using objc3c::runtime::RuntimeTypedDispatchResult;
  using objc3c::runtime::ResolveRuntimeDispatchTargetUnlocked;
  using objc3c::runtime::StoreDispatchResultContractUnlocked;

  RuntimeState &state = ProcessRuntimeState();
  RuntimeDispatchTarget dispatch_target;
  {
    std::lock_guard<std::mutex> lock(state.mutex);
    dispatch_target =
        ResolveRuntimeDispatchTargetUnlocked(state, receiver, selector);
  }
  if (dispatch_target.resolved_live_method &&
      dispatch_target.implementation != nullptr) {
    PushRuntimeDispatchFrame(receiver, dispatch_target.receiver_base_identity,
                             dispatch_target.runtime_property_accessor);
    const RuntimeTypedDispatchResult result = InvokeRuntimeMethodImplementation(
        dispatch_target.implementation, dispatch_target.return_kind,
        dispatch_target.parameter_count, a0, a1, a2, a3);
    const std::vector<int> autorelease_values =
        PopRuntimeDispatchFrameAutoreleaseValues();
    if (!autorelease_values.empty()) {
      std::lock_guard<std::mutex> lock(state.mutex);
      for (int value : autorelease_values) {
        ReleaseRuntimeValueUnlocked(state, value);
      }
    }
    if (!RuntimeDispatchStatusIsSuccess(result.status_code)) {
      RecordPostResolutionStrictDispatchFailure(
          state, result.status_code, result.return_kind,
          "resolved-method-invocation-error");
      return objc3c::runtime::MakeRuntimeDispatchI32Result(result.status_code,
                                                          0);
    }
    RecordTypedDispatchSuccess(state, result.return_kind);
    return objc3c::runtime::MakeRuntimeDispatchI32Result(
        OBJC3_RUNTIME_DISPATCH_STATUS_OK, result.value);
  }
  if (dispatch_target.resolved_live_method &&
      dispatch_target.builtin_kind != RuntimeBuiltinKind::None) {
    PushRuntimeDispatchFrame(receiver, dispatch_target.receiver_base_identity,
                             dispatch_target.runtime_property_accessor);
    const RuntimeTypedDispatchResult result = InvokeRuntimeBuiltinMethod(
        state, dispatch_target.builtin_kind, receiver,
        dispatch_target.receiver_base_identity,
        dispatch_target.runtime_property_accessor, a0, a1, a2, a3);
    const std::vector<int> autorelease_values =
        PopRuntimeDispatchFrameAutoreleaseValues();
    if (!autorelease_values.empty()) {
      std::lock_guard<std::mutex> lock(state.mutex);
      for (int value : autorelease_values) {
        ReleaseRuntimeValueUnlocked(state, value);
      }
    }
    if (!RuntimeDispatchStatusIsSuccess(result.status_code)) {
      RecordPostResolutionStrictDispatchFailure(
          state, result.status_code, result.return_kind,
          "runtime-builtin-invocation-error");
      return objc3c::runtime::MakeRuntimeDispatchI32Result(result.status_code,
                                                          0);
    }
    RecordTypedDispatchSuccess(state, result.return_kind);
    return objc3c::runtime::MakeRuntimeDispatchI32Result(
        OBJC3_RUNTIME_DISPATCH_STATUS_OK, result.value);
  }
  if (dispatch_target.resolved_live_method) {
    RecordPostResolutionStrictDispatchFailure(
        state, OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA,
        dispatch_target.return_kind, "resolved-method-missing-callable-error");
    return objc3c::runtime::MakeRuntimeDispatchI32Result(
        OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA, 0);
  }
  {
    std::lock_guard<std::mutex> lock(state.mutex);
    StoreDispatchResultContractUnlocked(
        state, dispatch_target.dispatch_status,
        objc3c::runtime::RuntimeMethodReturnKind::Unsupported);
  }
  return objc3c::runtime::MakeRuntimeDispatchI32Result(
      dispatch_target.dispatch_status, 0);
}

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

extern "C" int objc3_runtime_dispatch_i32(int receiver, const char *selector,
                                          int a0, int a1, int a2, int a3) {
  const objc3_runtime_dispatch_i32_result result =
      objc3_runtime_dispatch_i32_checked(receiver, selector, a0, a1, a2, a3);
  if (!objc3c::runtime::RuntimeDispatchStatusIsSuccess(result.status_code)) {
    std::fprintf(
        stderr,
        "%s [%s]\n",
        result.diagnostic_message != nullptr ? result.diagnostic_message
                                             : "runtime dispatch failed",
        result.diagnostic_code != nullptr ? result.diagnostic_code
                                          : "O3RT000");
    std::abort();
  }
  return result.value;
}
