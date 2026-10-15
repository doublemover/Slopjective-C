#include "ir/objc3_ir_emitter_statement_services.h"

#include "ir/objc3_ir_emitter_block_value_services.h"
#include "ir/objc3_ir_emitter_expression_services.h"
#include "ir/objc3_ir_emitter_service_contexts.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_expression_call_orchestration.h"
#include "ir/objc3_ir_function_signature_model.h"
#include "ir/objc3_ir_statement_orchestration.h"

namespace {

const LoweredFunctionSignature *LookupObjc3IRStatementFunctionSignature(
    const Objc3IREmitterServiceContextState &state,
    const std::string &name) {
  auto signature_it = state.function_signatures.find(name);
  if (signature_it == state.function_signatures.end()) {
    return nullptr;
  }
  return &signature_it->second;
}

}  // namespace

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
          },
          [state](const std::string &name)
              -> const LoweredFunctionSignature * {
            return LookupObjc3IRStatementFunctionSignature(state, name);
          }}};
}
