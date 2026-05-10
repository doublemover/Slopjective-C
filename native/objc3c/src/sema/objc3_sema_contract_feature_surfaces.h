inline constexpr const char
    *kObjc3MetaprogrammingExpansionBehaviorSemanticModelRule =
        "derive-macro-package-provenance-and-property-behavior-source-surfaces-now-share-one-truthful-sema-packet-while-real-derive-expansion-macro-execution-and-property-behavior-runtime-materialization-remain-later-runtime-work";
inline constexpr const char
    *kObjc3MetaprogrammingExpansionBehaviorSemanticModelDeferredRule =
        "derive-body-expansion-macro-sandbox-execution-and-property-behavior-runtime-hooks-remain-deferred-to-later-runtime-lanes";

struct Objc3MetaprogrammingExpansionBehaviorSemanticModelSummary {
  std::string contract_id =
      kObjc3MetaprogrammingExpansionBehaviorSemanticModelContractId;
  std::string frontend_dependency_contract_id =
      kObjc3MetaprogrammingExpansionBehaviorSemanticModelDependencyContractId;
  std::string surface_path =
      kObjc3MetaprogrammingExpansionBehaviorSemanticModelSurfacePath;
  std::string semantic_model =
      kObjc3MetaprogrammingExpansionBehaviorSemanticModelRule;
  std::string deferred_model =
      kObjc3MetaprogrammingExpansionBehaviorSemanticModelDeferredRule;
  std::size_t derive_marker_sites = 0;
  std::size_t macro_marker_sites = 0;
  std::size_t macro_package_sites = 0;
  std::size_t macro_provenance_sites = 0;
  std::size_t expansion_visible_macro_sites = 0;
  std::size_t property_behavior_sites = 0;
  std::size_t interface_property_behavior_sites = 0;
  std::size_t implementation_property_behavior_sites = 0;
  std::size_t protocol_property_behavior_sites = 0;
  std::size_t synthesized_binding_visible_sites = 0;
  std::size_t synthesized_getter_visible_sites = 0;
  std::size_t synthesized_setter_visible_sites = 0;
  std::size_t property_behavior_contract_violation_sites = 0;
  bool source_dependency_required = false;
  bool derive_macro_source_supported = false;
  bool macro_package_provenance_surface_reused = false;
  bool property_behavior_source_supported = false;
  bool synthesized_visibility_surface_reused = false;
  bool derive_synthesis_deferred = false;
  bool macro_execution_deferred = false;
  bool property_behavior_runtime_deferred = false;
  bool deterministic = false;
  bool ready_for_core_implementation = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3MetaprogrammingExpansionBehaviorSemanticModelSummary(
    const Objc3MetaprogrammingExpansionBehaviorSemanticModelSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.frontend_dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() &&
         summary.source_dependency_required &&
         summary.derive_macro_source_supported &&
         summary.macro_package_provenance_surface_reused &&
         summary.property_behavior_source_supported &&
         summary.synthesized_visibility_surface_reused &&
         summary.derive_synthesis_deferred &&
         summary.macro_execution_deferred &&
         summary.property_behavior_runtime_deferred && summary.deterministic &&
         summary.ready_for_core_implementation && !summary.replay_key.empty() &&
         summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3MetaprogrammingDeriveExpansionInventoryDependencyContractId =
        "objc3c.metaprogramming.expansion.behavior.semantic.model.v1";
inline constexpr const char
    *kObjc3MetaprogrammingDeriveExpansionInventoryContractId =
        "objc3c.metaprogramming.derive.expansion.inventory.v1";
inline constexpr const char
    *kObjc3MetaprogrammingDeriveExpansionInventorySurfacePath =
        "frontend.pipeline.semantic_surface.objc_metaprogramming_derive_expansion_inventory";
inline constexpr const char
    *kObjc3MetaprogrammingDeriveExpansionInventoryRule =
        "supported-derive-requests-now-expand-into-a-deterministic-selector-inventory-while-unsupported-derive-names-category-topologies-and-selector-collisions-fail-closed-in-sema";
inline constexpr const char
    *kObjc3MetaprogrammingDeriveExpansionInventoryDeferredRule =
        "runtime-backed-derived-method-body-materialization-macro-execution-and-property-behavior-runtime-hooks-remain-deferred-to-later-runtime-lanes";

struct Objc3MetaprogrammingDeriveExpansionInventorySummary {
  std::string contract_id = kObjc3MetaprogrammingDeriveExpansionInventoryContractId;
  std::string semantic_dependency_contract_id =
      kObjc3MetaprogrammingDeriveExpansionInventoryDependencyContractId;
  std::string surface_path = kObjc3MetaprogrammingDeriveExpansionInventorySurfacePath;
  std::string semantic_model = kObjc3MetaprogrammingDeriveExpansionInventoryRule;
  std::string deferred_model = kObjc3MetaprogrammingDeriveExpansionInventoryDeferredRule;
  std::size_t derive_request_sites = 0;
  std::size_t supported_derive_request_sites = 0;
  std::size_t unsupported_derive_request_sites = 0;
  std::size_t unsupported_topology_sites = 0;
  std::size_t equatable_alias_sites = 0;
  std::size_t equality_derive_sites = 0;
  std::size_t hash_derive_sites = 0;
  std::size_t debug_description_derive_sites = 0;
  std::size_t selector_conflict_sites = 0;
  std::size_t generated_method_entry_count = 0;
  std::vector<std::string> expansion_inventory_rows_lexicographic;
  bool semantic_dependency_required = false;
  bool supported_derive_inventory_landed = false;
  bool unsupported_derive_fail_closed = false;
  bool unsupported_topology_fail_closed = false;
  bool selector_conflicts_fail_closed = false;
  bool runtime_materialization_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3MetaprogrammingDeriveExpansionInventorySummary(
    const Objc3MetaprogrammingDeriveExpansionInventorySummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.semantic_dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() &&
         summary.semantic_dependency_required &&
         summary.supported_derive_inventory_landed &&
         summary.unsupported_derive_fail_closed &&
         summary.unsupported_topology_fail_closed &&
         summary.selector_conflicts_fail_closed &&
         summary.runtime_materialization_deferred &&
         summary.deterministic && summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3MetaprogrammingMacroSafetySandboxDeterminismDependencyContractId =
        "objc3c.metaprogramming.derive.expansion.inventory.v1";
inline constexpr const char
    *kObjc3MetaprogrammingMacroSafetySandboxDeterminismContractId =
        "objc3c.metaprogramming.macro.safety.sandbox.determinism.semantics.v1";
inline constexpr const char
    *kObjc3MetaprogrammingMacroSafetySandboxDeterminismSurfacePath =
        "frontend.pipeline.semantic_surface.objc_metaprogramming_macro_safety_sandbox_and_determinism_semantics";
inline constexpr const char
    *kObjc3MetaprogrammingMacroSafetySandboxDeterminismRule =
        "macro-callable-safety-sandbox-namespace-and-deterministic-provenance-semantics-are-live-in-sema-while-runnable-macro-execution-remains-deferred";
inline constexpr const char
    *kObjc3MetaprogrammingMacroSafetySandboxDeterminismDeferredRule =
        "runnable-macro-execution-runtime-package-loading-and-expanded-body-materialization-remain-deferred-to-later-runtime-lanes";

struct Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary {
  std::string contract_id =
      kObjc3MetaprogrammingMacroSafetySandboxDeterminismContractId;
  std::string semantic_dependency_contract_id =
      kObjc3MetaprogrammingMacroSafetySandboxDeterminismDependencyContractId;
  std::string surface_path =
      kObjc3MetaprogrammingMacroSafetySandboxDeterminismSurfacePath;
  std::string semantic_model =
      kObjc3MetaprogrammingMacroSafetySandboxDeterminismRule;
  std::string deferred_model =
      kObjc3MetaprogrammingMacroSafetySandboxDeterminismDeferredRule;
  std::size_t macro_marker_sites = 0;
  std::size_t macro_package_sites = 0;
  std::size_t macro_provenance_sites = 0;
  std::size_t expansion_visible_macro_sites = 0;
  std::size_t safe_macro_callable_sites = 0;
  std::size_t incomplete_macro_metadata_sites = 0;
  std::size_t orphan_macro_metadata_sites = 0;
  std::size_t invalid_package_sites = 0;
  std::size_t invalid_provenance_sites = 0;
  std::size_t nondeterministic_callable_sites = 0;
  std::size_t unsupported_callable_topology_sites = 0;
  bool semantic_dependency_required = false;
  bool metadata_completeness_enforced = false;
  bool sandbox_namespace_enforced = false;
  bool provenance_determinism_enforced = false;
  bool callable_determinism_enforced = false;
  bool macro_execution_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3MetaprogrammingMacroSafetySandboxDeterminismSummary(
    const Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.semantic_dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() &&
         summary.semantic_dependency_required &&
         summary.metadata_completeness_enforced &&
         summary.sandbox_namespace_enforced &&
         summary.provenance_determinism_enforced &&
         summary.callable_determinism_enforced &&
         summary.macro_execution_deferred && summary.deterministic &&
         summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3MetaprogrammingPropertyBehaviorLegalityCompatibilityDependencyContractId =
        "objc3c.metaprogramming.macro.safety.sandbox.determinism.semantics.v1";
inline constexpr const char
    *kObjc3MetaprogrammingPropertyBehaviorLegalityCompatibilityContractId =
        "objc3c.metaprogramming.property.behavior.legality.interaction.completion.v1";
inline constexpr const char
    *kObjc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySurfacePath =
        "frontend.pipeline.semantic_surface.objc_metaprogramming_property_behavior_legality_and_interaction_completion";
inline constexpr const char
    *kObjc3MetaprogrammingPropertyBehaviorLegalityCompatibilityRule =
        "supported-property-behavior-names-and-owner-interaction-legality-now-fail-closed-in-sema-while-runtime-backed-behavior-materialization-remains-deferred";
inline constexpr const char
    *kObjc3MetaprogrammingPropertyBehaviorLegalityCompatibilityDeferredRule =
        "runnable-property-behavior-hooks-observation-materialization-and-projection-runtime-support-remain-deferred-to-later-runtime-lanes";

struct Objc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary {
  std::string contract_id =
      kObjc3MetaprogrammingPropertyBehaviorLegalityCompatibilityContractId;
  std::string semantic_dependency_contract_id =
      kObjc3MetaprogrammingPropertyBehaviorLegalityCompatibilityDependencyContractId;
  std::string surface_path =
      kObjc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySurfacePath;
  std::string semantic_model =
      kObjc3MetaprogrammingPropertyBehaviorLegalityCompatibilityRule;
  std::string deferred_model =
      kObjc3MetaprogrammingPropertyBehaviorLegalityCompatibilityDeferredRule;
  std::size_t property_behavior_sites = 0;
  std::size_t supported_behavior_sites = 0;
  std::size_t unsupported_behavior_sites = 0;
  std::size_t observed_behavior_sites = 0;
  std::size_t projected_behavior_sites = 0;
  std::size_t observed_on_protocol_sites = 0;
  std::size_t observed_readonly_conflict_sites = 0;
  std::size_t projected_writable_conflict_sites = 0;
  std::size_t non_object_behavior_sites = 0;
  bool semantic_dependency_required = false;
  bool supported_behavior_inventory_landed = false;
  bool unsupported_behavior_fail_closed = false;
  bool owner_topology_fail_closed = false;
  bool interaction_legality_fail_closed = false;
  bool storage_legality_fail_closed = false;
  bool runtime_materialization_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary(
    const Objc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.semantic_dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() &&
         summary.semantic_dependency_required &&
         summary.supported_behavior_inventory_landed &&
         summary.unsupported_behavior_fail_closed &&
         summary.owner_topology_fail_closed &&
         summary.interaction_legality_fail_closed &&
         summary.storage_legality_fail_closed &&
         summary.runtime_materialization_deferred &&
         summary.deterministic &&
         summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

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
    "status-to-error-execution-bridge-temporaries-native-error-abi-and-runnable-bridge-lowering-remain-deferred-to-lanes-c-and-d";

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

struct Objc3ModuleImportGraphSummary {
  std::size_t module_import_graph_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3NamespaceCollisionShadowingSummary {
  std::size_t namespace_collision_shadowing_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3PublicPrivateApiPartitionSummary {
  std::size_t public_private_api_partition_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3IncrementalModuleCacheInvalidationSummary {
  std::size_t incremental_module_cache_invalidation_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t cache_invalidation_candidate_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3CrossModuleConformanceSummary {
  std::size_t cross_module_conformance_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t cache_invalidation_candidate_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ThrowsPropagationSummary {
  std::size_t throws_propagation_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t cache_invalidation_candidate_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3UnwindCleanupSummary {
  std::size_t unwind_cleanup_sites = 0;
  std::size_t exceptional_exit_sites = 0;
  std::size_t cleanup_action_sites = 0;
  std::size_t cleanup_scope_sites = 0;
  std::size_t cleanup_resume_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t fail_closed_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3AsyncContinuationSummary {
  std::size_t async_continuation_sites = 0;
  std::size_t async_keyword_sites = 0;
  std::size_t async_function_sites = 0;
  std::size_t continuation_allocation_sites = 0;
  std::size_t continuation_resume_sites = 0;
  std::size_t continuation_suspend_sites = 0;
  std::size_t async_state_machine_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3AwaitLoweringSuspensionStateSummary {
  std::size_t await_suspension_sites = 0;
  std::size_t await_keyword_sites = 0;
  std::size_t await_suspension_point_sites = 0;
  std::size_t await_resume_sites = 0;
  std::size_t await_state_machine_sites = 0;
  std::size_t await_continuation_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ActorIsolationSendabilitySummary {
  std::size_t actor_isolation_sendability_sites = 0;
  std::size_t actor_isolation_decl_sites = 0;
  std::size_t actor_hop_sites = 0;
  std::size_t sendable_annotation_sites = 0;
  std::size_t non_sendable_crossing_sites = 0;
  std::size_t isolation_boundary_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

inline constexpr const char
    *kObjc3ConcurrencyActorIsolationSendableSemanticModelDependencyContractId =
        kObjc3ActorMemberIsolationSourceClosureContractId;
inline constexpr const char
    *kObjc3ConcurrencyActorIsolationSendableSemanticModelContractId =
        "objc3c.concurrency.actor.isolation.sendable.semantic.model.v1";
inline constexpr const char
    *kObjc3ConcurrencyActorIsolationSendableSemanticModelSurfacePath =
        "frontend.pipeline.semantic_surface.objc_concurrency_actor_isolation_and_sendable_semantic_model";
inline constexpr const char
    *kObjc3ConcurrencyActorIsolationSendableSemanticModelRule =
        "actor-member-source-closure-and-parser-owned-actor-sendability-profiles-now-publish-one-deterministic-sema-packet-while-cross-actor-legality-sendable-enforcement-and-runnable-actor-runtime-behavior-remain-later-runtime-work";
inline constexpr const char
    *kObjc3ConcurrencyActorIsolationSendableSemanticModelDeferredRule =
        "dedicated-actor-isolation-diagnostics-cross-actor-sendable-enforcement-executor-scheduling-and-runnable-actor-runtime-behavior-remain-deferred-to-later-runtime-lanes";

struct Objc3ConcurrencyActorIsolationSendableSemanticModelSummary {
  std::string contract_id =
      kObjc3ConcurrencyActorIsolationSendableSemanticModelContractId;
  std::string frontend_dependency_contract_id =
      kObjc3ConcurrencyActorIsolationSendableSemanticModelDependencyContractId;
  std::string surface_path =
      kObjc3ConcurrencyActorIsolationSendableSemanticModelSurfacePath;
  std::string semantic_model =
      kObjc3ConcurrencyActorIsolationSendableSemanticModelRule;
  std::string deferred_model =
      kObjc3ConcurrencyActorIsolationSendableSemanticModelDeferredRule;
  std::size_t actor_interface_sites = 0;
  std::size_t actor_method_sites = 0;
  std::size_t actor_property_sites = 0;
  std::size_t objc_nonisolated_annotation_sites = 0;
  std::size_t actor_member_executor_annotation_sites = 0;
  std::size_t actor_async_method_sites = 0;
  std::size_t actor_member_metadata_sites = 0;
  std::size_t actor_isolation_sendability_sites = 0;
  std::size_t actor_isolation_decl_sites = 0;
  std::size_t actor_hop_sites = 0;
  std::size_t sendable_annotation_sites = 0;
  std::size_t non_sendable_crossing_sites = 0;
  std::size_t isolation_boundary_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool source_dependency_required = false;
  bool actor_member_source_supported = false;
  bool actor_isolation_sendability_profile_normalized = false;
  bool strict_concurrency_selection_fail_closed = false;
  bool actor_runtime_deferred = false;
  bool executor_runtime_deferred = false;
  bool cross_actor_enforcement_deferred = false;
  bool deterministic = false;
  bool ready_for_semantic_expansion = false;
  std::string failure_reason;
  std::string replay_key;
};

inline bool IsReadyObjc3ConcurrencyActorIsolationSendableSemanticModelSummary(
    const Objc3ConcurrencyActorIsolationSendableSemanticModelSummary &summary) {
  return summary.source_dependency_required &&
         summary.actor_member_source_supported &&
         summary.actor_isolation_sendability_profile_normalized &&
         summary.strict_concurrency_selection_fail_closed &&
         summary.actor_runtime_deferred &&
         summary.executor_runtime_deferred &&
         summary.cross_actor_enforcement_deferred &&
         summary.deterministic && summary.ready_for_semantic_expansion &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3ConcurrencyActorIsolationSendabilityEnforcementDependencyContractId =
        kObjc3ConcurrencyActorIsolationSendableSemanticModelContractId;
inline constexpr const char
    *kObjc3ConcurrencyActorIsolationSendabilityEnforcementContractId =
        "objc3c.concurrency.actor.isolation.sendability.enforcement.v1";
inline constexpr const char
    *kObjc3ConcurrencyActorIsolationSendabilityEnforcementSurfacePath =
        "frontend.pipeline.semantic_surface.objc_concurrency_actor_isolation_and_sendability_enforcement";
inline constexpr const char
    *kObjc3ConcurrencyActorIsolationSendabilityEnforcementRule =
        "actor-method-semantics-now-fail-closed-for-non-actor-objc-nonisolated-usage-invalid-nonisolated-combinations-non-async-actor-hops-and-non-sendable-crossings-while-runnable-actor-mailbox-runtime-remains-later-runtime-work";
inline constexpr const char
    *kObjc3ConcurrencyActorIsolationSendabilityEnforcementDeferredRule =
        "full-cross-module-actor-runtime-mailboxes-race-hazard-closure-and-runnable-strict-concurrency-scheduling-remain-deferred-to-later-runtime-lanes";

struct Objc3ConcurrencyActorIsolationSendabilityEnforcementSummary {
  std::string contract_id =
      kObjc3ConcurrencyActorIsolationSendabilityEnforcementContractId;
  std::string dependency_contract_id =
      kObjc3ConcurrencyActorIsolationSendabilityEnforcementDependencyContractId;
  std::string surface_path =
      kObjc3ConcurrencyActorIsolationSendabilityEnforcementSurfacePath;
  std::string semantic_model =
      kObjc3ConcurrencyActorIsolationSendabilityEnforcementRule;
  std::string deferred_model =
      kObjc3ConcurrencyActorIsolationSendabilityEnforcementDeferredRule;
  std::size_t actor_interface_sites = 0;
  std::size_t actor_method_sites = 0;
  std::size_t objc_nonisolated_annotation_sites = 0;
  std::size_t actor_member_executor_annotation_sites = 0;
  std::size_t actor_async_method_sites = 0;
  std::size_t actor_hop_sites = 0;
  std::size_t non_sendable_crossing_sites = 0;
  std::size_t total_nonisolated_method_sites = 0;
  std::size_t illegal_non_actor_nonisolated_sites = 0;
  std::size_t illegal_nonisolated_async_sites = 0;
  std::size_t illegal_nonisolated_executor_sites = 0;
  std::size_t illegal_actor_hop_without_async_sites = 0;
  std::size_t illegal_non_sendable_crossing_sites = 0;
  bool dependency_required = false;
  bool non_actor_nonisolated_fail_closed = false;
  bool nonisolated_combination_fail_closed = false;
  bool actor_hop_async_boundary_enforced = false;
  bool non_sendable_crossing_fail_closed = false;
  bool runnable_lowering_deferred = false;
  bool actor_runtime_deferred = false;
  bool executor_runtime_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string failure_reason;
  std::string replay_key;
};

inline bool IsReadyObjc3ConcurrencyActorIsolationSendabilityEnforcementSummary(
    const Objc3ConcurrencyActorIsolationSendabilityEnforcementSummary &summary) {
  return summary.dependency_required &&
         summary.non_actor_nonisolated_fail_closed &&
         summary.nonisolated_combination_fail_closed &&
         summary.actor_hop_async_boundary_enforced &&
         summary.non_sendable_crossing_fail_closed &&
         summary.runnable_lowering_deferred && summary.actor_runtime_deferred &&
         summary.executor_runtime_deferred && summary.deterministic &&
         summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsDependencyContractId =
        kObjc3ConcurrencyActorIsolationSendabilityEnforcementContractId;
inline constexpr const char
    *kObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsContractId =
        "objc3c.concurrency.actor.race.hazard.escape.diagnostics.v1";
inline constexpr const char
    *kObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsSurfacePath =
        "frontend.pipeline.semantic_surface.objc_concurrency_actor_race_hazard_and_escape_diagnostics";
inline constexpr const char
    *kObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsRule =
        "actor-method-task-handoff-now-fails-closed-without-race-guard-replay-proof-and-actor-isolation-coverage-while-escaping-block-literals-in-that-hazard-slice-remain-unsupported";
inline constexpr const char
    *kObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsDeferredRule =
        "runnable-actor-mailboxes-cross-module-isolation-runtime-and-full-strict-concurrency-escape-analysis-remain-deferred-to-later-runtime-lanes";

struct Objc3ConcurrencyActorRaceHazardEscapeDiagnosticsSummary {
  std::string contract_id =
      kObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsContractId;
  std::string dependency_contract_id =
      kObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsDependencyContractId;
  std::string surface_path =
      kObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsSurfacePath;
  std::string semantic_model =
      kObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsRule;
  std::string deferred_model =
      kObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsDeferredRule;
  std::size_t actor_method_sites = 0;
  std::size_t replay_proof_sites = 0;
  std::size_t race_guard_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t actor_isolation_sites = 0;
  std::size_t escaping_block_literal_sites = 0;
  std::size_t illegal_missing_race_guard_sites = 0;
  std::size_t illegal_missing_replay_proof_sites = 0;
  std::size_t illegal_missing_actor_isolation_sites = 0;
  std::size_t illegal_escaping_block_literal_sites = 0;
  bool dependency_required = false;
  bool race_guard_fail_closed = false;
  bool replay_proof_fail_closed = false;
  bool actor_isolation_boundary_fail_closed = false;
  bool escaping_block_fail_closed = false;
  bool runnable_lowering_deferred = false;
  bool actor_runtime_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string failure_reason;
  std::string replay_key;
};

inline bool IsReadyObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsSummary(
    const Objc3ConcurrencyActorRaceHazardEscapeDiagnosticsSummary &summary) {
  return summary.dependency_required && summary.race_guard_fail_closed &&
         summary.replay_proof_fail_closed &&
         summary.actor_isolation_boundary_fail_closed &&
         summary.escaping_block_fail_closed &&
         summary.runnable_lowering_deferred && summary.actor_runtime_deferred &&
         summary.deterministic && summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3TaskRuntimeCancellationSummary {
  std::size_t task_runtime_interop_sites = 0;
  std::size_t runtime_hook_sites = 0;
  std::size_t cancellation_check_sites = 0;
  std::size_t cancellation_handler_sites = 0;
  std::size_t suspension_point_sites = 0;
  std::size_t cancellation_propagation_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ConcurrencyReplayRaceGuardSummary {
  std::size_t concurrency_replay_race_guard_sites = 0;
  std::size_t concurrency_replay_sites = 0;
  std::size_t replay_proof_sites = 0;
  std::size_t race_guard_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t actor_isolation_sites = 0;
  std::size_t deterministic_schedule_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3UnsafePointerExtensionSummary {
  std::size_t unsafe_pointer_extension_sites = 0;
  std::size_t unsafe_keyword_sites = 0;
  std::size_t pointer_arithmetic_sites = 0;
  std::size_t raw_pointer_type_sites = 0;
  std::size_t unsafe_operation_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3InlineAsmIntrinsicGovernanceSummary {
  std::size_t inline_asm_intrinsic_sites = 0;
  std::size_t inline_asm_sites = 0;
  std::size_t intrinsic_sites = 0;
  std::size_t governed_intrinsic_sites = 0;
  std::size_t privileged_intrinsic_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3NSErrorBridgingSummary {
  std::size_t ns_error_bridging_sites = 0;
  std::size_t ns_error_parameter_sites = 0;
  std::size_t ns_error_out_parameter_sites = 0;
  std::size_t ns_error_bridge_path_sites = 0;
  std::size_t failable_call_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t bridge_boundary_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ResultLikeLoweringSummary {
  std::size_t result_like_sites = 0;
  std::size_t result_success_sites = 0;
  std::size_t result_failure_sites = 0;
  std::size_t result_branch_sites = 0;
  std::size_t result_payload_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t branch_merge_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ErrorDiagnosticsRecoverySummary {
  std::size_t error_diagnostics_recovery_sites = 0;
  std::size_t diagnostic_emit_sites = 0;
  std::size_t recovery_anchor_sites = 0;
  std::size_t recovery_boundary_sites = 0;
  std::size_t fail_closed_diagnostic_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

inline constexpr const char
    *kObjc3CrossModuleSemanticContractsDiagnosticsContractId =
        "objc3c.cross_module.semantic.contracts.diagnostics.closure.v1";
inline constexpr const char
    *kObjc3CrossModuleSemanticContractsDiagnosticsSurfacePath =
        "frontend.pipeline.semantic_surface.objc_cross_module_semantic_contracts_and_diagnostics";
inline constexpr const char
    *kObjc3CrossModuleSemanticContractsDiagnosticsRule =
        "module-import-namespace-api-partition-cache-invalidation-cross-module-conformance-and-diagnostic-recovery-packets-share-one-deterministic-semantic-contract-rooted-in-live-sema-and-lowering-surfaces";

struct Objc3CrossModuleSemanticContractsDiagnosticsSummary {
  std::string contract_id =
      kObjc3CrossModuleSemanticContractsDiagnosticsContractId;
  std::string surface_path =
      kObjc3CrossModuleSemanticContractsDiagnosticsSurfacePath;
  std::string semantic_model =
      kObjc3CrossModuleSemanticContractsDiagnosticsRule;
  std::size_t module_import_graph_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t namespace_collision_shadowing_sites = 0;
  std::size_t public_private_api_partition_sites = 0;
  std::size_t incremental_module_cache_invalidation_sites = 0;
  std::size_t cross_module_conformance_sites = 0;
  std::size_t normalized_cross_module_sites = 0;
  std::size_t cache_invalidation_candidate_sites = 0;
  std::size_t diagnostic_recovery_sites = 0;
  std::size_t diagnostic_emit_sites = 0;
  std::size_t recovery_anchor_sites = 0;
  std::size_t recovery_boundary_sites = 0;
  std::size_t fail_closed_diagnostic_sites = 0;
  std::size_t diagnostic_normalized_sites = 0;
  std::size_t diagnostic_gate_blocked_sites = 0;
  std::size_t interop_import_module_annotation_sites = 0;
  std::size_t interop_imported_module_name_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool module_import_graph_semantics_landed = false;
  bool namespace_collision_semantics_landed = false;
  bool public_private_partition_semantics_landed = false;
  bool incremental_cache_semantics_landed = false;
  bool cross_module_conformance_semantics_landed = false;
  bool diagnostic_recovery_semantics_landed = false;
  bool interop_import_semantics_landed = false;
  bool deterministic = true;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3SymbolGraphScopeResolutionSummary {
  std::size_t global_symbol_nodes = 0;
  std::size_t function_symbol_nodes = 0;
  std::size_t interface_symbol_nodes = 0;
  std::size_t implementation_symbol_nodes = 0;
  std::size_t interface_property_symbol_nodes = 0;
  std::size_t implementation_property_symbol_nodes = 0;
  std::size_t interface_method_symbol_nodes = 0;
  std::size_t implementation_method_symbol_nodes = 0;
  std::size_t top_level_scope_symbols = 0;
  std::size_t nested_scope_symbols = 0;
  std::size_t scope_frames_total = 0;
  std::size_t implementation_interface_resolution_sites = 0;
  std::size_t implementation_interface_resolution_hits = 0;
  std::size_t implementation_interface_resolution_misses = 0;
  std::size_t method_resolution_sites = 0;
  std::size_t method_resolution_hits = 0;
  std::size_t method_resolution_misses = 0;
  bool deterministic = true;

  std::size_t symbol_nodes_total() const {
    return global_symbol_nodes + function_symbol_nodes + interface_symbol_nodes + implementation_symbol_nodes +
           interface_property_symbol_nodes + implementation_property_symbol_nodes + interface_method_symbol_nodes +
           implementation_method_symbol_nodes;
  }

  std::size_t resolution_sites_total() const {
    return implementation_interface_resolution_sites + method_resolution_sites;
  }

  std::size_t resolution_hits_total() const {
    return implementation_interface_resolution_hits + method_resolution_hits;
  }

  std::size_t resolution_misses_total() const {
    return implementation_interface_resolution_misses + method_resolution_misses;
  }
};

struct Objc3MethodLookupOverrideConflictSummary {
  // anchor: lane-B executable metadata semantic validation consumes
  // this deterministic override summary as the canonical legality handoff for
  // superclass override checks before lowering admission exists.
  std::size_t method_lookup_sites = 0;
  std::size_t method_lookup_hits = 0;
  std::size_t method_lookup_misses = 0;
  std::size_t override_lookup_sites = 0;
  std::size_t override_lookup_hits = 0;
  std::size_t override_lookup_misses = 0;
  std::size_t override_conflicts = 0;
  std::size_t unresolved_base_interfaces = 0;
  bool deterministic = true;

  std::size_t total_lookup_sites() const { return method_lookup_sites + override_lookup_sites; }
  std::size_t total_lookup_hits() const { return method_lookup_hits + override_lookup_hits; }
  std::size_t total_lookup_misses() const { return method_lookup_misses + override_lookup_misses; }
};

struct Objc3PropertySynthesisIvarBindingSummary {
  // export-legality anchor: these counts are the canonical sema
  // preconditions for property/ivar runtime export and must not degrade into
  // generic property-declaration totals.
  // semantic-closure gate anchor: lane-E consumes this property/ivar
  // legality summary together with the A003 graph, C003 projection, and D002
  // packaging proofs before the next runtime step section emission begins.
  // corpus-sync anchor: representative legality corpus cases keep
  // these counts deterministic so docs and integrated gate coverage stay
  // aligned on the real runner path.
  // default-binding semantics anchor: matched class implementations
  // resolve synthesis from interface-declared properties first, then fold in
  // optional implementation redeclarations without making redeclaration a
  // prerequisite for default ivar binding.
  std::size_t property_synthesis_sites = 0;
  std::size_t property_synthesis_explicit_ivar_bindings = 0;
  std::size_t property_synthesis_default_ivar_bindings = 0;
  std::size_t interface_owned_property_synthesis_sites = 0;
  std::size_t implementation_property_redeclaration_sites = 0;
  std::size_t ivar_binding_sites = 0;
  std::size_t ivar_binding_resolved = 0;
  std::size_t ivar_binding_missing = 0;
  std::size_t ivar_binding_conflicts = 0;
  bool deterministic = true;
};

struct Objc3IdClassSelObjectPointerTypeCheckingSummary {
  std::size_t canonical_reference_form_count = 0;
  std::size_t canonical_message_scalar_form_count = 0;
  std::size_t canonical_bridge_top_form_count = 0;
  std::size_t param_type_sites = 0;
  std::size_t param_id_spelling_sites = 0;
  std::size_t param_class_spelling_sites = 0;
  std::size_t param_sel_spelling_sites = 0;
  std::size_t param_instancetype_spelling_sites = 0;
  std::size_t param_object_pointer_type_sites = 0;
  std::size_t return_type_sites = 0;
  std::size_t return_id_spelling_sites = 0;
  std::size_t return_class_spelling_sites = 0;
  std::size_t return_sel_spelling_sites = 0;
  std::size_t return_instancetype_spelling_sites = 0;
  std::size_t return_object_pointer_type_sites = 0;
  std::size_t property_type_sites = 0;
  std::size_t property_id_spelling_sites = 0;
  std::size_t property_class_spelling_sites = 0;
  std::size_t property_sel_spelling_sites = 0;
  std::size_t property_instancetype_spelling_sites = 0;
  std::size_t property_object_pointer_type_sites = 0;
  bool canonical_reference_forms_unique = false;
  bool canonical_message_scalar_forms_unique = false;
  bool canonical_bridge_top_forms_unique = false;
  bool canonical_bridge_top_subset_of_reference = false;
  bool canonical_type_form_scaffold_ready = false;
  bool canonical_type_form_diagnostics_hardening_consistent = false;
  bool canonical_type_form_diagnostics_hardening_ready = false;
  std::string canonical_type_form_diagnostics_hardening_key;
  bool canonical_type_form_recovery_determinism_consistent = false;
  bool canonical_type_form_recovery_determinism_ready = false;
  std::string canonical_type_form_recovery_determinism_key;
  bool canonical_type_form_conformance_matrix_consistent = false;
  bool canonical_type_form_conformance_matrix_ready = false;
  std::string canonical_type_form_conformance_matrix_key;
  std::size_t canonical_type_form_conformance_corpus_case_count = 0;
  std::size_t canonical_type_form_conformance_corpus_passed_case_count = 0;
  std::size_t canonical_type_form_conformance_corpus_failed_case_count = 0;
  bool canonical_type_form_conformance_corpus_consistent = false;
  bool canonical_type_form_conformance_corpus_ready = false;
  std::string canonical_type_form_conformance_corpus_key;
  std::size_t canonical_type_form_performance_quality_required_guardrail_count = 0;
  std::size_t canonical_type_form_performance_quality_passed_guardrail_count = 0;
  std::size_t canonical_type_form_performance_quality_failed_guardrail_count = 0;
  bool canonical_type_form_performance_quality_guardrails_consistent = false;
  bool canonical_type_form_performance_quality_guardrails_ready = false;
  std::string canonical_type_form_performance_quality_guardrails_key;
  bool deterministic = true;
};

struct Objc3MessageSendSelectorLoweringSiteMetadata {
  std::string selector;
  std::string selector_lowering_symbol;
  std::size_t argument_count = 0;
  std::size_t selector_piece_count = 0;
  std::size_t selector_argument_piece_count = 0;
  bool unary_form = false;
  bool keyword_form = false;
  bool selector_lowering_is_normalized = false;
  bool receiver_is_nil_literal = false;
  bool nil_receiver_semantics_enabled = false;
  bool nil_receiver_foldable = false;
  bool nil_receiver_requires_runtime_dispatch = true;
  bool nil_receiver_semantics_is_normalized = false;
  bool runtime_link_host_link_required = true;
  bool runtime_link_host_link_elided = false;
  std::size_t runtime_link_host_link_runtime_dispatch_arg_slots = 0;
  std::size_t runtime_link_host_link_declaration_parameter_count = 0;
  std::string runtime_dispatch_bridge_symbol;
  std::string runtime_link_host_link_symbol;
  bool runtime_link_host_link_is_normalized = false;
  bool receiver_is_super_identifier = false;
  bool super_dispatch_enabled = false;
  bool super_dispatch_requires_class_context = false;
  bool super_dispatch_semantics_is_normalized = false;
  std::string method_family_name;
  bool method_family_returns_retained_result = false;
  bool method_family_returns_related_result = false;
  bool method_family_semantics_is_normalized = false;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3MessageSendSelectorLoweringSummary {
  std::size_t message_send_sites = 0;
  std::size_t unary_form_sites = 0;
  std::size_t keyword_form_sites = 0;
  std::size_t selector_lowering_symbol_sites = 0;
  std::size_t selector_lowering_piece_entries = 0;
  std::size_t selector_lowering_argument_piece_entries = 0;
  std::size_t selector_lowering_normalized_sites = 0;
  std::size_t selector_lowering_form_mismatch_sites = 0;
  std::size_t selector_lowering_arity_mismatch_sites = 0;
  std::size_t selector_lowering_symbol_mismatch_sites = 0;
  std::size_t selector_lowering_missing_symbol_sites = 0;
  std::size_t selector_lowering_contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3DispatchAbiMarshallingSummary {
  std::size_t message_send_sites = 0;
  std::size_t receiver_slots = 0;
  std::size_t selector_symbol_slots = 0;
  std::size_t argument_slots = 0;
  std::size_t keyword_argument_slots = 0;
  std::size_t unary_argument_slots = 0;
  std::size_t arity_mismatch_sites = 0;
  std::size_t missing_selector_symbol_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3NilReceiverSemanticsFoldabilitySummary {
  std::size_t message_send_sites = 0;
  std::size_t receiver_nil_literal_sites = 0;
  std::size_t nil_receiver_semantics_enabled_sites = 0;
  std::size_t nil_receiver_foldable_sites = 0;
  std::size_t nil_receiver_runtime_dispatch_required_sites = 0;
  std::size_t non_nil_receiver_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3SuperDispatchMethodFamilySummary {
  std::size_t message_send_sites = 0;
  std::size_t receiver_super_identifier_sites = 0;
  std::size_t super_dispatch_enabled_sites = 0;
  std::size_t super_dispatch_requires_class_context_sites = 0;
  std::size_t method_family_init_sites = 0;
  std::size_t method_family_copy_sites = 0;
  std::size_t method_family_mutable_copy_sites = 0;
  std::size_t method_family_new_sites = 0;
  std::size_t method_family_none_sites = 0;
  std::size_t method_family_returns_retained_result_sites = 0;
  std::size_t method_family_returns_related_result_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3RuntimeLinkHostLinkSummary {
  std::size_t message_send_sites = 0;
  std::size_t runtime_link_required_sites = 0;
  std::size_t runtime_link_elided_sites = 0;
  std::size_t runtime_dispatch_arg_slots = 0;
  std::size_t runtime_dispatch_declaration_parameter_count = 0;
  std::size_t contract_violation_sites = 0;
  std::string runtime_dispatch_symbol = kObjc3RuntimeLinkHostLinkDefaultDispatchSymbol;
  bool default_runtime_dispatch_symbol_binding = true;
  bool deterministic = true;
};

struct Objc3RetainReleaseOperationSummary {
  std::size_t ownership_qualified_sites = 0;
  std::size_t retain_insertion_sites = 0;
  std::size_t release_insertion_sites = 0;
  std::size_t autorelease_insertion_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3WeakUnownedSemanticsSummary {
  std::size_t ownership_candidate_sites = 0;
  std::size_t weak_reference_sites = 0;
  std::size_t unowned_reference_sites = 0;
  std::size_t unowned_safe_reference_sites = 0;
  std::size_t weak_unowned_conflict_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ArcDiagnosticsFixitSummary {
  // freezes the advanced diagnostics taxonomy/portability contract on
  // top of this deterministic ARC/fix-it baseline rather than inventing a
  // parallel semantic diagnostics summary.
  std::size_t ownership_arc_diagnostic_candidate_sites = 0;
  std::size_t ownership_arc_fixit_available_sites = 0;
  std::size_t ownership_arc_profiled_sites = 0;
  std::size_t ownership_arc_weak_unowned_conflict_diagnostic_sites = 0;
  std::size_t ownership_arc_empty_fixit_hint_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

inline constexpr const char *kObjc3ToolingDiagnosticTaxonomyPortabilityContractId =
    "objc3c.tooling.diagnostic.taxonomy.portability.contract.v1";
inline constexpr const char *kObjc3ToolingDiagnosticTaxonomyPortabilityDiagnosticNamespace =
    "O3S";
inline constexpr const char *kObjc3ToolingFeatureSpecificFixitSynthesisContractId =
    "objc3c.tooling.feature.specific.fixit.synthesis.v1";
inline constexpr const char
    *kObjc3ToolingLegacyCanonicalMigrationSemanticsContractId =
        "objc3c.tooling.legacy.canonical.migration.semantics.v1";
inline constexpr const char
    *kObjc3ToolingLegacyCanonicalMigrationDiagnosticCode = "O3S216";

#include "sema/objc3_sema_contract_block_surfaces.h"
