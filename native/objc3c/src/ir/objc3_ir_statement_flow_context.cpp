#include "ir/objc3_ir_statement_flow_context.h"

Objc3IRScopeCleanupEmissionCallbacks
BuildObjc3IRStatementFlowScopeCleanupCallbacks(
    const Objc3IRStatementFlowContextServices &services) {
  return Objc3IRScopeCleanupEmissionCallbacks{
      services.new_temp, services.new_label, services.emit_statement};
}

Objc3IRFunctionLocalFlowContext BuildObjc3IRStatementFlowFunctionLocalContext(
    bool arc_mode_enabled,
    const Objc3IRStatementFlowContextServices &services) {
  return Objc3IRFunctionLocalFlowContext{
      arc_mode_enabled,
      BuildObjc3IRStatementFlowScopeCleanupCallbacks(services)};
}
