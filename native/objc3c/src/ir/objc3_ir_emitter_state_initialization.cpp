#include "ir/objc3_ir_emitter_state_initialization.h"

#include <utility>

#include "ir/objc3_ir_canonical_literal_pools.h"
#include "ir/objc3_ir_class_receiver_bindings.h"
#include "ir/objc3_ir_function_effect_analysis.h"
#include "ir/objc3_ir_function_signature_model.h"
#include "ir/objc3_ir_method_definition_plan.h"
#include "ir/objc3_ir_runtime_metadata_emission.h"

namespace {

void DiscoverObjc3IREmitterGlobalAndFunctionState(
    const Objc3Program &program,
    Objc3IREmitterStateInitialization &state) {
  for (const auto &global : program.globals) {
    state.globals.insert(global.name);
  }

  for (const auto &fn : program.functions) {
    state.function_arity[fn.name] = fn.params.size();
    if (fn.is_pure) {
      state.declared_pure_functions.insert(fn.name);
    }
    if (!fn.is_prototype && state.defined_functions.insert(fn.name).second) {
      state.function_definitions.push_back(&fn);
    }
  }
}

bool IngestObjc3IREmitterMethodDefinitionPlan(
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

void BuildObjc3IREmitterLiteralPoolState(
    const Objc3Program &program,
    const Objc3IRFrontendMetadata &frontend_metadata,
    Objc3IREmitterStateInitialization &state) {
  Objc3IRCanonicalLiteralPools canonical_literal_pools =
      BuildObjc3IRCanonicalLiteralPools(program, frontend_metadata);
  state.selector_pool_globals =
      std::move(canonical_literal_pools.selector_pool_globals);
  state.runtime_string_pool_globals =
      std::move(canonical_literal_pools.runtime_string_pool_globals);
  state.typed_keypath_artifacts =
      std::move(canonical_literal_pools.typed_keypath_artifacts);
}

void BuildObjc3IREmitterFunctionEffectState(
    Objc3IREmitterStateInitialization &state) {
  Objc3IRFunctionEffectAnalysis function_effect_analysis =
      BuildObjc3IRFunctionEffectAnalysis(
          Objc3IRFunctionEffectAnalysisOptions{
              state.function_definitions, state.globals,
              state.defined_functions, state.declared_pure_functions});
  state.mutable_global_symbols =
      std::move(function_effect_analysis.mutable_global_symbols);
  state.function_effects = std::move(function_effect_analysis.function_effects);
  state.impure_functions = std::move(function_effect_analysis.impure_functions);
}

}  // namespace

Objc3IREmitterStateInitialization BuildObjc3IREmitterStateInitialization(
    const Objc3Program &program,
    const Objc3LoweringContract &lowering_contract,
    const Objc3IRFrontendMetadata &frontend_metadata) {
  Objc3IREmitterStateInitialization state;
  state.runtime_metadata_symbols =
      BuildObjc3IRRuntimeMetadataSymbols(program.module_name,
                                         frontend_metadata);
  if (!TryBuildObjc3LoweringIRBoundary(
          lowering_contract, state.lowering_ir_boundary,
          state.boundary_error)) {
    return state;
  }

  state.vector_signature_function_count = CountVectorSignatureFunctions(program);
  DiscoverObjc3IREmitterGlobalAndFunctionState(program, state);
  if (!IngestObjc3IREmitterMethodDefinitionPlan(
          program, frontend_metadata, state)) {
    return state;
  }

  state.function_signatures = BuildLoweredFunctionSignatures(program);
  state.class_receiver_constants =
      BuildObjc3IRKnownClassReceiverConstants(program);
  BuildObjc3IREmitterLiteralPoolState(program, frontend_metadata, state);
  BuildObjc3IREmitterFunctionEffectState(state);
  return state;
}
