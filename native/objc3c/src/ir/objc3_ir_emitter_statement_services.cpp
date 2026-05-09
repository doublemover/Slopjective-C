#include "ir/objc3_ir_emitter_statement_services.h"

#include "ir/objc3_ir_emitter_expression_services.h"
#include "ir/objc3_ir_expression_call_orchestration.h"

Objc3IRStatementOrchestrationOptions
BuildObjc3IREmitterStatementOrchestrationOptions(
    const Objc3IREmitterServiceContextState &state,
    const Objc3IREmitterServiceContextCallbacks &callbacks) {
  return Objc3IRStatementOrchestrationOptions{
      state.frontend_metadata.arc_mode_enabled,
      Objc3IRStatementOrchestrationServices{
          [state, callbacks](const Expr *expr, FunctionContext &callback_ctx) {
            return EmitObjc3IRExpressionCall(
                expr, callback_ctx,
                BuildObjc3IREmitterExpressionCallEmissionOptions(
                    state, callbacks));
          },
          callbacks.new_temp,
          callbacks.new_label,
          callbacks.emit_unsupported_i32_value,
          [state, callbacks]() {
            return BuildObjc3IREmitterBlockLoweringContext(state, callbacks);
          },
          [state, callbacks]() {
            return BuildObjc3IREmitterValueMaterializationContext(
                state, callbacks);
          },
          [state, callbacks]() {
            return BuildObjc3IREmitterCompileTimeProofAnalysisContext(
                state, callbacks);
          }}};
}
