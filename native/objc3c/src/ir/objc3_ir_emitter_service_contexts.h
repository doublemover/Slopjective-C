#pragma once

#include <cstddef>
#include <functional>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_emitter_context.h"
#include "ir/objc3_ir_function_orchestration.h"
#include "ir/objc3_ir_function_signature_model.h"
#include "ir/objc3_ir_method_definition_plan.h"
#include "ir/objc3_ir_module_body_orchestration.h"
#include "ir/objc3_ir_module_metadata_publication.h"
#include "ir/objc3_ir_runtime_dispatch_state.h"
#include "ir/objc3_ir_runtime_metadata_emission.h"
#include "ir/objc3_ir_synthetic_method_emission.h"
#include "lower/objc3_lowering_contract.h"

struct Objc3IREmitterServiceContextState {
  const Objc3Program &program;
  const Objc3IRFrontendMetadata &frontend_metadata;
  const Objc3LoweringIRBoundary &lowering_ir_boundary;
  const Objc3IRRuntimeMetadataSymbols &runtime_metadata_symbols;
  const std::unordered_set<std::string> &globals;
  const std::unordered_set<std::string> &mutable_global_symbols;
  std::unordered_map<std::string, int> &global_const_values;
  std::unordered_set<std::string> &global_nil_proven_symbols;
  const std::unordered_set<std::string> &defined_functions;
  const std::unordered_set<std::string> &declared_pure_functions;
  const std::vector<const FunctionDecl *> &function_definitions;
  const std::vector<Objc3IRMethodDefinition> &method_definitions;
  const std::vector<Objc3IRMetaprogrammingGlobalArtifact>
      &metaprogramming_global_artifacts;
  const std::unordered_set<std::string> &impure_functions;
  const std::unordered_map<std::string, std::size_t> &function_arity;
  const std::map<std::string, LoweredFunctionSignature> &function_signatures;
  const std::unordered_map<std::string, std::string>
      &direct_dispatch_symbols_by_key;
  const std::map<std::string, std::string> &selector_pool_globals;
  const std::map<std::string, std::string> &runtime_string_pool_globals;
  const std::map<std::string, TypedKeyPathArtifact> &typed_keypath_artifacts;
  const std::unordered_map<std::string, int> &class_receiver_constants;
  std::size_t synthesized_property_accessor_count = 0;
  std::size_t vector_signature_function_count = 0;
  std::vector<std::string> &block_function_definitions;
  std::unordered_set<std::string> &emitted_block_invoke_symbols;
  std::unordered_set<std::string> &emitted_block_copy_helper_symbols;
  std::unordered_set<std::string> &emitted_block_dispose_helper_symbols;
  Objc3IRRuntimeDispatchCallState &runtime_dispatch_call_state;
  Objc3IRSyntheticMethodEmissionStats &synthetic_method_stats;
};

struct Objc3IREmitterServiceContextCallbacks {
  std::function<std::string(FunctionContext &ctx)> new_temp;
  std::function<std::string(FunctionContext &ctx, const std::string &prefix)>
      new_label;
  std::function<std::string(const std::string &reason)>
      emit_unsupported_i32_value;
};

Objc3IREmitterServiceContextCallbacks
BuildObjc3IREmitterServiceContextCallbacks(
    std::function<std::string(const std::string &reason)>
        emit_unsupported_i32_value);

#include "ir/objc3_ir_emitter_block_value_services.h"

Objc3IRFunctionOrchestrationOptions
BuildObjc3IREmitterFunctionOrchestrationOptions(
    const Objc3IREmitterServiceContextState &state,
    const Objc3IREmitterServiceContextCallbacks &callbacks);

Objc3IRModuleBodyOrchestrationOptions
BuildObjc3IREmitterModuleBodyOrchestrationOptions(
    const Objc3IREmitterServiceContextState &state);

Objc3IRModuleBodyOrchestrationCallbacks
BuildObjc3IREmitterModuleBodyOrchestrationCallbacks(
    const Objc3IREmitterServiceContextState &state,
    const Objc3IREmitterServiceContextCallbacks &callbacks);

Objc3IRModuleMetadataPublicationOptions
BuildObjc3IREmitterModuleMetadataPublicationOptions(
    const Objc3IREmitterServiceContextState &state);
