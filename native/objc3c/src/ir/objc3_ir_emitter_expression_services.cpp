#include "ir/objc3_ir_emitter_expression_services.h"

#include "ir/objc3_ir_emitter_expression_services_callbacks.h"
#include "ir/objc3_ir_emitter_service_contexts.h"
#include "ir/objc3_ir_expression_call_orchestration.h"
#include "ir/objc3_ir_frontend_metadata.h"

Objc3IRExpressionCallEmissionOptions
BuildObjc3IREmitterExpressionCallEmissionOptions(
    const Objc3IREmitterServiceContextState &state,
    const Objc3IREmitterServiceContextCallbacks &callbacks) {
  return Objc3IRExpressionCallEmissionOptions{
      state.selector_pool_globals,
      state.runtime_string_pool_globals,
      state.class_receiver_constants,
      state.direct_dispatch_symbols_by_key,
      state.direct_dispatch_signatures_by_key,
      state.runtime_dispatch_return_types_by_key,
      state.runtime_dispatch_return_value_optional_carriers_by_key,
      state.runtime_dispatch_superclass_by_name,
      state.lowering_ir_boundary.runtime_dispatch_arg_slots,
      state.lowering_ir_boundary.runtime_dispatch_symbol,
      state.runtime_dispatch_call_state,
      state.frontend_metadata.arc_mode_enabled,
      state.defined_functions,
      state.declared_pure_functions,
      state.impure_functions,
      BuildObjc3IREmitterExpressionCallEmissionServices(state, callbacks)};
}
