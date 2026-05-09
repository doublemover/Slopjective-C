#include "ir/objc3_ir_emitter.h"

#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_canonical_literal_pools.h"
#include "ir/objc3_ir_class_receiver_bindings.h"
#include "ir/objc3_ir_concurrency_identity.h"
#include "ir/objc3_ir_emission_helpers.h"
#include "ir/objc3_ir_emitter_context.h"
#include "ir/objc3_ir_emitter_service_contexts.h"
#include "ir/objc3_ir_function_effect_analysis.h"
#include "ir/objc3_ir_function_signature_model.h"
#include "ir/objc3_ir_method_definition_plan.h"
#include "ir/objc3_ir_module_body_orchestration.h"
#include "ir/objc3_ir_module_metadata_publication.h"
#include "ir/objc3_ir_runtime_dispatch_state.h"
#include "ir/objc3_ir_runtime_metadata_emission.h"
#include "ir/objc3_ir_synthetic_method_emission.h"

class Objc3IREmitter {
 public:
  Objc3IREmitter(const Objc3Program &program,
                 const Objc3LoweringContract &lowering_contract,
                 const Objc3IRFrontendMetadata &frontend_metadata)
      : program_(program),
        frontend_metadata_(frontend_metadata),
        runtime_metadata_symbols_(
            BuildObjc3IRRuntimeMetadataSymbols(program.module_name,
                                               frontend_metadata)) {
    if (!TryBuildObjc3LoweringIRBoundary(lowering_contract, lowering_ir_boundary_, boundary_error_)) {
      return;
    }
    vector_signature_function_count_ = CountVectorSignatureFunctions(program_);
    for (const auto &global : program_.globals) {
      globals_.insert(global.name);
    }
    for (const auto &fn : program_.functions) {
      function_arity_[fn.name] = fn.params.size();
      if (fn.is_pure) {
        declared_pure_functions_.insert(fn.name);
      }
      if (!fn.is_prototype && defined_functions_.insert(fn.name).second) {
        function_definitions_.push_back(&fn);
      }
    }
    Objc3IRMethodDefinitionPlan method_definition_plan =
        BuildObjc3IRMethodDefinitionPlan(program_, frontend_metadata_);
    if (!method_definition_plan.error.empty()) {
      boundary_error_ = method_definition_plan.error;
      return;
    }
    method_definitions_ = method_definition_plan.method_definitions;
    direct_dispatch_symbols_by_key_ =
        method_definition_plan.direct_dispatch_symbols_by_key;
    metaprogramming_global_artifacts_ =
        method_definition_plan.metaprogramming_global_artifacts;
    synthesized_property_accessor_count_ =
        method_definition_plan.synthesized_property_accessor_count;
    metaprogramming_derived_method_count_ =
        method_definition_plan.metaprogramming_derived_method_count;
    function_signatures_ = BuildLoweredFunctionSignatures(program_);
    class_receiver_constants_ =
        BuildObjc3IRKnownClassReceiverConstants(program_);
    Objc3IRCanonicalLiteralPools canonical_literal_pools =
        BuildObjc3IRCanonicalLiteralPools(program_, frontend_metadata_);
    selector_pool_globals_ =
        std::move(canonical_literal_pools.selector_pool_globals);
    runtime_string_pool_globals_ =
        std::move(canonical_literal_pools.runtime_string_pool_globals);
    typed_keypath_artifacts_ =
        std::move(canonical_literal_pools.typed_keypath_artifacts);
    Objc3IRFunctionEffectAnalysis function_effect_analysis =
        BuildObjc3IRFunctionEffectAnalysis(
            Objc3IRFunctionEffectAnalysisOptions{
                function_definitions_, globals_, defined_functions_,
                declared_pure_functions_});
    mutable_global_symbols_ =
        std::move(function_effect_analysis.mutable_global_symbols);
    function_effects_ = std::move(function_effect_analysis.function_effects);
    impure_functions_ = std::move(function_effect_analysis.impure_functions);
  }

  bool Emit(std::string &ir, std::string &error) {
    runtime_dispatch_call_state_.Reset();
    synthetic_method_stats_ = {};
    unsupported_fail_closed_path_triggered_ = false;
    unsupported_fail_closed_path_reason_.clear();
    block_function_definitions_.clear();
    emitted_block_invoke_symbols_.clear();
    emitted_block_copy_helper_symbols_.clear();
    emitted_block_dispose_helper_symbols_.clear();

    if (!boundary_error_.empty()) {
      error = boundary_error_;
      return false;
    }
    Objc3IREmitterServiceContextState service_state =
        ServiceContextState();
    Objc3IREmitterServiceContextCallbacks service_callbacks =
        ServiceContextCallbacks();
    Objc3IRModuleBodyOrchestrationOptions module_body_options =
        BuildObjc3IREmitterModuleBodyOrchestrationOptions(service_state);
    std::ostringstream body;
    if (!EmitObjc3IRModuleBodyOrchestration(
            module_body_options,
            BuildObjc3IREmitterModuleBodyOrchestrationCallbacks(
                service_state, service_callbacks),
            body, error)) {
      return false;
    }

    if (unsupported_fail_closed_path_triggered_) {
      error = "lowering encountered unsupported fail-closed path: " + unsupported_fail_closed_path_reason_;
      return false;
    }

    std::ostringstream out;
    EmitObjc3IRModuleMetadataPublication(
        BuildObjc3IREmitterModuleMetadataPublicationOptions(service_state),
        out);
    EmitObjc3IRModuleFrontendMetadataBoundaryPublication(
        module_body_options, out);
    // Historical extraction contract markers retained for fail-closed tooling:
    // out << "declare i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32, ptr";
    // for (std::size_t i = 0; i < lowering_ir_boundary_.runtime_dispatch_arg_slots; ++i) {
    //   out << ", i32";
    // }
    // out << ")\n\n";
    EmitObjc3IRModuleEmissionSurfacePublications(
        module_body_options, synthetic_method_stats_, out);
    out << body.str();
    ir = out.str();
    return true;
  }

 private:
  Objc3IREmitterServiceContextState ServiceContextState() {
    return Objc3IREmitterServiceContextState{
        program_,
        frontend_metadata_,
        lowering_ir_boundary_,
        runtime_metadata_symbols_,
        globals_,
        mutable_global_symbols_,
        global_const_values_,
        global_nil_proven_symbols_,
        defined_functions_,
        declared_pure_functions_,
        function_definitions_,
        method_definitions_,
        metaprogramming_global_artifacts_,
        impure_functions_,
        function_arity_,
        function_signatures_,
        direct_dispatch_symbols_by_key_,
        selector_pool_globals_,
        runtime_string_pool_globals_,
        typed_keypath_artifacts_,
        class_receiver_constants_,
        synthesized_property_accessor_count_,
        vector_signature_function_count_,
        block_function_definitions_,
        emitted_block_invoke_symbols_,
        emitted_block_copy_helper_symbols_,
        emitted_block_dispose_helper_symbols_,
        runtime_dispatch_call_state_,
        synthetic_method_stats_};
  }

  Objc3IREmitterServiceContextCallbacks ServiceContextCallbacks() const {
    return BuildObjc3IREmitterServiceContextCallbacks(
        [this](const std::string &reason) {
          return EmitUnsupportedI32Value(reason);
        });
  }

  std::string EmitUnsupportedI32Value(const std::string &reason) const {
    if (!unsupported_fail_closed_path_triggered_) {
      unsupported_fail_closed_path_triggered_ = true;
      unsupported_fail_closed_path_reason_ = reason;
    }
    return "poison";
  }

  const Objc3Program &program_;
  Objc3IRFrontendMetadata frontend_metadata_;
  Objc3IRRuntimeMetadataSymbols runtime_metadata_symbols_;
  Objc3LoweringIRBoundary lowering_ir_boundary_;
  std::string boundary_error_;
  std::unordered_set<std::string> globals_;
  std::unordered_set<std::string> mutable_global_symbols_;
  std::unordered_map<std::string, int> global_const_values_;
  std::unordered_set<std::string> global_nil_proven_symbols_;
  std::unordered_set<std::string> defined_functions_;
  std::unordered_set<std::string> declared_pure_functions_;
  std::vector<const FunctionDecl *> function_definitions_;
  std::vector<Objc3IRMethodDefinition> method_definitions_;
  std::size_t synthesized_property_accessor_count_ = 0;
  std::vector<Objc3IRMetaprogrammingGlobalArtifact> metaprogramming_global_artifacts_;
  std::size_t metaprogramming_derived_method_count_ = 0;
  std::unordered_map<std::string, FunctionEffectInfo> function_effects_;
  std::unordered_set<std::string> impure_functions_;
  std::unordered_map<std::string, std::size_t> function_arity_;
  std::map<std::string, LoweredFunctionSignature> function_signatures_;
  std::unordered_map<std::string, std::string> direct_dispatch_symbols_by_key_;
  std::map<std::string, std::string> selector_pool_globals_;
  std::map<std::string, std::string> runtime_string_pool_globals_;
  std::map<std::string, TypedKeyPathArtifact> typed_keypath_artifacts_;
  std::unordered_map<std::string, int> class_receiver_constants_;
  std::size_t vector_signature_function_count_ = 0;
  mutable std::vector<std::string> block_function_definitions_;
  mutable std::unordered_set<std::string> emitted_block_invoke_symbols_;
  mutable std::unordered_set<std::string> emitted_block_copy_helper_symbols_;
  mutable std::unordered_set<std::string> emitted_block_dispose_helper_symbols_;
  mutable Objc3IRRuntimeDispatchCallState runtime_dispatch_call_state_;
  mutable Objc3IRSyntheticMethodEmissionStats synthetic_method_stats_;
  mutable bool unsupported_fail_closed_path_triggered_ = false;
  mutable std::string unsupported_fail_closed_path_reason_;
};

bool EmitObjc3IRText(const Objc3Program &program,
                     const Objc3LoweringContract &lowering_contract,
                     const Objc3IRFrontendMetadata &frontend_metadata,
                     std::string &ir,
                     std::string &error) {
  Objc3IREmitter emitter(program, lowering_contract, frontend_metadata);
  return emitter.Emit(ir, error);
}
