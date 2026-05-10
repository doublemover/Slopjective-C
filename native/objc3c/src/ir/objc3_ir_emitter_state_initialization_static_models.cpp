#include "ir/objc3_ir_emitter_state_initialization_static_models.h"

#include <utility>

#include "ir/objc3_ir_canonical_literal_pools.h"
#include "ir/objc3_ir_class_receiver_bindings.h"
#include "ir/objc3_ir_emitter_state_initialization.h"
#include "ir/objc3_ir_function_signature_model.h"
#include "ir/objc3_ir_method_definition_plan.h"

void CountObjc3IREmitterVectorSignatureState(
    const Objc3Program &program,
    Objc3IREmitterStateInitialization &state) {
  state.vector_signature_function_count = CountVectorSignatureFunctions(program);
}

bool IngestObjc3IREmitterMethodDefinitionState(
    const Objc3Program &program,
    const Objc3IRFrontendMetadata &frontend_metadata,
    Objc3IREmitterStateInitialization &state) {
  Objc3IRMethodDefinitionPlan method_definition_plan =
      BuildObjc3IRMethodDefinitionPlan(program, frontend_metadata);
  if (!method_definition_plan.error.empty()) {
    state.boundary_error = method_definition_plan.error;
    return false;
  }

  state.method_definitions = std::move(method_definition_plan.method_definitions);
  state.direct_dispatch_symbols_by_key =
      std::move(method_definition_plan.direct_dispatch_symbols_by_key);
  state.metaprogramming_global_artifacts =
      std::move(method_definition_plan.metaprogramming_global_artifacts);
  state.synthesized_property_accessor_count =
      method_definition_plan.synthesized_property_accessor_count;
  state.metaprogramming_derived_method_count =
      method_definition_plan.metaprogramming_derived_method_count;
  return true;
}

void BuildObjc3IREmitterStaticModelState(
    const Objc3Program &program,
    const Objc3IRFrontendMetadata &frontend_metadata,
    Objc3IREmitterStateInitialization &state) {
  state.function_signatures = BuildLoweredFunctionSignatures(program);
  state.class_receiver_constants =
      BuildObjc3IRKnownClassReceiverConstants(program);

  Objc3IRCanonicalLiteralPools canonical_literal_pools =
      BuildObjc3IRCanonicalLiteralPools(program, frontend_metadata);
  state.selector_pool_globals =
      std::move(canonical_literal_pools.selector_pool_globals);
  state.runtime_string_pool_globals =
      std::move(canonical_literal_pools.runtime_string_pool_globals);
  state.typed_keypath_artifacts =
      std::move(canonical_literal_pools.typed_keypath_artifacts);
}
