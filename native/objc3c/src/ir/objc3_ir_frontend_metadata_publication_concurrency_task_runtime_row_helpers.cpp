#include "ir/objc3_ir_frontend_metadata_publication_concurrency_task_runtime_row_helpers.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "lower/objc3_lowering_contract.h"

void BeginObjc3IRConcurrencyTaskRuntimeMetadataRow(
    const char *metadata_node_id, const std::string &contract_id,
    std::ostringstream &out) {
  out << metadata_node_id << " = !{!\"" << EscapeCStringLiteral(contract_id)
      << "\"";
}

void EmitObjc3IRConcurrencyTaskRuntimeStringField(
    const std::string &field_value, std::ostringstream &out) {
  out << ", !\"" << EscapeCStringLiteral(field_value) << "\"";
}

void EmitObjc3IRConcurrencyTaskRuntimeCommonSymbolFields(std::ostringstream &out) {
  EmitObjc3IRConcurrencyTaskRuntimeStringField(kObjc3RuntimeSpawnTaskI32Symbol,
                                               out);
  EmitObjc3IRConcurrencyTaskRuntimeStringField(
      kObjc3RuntimeEnterTaskGroupScopeI32Symbol, out);
  EmitObjc3IRConcurrencyTaskRuntimeStringField(
      kObjc3RuntimeAddTaskGroupTaskI32Symbol, out);
  EmitObjc3IRConcurrencyTaskRuntimeStringField(
      kObjc3RuntimeWaitTaskGroupNextI32Symbol, out);
  EmitObjc3IRConcurrencyTaskRuntimeStringField(
      kObjc3RuntimeCancelTaskGroupI32Symbol, out);
  EmitObjc3IRConcurrencyTaskRuntimeStringField(
      kObjc3RuntimeTaskIsCancelledI32Symbol, out);
  EmitObjc3IRConcurrencyTaskRuntimeStringField(
      kObjc3RuntimeTaskOnCancelI32Symbol, out);
  EmitObjc3IRConcurrencyTaskRuntimeStringField(kObjc3RuntimeExecutorHopI32Symbol,
                                               out);
  EmitObjc3IRConcurrencyTaskRuntimeStringField(
      "objc3_runtime_copy_task_runtime_state_for_testing", out);
}

void EndObjc3IRConcurrencyTaskRuntimeMetadataRow(std::ostringstream &out) {
  out << "}\n";
}
