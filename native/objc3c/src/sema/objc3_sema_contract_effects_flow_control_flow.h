#pragma once

#include <cstddef>
#include <string>

inline constexpr const char
    *kObjc3ControlFlowControlFlowSemanticModelFrontendDependencyContractId =
        "objc3c.control_flow.control.flow.source.closure.v1";
inline constexpr const char *kObjc3ControlFlowControlFlowSemanticModelContractId =
    "objc3c.control_flow.control.flow.semantic.model.v1";
inline constexpr const char *kObjc3ControlFlowControlFlowSemanticModelSurfacePath =
    "frontend.pipeline.semantic_surface.objc_control_flow_control_flow_semantic_model";
inline constexpr const char *kObjc3ControlFlowControlFlowSemanticModelRule =
    "guard-refinement-plus-statement-match-exhaustiveness-and-defer-legality-semantics-are-live-while-defer-cleanup-lowering-remains-a-later-lane-c-runtime-step";
inline constexpr const char *kObjc3ControlFlowControlFlowSemanticModelDeferRule =
    "defer-statement-lifo-cleanup-order-and-defer-mediated-nonlocal-exit-legality-are-live-in-sema-while-runtime-cleanup-execution-remains-deferred-to-later-lowering-and-runtime-work";
inline constexpr const char *kObjc3ControlFlowControlFlowSemanticModelMatchRule =
    "statement-match-and-expression-match-enforce-catch-all-bool-result-case-and-guarded-pattern-exhaustiveness-with-case-local-binding-scopes-and-expression-result-typing-while-type-test-patterns-remain-fail-closed";
inline constexpr const char *kObjc3ControlFlowControlFlowSemanticModelExitRule =
    "break-and-continue-restrictions-plus-defer-body-nonlocal-exit-legality-are-live-in-sema-while-runtime-cleanup-execution-remains-deferred";

struct Objc3ControlFlowControlFlowSemanticModelSummary {
  std::string contract_id = kObjc3ControlFlowControlFlowSemanticModelContractId;
  std::string frontend_dependency_contract_id =
      kObjc3ControlFlowControlFlowSemanticModelFrontendDependencyContractId;
  std::string surface_path =
      kObjc3ControlFlowControlFlowSemanticModelSurfacePath;
  std::string semantic_model = kObjc3ControlFlowControlFlowSemanticModelRule;
  std::string defer_model = kObjc3ControlFlowControlFlowSemanticModelDeferRule;
  std::string match_model = kObjc3ControlFlowControlFlowSemanticModelMatchRule;
  std::string non_local_exit_model =
      kObjc3ControlFlowControlFlowSemanticModelExitRule;
  std::size_t guard_binding_semantic_sites = 0;
  std::size_t guard_binding_clause_semantic_sites = 0;
  std::size_t guard_condition_statement_sites = 0;
  std::size_t guard_condition_clause_semantic_sites = 0;
  std::size_t guard_exit_enforcement_sites = 0;
  std::size_t guard_refinement_sites = 0;
  std::size_t match_statement_semantic_sites = 0;
  std::size_t match_default_pattern_sites = 0;
  std::size_t match_wildcard_pattern_sites = 0;
  std::size_t match_literal_pattern_sites = 0;
  std::size_t match_binding_scope_sites = 0;
  std::size_t match_result_case_scope_sites = 0;
  std::size_t match_guard_condition_sites = 0;
  std::size_t match_exhaustive_statement_sites = 0;
  std::size_t match_bool_exhaustive_sites = 0;
  std::size_t match_result_case_exhaustive_sites = 0;
  std::size_t match_non_exhaustive_diagnostic_sites = 0;
  std::size_t match_exhaustiveness_deferred_sites = 0;
  std::size_t match_expression_semantic_sites = 0;
  std::size_t match_expression_result_type_sites = 0;
  std::size_t match_expression_guard_condition_sites = 0;
  std::size_t match_expression_binding_scope_sites = 0;
  std::size_t match_expression_result_case_scope_sites = 0;
  std::size_t match_expression_exhaustive_sites = 0;
  std::size_t match_expression_non_exhaustive_diagnostic_sites = 0;
  std::size_t match_expression_lowering_eligible_sites = 0;
  std::size_t match_expression_guard_effect_fail_closed_sites = 0;
  std::size_t match_expression_result_type_mismatch_sites = 0;
  std::size_t defer_statement_semantic_sites = 0;
  std::size_t defer_scope_cleanup_order_sites = 0;
  std::size_t defer_nonlocal_exit_diagnostic_sites = 0;
  std::size_t break_statement_sites = 0;
  std::size_t continue_statement_sites = 0;
  std::size_t break_restriction_diagnostic_sites = 0;
  std::size_t continue_restriction_diagnostic_sites = 0;
  bool source_dependency_required = false;
  bool guard_refinement_semantics_landed = false;
  bool guard_exit_enforcement_landed = false;
  bool match_binding_scope_semantics_landed = false;
  bool match_result_case_scope_semantics_landed = false;
  bool match_exhaustiveness_semantics_landed = false;
  bool match_exhaustiveness_deferred = false;
  bool defer_cleanup_order_semantics_landed = false;
  bool defer_nonlocal_exit_semantics_landed = false;
  bool defer_cleanup_order_deferred = false;
  bool defer_nonlocal_exit_deferred = false;
  bool non_local_exit_restrictions_landed = false;
  bool deterministic = true;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ControlFlowControlFlowSemanticModelSummary(
    const Objc3ControlFlowControlFlowSemanticModelSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.frontend_dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.defer_model.empty() && !summary.match_model.empty() &&
         !summary.non_local_exit_model.empty() &&
         summary.source_dependency_required &&
         summary.guard_refinement_semantics_landed &&
         summary.guard_exit_enforcement_landed &&
         summary.match_binding_scope_semantics_landed &&
         summary.match_result_case_scope_semantics_landed &&
         summary.match_exhaustiveness_semantics_landed &&
         summary.match_expression_result_type_sites <=
             summary.match_expression_semantic_sites &&
         summary.match_expression_lowering_eligible_sites <=
             summary.match_expression_semantic_sites &&
         summary.match_expression_guard_effect_fail_closed_sites <=
             summary.match_expression_semantic_sites &&
         summary.match_expression_result_type_mismatch_sites <=
             summary.match_expression_semantic_sites &&
         !summary.match_exhaustiveness_deferred &&
         summary.defer_cleanup_order_semantics_landed &&
         summary.defer_nonlocal_exit_semantics_landed &&
         !summary.defer_cleanup_order_deferred &&
         !summary.defer_nonlocal_exit_deferred &&
         summary.non_local_exit_restrictions_landed &&
         summary.deterministic && summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}
