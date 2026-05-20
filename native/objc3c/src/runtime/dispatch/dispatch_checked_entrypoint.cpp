#include "runtime/dispatch/dispatch_checked_entrypoint.h"

#include "runtime/dispatch/dispatch_result_state.h"
#include "runtime/dispatch/dispatch_target_resolution.h"
#include "runtime/dispatch/runtime_method_return.h"
#include "runtime/dispatch/strict_dispatch_execution.h"
#include "runtime/dispatch/typed_dispatch_result.h"
#include "runtime/public/objc3_runtime_result_materialization_contract.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"

#include <mutex>

namespace objc3c::runtime {
namespace {

objc3_runtime_dispatch_typed_result ExecuteRuntimeDispatchTargetChecked(
    int receiver, const RuntimeDispatchTarget &dispatch_target, int a0, int a1,
    int a2, int a3) {
  RuntimeState &state = ProcessRuntimeState();
  if (dispatch_target.resolved_live_method) {
    const RuntimeTypedDispatchResult result =
        ExecuteResolvedRuntimeDispatchTargetStrict(
            state, receiver, dispatch_target, a0, a1, a2, a3);
    return MakeRuntimeDispatchTypedResult(
        result.status_code, result.value,
        RuntimeMethodReturnKindDispatchAbiCode(result.return_kind));
  }
  {
    std::lock_guard<std::mutex> lock(state.mutex);
    StoreDispatchResultContractUnlocked(
        state, dispatch_target.dispatch_status,
        RuntimeMethodReturnKind::Unsupported);
  }
  return MakeRuntimeDispatchTypedResult(
      dispatch_target.dispatch_status, 0,
      OBJC3_RUNTIME_DISPATCH_RETURN_KIND_UNSUPPORTED);
}

}  // namespace

objc3_runtime_dispatch_i32_result ExecuteRuntimeDispatchI32Checked(
    int receiver, const char *selector, int a0, int a1, int a2, int a3) {
  return MakeRuntimeDispatchI32ResultFromTypedResult(
      ExecuteRuntimeDispatchTypedChecked(receiver, selector, a0, a1, a2, a3));
}

objc3_runtime_dispatch_typed_result ExecuteRuntimeDispatchTypedChecked(
    int receiver, const char *selector, int a0, int a1, int a2, int a3) {
  RuntimeState &state = ProcessRuntimeState();
  RuntimeDispatchTarget dispatch_target;
  {
    std::lock_guard<std::mutex> lock(state.mutex);
    dispatch_target =
        ResolveRuntimeDispatchTargetUnlocked(state, receiver, selector);
  }
  return ExecuteRuntimeDispatchTargetChecked(
      receiver, dispatch_target, a0, a1, a2, a3);
}

objc3_runtime_dispatch_i32_result ExecuteRuntimeDispatchI32FromClassChecked(
    int receiver, const char *lookup_start_class_name, const char *selector,
    int a0, int a1, int a2, int a3) {
  return MakeRuntimeDispatchI32ResultFromTypedResult(
      ExecuteRuntimeDispatchTypedFromClassChecked(
          receiver, lookup_start_class_name, selector, a0, a1, a2, a3));
}

objc3_runtime_dispatch_typed_result
ExecuteRuntimeDispatchTypedFromClassChecked(
    int receiver, const char *lookup_start_class_name, const char *selector,
    int a0, int a1, int a2, int a3) {
  RuntimeState &state = ProcessRuntimeState();
  RuntimeDispatchTarget dispatch_target;
  {
    std::lock_guard<std::mutex> lock(state.mutex);
    dispatch_target = ResolveRuntimeDispatchTargetFromClassUnlocked(
        state, receiver, lookup_start_class_name, selector);
  }
  return ExecuteRuntimeDispatchTargetChecked(
      receiver, dispatch_target, a0, a1, a2, a3);
}

}  // namespace objc3c::runtime
