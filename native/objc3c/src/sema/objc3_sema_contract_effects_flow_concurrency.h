#pragma once

#include <cstddef>
#include <string>

inline constexpr const char *kObjc3EffectsOwnershipSemanticModelContractId =
    "objc3c.effects.ownership.semantic.model.closure.v1";
inline constexpr const char *kObjc3EffectsOwnershipSemanticModelSurfacePath =
    "frontend.pipeline.semantic_surface.objc_effects_ownership_semantic_model";
inline constexpr const char *kObjc3EffectsOwnershipSemanticModelRule =
    "arc-block-throws-async-actor-and-foreign-boundary-semantics-share-one-deterministic-effects-and-ownership-summary-rooted-in-live-sema-lowering-surfaces";

struct Objc3EffectsOwnershipSemanticModelSummary {
  std::string contract_id = kObjc3EffectsOwnershipSemanticModelContractId;
  std::string surface_path = kObjc3EffectsOwnershipSemanticModelSurfacePath;
  std::string semantic_model = kObjc3EffectsOwnershipSemanticModelRule;
  std::size_t arc_ownership_qualified_sites = 0;
  std::size_t retain_insertion_sites = 0;
  std::size_t release_insertion_sites = 0;
  std::size_t autorelease_insertion_sites = 0;
  std::size_t weak_zeroing_sites = 0;
  std::size_t unowned_reference_sites = 0;
  std::size_t weak_unowned_conflict_sites = 0;
  std::size_t autoreleasepool_scope_sites = 0;
  std::size_t cleanup_order_exit_sites = 0;
  std::size_t block_literal_sites = 0;
  std::size_t stack_to_heap_promotion_sites = 0;
  std::size_t byref_forwarding_cell_sites = 0;
  std::size_t copy_helper_required_sites = 0;
  std::size_t dispose_helper_required_sites = 0;
  std::size_t copy_helper_symbolized_sites = 0;
  std::size_t dispose_helper_symbolized_sites = 0;
  std::size_t captured_object_lifetime_sites = 0;
  std::size_t throws_propagation_sites = 0;
  std::size_t unwind_cleanup_sites = 0;
  std::size_t bridged_error_sites = 0;
  std::size_t nested_cleanup_sites = 0;
  std::size_t foreign_boundary_sites = 0;
  std::size_t async_continuation_sites = 0;
  std::size_t continuation_resume_sites = 0;
  std::size_t continuation_suspend_sites = 0;
  std::size_t async_state_machine_sites = 0;
  std::size_t cancellation_propagation_sites = 0;
  std::size_t actor_isolation_sites = 0;
  std::size_t actor_hop_sites = 0;
  std::size_t sendability_check_sites = 0;
  std::size_t reentrancy_policy_sites = 0;
  std::size_t imported_actor_api_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool arc_semantics_landed = false;
  bool block_escape_semantics_landed = false;
  bool throws_cleanup_semantics_landed = false;
  bool async_task_semantics_landed = false;
  bool actor_semantics_landed = false;
  bool foreign_boundary_semantics_landed = false;
  bool deterministic = true;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

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
    "statement-match-enforces-catch-all-bool-and-result-case-exhaustiveness-with-case-local-binding-scopes-while-result-payload-typing-remains-deferred";
inline constexpr const char *kObjc3ControlFlowControlFlowSemanticModelExitRule =
    "break-and-continue-restrictions-plus-defer-body-nonlocal-exit-legality-are-live-in-sema-while-runtime-cleanup-execution-remains-deferred";

struct Objc3ControlFlowControlFlowSemanticModelSummary {
  std::string contract_id = kObjc3ControlFlowControlFlowSemanticModelContractId;
  std::string frontend_dependency_contract_id =
      kObjc3ControlFlowControlFlowSemanticModelFrontendDependencyContractId;
  std::string surface_path = kObjc3ControlFlowControlFlowSemanticModelSurfacePath;
  std::string semantic_model = kObjc3ControlFlowControlFlowSemanticModelRule;
  std::string defer_model = kObjc3ControlFlowControlFlowSemanticModelDeferRule;
  std::string match_model = kObjc3ControlFlowControlFlowSemanticModelMatchRule;
  std::string non_local_exit_model = kObjc3ControlFlowControlFlowSemanticModelExitRule;
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
  std::size_t match_exhaustive_statement_sites = 0;
  std::size_t match_bool_exhaustive_sites = 0;
  std::size_t match_result_case_exhaustive_sites = 0;
  std::size_t match_non_exhaustive_diagnostic_sites = 0;
  std::size_t match_exhaustiveness_deferred_sites = 0;
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
         !summary.match_exhaustiveness_deferred &&
         summary.defer_cleanup_order_semantics_landed &&
         summary.defer_nonlocal_exit_semantics_landed &&
         !summary.defer_cleanup_order_deferred &&
         !summary.defer_nonlocal_exit_deferred &&
         summary.non_local_exit_restrictions_landed &&
         summary.deterministic && summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char *kObjc3ErrorHandlingErrorSemanticModelFrontendDependencyContractId =
    "objc3c.error_handling.error.source.closure.v1";
inline constexpr const char *kObjc3ErrorHandlingErrorSemanticModelContractId =
    "objc3c.error_handling.error.semantic.model.v1";
inline constexpr const char *kObjc3ErrorHandlingErrorSemanticModelSurfacePath =
    "frontend.pipeline.semantic_surface.objc_error_handling_error_semantic_model";
inline constexpr const char *kObjc3ErrorHandlingErrorSemanticModelRule =
    "throws-declaration-semantics-plus-deterministic-result-and-nserror-profile-carriage-are-live-while-try-throw-do-catch-propagation-and-native-error-runtime-behavior-remain-deferred";
inline constexpr const char *kObjc3ErrorHandlingErrorSemanticModelDeferredRule =
    "try-throw-do-catch-postfix-propagation-status-to-error-execution-bridge-temporaries-and-native-thrown-error-abi-remain-fail-closed-or-later-lane-work";

struct Objc3ErrorHandlingErrorSemanticModelSummary {
  std::string contract_id = kObjc3ErrorHandlingErrorSemanticModelContractId;
  std::string frontend_dependency_contract_id =
      kObjc3ErrorHandlingErrorSemanticModelFrontendDependencyContractId;
  std::string surface_path = kObjc3ErrorHandlingErrorSemanticModelSurfacePath;
  std::string semantic_model = kObjc3ErrorHandlingErrorSemanticModelRule;
  std::string deferred_model = kObjc3ErrorHandlingErrorSemanticModelDeferredRule;
  std::size_t throws_declaration_sites = 0;
  std::size_t function_throws_declaration_sites = 0;
  std::size_t method_throws_declaration_sites = 0;
  std::size_t result_like_sites = 0;
  std::size_t result_success_sites = 0;
  std::size_t result_failure_sites = 0;
  std::size_t result_branch_sites = 0;
  std::size_t result_payload_sites = 0;
  std::size_t ns_error_bridging_sites = 0;
  std::size_t ns_error_out_parameter_sites = 0;
  std::size_t ns_error_bridge_path_sites = 0;
  std::size_t objc_nserror_attribute_sites = 0;
  std::size_t objc_status_code_attribute_sites = 0;
  std::size_t status_code_success_clause_sites = 0;
  std::size_t status_code_error_type_clause_sites = 0;
  std::size_t status_code_mapping_clause_sites = 0;
  std::size_t placeholder_throws_propagation_sites = 0;
  std::size_t placeholder_unwind_cleanup_sites = 0;
  bool source_dependency_required = false;
  bool throws_declaration_semantics_landed = false;
  bool result_carrier_profile_semantics_landed = false;
  bool ns_error_bridging_profile_semantics_landed = false;
  bool bridge_marker_semantics_landed = false;
  bool parser_fail_closed_boundary_required = false;
  bool parser_fail_closed_boundary_preserved = false;
  bool propagation_runtime_deferred = false;
  bool status_to_error_runtime_deferred = false;
  bool native_error_abi_deferred = false;
  bool placeholder_throws_summary_carried = false;
  bool deterministic = true;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline constexpr const char *kObjc3ConcurrencyAsyncEffectSuspensionSemanticModelFrontendDependencyContractId =
    "objc3c.concurrency.async.source.closure.v1";
inline constexpr const char *kObjc3ConcurrencyAsyncEffectSuspensionSemanticModelContractId =
    "objc3c.concurrency.async.effect.suspension.semantic.model.v1";
inline constexpr const char *kObjc3ConcurrencyAsyncEffectSuspensionSemanticModelSurfacePath =
    "frontend.pipeline.semantic_surface.objc_concurrency_async_effect_and_suspension_semantic_model";
inline constexpr const char *kObjc3ConcurrencyAsyncEffectSuspensionSemanticModelRule =
    "async-effect-and-await-legality-semantics-plus-deterministic-continuation-suspension-and-concurrency-profile-carriage-are-live-while-runnable-frame-lowering-cleanup-and-executor-runtime-integration-remain-later-runtime-work";
inline constexpr const char *kObjc3ConcurrencyAsyncEffectSuspensionSemanticModelDeferredRule =
    "async-frame-abi-resume-lowering-suspension-cleanup-task-runtime-execution-and-executor-dispatch-remain-deferred-to-later-runtime-lanes";

struct Objc3ConcurrencyAsyncEffectSuspensionSemanticModelSummary {
  std::string contract_id =
      kObjc3ConcurrencyAsyncEffectSuspensionSemanticModelContractId;
  std::string frontend_dependency_contract_id =
      kObjc3ConcurrencyAsyncEffectSuspensionSemanticModelFrontendDependencyContractId;
  std::string surface_path =
      kObjc3ConcurrencyAsyncEffectSuspensionSemanticModelSurfacePath;
  std::string semantic_model =
      kObjc3ConcurrencyAsyncEffectSuspensionSemanticModelRule;
  std::string deferred_model =
      kObjc3ConcurrencyAsyncEffectSuspensionSemanticModelDeferredRule;
  std::size_t async_continuation_sites = 0;
  std::size_t async_keyword_sites = 0;
  std::size_t async_function_sites = 0;
  std::size_t async_method_sites = 0;
  std::size_t executor_attribute_sites = 0;
  std::size_t executor_main_sites = 0;
  std::size_t executor_global_sites = 0;
  std::size_t executor_named_sites = 0;
  std::size_t continuation_allocation_sites = 0;
  std::size_t continuation_resume_sites = 0;
  std::size_t continuation_suspend_sites = 0;
  std::size_t async_state_machine_sites = 0;
  std::size_t await_suspension_sites = 0;
  std::size_t await_keyword_sites = 0;
  std::size_t await_expression_sites = 0;
  std::size_t await_suspension_point_sites = 0;
  std::size_t await_resume_sites = 0;
  std::size_t await_state_machine_sites = 0;
  std::size_t await_continuation_sites = 0;
  std::size_t actor_isolation_sendability_sites = 0;
  std::size_t actor_isolation_decl_sites = 0;
  std::size_t actor_hop_sites = 0;
  std::size_t sendable_annotation_sites = 0;
  std::size_t non_sendable_crossing_sites = 0;
  std::size_t isolation_boundary_sites = 0;
  std::size_t task_runtime_interop_sites = 0;
  std::size_t runtime_hook_sites = 0;
  std::size_t cancellation_check_sites = 0;
  std::size_t cancellation_handler_sites = 0;
  std::size_t suspension_point_sites = 0;
  std::size_t cancellation_propagation_sites = 0;
  std::size_t concurrency_replay_race_guard_sites = 0;
  std::size_t concurrency_replay_sites = 0;
  std::size_t replay_proof_sites = 0;
  std::size_t race_guard_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t actor_isolation_sites = 0;
  std::size_t deterministic_schedule_sites = 0;
  bool source_dependency_required = false;
  bool async_declaration_semantics_landed = false;
  bool executor_affinity_semantics_landed = false;
  bool await_legality_semantics_landed = false;
  bool continuation_profile_semantics_landed = false;
  bool await_suspension_profile_semantics_landed = false;
  bool actor_isolation_sendability_semantics_landed = false;
  bool task_runtime_cancellation_semantics_landed = false;
  bool concurrency_replay_race_guard_semantics_landed = false;
  bool runnable_lowering_deferred = false;
  bool executor_runtime_deferred = false;
  bool deterministic = true;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ConcurrencyAsyncEffectSuspensionSemanticModelSummary(
    const Objc3ConcurrencyAsyncEffectSuspensionSemanticModelSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.frontend_dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() &&
         summary.source_dependency_required &&
         summary.async_declaration_semantics_landed &&
         summary.executor_affinity_semantics_landed &&
         summary.await_legality_semantics_landed &&
         summary.continuation_profile_semantics_landed &&
         summary.await_suspension_profile_semantics_landed &&
         summary.actor_isolation_sendability_semantics_landed &&
         summary.task_runtime_cancellation_semantics_landed &&
         summary.concurrency_replay_race_guard_semantics_landed &&
         summary.runnable_lowering_deferred &&
         summary.executor_runtime_deferred && summary.deterministic &&
         summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char *kObjc3ConcurrencyTaskExecutorCancellationSemanticModelDependencyContractId =
    "objc3c.concurrency.task.group.cancellation.source.closure.v1";
inline constexpr const char *kObjc3ConcurrencyTaskExecutorCancellationSemanticModelContractId =
    "objc3c.concurrency.task.executor.cancellation.semantic.model.v1";
inline constexpr const char *kObjc3ConcurrencyTaskExecutorCancellationSemanticModelSurfacePath =
    "frontend.pipeline.semantic_surface.objc_concurrency_task_executor_and_cancellation_semantic_model";
inline constexpr const char *kObjc3ConcurrencyTaskExecutorCancellationSemanticModelRule =
    "task-lifetime-executor-affinity-cancellation-observation-and-structured-task-legality-are-live-in-sema-while-runnable-task-allocation-and-scheduler-execution-remain-later-runtime-work";
inline constexpr const char *kObjc3ConcurrencyTaskExecutorCancellationSemanticModelDeferredRule =
    "task-allocation-executor-hop-runtime-task-group-execution-and-scheduler-backed-cancellation-remain-deferred-to-later-runtime-lanes";

struct Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary {
  std::string contract_id =
      kObjc3ConcurrencyTaskExecutorCancellationSemanticModelContractId;
  std::string frontend_dependency_contract_id =
      kObjc3ConcurrencyTaskExecutorCancellationSemanticModelDependencyContractId;
  std::string surface_path =
      kObjc3ConcurrencyTaskExecutorCancellationSemanticModelSurfacePath;
  std::string semantic_model =
      kObjc3ConcurrencyTaskExecutorCancellationSemanticModelRule;
  std::string deferred_model =
      kObjc3ConcurrencyTaskExecutorCancellationSemanticModelDeferredRule;
  std::size_t async_callable_sites = 0;
  std::size_t executor_attribute_sites = 0;
  std::size_t task_creation_sites = 0;
  std::size_t task_group_scope_sites = 0;
  std::size_t task_group_add_task_sites = 0;
  std::size_t task_group_wait_next_sites = 0;
  std::size_t task_group_cancel_all_sites = 0;
  std::size_t task_runtime_interop_sites = 0;
  std::size_t runtime_hook_sites = 0;
  std::size_t cancellation_check_sites = 0;
  std::size_t cancellation_handler_sites = 0;
  std::size_t suspension_point_sites = 0;
  std::size_t cancellation_propagation_sites = 0;
  bool source_dependency_required = false;
  bool task_lifetime_semantics_landed = false;
  bool executor_affinity_semantics_landed = false;
  bool cancellation_observation_semantics_landed = false;
  bool structured_task_legality_semantics_landed = false;
  bool runnable_lowering_deferred = false;
  bool executor_runtime_deferred = false;
  bool scheduler_runtime_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ConcurrencyTaskExecutorCancellationSemanticModelSummary(
    const Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.frontend_dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() &&
         summary.source_dependency_required &&
         summary.task_lifetime_semantics_landed &&
         summary.executor_affinity_semantics_landed &&
         summary.cancellation_observation_semantics_landed &&
         summary.structured_task_legality_semantics_landed &&
         summary.runnable_lowering_deferred &&
         summary.executor_runtime_deferred &&
         summary.scheduler_runtime_deferred && summary.deterministic &&
         summary.ready_for_lowering_and_runtime && !summary.replay_key.empty() &&
         summary.failure_reason.empty();
}
