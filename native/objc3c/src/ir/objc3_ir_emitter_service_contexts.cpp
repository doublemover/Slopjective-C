#include "ir/objc3_ir_emitter_service_contexts.h"

#include <cstddef>
#include <string>
#include <utility>

#include "ir/objc3_ir_compile_time_proof_analysis.h"
#include "ir/objc3_ir_emitter_block_value_services.h"
#include "ir/objc3_ir_emitter_statement_services.h"

namespace {

std::string NewObjc3IREmitterServiceTemp(FunctionContext &ctx) {
  return "%t" + std::to_string(ctx.temp_counter++);
}

std::string NewObjc3IREmitterServiceLabel(FunctionContext &ctx,
                                          const std::string &prefix) {
  return prefix + std::to_string(ctx.label_counter++);
}

}  // namespace

Objc3IREmitterServiceContextCallbacks
BuildObjc3IREmitterServiceContextCallbacks(
    std::function<std::string(const std::string &reason)>
        emit_unsupported_i32_value) {
  return Objc3IREmitterServiceContextCallbacks{
      NewObjc3IREmitterServiceTemp,
      NewObjc3IREmitterServiceLabel,
      std::move(emit_unsupported_i32_value)};
}

Objc3IRFunctionOrchestrationOptions
BuildObjc3IREmitterFunctionOrchestrationOptions(
    const Objc3IREmitterServiceContextState &state,
    const Objc3IREmitterServiceContextCallbacks &callbacks) {
  return Objc3IRFunctionOrchestrationOptions{
      state.program,
      state.frontend_metadata.arc_mode_enabled,
      state.class_receiver_constants,
      BuildObjc3IREmitterStatementOrchestrationOptions(state, callbacks),
      state.synthetic_method_stats};
}

Objc3IRModuleBodyOrchestrationOptions
BuildObjc3IREmitterModuleBodyOrchestrationOptions(
    const Objc3IREmitterServiceContextState &state) {
  return Objc3IRModuleBodyOrchestrationOptions{
      state.program,
      state.frontend_metadata,
      state.lowering_ir_boundary,
      state.runtime_metadata_symbols,
      state.mutable_global_symbols,
      state.global_const_values,
      state.global_nil_proven_symbols,
      state.metaprogramming_global_artifacts,
      state.function_definitions,
      state.method_definitions,
      state.block_function_definitions,
      state.function_signatures,
      state.defined_functions,
      state.function_arity,
      state.selector_pool_globals,
      state.runtime_string_pool_globals,
      state.typed_keypath_artifacts,
      state.synthesized_property_accessor_count,
      state.runtime_dispatch_call_state};
}

Objc3IRModuleBodyOrchestrationCallbacks
BuildObjc3IREmitterModuleBodyOrchestrationCallbacks(
    const Objc3IREmitterServiceContextState &state,
    const Objc3IREmitterServiceContextCallbacks &callbacks) {
  return Objc3IRModuleBodyOrchestrationCallbacks{
      [state, callbacks](const Expr *expr) {
        return IsObjc3IRCompileTimeGlobalNilExpr(
            expr,
            BuildObjc3IREmitterCompileTimeProofAnalysisContext(
                state, callbacks));
      },
      [state, callbacks]() {
        return BuildObjc3IREmitterFunctionOrchestrationOptions(
            state, callbacks);
      },
      [callbacks](const std::string &reason) {
        (void)callbacks.emit_unsupported_i32_value(reason);
      }};
}

Objc3IRModuleMetadataPublicationOptions
BuildObjc3IREmitterModuleMetadataPublicationOptions(
    const Objc3IREmitterServiceContextState &state) {
  return Objc3IRModuleMetadataPublicationOptions{
      state.program.module_name,
      state.frontend_metadata,
      state.lowering_ir_boundary,
      state.synthesized_property_accessor_count,
      state.vector_signature_function_count};
}
