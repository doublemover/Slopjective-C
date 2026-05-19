#include "ir/objc3_ir_emitter_state_initialization_function_state.h"

#include <utility>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_emitter_state_initialization.h"
#include "ir/objc3_ir_function_effect_analysis.h"

void DiscoverObjc3IREmitterFunctionState(
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
