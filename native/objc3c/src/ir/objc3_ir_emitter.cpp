#include "ir/objc3_ir_emitter.h"

#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_block_lowering.h"
#include "ir/objc3_ir_canonical_literal_pools.h"
#include "ir/objc3_ir_class_receiver_bindings.h"
#include "ir/objc3_ir_compile_time_proof_analysis.h"
#include "ir/objc3_ir_concurrency_identity.h"
#include "ir/objc3_ir_emission_helpers.h"
#include "ir/objc3_ir_emitter_context.h"
#include "ir/objc3_ir_expression_call_orchestration.h"
#include "ir/objc3_ir_function_effect_analysis.h"
#include "ir/objc3_ir_function_orchestration.h"
#include "ir/objc3_ir_method_definition_plan.h"
#include "ir/objc3_ir_module_body_orchestration.h"
#include "ir/objc3_ir_module_metadata_publication.h"
#include "ir/objc3_ir_runtime_dispatch_state.h"
#include "ir/objc3_ir_runtime_metadata_emission.h"
#include "ir/objc3_ir_statement_orchestration.h"
#include "ir/objc3_ir_synthetic_method_emission.h"
#include "ir/objc3_ir_value_materialization.h"
#include "parse/objc3_parse_support.h"

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
    Objc3IRModuleBodyOrchestrationOptions module_body_options =
        ModuleBodyOrchestrationOptions();
    std::ostringstream body;
    if (!EmitObjc3IRModuleBodyOrchestration(
            module_body_options,
            Objc3IRModuleBodyOrchestrationCallbacks{
                [this](const Expr *expr) {
                  return IsObjc3IRCompileTimeGlobalNilExpr(
                      expr, CompileTimeProofAnalysisContext());
                },
                [this]() { return FunctionOrchestrationOptions(); },
                [this](const std::string &reason) {
                  EmitUnsupportedI32Value(reason);
                }},
            body, error)) {
      return false;
    }

    if (unsupported_fail_closed_path_triggered_) {
      error = "lowering encountered unsupported fail-closed path: " + unsupported_fail_closed_path_reason_;
      return false;
    }

    std::ostringstream out;
    EmitObjc3IRModuleMetadataPublication(
        Objc3IRModuleMetadataPublicationOptions{
            program_.module_name, frontend_metadata_, lowering_ir_boundary_,
            synthesized_property_accessor_count_,
            vector_signature_function_count_},
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
  std::string NewTemp(FunctionContext &ctx) const { return "%t" + std::to_string(ctx.temp_counter++); }

  std::string NewLabel(FunctionContext &ctx, const std::string &prefix) const {
    return prefix + std::to_string(ctx.label_counter++);
  }

  Objc3IRBlockLoweringContext BlockLoweringContext() const {
    return Objc3IRBlockLoweringContext{
        Objc3IRBlockLoweringState{
            &block_function_definitions_,
            &emitted_block_invoke_symbols_,
            &emitted_block_copy_helper_symbols_,
            &emitted_block_dispose_helper_symbols_},
        BuildObjc3IRStatementOrchestrationScopeCleanupCallbacks(
            StatementOrchestrationOptions()),
        Objc3IRBlockLoweringCallbacks{
            [this](const std::string &reason) {
              return EmitUnsupportedI32Value(reason);
            },
            [this](const Expr *expr, FunctionContext &callback_ctx) {
              return EmitObjc3IRExpressionCall(
                  expr, callback_ctx, ExpressionCallEmissionOptions());
            },
            [this](const Stmt *stmt, FunctionContext &callback_ctx) {
              EmitObjc3IRStatementOrchestration(
                  stmt, callback_ctx, StatementOrchestrationOptions());
            },
            [this](const FunctionContext &callback_ctx,
                   const std::string &name) {
              return LookupObjc3IRVarPtr(
                  callback_ctx, name, ValueMaterializationContext());
            },
            [this](const std::string &name, FunctionContext &callback_ctx) {
              return EmitObjc3IRIdentifierValue(
                  name, callback_ctx, ValueMaterializationContext());
            }}};
  }

  Objc3IRCompileTimeProofAnalysisContext CompileTimeProofAnalysisContext()
      const {
    return Objc3IRCompileTimeProofAnalysisContext{
        global_nil_proven_symbols_,
        global_const_values_,
        [this](const FunctionContext &callback_ctx,
               const std::string &name) {
          return LookupObjc3IRVarPtr(
              callback_ctx, name, ValueMaterializationContext());
        }};
  }

  Objc3IRValueMaterializationContext ValueMaterializationContext() const {
    return Objc3IRValueMaterializationContext{
        globals_,
        typed_keypath_artifacts_,
        [this](FunctionContext &callback_ctx) {
          return NewTemp(callback_ctx);
        },
        [this](const std::string &reason) {
          return EmitUnsupportedI32Value(reason);
        },
        [this]() { return BlockLoweringContext(); }};
  }

  Objc3IRStatementOrchestrationOptions StatementOrchestrationOptions() const {
    return Objc3IRStatementOrchestrationOptions{
        frontend_metadata_.arc_mode_enabled,
        Objc3IRStatementOrchestrationServices{
            [this](const Expr *expr, FunctionContext &callback_ctx) {
              return EmitObjc3IRExpressionCall(
                  expr, callback_ctx, ExpressionCallEmissionOptions());
            },
            [this](FunctionContext &callback_ctx) {
              return NewTemp(callback_ctx);
            },
            [this](FunctionContext &callback_ctx,
                   const std::string &prefix) {
              return NewLabel(callback_ctx, prefix);
            },
            [this](const std::string &reason) {
              return EmitUnsupportedI32Value(reason);
            },
            [this]() { return BlockLoweringContext(); },
            [this]() { return ValueMaterializationContext(); },
            [this]() { return CompileTimeProofAnalysisContext(); }}};
  }

  Objc3IRExpressionCallEmissionOptions ExpressionCallEmissionOptions() const {
    return Objc3IRExpressionCallEmissionOptions{
        selector_pool_globals_,
        class_receiver_constants_,
        direct_dispatch_symbols_by_key_,
        lowering_ir_boundary_.runtime_dispatch_arg_slots,
        lowering_ir_boundary_.runtime_dispatch_symbol,
        runtime_dispatch_call_state_,
        defined_functions_,
        declared_pure_functions_,
        impure_functions_,
        Objc3IRExpressionCallEmissionServices{
            [this](FunctionContext &callback_ctx) {
              return NewTemp(callback_ctx);
            },
            [this](FunctionContext &callback_ctx,
                   const std::string &prefix) {
              return NewLabel(callback_ctx, prefix);
            },
            [this](const std::string &reason) {
              return EmitUnsupportedI32Value(reason);
            },
            [this](FunctionContext &callback_ctx) {
              InvalidateObjc3IRGlobalProofState(callback_ctx);
            },
            [this](const std::string &name, FunctionContext &callback_ctx) {
              return EmitObjc3IRIdentifierValue(
                  name, callback_ctx, ValueMaterializationContext());
            },
            [this](const Expr &callback_expr) {
              return EmitObjc3IRTypedKeyPathLiteralValue(
                  callback_expr, ValueMaterializationContext());
            },
            [this](const std::string &name)
                -> const LoweredFunctionSignature * {
              auto signature_it = function_signatures_.find(name);
              if (signature_it == function_signatures_.end()) {
                return nullptr;
              }
              return &signature_it->second;
            },
            [this]() { return BlockLoweringContext(); },
            [this]() {
              return BuildObjc3IRStatementOrchestrationFunctionLocalContext(
                  StatementOrchestrationOptions());
            },
            [this]() { return CompileTimeProofAnalysisContext(); }}};
  }

  Objc3IRFunctionOrchestrationOptions FunctionOrchestrationOptions() const {
    return Objc3IRFunctionOrchestrationOptions{
        program_,
        frontend_metadata_.arc_mode_enabled,
        class_receiver_constants_,
        StatementOrchestrationOptions(),
        synthetic_method_stats_};
  }

  Objc3IRModuleBodyOrchestrationOptions ModuleBodyOrchestrationOptions() {
    return Objc3IRModuleBodyOrchestrationOptions{
        program_,
        frontend_metadata_,
        lowering_ir_boundary_,
        runtime_metadata_symbols_,
        mutable_global_symbols_,
        global_const_values_,
        global_nil_proven_symbols_,
        metaprogramming_global_artifacts_,
        function_definitions_,
        method_definitions_,
        block_function_definitions_,
        function_signatures_,
        defined_functions_,
        function_arity_,
        selector_pool_globals_,
        runtime_string_pool_globals_,
        typed_keypath_artifacts_,
        synthesized_property_accessor_count_,
        runtime_dispatch_call_state_};
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
