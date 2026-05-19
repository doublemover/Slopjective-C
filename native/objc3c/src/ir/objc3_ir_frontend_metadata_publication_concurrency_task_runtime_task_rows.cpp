#include "ir/objc3_ir_frontend_metadata_publication_concurrency_task_runtime_task_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_concurrency_task_runtime_row_helpers.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRConcurrencyTaskRuntimeAbiMetadataNode(std::ostringstream &out) {
  BeginObjc3IRConcurrencyTaskRuntimeMetadataRow(
      "!93", kObjc3ConcurrencyTaskRuntimeAbiCompletionContractId, out);
  EmitObjc3IRConcurrencyTaskRuntimeStringField(
      kObjc3ConcurrencyTaskRuntimeAbiCompletionSurfacePath, out);
  EmitObjc3IRConcurrencyTaskRuntimeCommonSymbolFields(out);
  EndObjc3IRConcurrencyTaskRuntimeMetadataRow(out);
}
