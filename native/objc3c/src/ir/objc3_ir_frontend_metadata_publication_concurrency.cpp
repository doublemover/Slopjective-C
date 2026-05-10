#include "ir/objc3_ir_frontend_metadata_publication_concurrency.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRConcurrencyRuntimeMetadataNodes(std::ostringstream &out) {
  out << "!91 = !{!\""
      << EscapeCStringLiteral(kObjc3ConcurrencyContinuationRuntimeHelperContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ConcurrencyContinuationRuntimeHelperSourceModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ConcurrencyContinuationRuntimeHelperAbiModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ConcurrencyContinuationRuntimeHelperExecutionModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeAllocateAsyncContinuationI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeHandoffAsyncContinuationToExecutorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeResumeAsyncContinuationI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ConcurrencyContinuationRuntimeHelperFailClosedModel)
      << "\"}\n";
  out << "!92 = !{!\""
      << EscapeCStringLiteral(
             kObjc3ConcurrencyLiveContinuationRuntimeIntegrationContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ConcurrencyLiveContinuationRuntimeIntegrationSourceModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ConcurrencyLiveContinuationRuntimeIntegrationExecutionModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ConcurrencyLiveContinuationRuntimeIntegrationPackagingModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeAllocateAsyncContinuationI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeHandoffAsyncContinuationToExecutorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeResumeAsyncContinuationI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ConcurrencyLiveContinuationRuntimeIntegrationFailClosedModel)
      << "\"}\n";
  out << "!93 = !{!\""
      << EscapeCStringLiteral(kObjc3ConcurrencyTaskRuntimeAbiCompletionContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ConcurrencyTaskRuntimeAbiCompletionSurfacePath)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeSpawnTaskI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeEnterTaskGroupScopeI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeAddTaskGroupTaskI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeWaitTaskGroupNextI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeCancelTaskGroupI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeTaskIsCancelledI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeTaskOnCancelI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeExecutorHopI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral("objc3_runtime_copy_task_runtime_state_for_testing")
      << "\"}\n";
  out << "!94 = !{!\""
      << EscapeCStringLiteral(kObjc3ConcurrencySchedulerExecutorRuntimeContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ConcurrencySchedulerExecutorRuntimeSourceModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ConcurrencySchedulerExecutorRuntimeAbiModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ConcurrencySchedulerExecutorRuntimeExecutionModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ConcurrencySchedulerExecutorRuntimePackagingModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ConcurrencySchedulerExecutorRuntimeFailClosedModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeSpawnTaskI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeEnterTaskGroupScopeI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeAddTaskGroupTaskI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeWaitTaskGroupNextI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeCancelTaskGroupI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeTaskIsCancelledI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeTaskOnCancelI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeExecutorHopI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral("objc3_runtime_copy_task_runtime_state_for_testing")
      << "\"}\n";
  out << "!95 = !{!\""
      << EscapeCStringLiteral(kObjc3ConcurrencyLiveTaskRuntimeIntegrationContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ConcurrencyLiveTaskRuntimeIntegrationSourceModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ConcurrencyLiveTaskRuntimeIntegrationExecutionModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ConcurrencyLiveTaskRuntimeIntegrationPackagingModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ConcurrencyLiveTaskRuntimeIntegrationFailClosedModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeSpawnTaskI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeEnterTaskGroupScopeI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeAddTaskGroupTaskI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeWaitTaskGroupNextI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeCancelTaskGroupI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeTaskIsCancelledI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeTaskOnCancelI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeExecutorHopI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral("objc3_runtime_copy_task_runtime_state_for_testing")
      << "\"}\n";
  out << "!96 = !{!\""
      << EscapeCStringLiteral(kObjc3ConcurrencyTaskRuntimeHardeningContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ConcurrencyTaskRuntimeHardeningSourceModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ConcurrencyTaskRuntimeHardeningExecutionModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ConcurrencyTaskRuntimeHardeningPackagingModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ConcurrencyTaskRuntimeHardeningFailClosedModel)
      << "\", !\""
      << EscapeCStringLiteral("objc3_runtime_reset_for_testing")
      << "\", !\""
      << EscapeCStringLiteral("objc3_runtime_copy_task_runtime_state_for_testing")
      << "\", !\""
      << EscapeCStringLiteral(
             "objc3_runtime_copy_memory_management_state_for_testing")
      << "\", !\""
      << EscapeCStringLiteral("objc3_runtime_copy_arc_debug_state_for_testing")
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimePushAutoreleasepoolScopeSymbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimePopAutoreleasepoolScopeSymbol)
      << "\"}\n";
}
