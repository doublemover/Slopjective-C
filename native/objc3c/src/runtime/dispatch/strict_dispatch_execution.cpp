#include "runtime/dispatch/strict_dispatch_execution.h"

#include "runtime/dispatch/builtin_methods.h"
#include "runtime/dispatch/dispatch_result_state.h"
#include "runtime/dispatch/dispatch_status.h"
#include "runtime/dispatch/method_invocation.h"
#include "runtime/dispatch/typed_dispatch_result.h"
#include "runtime/memory/arc_value_lifetime.h"
#include "runtime/memory/dispatch_frame_state.h"
#include "runtime/public/objc3_runtime_result_materialization_contract.h"
#include "runtime/state/runtime_state_records.h"

#include <mutex>
#include <vector>

namespace objc3c::runtime {
namespace {

void ReleaseDispatchFrameAutoreleaseValues(RuntimeState &state) {
  const std::vector<int> autorelease_values =
      PopRuntimeDispatchFrameAutoreleaseValues();
  if (autorelease_values.empty()) {
    return;
  }
  std::lock_guard<std::mutex> lock(state.mutex);
  for (int value : autorelease_values) {
    ReleaseRuntimeValueUnlocked(state, value);
  }
}

objc3_runtime_dispatch_i32_result CompleteStrictInvocationResult(
    RuntimeState &state, RuntimeTypedDispatchResult result,
    const char *strict_failure_path) {
  result = NormalizeRuntimeTypedDispatchResult(result);
  ReleaseDispatchFrameAutoreleaseValues(state);
  if (!RuntimeDispatchStatusIsSuccess(result.status_code)) {
    RecordPostResolutionStrictDispatchFailure(
        state, result.status_code, result.return_kind, strict_failure_path);
    return MakeRuntimeDispatchI32TypedResult(
        result.status_code, 0,
        RuntimeMethodReturnKindDispatchAbiCode(result.return_kind));
  }
  RecordTypedDispatchSuccess(state, result.return_kind);
  return MakeRuntimeDispatchI32TypedResult(
      OBJC3_RUNTIME_DISPATCH_STATUS_OK, result.value,
      RuntimeMethodReturnKindDispatchAbiCode(result.return_kind));
}

}  // namespace

objc3_runtime_dispatch_i32_result ExecuteResolvedRuntimeDispatchTargetStrict(
    RuntimeState &state, int receiver,
    const RuntimeDispatchTarget &dispatch_target, int a0, int a1, int a2,
    int a3) {
  if (!dispatch_target.resolved_live_method) {
    RecordPostResolutionStrictDispatchFailure(
        state, OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA,
        dispatch_target.return_kind, "resolved-method-precondition-error");
    return MakeRuntimeDispatchI32TypedResult(
        OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA, 0,
        RuntimeMethodReturnKindDispatchAbiCode(dispatch_target.return_kind));
  }

  PushRuntimeDispatchFrame(receiver, dispatch_target.receiver_base_identity,
                           dispatch_target.runtime_property_accessor);
  if (dispatch_target.implementation != nullptr) {
    return CompleteStrictInvocationResult(
        state,
        InvokeRuntimeMethodImplementation(
            dispatch_target.implementation, dispatch_target.return_kind,
            dispatch_target.parameter_count, a0, a1, a2, a3),
        "resolved-method-invocation-error");
  }
  if (dispatch_target.builtin_kind != RuntimeBuiltinKind::None) {
    return CompleteStrictInvocationResult(
        state,
        InvokeRuntimeBuiltinMethod(
            state, dispatch_target.builtin_kind, receiver,
            dispatch_target.receiver_base_identity,
            dispatch_target.runtime_property_accessor, a0, a1, a2, a3),
        "runtime-builtin-invocation-error");
  }

  ReleaseDispatchFrameAutoreleaseValues(state);
  RecordPostResolutionStrictDispatchFailure(
      state, OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA,
      dispatch_target.return_kind, "resolved-method-missing-callable-error");
  return MakeRuntimeDispatchI32TypedResult(
      OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA, 0,
      RuntimeMethodReturnKindDispatchAbiCode(dispatch_target.return_kind));
}

}  // namespace objc3c::runtime
