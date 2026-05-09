#pragma once

#include <cstddef>
#include <functional>
#include <iosfwd>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_emitter_context.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_function_orchestration.h"
#include "ir/objc3_ir_function_signature_model.h"
#include "ir/objc3_ir_method_definition_plan.h"
#include "ir/objc3_ir_runtime_dispatch_state.h"
#include "ir/objc3_ir_runtime_metadata_emission.h"
#include "lower/objc3_lowering_contract.h"

struct Objc3IRSyntheticMethodEmissionStats;

struct Objc3IRModuleBodyOrchestrationOptions {
  const Objc3Program &program;
  const Objc3IRFrontendMetadata &frontend_metadata;
  const Objc3LoweringIRBoundary &lowering_ir_boundary;
  const Objc3IRRuntimeMetadataSymbols &runtime_metadata_symbols;
  const std::unordered_set<std::string> &mutable_global_symbols;
  std::unordered_map<std::string, int> &global_const_values;
  std::unordered_set<std::string> &global_nil_proven_symbols;
  const std::vector<Objc3IRMetaprogrammingGlobalArtifact>
      &metaprogramming_global_artifacts;
  const std::vector<const FunctionDecl *> &function_definitions;
  const std::vector<Objc3IRMethodDefinition> &method_definitions;
  const std::vector<std::string> &block_function_definitions;
  const std::map<std::string, LoweredFunctionSignature> &function_signatures;
  const std::unordered_set<std::string> &defined_functions;
  const std::unordered_map<std::string, std::size_t> &function_arity;
  const std::map<std::string, std::string> &selector_pool_globals;
  const std::map<std::string, std::string> &runtime_string_pool_globals;
  const std::map<std::string, TypedKeyPathArtifact> &typed_keypath_artifacts;
  std::size_t synthesized_property_accessor_count = 0;
  Objc3IRRuntimeDispatchCallState &runtime_dispatch_call_state;
};

struct Objc3IRModuleBodyOrchestrationCallbacks {
  std::function<bool(const Expr *expr)> is_compile_time_global_nil_expr;
  std::function<Objc3IRFunctionOrchestrationOptions()>
      build_function_orchestration_options;
  std::function<void(const std::string &reason)>
      mark_unsupported_fail_closed_path;
};

bool EmitObjc3IRModuleBodyOrchestration(
    const Objc3IRModuleBodyOrchestrationOptions &options,
    const Objc3IRModuleBodyOrchestrationCallbacks &callbacks,
    std::ostringstream &body, std::string &error);

void EmitObjc3IRModuleFrontendMetadataBoundaryPublication(
    const Objc3IRModuleBodyOrchestrationOptions &options,
    std::ostringstream &out);

void EmitObjc3IRModuleEmissionSurfacePublications(
    const Objc3IRModuleBodyOrchestrationOptions &options,
    const Objc3IRSyntheticMethodEmissionStats &synthetic_method_stats,
    std::ostringstream &out);
