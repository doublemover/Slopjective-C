#include "ir/objc3_ir_emitter_block_value_services_block_lowering.h"

#include <string>

#include "ir/objc3_ir_block_lowering.h"
#include "ir/objc3_ir_emitter_block_value_services_value_materialization.h"
#include "ir/objc3_ir_emitter_expression_services.h"
#include "ir/objc3_ir_emitter_service_contexts.h"
#include "ir/objc3_ir_emitter_statement_services.h"
#include "ir/objc3_ir_expression_call_orchestration.h"
#include "ir/objc3_ir_statement_orchestration.h"
#include "ir/objc3_ir_value_materialization.h"

Objc3IRBlockLoweringContext BuildObjc3IREmitterBlockLoweringContext(
    const Objc3IREmitterServiceContextState &state,
    const Objc3IREmitterServiceContextCallbacks &callbacks) {
  return Objc3IRBlockLoweringContext{
      Objc3IRBlockLoweringState{
          &state.block_function_definitions,
          &state.emitted_block_invoke_symbols,
          &state.emitted_block_copy_helper_symbols,
          &state.emitted_block_dispose_helper_symbols},
      BuildObjc3IRStatementOrchestrationScopeCleanupCallbacks(
          BuildObjc3IREmitterStatementOrchestrationOptions(state, callbacks)),
      Objc3IRBlockLoweringCallbacks{
          [callbacks](const std::string &reason) {
            return callbacks.emit_unsupported_i32_value(reason);
          },
          [state, callbacks](const Expr *expr, FunctionContext &callback_ctx) {
            return EmitObjc3IRExpressionCall(
                expr, callback_ctx,
                BuildObjc3IREmitterExpressionCallEmissionOptions(
                    state, callbacks));
          },
          [state, callbacks](const Stmt *stmt, FunctionContext &callback_ctx) {
            EmitObjc3IRStatementOrchestration(
                stmt, callback_ctx,
                BuildObjc3IREmitterStatementOrchestrationOptions(
                    state, callbacks));
          },
          [state, callbacks](const FunctionContext &callback_ctx,
                             const std::string &name) {
            return LookupObjc3IRVarPtr(
                callback_ctx, name,
                BuildObjc3IREmitterValueMaterializationContext(
                    state, callbacks));
          },
          [state, callbacks](const std::string &name,
                             FunctionContext &callback_ctx) {
            return EmitObjc3IRIdentifierValue(
                name, callback_ctx,
                BuildObjc3IREmitterValueMaterializationContext(
                    state, callbacks));
          }}};
}
