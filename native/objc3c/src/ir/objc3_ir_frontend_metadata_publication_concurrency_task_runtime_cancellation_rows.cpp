#include "ir/objc3_ir_frontend_metadata_publication_concurrency_task_runtime_cancellation_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_concurrency_task_runtime_row_helpers.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRConcurrencyTaskRuntimeCancellationMetadataNode(
    std::ostringstream &out) {
  BeginObjc3IRConcurrencyTaskRuntimeMetadataRow(
      "!94", kObjc3ConcurrencySchedulerExecutorRuntimeContractId, out);
  EmitObjc3IRConcurrencyTaskRuntimeStringField(
      kObjc3ConcurrencySchedulerExecutorRuntimeSourceModel, out);
  EmitObjc3IRConcurrencyTaskRuntimeStringField(
      kObjc3ConcurrencySchedulerExecutorRuntimeAbiModel, out);
  EmitObjc3IRConcurrencyTaskRuntimeStringField(
      kObjc3ConcurrencySchedulerExecutorRuntimeExecutionModel, out);
  EmitObjc3IRConcurrencyTaskRuntimeStringField(
      kObjc3ConcurrencySchedulerExecutorRuntimePackagingModel, out);
  EmitObjc3IRConcurrencyTaskRuntimeStringField(
      kObjc3ConcurrencySchedulerExecutorRuntimeFailClosedModel, out);
  EmitObjc3IRConcurrencyTaskRuntimeCommonSymbolFields(out);
  EndObjc3IRConcurrencyTaskRuntimeMetadataRow(out);
}
