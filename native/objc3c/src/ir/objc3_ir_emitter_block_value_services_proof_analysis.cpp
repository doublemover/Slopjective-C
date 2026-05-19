#include "ir/objc3_ir_emitter_block_value_services_proof_analysis.h"

#include <string>

#include "ir/objc3_ir_compile_time_proof_analysis.h"
#include "ir/objc3_ir_emitter_block_value_services_value_materialization.h"
#include "ir/objc3_ir_emitter_service_contexts.h"
#include "ir/objc3_ir_value_materialization.h"

Objc3IRCompileTimeProofAnalysisContext
BuildObjc3IREmitterCompileTimeProofAnalysisContext(
    const Objc3IREmitterServiceContextState &state,
    const Objc3IREmitterServiceContextCallbacks &callbacks) {
  return Objc3IRCompileTimeProofAnalysisContext{
      state.global_nil_proven_symbols,
      state.global_const_values,
      [state, callbacks](const FunctionContext &callback_ctx,
                         const std::string &name) {
        return LookupObjc3IRVarPtr(
            callback_ctx, name,
            BuildObjc3IREmitterValueMaterializationContext(state, callbacks));
      }};
}
