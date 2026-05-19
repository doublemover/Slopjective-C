#include "ir/objc3_ir_emitter_module_services_function.h"

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_emitter_service_contexts.h"
#include "ir/objc3_ir_emitter_statement_services.h"
#include "ir/objc3_ir_function_orchestration.h"

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
