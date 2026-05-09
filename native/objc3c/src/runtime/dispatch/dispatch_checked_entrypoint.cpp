#include "runtime/dispatch/dispatch_checked_entrypoint.h"

#include "runtime/dispatch/dispatch_result_state.h"
#include "runtime/dispatch/dispatch_target_resolution.h"
#include "runtime/dispatch/runtime_method_return.h"
#include "runtime/dispatch/strict_dispatch_execution.h"
#include "runtime/public/objc3_runtime_result_materialization_contract.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"

#include <mutex>

namespace objc3c::runtime {

objc3_runtime_dispatch_i32_result ExecuteRuntimeDispatchI32Checked(
    int receiver, const char *selector, int a0, int a1, int a2, int a3) {
  RuntimeState &state = ProcessRuntimeState();
  RuntimeDispatchTarget dispatch_target;
  {
    std::lock_guard<std::mutex> lock(state.mutex);
    dispatch_target =
        ResolveRuntimeDispatchTargetUnlocked(state, receiver, selector);
  }
  if (dispatch_target.resolved_live_method) {
    return ExecuteResolvedRuntimeDispatchTargetStrict(
        state, receiver, dispatch_target, a0, a1, a2, a3);
  }
  {
    std::lock_guard<std::mutex> lock(state.mutex);
    StoreDispatchResultContractUnlocked(
        state, dispatch_target.dispatch_status,
        RuntimeMethodReturnKind::Unsupported);
  }
  return MakeRuntimeDispatchI32Result(dispatch_target.dispatch_status, 0);
}

}  // namespace objc3c::runtime
