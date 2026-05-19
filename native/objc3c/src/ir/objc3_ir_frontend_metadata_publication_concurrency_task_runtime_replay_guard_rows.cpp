#include "ir/objc3_ir_frontend_metadata_publication_concurrency_task_runtime_replay_guard_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_concurrency_task_runtime_row_helpers.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRConcurrencyTaskRuntimeReplayGuardMetadataNode(
    std::ostringstream &out) {
  BeginObjc3IRConcurrencyTaskRuntimeMetadataRow(
      "!95", kObjc3ConcurrencyLiveTaskRuntimeIntegrationContractId, out);
  EmitObjc3IRConcurrencyTaskRuntimeStringField(
      kObjc3ConcurrencyLiveTaskRuntimeIntegrationSourceModel, out);
  EmitObjc3IRConcurrencyTaskRuntimeStringField(
      kObjc3ConcurrencyLiveTaskRuntimeIntegrationExecutionModel, out);
  EmitObjc3IRConcurrencyTaskRuntimeStringField(
      kObjc3ConcurrencyLiveTaskRuntimeIntegrationPackagingModel, out);
  EmitObjc3IRConcurrencyTaskRuntimeStringField(
      kObjc3ConcurrencyLiveTaskRuntimeIntegrationFailClosedModel, out);
  EmitObjc3IRConcurrencyTaskRuntimeCommonSymbolFields(out);
  EndObjc3IRConcurrencyTaskRuntimeMetadataRow(out);
}
