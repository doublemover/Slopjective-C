#include "ir/objc3_ir_emitter.h"

#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_emitter_context.h"
#include "ir/objc3_ir_emitter_service_contexts.h"
#include "ir/objc3_ir_emitter_state_initialization.h"
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
        frontend_metadata_(frontend_metadata) {
    ApplyStateInitialization(BuildObjc3IREmitterStateInitialization(
        program_, lowering_contract, frontend_metadata_));
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
  void ApplyStateInitialization(
      Objc3IREmitterStateInitialization initialization) {
    runtime_metadata_symbols_ =
        std::move(initialization.runtime_metadata_symbols);
    lowering_ir_boundary_ = std::move(initialization.lowering_ir_boundary);
    boundary_error_ = std::move(initialization.boundary_error);
    globals_ = std::move(initialization.globals);
    mutable_global_symbols_ =
        std::move(initialization.mutable_global_symbols);
    defined_functions_ = std::move(initialization.defined_functions);
    declared_pure_functions_ =
        std::move(initialization.declared_pure_functions);
    function_definitions_ = std::move(initialization.function_definitions);
    method_definitions_ = std::move(initialization.method_definitions);
    synthesized_property_accessor_count_ =
        initialization.synthesized_property_accessor_count;
    metaprogramming_global_artifacts_ =
        std::move(initialization.metaprogramming_global_artifacts);
    metaprogramming_derived_method_count_ =
        initialization.metaprogramming_derived_method_count;
    function_effects_ = std::move(initialization.function_effects);
    impure_functions_ = std::move(initialization.impure_functions);
    function_arity_ = std::move(initialization.function_arity);
    function_signatures_ = std::move(initialization.function_signatures);
    direct_dispatch_symbols_by_key_ =
        std::move(initialization.direct_dispatch_symbols_by_key);
    selector_pool_globals_ =
        std::move(initialization.selector_pool_globals);
    runtime_string_pool_globals_ =
        std::move(initialization.runtime_string_pool_globals);
    typed_keypath_artifacts_ =
        std::move(initialization.typed_keypath_artifacts);
    class_receiver_constants_ =
        std::move(initialization.class_receiver_constants);
    vector_signature_function_count_ =
        initialization.vector_signature_function_count;
  }

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
