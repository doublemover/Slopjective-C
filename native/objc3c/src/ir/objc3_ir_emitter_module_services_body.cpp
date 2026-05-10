#include "ir/objc3_ir_emitter_module_services_body.h"

#include <string>

#include "ir/objc3_ir_compile_time_proof_analysis.h"
#include "ir/objc3_ir_emitter_block_value_services_proof_analysis.h"
#include "ir/objc3_ir_emitter_module_services_function.h"
#include "ir/objc3_ir_emitter_service_contexts.h"
#include "ir/objc3_ir_module_body_orchestration.h"

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
