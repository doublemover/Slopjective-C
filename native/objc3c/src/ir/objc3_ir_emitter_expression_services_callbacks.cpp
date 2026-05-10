#include "ir/objc3_ir_emitter_expression_services_callbacks.h"

#include <string>

#include "ir/objc3_ir_compile_time_proof_analysis.h"
#include "ir/objc3_ir_emitter_block_value_services_block_lowering.h"
#include "ir/objc3_ir_emitter_block_value_services_proof_analysis.h"
#include "ir/objc3_ir_emitter_block_value_services_value_materialization.h"
#include "ir/objc3_ir_emitter_service_contexts.h"
#include "ir/objc3_ir_emitter_statement_services.h"
#include "ir/objc3_ir_expression_call_orchestration.h"
#include "ir/objc3_ir_function_signature_model.h"
#include "ir/objc3_ir_statement_orchestration.h"
#include "ir/objc3_ir_value_materialization.h"

namespace {

const LoweredFunctionSignature *LookupObjc3IREmitterFunctionSignature(
    const Objc3IREmitterServiceContextState &state,
    const std::string &name) {
  auto signature_it = state.function_signatures.find(name);
  if (signature_it == state.function_signatures.end()) {
    return nullptr;
  }
  return &signature_it->second;
}

}  // namespace

Objc3IRExpressionCallEmissionServices
BuildObjc3IREmitterExpressionCallEmissionServices(
    const Objc3IREmitterServiceContextState &state,
    const Objc3IREmitterServiceContextCallbacks &callbacks) {
  return Objc3IRExpressionCallEmissionServices{
      callbacks.new_temp,
      callbacks.new_label,
      callbacks.emit_unsupported_i32_value,
      [](FunctionContext &callback_ctx) {
        InvalidateObjc3IRGlobalProofState(callback_ctx);
      },
      [state, callbacks](const std::string &name,
                         FunctionContext &callback_ctx) {
        return EmitObjc3IRIdentifierValue(
            name, callback_ctx,
            BuildObjc3IREmitterValueMaterializationContext(state, callbacks));
      },
      [state, callbacks](const Expr &callback_expr) {
        return EmitObjc3IRTypedKeyPathLiteralValue(
            callback_expr,
            BuildObjc3IREmitterValueMaterializationContext(state, callbacks));
      },
      [state](const std::string &name) -> const LoweredFunctionSignature * {
        return LookupObjc3IREmitterFunctionSignature(state, name);
      },
      [state, callbacks]() {
        return BuildObjc3IREmitterBlockLoweringContext(state, callbacks);
      },
      [state, callbacks]() {
        return BuildObjc3IRStatementOrchestrationFunctionLocalContext(
            BuildObjc3IREmitterStatementOrchestrationOptions(state, callbacks));
      },
      [state, callbacks]() {
        return BuildObjc3IREmitterCompileTimeProofAnalysisContext(
            state, callbacks);
      }};
}
