#include "lower/objc3_lowering_contract.h"

#include "lower/metadata/lowering_metadata_helpers.h"

#include <sstream>
#include <string>

std::string Objc3OwnershipSystemHelperRuntimeContractSummary() {
  std::ostringstream out;
  // runtime/helper-freeze anchor: lane-D freezes the currently
  // supported Part 8 runtime/helper proof as a reuse boundary over the private
  // ARC/autorelease helper cluster and testing snapshots. Cleanup execution and
  // resource invalidation still ride existing cleanup lowering plus
  // autoreleasepool state; retainable-family helper integration rides the same
  // retain/release/autorelease entrypoints; no public runtime ABI widening or
  // new Part 8 import surface is claimed here.
  out << "contract=" << kObjc3OwnershipSystemHelperRuntimeContractId
      << ";source_contract=" << kObjc3OwnershipSystemExtensionLoweringContractId
      << ";abi_completion_contract="
      << kObjc3OwnershipBorrowedRetainableAbiCompletionContractId
      << ";source_model=" << kObjc3OwnershipSystemHelperRuntimeSourceModel
      << ";abi_model=" << kObjc3OwnershipSystemHelperRuntimeAbiModel
      << ";packaging_model=" << kObjc3OwnershipSystemHelperRuntimePackagingModel
      << ";retain_symbol=" << kObjc3RuntimeRetainI32Symbol
      << ";release_symbol=" << kObjc3RuntimeReleaseI32Symbol
      << ";autorelease_symbol=" << kObjc3RuntimeAutoreleaseI32Symbol
      << ";autoreleasepool_push_symbol="
      << kObjc3RuntimePushAutoreleasepoolScopeSymbol
      << ";autoreleasepool_pop_symbol="
      << kObjc3RuntimePopAutoreleasepoolScopeSymbol
      << ";memory_snapshot_symbol="
      << "objc3_runtime_copy_memory_management_state_for_testing"
      << ";arc_debug_snapshot_symbol="
      << "objc3_runtime_copy_arc_debug_state_for_testing"
      << ";fail_closed_model=" << kObjc3OwnershipSystemHelperRuntimeFailClosedModel
      << ";follow_on_surface=objc3c.ownership.systemhelper.runtime.v1";
  return out.str();
}

std::string Objc3OwnershipLiveCleanupRetainableIntegrationSummary() {
  std::ostringstream out;
  // live runtime-integration anchor: the supported Part 8 cleanup /
  // retainable-family slice now proves linked execution through emitted
  // scope-exit cleanup calls and the private ARC/autorelease helper cluster
  // frozen in D001. This remains a narrow executable slice, not a broader
  // borrowed-lifetime or escaping-ownership runtime claim.
  out << "contract=" << kObjc3OwnershipLiveCleanupRetainableIntegrationContractId
      << ";helper_contract=" << kObjc3OwnershipSystemHelperRuntimeContractId
      << ";abi_completion_contract="
      << kObjc3OwnershipBorrowedRetainableAbiCompletionContractId
      << ";source_model="
      << kObjc3OwnershipLiveCleanupRetainableIntegrationSourceModel
      << ";execution_model="
      << kObjc3OwnershipLiveCleanupRetainableIntegrationExecutionModel
      << ";packaging_model="
      << kObjc3OwnershipLiveCleanupRetainableIntegrationPackagingModel
      << ";retain_symbol=" << kObjc3RuntimeRetainI32Symbol
      << ";release_symbol=" << kObjc3RuntimeReleaseI32Symbol
      << ";autorelease_symbol=" << kObjc3RuntimeAutoreleaseI32Symbol
      << ";autoreleasepool_push_symbol="
      << kObjc3RuntimePushAutoreleasepoolScopeSymbol
      << ";autoreleasepool_pop_symbol="
      << kObjc3RuntimePopAutoreleasepoolScopeSymbol
      << ";memory_snapshot_symbol="
      << "objc3_runtime_copy_memory_management_state_for_testing"
      << ";arc_debug_snapshot_symbol="
      << "objc3_runtime_copy_arc_debug_state_for_testing"
      << ";fail_closed_model="
      << kObjc3OwnershipLiveCleanupRetainableIntegrationFailClosedModel
      << ";follow_on_surface=objc3c.ownership.livecleanup.integration.v1";
  return out.str();
}

std::string Objc3ConcurrencyContinuationRuntimeHelperSummary() {
  std::ostringstream out;
  // continuation/runtime-helper anchor: lane-D freezes the first
  // real private Part 7 helper ABI. The helper cluster now allocates logical
  // continuation handles and supports deterministic handoff/resume/cancel
  // probes even though the current direct-call async lowering slice still does
  // not consume those helpers for live suspension.
  out << "contract=" << kObjc3ConcurrencyContinuationRuntimeHelperContractId
      << ";source_contract="
      << "objc3c.concurrency.continuation.abi.async.lowering.contract.v1"
      << ";direct_call_contract="
      << "objc3c.concurrency.async.direct.call.lowering.v1"
      << ";cleanup_contract="
      << "objc3c.concurrency.suspension.autorelease.cleanup.integration.v1"
      << ";source_model=" << kObjc3ConcurrencyContinuationRuntimeHelperSourceModel
      << ";abi_model=" << kObjc3ConcurrencyContinuationRuntimeHelperAbiModel
      << ";execution_model="
      << kObjc3ConcurrencyContinuationRuntimeHelperExecutionModel
      << ";allocate_symbol="
      << kObjc3RuntimeAllocateAsyncContinuationI32Symbol
      << ";handoff_symbol="
      << kObjc3RuntimeHandoffAsyncContinuationToExecutorI32Symbol
      << ";resume_symbol=" << kObjc3RuntimeResumeAsyncContinuationI32Symbol
      << ";cancel_symbol=" << kObjc3RuntimeCancelAsyncContinuationI32Symbol
      << ";snapshot_symbol=objc3_runtime_copy_async_continuation_state_for_testing"
      << ";fail_closed_model="
      << kObjc3ConcurrencyContinuationRuntimeHelperFailClosedModel
      << ";follow_on_surface=objc3c.concurrency.continuation.runtimehelpersurface.v1";
  return out.str();
}

std::string Objc3ConcurrencyLiveContinuationRuntimeIntegrationSummary() {
  std::ostringstream out;
  // live continuation/runtime integration anchor: the supported
  // non-suspending async slice now executes through the private continuation
  // helper cluster rather than merely advertising its ABI boundary in IR.
  out << "contract=" << kObjc3ConcurrencyLiveContinuationRuntimeIntegrationContractId
      << ";helper_contract=" << kObjc3ConcurrencyContinuationRuntimeHelperContractId
      << ";direct_call_contract="
      << "objc3c.concurrency.async.direct.call.lowering.v1"
      << ";cleanup_contract="
      << "objc3c.concurrency.suspension.autorelease.cleanup.integration.v1"
      << ";source_model="
      << kObjc3ConcurrencyLiveContinuationRuntimeIntegrationSourceModel
      << ";execution_model="
      << kObjc3ConcurrencyLiveContinuationRuntimeIntegrationExecutionModel
      << ";packaging_model="
      << kObjc3ConcurrencyLiveContinuationRuntimeIntegrationPackagingModel
      << ";allocate_symbol="
      << kObjc3RuntimeAllocateAsyncContinuationI32Symbol
      << ";handoff_symbol="
      << kObjc3RuntimeHandoffAsyncContinuationToExecutorI32Symbol
      << ";resume_symbol=" << kObjc3RuntimeResumeAsyncContinuationI32Symbol
      << ";cancel_symbol=" << kObjc3RuntimeCancelAsyncContinuationI32Symbol
      << ";snapshot_symbol=objc3_runtime_copy_async_continuation_state_for_testing"
      << ";fail_closed_model="
      << kObjc3ConcurrencyLiveContinuationRuntimeIntegrationFailClosedModel
      << ";follow_on_surface=objc3c.concurrency.continuation.liveruntimeintegration.v1";
  return out.str();
}

std::string Objc3ConcurrencySchedulerExecutorRuntimeSummary() {
  std::ostringstream out;
  out << "contract=" << kObjc3ConcurrencySchedulerExecutorRuntimeContractId
      << ";source_model=" << kObjc3ConcurrencySchedulerExecutorRuntimeSourceModel
      << ";abi_model=" << kObjc3ConcurrencySchedulerExecutorRuntimeAbiModel
      << ";execution_model="
      << kObjc3ConcurrencySchedulerExecutorRuntimeExecutionModel
      << ";packaging_model="
      << kObjc3ConcurrencySchedulerExecutorRuntimePackagingModel
      << ";helper_cluster="
      << "spawn_task,enter_task_group_scope,add_task_group_task,"
         "wait_task_group_next,cancel_task_group,task_is_cancelled,"
         "task_on_cancel,executor_hop"
      << ";snapshot_symbol=objc3_runtime_copy_task_runtime_state_for_testing"
      << ";fail_closed_model="
      << kObjc3ConcurrencySchedulerExecutorRuntimeFailClosedModel
      << ";follow_on_surface=objc3c.concurrency.schedulerexecutor.runtime.v1";
  return out.str();
}

std::string Objc3ConcurrencyLiveTaskRuntimeIntegrationSummary() {
  std::ostringstream out;
  // live task runtime anchor: the private helper cluster frozen in
  // D001 now serves as a live execution surface for the supported task spawn,
  // task-group, cancellation, and executor-hop slice rather than only an ABI
  // contract. Front-door metadata export gates remain deferred outside D002.
  out << "contract=" << kObjc3ConcurrencyLiveTaskRuntimeIntegrationContractId
      << ";helper_contract=" << kObjc3ConcurrencySchedulerExecutorRuntimeContractId
      << ";lowering_contract="
      << "objc3c.concurrency.task.runtime.lowering.implementation.v1"
      << ";abi_contract="
      << "objc3c.concurrency.task.runtime.abi.completion.v1"
      << ";source_model=" << kObjc3ConcurrencyLiveTaskRuntimeIntegrationSourceModel
      << ";execution_model="
      << kObjc3ConcurrencyLiveTaskRuntimeIntegrationExecutionModel
      << ";packaging_model="
      << kObjc3ConcurrencyLiveTaskRuntimeIntegrationPackagingModel
      << ";spawn_symbol=" << kObjc3RuntimeSpawnTaskI32Symbol
      << ";scope_symbol=" << kObjc3RuntimeEnterTaskGroupScopeI32Symbol
      << ";add_symbol=" << kObjc3RuntimeAddTaskGroupTaskI32Symbol
      << ";wait_symbol=" << kObjc3RuntimeWaitTaskGroupNextI32Symbol
      << ";cancel_symbol=" << kObjc3RuntimeCancelTaskGroupI32Symbol
      << ";cancelled_symbol=" << kObjc3RuntimeTaskIsCancelledI32Symbol
      << ";on_cancel_symbol=" << kObjc3RuntimeTaskOnCancelI32Symbol
      << ";executor_hop_symbol=" << kObjc3RuntimeExecutorHopI32Symbol
      << ";snapshot_symbol=objc3_runtime_copy_task_runtime_state_for_testing"
      << ";fail_closed_model="
      << kObjc3ConcurrencyLiveTaskRuntimeIntegrationFailClosedModel
      << ";follow_on_surface=objc3c.concurrency.taskruntime.liveintegration.v1";
  return out.str();
}

std::string Objc3ConcurrencyTaskRuntimeHardeningSummary() {
  std::ostringstream out;
  // hardening anchor: live task helper execution now carries one
  // explicit edge-case/runtime-stability packet for cancellation cleanup,
  // autorelease scopes, and reset-stable replay proof.
  out << "contract=" << kObjc3ConcurrencyTaskRuntimeHardeningContractId
      << ";live_runtime_contract="
      << kObjc3ConcurrencyLiveTaskRuntimeIntegrationContractId
      << ";source_model=" << kObjc3ConcurrencyTaskRuntimeHardeningSourceModel
      << ";execution_model=" << kObjc3ConcurrencyTaskRuntimeHardeningExecutionModel
      << ";packaging_model=" << kObjc3ConcurrencyTaskRuntimeHardeningPackagingModel
      << ";reset_symbol=objc3_runtime_reset_for_testing"
      << ";task_snapshot_symbol=objc3_runtime_copy_task_runtime_state_for_testing"
      << ";memory_snapshot_symbol=objc3_runtime_copy_memory_management_state_for_testing"
      << ";arc_snapshot_symbol=objc3_runtime_copy_arc_debug_state_for_testing"
      << ";push_scope_symbol=" << kObjc3RuntimePushAutoreleasepoolScopeSymbol
      << ";pop_scope_symbol=" << kObjc3RuntimePopAutoreleasepoolScopeSymbol
      << ";fail_closed_model=" << kObjc3ConcurrencyTaskRuntimeHardeningFailClosedModel
      << ";follow_on_surface=objc3c.concurrency.taskruntime.hardening.v1";
  return out.str();
}
