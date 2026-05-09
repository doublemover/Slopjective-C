#include "runtime/dispatch/dispatch_api.h"

#include "runtime/dispatch/dispatch_result_state.h"
#include "runtime/dispatch/dispatch_status.h"
#include "runtime/dispatch/dispatch_target_resolution.h"
#include "runtime/dispatch/strict_dispatch_execution.h"
#include "runtime/public/objc3_runtime_api.h"
#include "runtime/public/objc3_runtime_result_materialization_contract.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"

#include <cstdio>
#include <cstdlib>
#include <mutex>

extern "C" objc3_runtime_dispatch_i32_result objc3_runtime_dispatch_i32_checked(
    int receiver, const char *selector, int a0, int a1, int a2, int a3) {
  using objc3c::runtime::ExecuteResolvedRuntimeDispatchTargetStrict;
  using objc3c::runtime::ProcessRuntimeState;
  using objc3c::runtime::RuntimeDispatchTarget;
  using objc3c::runtime::RuntimeDispatchStatusIsSuccess;
  using objc3c::runtime::RuntimeState;
  using objc3c::runtime::ResolveRuntimeDispatchTargetUnlocked;
  using objc3c::runtime::StoreDispatchResultContractUnlocked;

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
