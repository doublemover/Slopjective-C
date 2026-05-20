#pragma once

#include <cstddef>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ir/objc3_ir_emitter_context.h"
#include "ir/objc3_ir_function_signature_model.h"
#include "ir/objc3_ir_method_definition_plan.h"
#include "ir/objc3_ir_runtime_metadata_emission.h"
#include "lower/objc3_lowering_contract.h"

struct FunctionDecl;
struct Objc3IRFrontendMetadata;
struct Objc3Program;

struct Objc3IREmitterStateInitialization {
  Objc3IRRuntimeMetadataSymbols runtime_metadata_symbols;
  Objc3LoweringIRBoundary lowering_ir_boundary;
  std::string boundary_error;
  std::unordered_set<std::string> globals;
  std::unordered_set<std::string> mutable_global_symbols;
  std::unordered_set<std::string> defined_functions;
  std::unordered_set<std::string> declared_pure_functions;
  std::vector<const FunctionDecl *> function_definitions;
  std::vector<Objc3IRMethodDefinition> method_definitions;
  std::size_t synthesized_property_accessor_count = 0;
  std::vector<Objc3IRMetaprogrammingGlobalArtifact>
      metaprogramming_global_artifacts;
  std::size_t metaprogramming_derived_method_count = 0;
  std::unordered_map<std::string, FunctionEffectInfo> function_effects;
  std::unordered_set<std::string> impure_functions;
  std::unordered_map<std::string, std::size_t> function_arity;
  std::map<std::string, LoweredFunctionSignature> function_signatures;
  std::unordered_map<std::string, std::string> direct_dispatch_symbols_by_key;
  std::unordered_map<std::string, Objc3IRDirectDispatchSignature>
      direct_dispatch_signatures_by_key;
  std::unordered_map<std::string, ValueType> runtime_dispatch_return_types_by_key;
  std::unordered_map<std::string, std::string> runtime_dispatch_superclass_by_name;
  std::map<std::string, std::string> selector_pool_globals;
  std::map<std::string, std::string> runtime_string_pool_globals;
  std::map<std::string, TypedKeyPathArtifact> typed_keypath_artifacts;
  std::unordered_map<std::string, int> class_receiver_constants;
  std::size_t vector_signature_function_count = 0;
};

Objc3IREmitterStateInitialization BuildObjc3IREmitterStateInitialization(
    const Objc3Program &program,
    const Objc3LoweringContract &lowering_contract,
    const Objc3IRFrontendMetadata &frontend_metadata);
