#pragma once

#include "sema/objc3_sema_contract_metaprogramming_surfaces.h"

inline constexpr const char
    *kObjc3ConcurrencyStructuredTaskCancellationSemanticSummaryDependencyContractId =
        "objc3c.concurrency.task.executor.cancellation.semantic.model.v1";
inline constexpr const char
    *kObjc3ConcurrencyStructuredTaskCancellationSemanticSummaryContractId =
        "objc3c.concurrency.structured.task.cancellation.semantics.v1";
inline constexpr const char
    *kObjc3ConcurrencyStructuredTaskCancellationSemanticSummarySurfacePath =
        "frontend.pipeline.semantic_surface.objc_concurrency_structured_task_and_cancellation_semantics";
inline constexpr const char
    *kObjc3ConcurrencyStructuredTaskCancellationSemanticSummaryRule =
        "structured-task-scope-task-hierarchy-and-cancellation-usage-semantics-are-live-in-sema-while-runnable-task-lowering-and-scheduler-execution-remain-later-runtime-work";
inline constexpr const char
    *kObjc3ConcurrencyStructuredTaskCancellationSemanticSummaryDeferredRule =
        "task-allocation-executor-hop-task-group-runtime-and-scheduler-backed-cancellation-remain-deferred-to-later-runtime-lanes";

struct Objc3ConcurrencyStructuredTaskCancellationSemanticSummary {
  std::string contract_id =
      kObjc3ConcurrencyStructuredTaskCancellationSemanticSummaryContractId;
  std::string dependency_contract_id =
      kObjc3ConcurrencyStructuredTaskCancellationSemanticSummaryDependencyContractId;
  std::string surface_path =
      kObjc3ConcurrencyStructuredTaskCancellationSemanticSummarySurfacePath;
  std::string semantic_model =
      kObjc3ConcurrencyStructuredTaskCancellationSemanticSummaryRule;
  std::string deferred_model =
      kObjc3ConcurrencyStructuredTaskCancellationSemanticSummaryDeferredRule;
  std::size_t async_callable_sites = 0;
  std::size_t task_creation_sites = 0;
  std::size_t task_group_scope_sites = 0;
  std::size_t task_group_add_task_sites = 0;
  std::size_t task_group_wait_next_sites = 0;
  std::size_t task_group_cancel_all_sites = 0;
  std::size_t cancellation_check_sites = 0;
  std::size_t cancellation_handler_sites = 0;
  std::size_t illegal_non_async_task_sites = 0;
  std::size_t illegal_task_group_scope_sites = 0;
  std::size_t illegal_task_hierarchy_sites = 0;
  std::size_t illegal_cancellation_usage_sites = 0;
  bool source_dependency_required = false;
  bool async_task_boundary_enforced = false;
  bool structured_task_scope_enforced = false;
  bool task_hierarchy_enforced = false;
  bool cancellation_usage_enforced = false;
  bool runnable_lowering_deferred = false;
  bool executor_runtime_deferred = false;
  bool scheduler_runtime_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ConcurrencyStructuredTaskCancellationSemanticSummary(
    const Objc3ConcurrencyStructuredTaskCancellationSemanticSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() &&
         summary.source_dependency_required &&
         summary.async_task_boundary_enforced &&
         summary.structured_task_scope_enforced &&
         summary.task_hierarchy_enforced &&
         summary.cancellation_usage_enforced &&
         summary.runnable_lowering_deferred &&
         summary.executor_runtime_deferred &&
         summary.scheduler_runtime_deferred && summary.deterministic &&
         summary.ready_for_lowering_and_runtime && !summary.replay_key.empty() &&
         summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3ConcurrencyExecutorHopAffinityCompatibilitySummaryDependencyContractId =
        "objc3c.concurrency.structured.task.cancellation.semantics.v1";
inline constexpr const char
    *kObjc3ConcurrencyExecutorHopAffinityCompatibilitySummaryContractId =
        "objc3c.concurrency.executor.hop.affinity.compatibility.v1";
inline constexpr const char
    *kObjc3ConcurrencyExecutorHopAffinityCompatibilitySummarySurfacePath =
        "frontend.pipeline.semantic_surface.objc_concurrency_executor_hop_and_affinity_compatibility_completion";
inline constexpr const char
    *kObjc3ConcurrencyExecutorHopAffinityCompatibilitySummaryRule =
        "executor-affinity-and-detached-task-hop-boundaries-are-live-in-sema-while-runnable-hop-lowering-and-scheduler-runtime-remain-later-runtime-work";
inline constexpr const char
    *kObjc3ConcurrencyExecutorHopAffinityCompatibilitySummaryDeferredRule =
        "executor-hop-lowering-task-spawn-runtime-and-scheduler-visible-execution-remain-deferred-to-later-runtime-lanes";

struct Objc3ConcurrencyExecutorHopAffinityCompatibilitySummary {
  std::string contract_id =
      kObjc3ConcurrencyExecutorHopAffinityCompatibilitySummaryContractId;
  std::string dependency_contract_id =
      kObjc3ConcurrencyExecutorHopAffinityCompatibilitySummaryDependencyContractId;
  std::string surface_path =
      kObjc3ConcurrencyExecutorHopAffinityCompatibilitySummarySurfacePath;
  std::string semantic_model =
      kObjc3ConcurrencyExecutorHopAffinityCompatibilitySummaryRule;
  std::string deferred_model =
      kObjc3ConcurrencyExecutorHopAffinityCompatibilitySummaryDeferredRule;
  std::size_t async_callable_sites = 0;
  std::size_t executor_affinity_sites = 0;
  std::size_t executor_main_sites = 0;
  std::size_t executor_global_sites = 0;
  std::size_t executor_named_sites = 0;
  std::size_t task_creation_sites = 0;
  std::size_t detached_task_creation_sites = 0;
  std::size_t illegal_missing_executor_affinity_sites = 0;
  std::size_t illegal_main_executor_detached_sites = 0;
  bool dependency_required = false;
  bool executor_affinity_required_for_task_callables_enforced = false;
  bool detached_task_hop_boundary_enforced = false;
  bool runnable_lowering_deferred = false;
  bool executor_runtime_deferred = false;
  bool scheduler_runtime_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ConcurrencyExecutorHopAffinityCompatibilitySummary(
    const Objc3ConcurrencyExecutorHopAffinityCompatibilitySummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() && summary.dependency_required &&
         summary.executor_affinity_required_for_task_callables_enforced &&
         summary.detached_task_hop_boundary_enforced &&
         summary.runnable_lowering_deferred &&
         summary.executor_runtime_deferred &&
         summary.scheduler_runtime_deferred && summary.deterministic &&
         summary.ready_for_lowering_and_runtime && !summary.replay_key.empty() &&
         summary.failure_reason.empty();
}

inline constexpr const char *kObjc3ConcurrencyAwaitSuspensionResumeSemanticSummaryDependencyContractId =
    "objc3c.concurrency.async.effect.suspension.semantic.model.v1";
inline constexpr const char *kObjc3ConcurrencyAwaitSuspensionResumeSemanticSummaryContractId =
    "objc3c.concurrency.await.suspension.resume.semantics.v1";
inline constexpr const char *kObjc3ConcurrencyAwaitSuspensionResumeSemanticSummarySurfacePath =
    "frontend.pipeline.semantic_surface.objc_concurrency_await_suspension_and_resume_semantics";
inline constexpr const char *kObjc3ConcurrencyAwaitSuspensionResumeSemanticSummaryRule =
    "await-placement-suspension-and-resume-semantics-are-live-in-sema-while-runnable-async-frame-lowering-and-executor-runtime-execution-remain-later-runtime-work";
inline constexpr const char *kObjc3ConcurrencyAwaitSuspensionResumeSemanticSummaryDeferredRule =
    "async-frame-layout-resume-lowering-suspension-cleanup-and-runtime-executor-scheduling-remain-deferred-to-later-runtime-lanes";

struct Objc3ConcurrencyAwaitSuspensionResumeSemanticSummary {
  std::string contract_id =
      kObjc3ConcurrencyAwaitSuspensionResumeSemanticSummaryContractId;
  std::string dependency_contract_id =
      kObjc3ConcurrencyAwaitSuspensionResumeSemanticSummaryDependencyContractId;
  std::string surface_path =
      kObjc3ConcurrencyAwaitSuspensionResumeSemanticSummarySurfacePath;
  std::string semantic_model =
      kObjc3ConcurrencyAwaitSuspensionResumeSemanticSummaryRule;
  std::string deferred_model =
      kObjc3ConcurrencyAwaitSuspensionResumeSemanticSummaryDeferredRule;
  std::size_t async_callable_sites = 0;
  std::size_t await_expression_sites = 0;
  std::size_t await_in_async_callable_sites = 0;
  std::size_t illegal_await_sites = 0;
  std::size_t await_suspension_point_sites = 0;
  std::size_t await_resume_sites = 0;
  std::size_t continuation_resume_sites = 0;
  std::size_t continuation_suspend_sites = 0;
  bool source_dependency_required = false;
  bool await_placement_enforced = false;
  bool suspension_profile_enforced = false;
  bool resume_profile_enforced = false;
  bool non_async_await_fail_closed = false;
  bool deterministic = true;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ConcurrencyAwaitSuspensionResumeSemanticSummary(
    const Objc3ConcurrencyAwaitSuspensionResumeSemanticSummary &summary) {
  return !summary.contract_id.empty() && !summary.dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() &&
         summary.source_dependency_required &&
         summary.await_placement_enforced &&
         summary.suspension_profile_enforced &&
         summary.resume_profile_enforced &&
         summary.non_async_await_fail_closed &&
         summary.deterministic && summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char *kObjc3ErrorHandlingTryDoCatchSemanticSummaryDependencyContractId =
    "objc3c.error_handling.error.semantic.model.v1";
inline constexpr const char *kObjc3ErrorHandlingTryDoCatchSemanticSummaryContractId =
    "objc3c.error_handling.try.throw.do.catch.semantics.v1";
inline constexpr const char *kObjc3ErrorHandlingTryDoCatchSemanticSummarySurfacePath =
    "frontend.pipeline.semantic_surface.objc_error_handling_try_do_catch_semantics";
inline constexpr const char *kObjc3ErrorHandlingTryDoCatchSemanticSummaryRule =
    "try-throw-and-do-catch-parse-and-undergo-deterministic-legality-checking-in-source-only-native-validation-while-lowering-and-runtime-integration-remain-later-lane-work";
inline constexpr const char *kObjc3ErrorHandlingTryDoCatchSemanticSummaryDeferredRule =
    "native-ir-object-execution-lowering-catch-transfer-and-thrown-error-abi-remain-deferred-to-lanes-c-and-d";

struct Objc3ErrorHandlingTryDoCatchSemanticSummary {
  std::string contract_id = kObjc3ErrorHandlingTryDoCatchSemanticSummaryContractId;
  std::string dependency_contract_id =
      kObjc3ErrorHandlingTryDoCatchSemanticSummaryDependencyContractId;
  std::string surface_path = kObjc3ErrorHandlingTryDoCatchSemanticSummarySurfacePath;
  std::string semantic_model = kObjc3ErrorHandlingTryDoCatchSemanticSummaryRule;
  std::string deferred_model = kObjc3ErrorHandlingTryDoCatchSemanticSummaryDeferredRule;
  std::size_t try_expression_sites = 0;
  std::size_t try_propagating_sites = 0;
  std::size_t try_optional_sites = 0;
  std::size_t try_forced_sites = 0;
  std::size_t throw_statement_sites = 0;
  std::size_t do_catch_sites = 0;
  std::size_t catch_clause_sites = 0;
  std::size_t catch_binding_sites = 0;
  std::size_t catch_all_sites = 0;
  std::size_t throwing_callable_try_sites = 0;
  std::size_t bridged_callable_try_sites = 0;
  std::size_t caller_propagation_sites = 0;
  std::size_t local_handler_sites = 0;
  std::size_t rethrow_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool source_dependency_required = true;
  bool try_surface_landed = false;
  bool throw_surface_landed = false;
  bool do_catch_surface_landed = false;
  bool throwing_context_legality_enforced = false;
  bool native_emit_remains_fail_closed = true;
  bool deterministic = true;
  bool ready_for_lowering_and_runtime = true;
  std::string replay_key;
  std::string failure_reason;
};

inline constexpr const char *kObjc3ErrorHandlingErrorBridgeLegalitySummaryDependencyContractId =
    "objc3c.error_handling.try.throw.do.catch.semantics.v1";
inline constexpr const char *kObjc3ErrorHandlingErrorBridgeLegalitySummaryContractId =
    "objc3c.error_handling.error.bridge.legality.v1";
inline constexpr const char *kObjc3ErrorHandlingErrorBridgeLegalitySummarySurfacePath =
    "frontend.pipeline.semantic_surface.objc_error_handling_error_bridge_legality";
inline constexpr const char *kObjc3ErrorHandlingErrorBridgeLegalitySummaryRule =
    "nserror-and-status-bridge-markers-undergo-deterministic-semantic-legality-checking-before-lowering-and-only-semantically-valid-bridge-surfaces-qualify-for-try";
inline constexpr const char *kObjc3ErrorHandlingErrorBridgeLegalitySummaryDeferredRule =
    "validated-nserror-and-status-bridge-markers-are-native-lowering-ready-while-generalized-foreign-exception-abi-and-broad-error-runtime-claims-remain-deferred";

struct Objc3ErrorHandlingErrorBridgeLegalitySummary {
  std::string contract_id = kObjc3ErrorHandlingErrorBridgeLegalitySummaryContractId;
  std::string dependency_contract_id =
      kObjc3ErrorHandlingErrorBridgeLegalitySummaryDependencyContractId;
  std::string surface_path = kObjc3ErrorHandlingErrorBridgeLegalitySummarySurfacePath;
  std::string semantic_model = kObjc3ErrorHandlingErrorBridgeLegalitySummaryRule;
  std::string deferred_model = kObjc3ErrorHandlingErrorBridgeLegalitySummaryDeferredRule;
  std::size_t bridge_callable_sites = 0;
  std::size_t objc_nserror_callable_sites = 0;
  std::size_t objc_status_code_callable_sites = 0;
  std::size_t semantically_valid_bridge_callable_sites = 0;
  std::size_t try_eligible_bridge_callable_sites = 0;
  std::size_t missing_error_out_parameter_sites = 0;
  std::size_t invalid_nserror_return_sites = 0;
  std::size_t invalid_status_return_sites = 0;
  std::size_t invalid_error_type_sites = 0;
  std::size_t missing_mapping_symbol_sites = 0;
  std::size_t invalid_mapping_signature_sites = 0;
  std::size_t throws_bridge_conflict_sites = 0;
  std::size_t marker_conflict_sites = 0;
  std::size_t unsupported_combination_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool source_dependency_required = true;
  bool bridge_legality_landed = false;
  bool try_bridge_filter_landed = false;
  bool unsupported_combinations_fail_closed = false;
  bool native_emit_remains_fail_closed = true;
  bool deterministic = true;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};
