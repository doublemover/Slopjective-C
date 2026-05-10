#include "ir/objc3_ir_frontend_metadata_publication_concurrency_task_runtime.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRConcurrencyTaskRuntimeMetadataNodes(std::ostringstream &out) {
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
}
