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

#include <cstdio>
#include <cstdlib>
#include <mutex>
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
