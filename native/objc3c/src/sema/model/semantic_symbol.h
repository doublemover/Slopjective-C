#pragma once

#include "sema/model/frontend_linkage_summaries.h"
#include "sema/model/frontend_type_source_closure.h"

inline constexpr const char *kObjc3ControlFlowControlFlowSourceClosureContractId =
    "objc3c.control_flow.control.flow.source.closure.v1";
inline constexpr const char *kObjc3ControlFlowControlFlowSourceClosureSurfacePath =
    "frontend.pipeline.semantic_surface.objc_control_flow_control_flow_source_closure";
inline constexpr const char *kObjc3ControlFlowControlFlowSourceClosureSourceModel =
    "guard-condition-lists-defer-statements-and-statement-match-patterns-are-live-frontend-owned-control-flow-surfaces-while-match-expression-guarded-patterns-and-type-test-patterns-remain-fail-closed";
inline constexpr const char *kObjc3ControlFlowControlFlowSourceClosureFailureModel =
    "match-remains-statement-only-and-guarded-or-type-test-patterns-remain-fail-closed-until-later-sema-lowering-and-runtime-work";

struct Objc3FrontendControlFlowControlFlowSourceClosureSummary {
  std::string contract_id = kObjc3ControlFlowControlFlowSourceClosureContractId;
  std::string frontend_surface_path =
      kObjc3ControlFlowControlFlowSourceClosureSurfacePath;
  std::string source_model = kObjc3ControlFlowControlFlowSourceClosureSourceModel;
  std::string failure_model = kObjc3ControlFlowControlFlowSourceClosureFailureModel;
  std::vector<std::string> supported_construct_ids = {
      kObjc3ControlFlowSourceSurfaceGuardBindings,
      kObjc3ControlFlowSourceSurfaceGuardConditionLists,
      kObjc3ControlFlowSourceSurfaceSwitchCasePatterns,
      kObjc3ControlFlowSourceSurfaceDeferStatements,
      kObjc3ControlFlowSourceSurfaceMatchStatement,
      kObjc3ControlFlowSourceSurfaceMatchWildcardPatterns,
      kObjc3ControlFlowSourceSurfaceMatchLiteralPatterns,
      kObjc3ControlFlowSourceSurfaceMatchBindingPatterns,
      kObjc3ControlFlowSourceSurfaceMatchResultCasePatterns,
  };
  std::vector<std::string> fail_closed_construct_ids = {
      kObjc3ControlFlowFailClosedConstructMatchExpression,
      kObjc3ControlFlowFailClosedConstructGuardedPatterns,
      kObjc3ControlFlowFailClosedConstructMatchTypeTestPatterns,
  };
  std::size_t guard_binding_sites = 0;
  std::size_t guard_binding_clause_sites = 0;
  std::size_t guard_boolean_condition_sites = 0;
  std::size_t switch_case_pattern_sites = 0;
  std::size_t switch_default_pattern_sites = 0;
  std::size_t defer_keyword_sites = 0;
  std::size_t match_statement_sites = 0;
  std::size_t match_case_pattern_sites = 0;
  std::size_t match_default_sites = 0;
  std::size_t match_wildcard_pattern_sites = 0;
  std::size_t match_literal_pattern_sites = 0;
  std::size_t match_binding_pattern_sites = 0;
  std::size_t match_result_case_pattern_sites = 0;
  bool guard_binding_source_supported = false;
  bool guard_condition_list_source_supported = false;
  bool switch_case_pattern_source_supported = false;
  bool match_statement_source_supported = false;
  bool match_wildcard_pattern_source_supported = false;
  bool match_literal_pattern_source_supported = false;
  bool match_binding_pattern_source_supported = false;
  bool match_result_case_pattern_source_supported = false;
  bool defer_statement_source_supported = false;
  bool defer_keyword_reserved = false;
  bool defer_fail_closed = false;
  bool match_expression_fail_closed = false;
  bool guarded_pattern_fail_closed = false;
  bool type_test_pattern_fail_closed = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

inline constexpr const char *kObjc3ErrorHandlingErrorSourceClosureContractId =
    "objc3c.error_handling.error.source.closure.v1";
inline constexpr const char *kObjc3ErrorHandlingErrorSourceClosureSurfacePath =
    "frontend.pipeline.semantic_surface.objc_error_handling_error_source_closure";
inline constexpr const char *kObjc3ErrorHandlingErrorSourceClosureSourceModel =
    "throws-declarations-result-carrier-profiles-nserror-bridging-profiles-and-canonical-error-bridge-markers-are-live-frontend-owned-source-surfaces-while-try-throw-and-do-catch-remain-reserved-fail-closed";
inline constexpr const char *kObjc3ErrorHandlingErrorSourceClosureFailureModel =
    "try-expressions-throw-statements-and-do-catch-remain-parse-owned-fail-closed-boundaries-until-runnable-error_handling-sema-lowering-and-runtime-work";

struct Objc3FrontendErrorHandlingErrorSourceClosureSummary {
  std::string contract_id = kObjc3ErrorHandlingErrorSourceClosureContractId;
  std::string frontend_surface_path = kObjc3ErrorHandlingErrorSourceClosureSurfacePath;
  std::string source_model = kObjc3ErrorHandlingErrorSourceClosureSourceModel;
  std::string failure_model = kObjc3ErrorHandlingErrorSourceClosureFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimThrowsDeclarations,
      kObjc3SourceOnlyFeatureClaimResultCarrierProfiles,
      kObjc3SourceOnlyFeatureClaimNSErrorBridgingProfiles,
  };
  std::vector<std::string> fail_closed_construct_ids = {
      kObjc3ErrorHandlingFailClosedConstructTryExpressions,
      kObjc3ErrorHandlingFailClosedConstructThrowStatements,
      kObjc3ErrorHandlingFailClosedConstructDoCatchStatements,
  };
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
  std::size_t try_keyword_sites = 0;
  std::size_t throw_keyword_sites = 0;
  std::size_t catch_keyword_sites = 0;
  bool throws_declaration_source_supported = false;
  bool result_carrier_source_supported = false;
  bool ns_error_bridging_source_supported = false;
  bool error_bridge_marker_source_supported = false;
  bool try_keyword_reserved = false;
  bool throw_keyword_reserved = false;
  bool catch_keyword_reserved = false;
  bool try_fail_closed = false;
  bool throw_fail_closed = false;
  bool do_catch_fail_closed = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

inline constexpr const char *kObjc3ConcurrencyAsyncSourceClosureContractId =
    "objc3c.concurrency.async.source.closure.v1";
inline constexpr const char *kObjc3ConcurrencyAsyncSourceClosureSurfacePath =
    "frontend.pipeline.semantic_surface.objc_concurrency_async_source_closure";
inline constexpr const char *kObjc3ConcurrencyAsyncSourceClosureSourceModel =
    "async-entry-await-expression-and-executor-affinity-syntax-are-live-frontend-owned-source-surfaces-while-continuation-lowering-suspension-cleanup-and-runtime-scheduling-remain-later-runtime-work";
inline constexpr const char *kObjc3ConcurrencyAsyncSourceClosureFailureModel =
    "frontend-source-closure-does-not-yet-claim-runnable-continuation-abi-suspension-cleanup-or-executor-runtime-behavior";
inline constexpr const char *kObjc3ConcurrencyActorMemberIsolationSourceClosureSurfacePath =
    "frontend.pipeline.semantic_surface.objc_concurrency_actor_member_and_isolation_source_closure";
inline constexpr const char *kObjc3ConcurrencyActorMemberIsolationSourceClosureSourceModel =
    "actor-class-declarations-actor-members-and-objc-nonisolated-annotations-are-live-frontend-owned-source-surfaces-while-actor-legality-diagnostics-and-runnable-actor-runtime-behavior-remain-later-runtime-work";
inline constexpr const char *kObjc3ConcurrencyActorMemberIsolationSourceClosureFailureModel =
    "frontend-source-closure-does-not-yet-claim-actor-member-legality-diagnostics-cross-actor-enforcement-or-runnable-actor-runtime-behavior";

struct Objc3FrontendConcurrencyAsyncSourceClosureSummary {
  std::string contract_id = kObjc3ConcurrencyAsyncSourceClosureContractId;
  std::string frontend_surface_path = kObjc3ConcurrencyAsyncSourceClosureSurfacePath;
  std::string source_model = kObjc3ConcurrencyAsyncSourceClosureSourceModel;
  std::string failure_model = kObjc3ConcurrencyAsyncSourceClosureFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimAsyncDeclarations,
      kObjc3SourceOnlyFeatureClaimAwaitExpressions,
      kObjc3SourceOnlyFeatureClaimExecutorAffinityAttributes,
  };
  std::size_t async_keyword_sites = 0;
  std::size_t async_function_sites = 0;
  std::size_t async_method_sites = 0;
  std::size_t await_keyword_sites = 0;
  std::size_t await_expression_sites = 0;
  std::size_t executor_attribute_sites = 0;
  std::size_t executor_main_sites = 0;
  std::size_t executor_global_sites = 0;
  std::size_t executor_named_sites = 0;
  bool async_function_source_supported = false;
  bool async_method_source_supported = false;
  bool await_expression_source_supported = false;
  bool executor_attribute_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary {
  std::string contract_id = kObjc3ActorMemberIsolationSourceClosureContractId;
  std::string frontend_surface_path =
      kObjc3ConcurrencyActorMemberIsolationSourceClosureSurfacePath;
  std::string source_model =
      kObjc3ConcurrencyActorMemberIsolationSourceClosureSourceModel;
  std::string failure_model =
      kObjc3ConcurrencyActorMemberIsolationSourceClosureFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimActorDeclarationMarkers,
      kObjc3SourceOnlyFeatureClaimActorMemberSurfaces,
      kObjc3SourceOnlyFeatureClaimIsolationAnnotationMarkers,
      kObjc3SourceOnlyFeatureClaimActorMetadataSurfaces,
  };
  std::size_t actor_interface_sites = 0;
  std::size_t actor_method_sites = 0;
  std::size_t actor_property_sites = 0;
  std::size_t objc_nonisolated_annotation_sites = 0;
  std::size_t actor_member_executor_annotation_sites = 0;
  std::size_t actor_async_method_sites = 0;
  std::size_t actor_member_metadata_sites = 0;
  bool actor_declaration_source_supported = false;
  bool actor_member_source_supported = false;
  bool isolation_annotation_source_supported = false;
  bool actor_metadata_surface_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

inline constexpr const char *kObjc3ConcurrencyTaskGroupCancellationSourceClosureSurfacePath =
    "frontend.pipeline.semantic_surface.objc_concurrency_task_group_and_cancellation_source_closure";
inline constexpr const char *kObjc3ConcurrencyTaskGroupCancellationSourceClosureSourceModel =
    "task-creation-task-group-and-cancellation-call-surfaces-are-live-frontend-owned-source-surfaces-while-runnable-task-allocation-executor-hops-and-scheduler-execution-remain-later-runtime-work";
inline constexpr const char *kObjc3ConcurrencyTaskGroupCancellationSourceClosureFailureModel =
    "frontend-source-closure-does-not-yet-claim-runnable-task-allocation-task-group-execution-or-scheduler-backed-cancellation-runtime-behavior";

inline constexpr const char *kObjc3OwnershipSystemExtensionSourceClosureSurfacePath =
    "frontend.pipeline.semantic_surface.objc_ownership_resource_borrowed_and_capture_list_source_closure";
inline constexpr const char *kObjc3OwnershipSystemExtensionSourceClosureSourceModel =
    "resource-let-attributes-borrowed-pointer-qualifiers-borrowed-return-relations-and-explicit-block-capture-lists-are-live-frontend-owned-source-surfaces-while-legality-diagnostics-cleanup-lowering-and-runtime-lifetime-enforcement-remain-later-runtime-work";
inline constexpr const char *kObjc3OwnershipSystemExtensionSourceClosureFailureModel =
    "frontend-source-closure-does-not-yet-claim-resource-cleanup-lowering-borrowed-escape-enforcement-or-runnable-capture-ownership-runtime-behavior";
inline constexpr const char *kObjc3OwnershipCleanupResourceCaptureSourceCompletionSurfacePath =
    "frontend.pipeline.semantic_surface.objc_ownership_cleanup_resource_and_capture_source_completion";
inline constexpr const char *kObjc3OwnershipCleanupResourceCaptureSourceCompletionSourceModel =
    "cleanup-hooks-resource-sugar-and-explicit-block-capture-surface-forms-are-live-frontend-owned-source-surfaces-while-legality-diagnostics-cleanup-lowering-and-runtime-resource-ownership-remain-later-runtime-work";
inline constexpr const char *kObjc3OwnershipCleanupResourceCaptureSourceCompletionFailureModel =
    "frontend-source-completion-does-not-yet-claim-cleanup-lowering-resource-runtime-behavior-or-borrowed-pointer-semantic-enforcement";
inline constexpr const char *kObjc3OwnershipRetainableCFamilySourceCompletionSurfacePath =
    "frontend.pipeline.semantic_surface.objc_ownership_retainable_c_family_source_completion";
inline constexpr const char *kObjc3OwnershipRetainableCFamilySourceCompletionSourceModel =
    "retainable-c-family-callable-annotations-and-compatibility-aliases-are-live-frontend-owned-source-surfaces-while-family-legality-arc-integration-and-runtime-interop-remain-later-runtime-work";
inline constexpr const char *kObjc3OwnershipRetainableCFamilySourceCompletionFailureModel =
    "frontend-source-completion-does-not-yet-claim-retainable-c-family-legality-arc-interop-or-runnable-family-runtime-behavior";
inline constexpr const char *kObjc3DispatchDispatchIntentSourceClosureSurfacePath =
    "frontend.pipeline.semantic_surface.objc_dispatch_dispatch_intent_and_dynamism_source_closure";
inline constexpr const char *kObjc3DispatchDispatchIntentSourceClosureSourceModel =
    "direct-final-sealed-and-dynamism-control-attributes-are-live-frontend-owned-source-surfaces-while-legality-lowering-metadata-and-runtime-dispatch-realization-remain-later-runtime-work";
inline constexpr const char *kObjc3DispatchDispatchIntentSourceClosureFailureModel =
    "frontend-source-closure-does-not-yet-claim-direct-dispatch-legality-final-sealed-enforcement-or-runnable-dispatch-boundary-behavior";
inline constexpr const char *kObjc3DispatchDispatchIntentSourceCompletionSurfacePath =
    "frontend.pipeline.semantic_surface.objc_dispatch_dispatch_intent_attribute_and_defaulting_source_completion";
inline constexpr const char *kObjc3DispatchDispatchIntentSourceCompletionSourceModel =
    "prefixed-container-attributes-direct-members-defaulting-and-dynamic-opt-out-surfaces-are-live-frontend-owned-source-surfaces-while-dispatch-legality-lowering-metadata-and-runtime-dispatch-realization-remain-later-runtime-work";
inline constexpr const char *kObjc3DispatchDispatchIntentSourceCompletionFailureModel =
    "frontend-source-completion-does-not-yet-claim-final-sealed-legality-enforcement-direct-call-lowering-or-runnable-dispatch-boundary-behavior";
inline constexpr const char *kObjc3MetaprogrammingMetaprogrammingSourceClosureSurfacePath =
    "frontend.pipeline.semantic_surface.objc_metaprogramming_derive_macro_property_behavior_source_closure";
inline constexpr const char *kObjc3MetaprogrammingMetaprogrammingSourceClosureSourceModel =
    "derive-markers-macro-markers-and-property-behavior-markers-are-live-parser-owned-source-surfaces-while-expansion-synthesis-and-runtime-behavior-remain-deferred";
inline constexpr const char *kObjc3MetaprogrammingMetaprogrammingSourceClosureFailureModel =
    "metaprogramming-stays-source-closure-only-with-no-macro-expansion-derived-conformance-synthesis-or-property-runtime-claims-yet";
inline constexpr const char *kObjc3MetaprogrammingMacroPackageProvenanceSourceCompletionSurfacePath =
    "frontend.pipeline.semantic_surface.objc_metaprogramming_macro_package_and_provenance_source_completion";
inline constexpr const char *kObjc3MetaprogrammingMacroPackageProvenanceSourceCompletionSourceModel =
    "macro-package-markers-macro-provenance-markers-and-expansion-visible-source-state-are-live-frontend-owned-surfaces-while-expansion-execution-and-sandboxing-remain-deferred";
inline constexpr const char *kObjc3MetaprogrammingMacroPackageProvenanceSourceCompletionFailureModel =
    "metaprogramming-source-completion-does-not-yet-claim-macro-expansion-engine-sandbox-or-runtime-package-loading";
inline constexpr const char *kObjc3MetaprogrammingPropertyBehaviorSourceCompletionSurfacePath =
    "frontend.pipeline.semantic_surface.objc_metaprogramming_property_behavior_and_synthesized_declaration_source_completion";
inline constexpr const char *kObjc3MetaprogrammingPropertyBehaviorSourceCompletionSourceModel =
    "behavior-bearing-properties-and-their-deterministic-synthesized-binding-and-accessor-visibility-source-state-are-live-frontend-owned-surfaces-while-real-property-behavior-expansion-and-runtime-hooks-remain-deferred";
inline constexpr const char *kObjc3MetaprogrammingPropertyBehaviorSourceCompletionFailureModel =
    "metaprogramming-source-completion-does-not-yet-claim-property-behavior-expansion-runtime-hooks-or-executable-synthesized-declaration-materialization";
inline constexpr const char *kObjc3InteropForeignImportSourceClosureSurfacePath =
    "frontend.pipeline.semantic_surface.objc_interop_foreign_declaration_and_import_source_closure";
inline constexpr const char *kObjc3InteropForeignImportSourceClosureSourceModel =
    "foreign-callable-markers-import-module-annotations-and-base-interop-annotation-surfaces-are-live-frontend-owned-source-surfaces-while-cpp-swift-annotation-completion-interface-preservation-lowering-and-runtime-bridge-generation-remain-later-runtime-work";
inline constexpr const char *kObjc3InteropForeignImportSourceClosureFailureModel =
    "interop-source-closure-does-not-yet-claim-cpp-swift-specific-annotation-completion-interface-emission-ffi-lowering-or-runnable-bridge-generation";
inline constexpr const char *kObjc3InteropCppSwiftInteropAnnotationSourceCompletionSurfacePath =
    "frontend.pipeline.semantic_surface.objc_interop_cpp_and_swift_interop_annotation_source_completion";
inline constexpr const char *kObjc3InteropCppSwiftInteropAnnotationSourceCompletionSourceModel =
    "swift-facing-name-private-markers-cpp-facing-name-markers-and-header-name-metadata-surfaces-are-live-frontend-owned-source-surfaces-while-interface-preservation-lowering-and-runtime-bridge-generation-remain-later-runtime-work";
inline constexpr const char *kObjc3InteropCppSwiftInteropAnnotationSourceCompletionFailureModel =
    "interop-source-completion-does-not-yet-claim-interface-emission-cpp-swift-runtime-bridging-or-runnable-cross-language-call-behavior";
inline constexpr const char
    *kObjc3ToolingDiagnosticsMigratorSourceInventorySurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_tooling_diagnostics_fixit_and_migrator_source_inventory";
inline constexpr const char
    *kObjc3ToolingDiagnosticsMigratorSourceInventorySourceModel =
        "advanced-error_handling-through-interop-source-packets-and-migration-hints-are-aggregated-into-one-frontend-owned-inventory-for-diagnostics-fix-its-and-migrator-planning-while-feature-specific-synthesis-report-emission-and-release-automation-remain-later-runtime-work";
inline constexpr const char
    *kObjc3ToolingDiagnosticsMigratorSourceInventoryFailureModel =
        "tooling-source-inventory-does-not-yet-claim-feature-specific-fix-it-rewrites-legacy-to-canonical-migration-application-or-machine-readable-release-report-emission";
inline constexpr const char
    *kObjc3ToolingMigrationCanonicalizationSourceCompletionSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_tooling_migration_and_canonicalization_source_completion";
inline constexpr const char
    *kObjc3ToolingMigrationCanonicalizationSourceCompletionSourceModel =
        "canonical-rejection-diagnostics-enabled-legacy-yes-no-null-token-sites-are-canonicalized-by-the-lexer-and-published-as-a-deterministic-frontend-source-completion-packet";
inline constexpr const char
    *kObjc3ToolingMigrationCanonicalizationSourceCompletionCompletionModel =
        "frontend-manifest-and-c-api-artifacts-now-publish-feature-aware-canonicalization-candidate-counts-and-rewrite-kind-counts-for-migrator-planning";
inline constexpr const char
    *kObjc3ToolingMigrationCanonicalizationSourceCompletionFailureModel =
        "tooling-source-completion-does-not-yet-apply-rewrites-or-emit-machine-readable-fix-it-edits-beyond-deterministic-candidate-inventory";
inline constexpr const char
    *kObjc3ToolingDiagnosticTaxonomyPortabilitySurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_tooling_diagnostic_taxonomy_and_portability_contract";
inline constexpr const char
    *kObjc3ToolingDiagnosticTaxonomyPortabilitySemanticModel =
        "advanced-sema-diagnostic-taxonomy-and-portability-freeze-publishes-one-deterministic-tooling-packet-over-the-live-arc-fixit-and-migration-surfaces";
inline constexpr const char
    *kObjc3ToolingDiagnosticTaxonomyPortabilityPortabilityModel =
        "advanced-feature-portability-claims-remain-anchored-to-completed-advanced-closeout-dependencies-while-tooling-freezes-the-frontdoor-sema-contract";
inline constexpr const char
    *kObjc3ToolingFeatureSpecificFixitSynthesisSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_tooling_feature_specific_fixit_synthesis";
inline constexpr const char
    *kObjc3ToolingFeatureSpecificFixitSynthesisSemanticModel =
        "tooling-now-publishes-one-deterministic-implementation-packet-over-the-live-migration-canonicalization-and-ownership-arc-fixit-slices";
inline constexpr const char
    *kObjc3ToolingLegacyCanonicalMigrationSemanticsSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_tooling_legacy_canonical_migration_semantics";
inline constexpr const char
    *kObjc3ToolingLegacyCanonicalMigrationSemanticsSemanticModel =
        "tooling-now-publishes-one-deterministic-retired-mode-packet-over-the-live-canonical-mode-legacy-literal-diagnostics-and-feature-specific-fixit-baseline";
inline constexpr const char
    *kObjc3ToolingLegacyCanonicalMigrationSemanticsLanguageProfileModel =
        "retired-mode-keeps-inventory-only-while-canonical-mode-fails-closed-on-legacy-yes-no-null-literals-with-o3s216-and-preserves-the-canonical-literal-happy-path";
inline constexpr const char
    *kObjc3InteropForeignSurfaceInterfacePreservationSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_interop_foreign_surface_interface_and_module_preservation";
inline constexpr const char
    *kObjc3InteropForeignSurfaceInterfacePreservationImportArtifactMemberName =
        "objc_interop_foreign_surface_interface_and_module_preservation";
inline constexpr const char
    *kObjc3InteropForeignSurfaceInterfacePreservationSourceModel =
        "runtime-import-surface-artifacts-preserve-interop-foreign-declaration-import-module-and-cpp-swift-facing-annotation-facts-for-separate-compilation-and-interface-inspection";
inline constexpr const char
    *kObjc3InteropForeignSurfaceInterfacePreservationModel =
        "provider-and-consumer-runtime-import-surface-artifacts-preserve-foreign-callable-import-module-and-cpp-swift-annotation-counts-and-module-name-facts-beyond-local-source-closure-packets";
inline constexpr const char
    *kObjc3InteropForeignSurfaceInterfacePreservationFailClosedModel =
        "missing-or-drifted-interop-preservation-packets-disable-cross-module-foreign-surface-preservation-claims";
inline constexpr const char
    *kObjc3InteropHeaderModuleBridgeGenerationContractId =
        "objc3c.interop.header.module.and.bridge.generation.v1";
inline constexpr const char
    *kObjc3InteropHeaderModuleBridgeGenerationSourceContractId =
        "objc3c.interop.bridge.packaging.and.toolchain.contract.v1";
inline constexpr const char
    *kObjc3InteropHeaderModuleBridgeGenerationPreservationContractId =
        "objc3c.interop.ffi.metadata.interface.preservation.v1";
inline constexpr const char
    *kObjc3InteropHeaderModuleBridgeGenerationSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_interop_header_module_and_bridge_generation";
inline constexpr const char
    *kObjc3InteropHeaderModuleBridgeGenerationImportArtifactMemberName =
        "objc_interop_header_module_and_bridge_generation";
inline constexpr const char
    *kObjc3InteropHeaderModuleBridgeGenerationModel =
        "compiler-emits-deterministic-header-modulemap-and-bridge-json-artifacts-for-supported-interop-foreign-callable-surfaces";
inline constexpr const char
    *kObjc3InteropHeaderModuleBridgeGenerationPackagingModel =
        "runtime-import-surfaces-and-cross-module-link-plans-preserve-generated-interop-header-module-and-bridge-artifact-paths";
inline constexpr const char
    *kObjc3InteropHeaderModuleBridgeGenerationFailClosedModel =
        "missing-generated-artifacts-or-drifted-import-surface-bridge-packets-disable-live-interop-bridge-generation-claims";
inline constexpr const char
    *kObjc3RuntimeStorageReflectionArtifactPreservationContractId =
        "objc3c.runtime.storage.reflection.artifact.preservation.v1";
inline constexpr const char
    *kObjc3RuntimeStorageReflectionArtifactPreservationSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_runtime_storage_reflection_artifact_preservation";
inline constexpr const char
    *kObjc3RuntimeStorageReflectionArtifactPreservationImportArtifactMemberName =
        "objc_runtime_storage_reflection_artifact_preservation";
inline constexpr const char
    *kObjc3RuntimeStorageReflectionArtifactPreservationSourceModel =
        "runtime-metadata-source-records-preserve-property-ivar-accessor-layout-and-runtime-helper-facts-for-separate-compilation";
inline constexpr const char
    *kObjc3RuntimeStorageReflectionArtifactPreservationModel =
        "provider-and-consumer-runtime-import-surfaces-and-cross-module-link-plans-preserve-property-ivar-accessor-layout-and-runtime-helper-facts-beyond-local-ir-object-emission";
inline constexpr const char
    *kObjc3RuntimeStorageReflectionArtifactPreservationFailClosedModel =
        "missing-or-drifted-storage-reflection-preservation-packets-disable-cross-module-storage-reflection-claims";
inline constexpr const char
    *kObjc3RuntimeBlockOwnershipArtifactPreservationContractId =
        "objc3c.runtime.block.ownership.artifact.preservation.v1";
inline constexpr const char
    *kObjc3RuntimeBlockOwnershipArtifactPreservationSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_runtime_block_ownership_artifact_preservation";
inline constexpr const char
    *kObjc3RuntimeBlockOwnershipArtifactPreservationImportArtifactMemberName =
        "objc_runtime_block_ownership_artifact_preservation";
inline constexpr const char
    *kObjc3RuntimeBlockOwnershipArtifactPreservationSourceModel =
        "runtime-block-lowering-helper-surfaces-preserve-invoke-thunk-byref-copy-dispose-escape-and-runtime-link-facts-for-separate-compilation";
inline constexpr const char
    *kObjc3RuntimeBlockOwnershipArtifactPreservationModel =
        "provider-and-consumer-runtime-import-surfaces-and-cross-module-link-plans-preserve-block-ownership-lowering-helper-and-runtime-link-facts-beyond-local-ir-object-emission";
inline constexpr const char
    *kObjc3RuntimeBlockOwnershipArtifactPreservationFailClosedModel =
        "missing-or-drifted-block-ownership-preservation-packets-disable-cross-module-block-ownership-claims";

struct Objc3FrontendOwnershipSystemExtensionSourceClosureSummary {
  std::string contract_id = kObjc3OwnershipSystemExtensionSourceClosureContractId;
  std::string frontend_surface_path = kObjc3OwnershipSystemExtensionSourceClosureSurfacePath;
  std::string source_model = kObjc3OwnershipSystemExtensionSourceClosureSourceModel;
  std::string failure_model = kObjc3OwnershipSystemExtensionSourceClosureFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimResourceHandleAnnotations,
      kObjc3SourceOnlyFeatureClaimBorrowedPointerAnnotations,
      kObjc3SourceOnlyFeatureClaimBorrowedReturnRelations,
      kObjc3SourceOnlyFeatureClaimExplicitBlockCaptureLists,
  };
  std::size_t resource_attribute_sites = 0;
  std::size_t resource_close_clause_sites = 0;
  std::size_t resource_invalid_clause_sites = 0;
  std::size_t borrowed_pointer_sites = 0;
  std::size_t returns_borrowed_attribute_sites = 0;
  std::size_t explicit_capture_list_sites = 0;
  std::size_t explicit_capture_item_sites = 0;
  std::size_t explicit_capture_weak_sites = 0;
  std::size_t explicit_capture_unowned_sites = 0;
  std::size_t explicit_capture_move_sites = 0;
  std::size_t explicit_capture_plain_sites = 0;
  bool resource_attribute_source_supported = false;
  bool borrowed_pointer_source_supported = false;
  bool returns_borrowed_source_supported = false;
  bool explicit_capture_list_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendOwnershipCleanupResourceCaptureSourceCompletionSummary {
  std::string contract_id =
      kObjc3OwnershipCleanupResourceCaptureSurfaceCompletionContractId;
  std::string frontend_surface_path =
      kObjc3OwnershipCleanupResourceCaptureSourceCompletionSurfacePath;
  std::string source_model =
      kObjc3OwnershipCleanupResourceCaptureSourceCompletionSourceModel;
  std::string failure_model =
      kObjc3OwnershipCleanupResourceCaptureSourceCompletionFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimCleanupHookAnnotations,
      kObjc3SourceOnlyFeatureClaimResourceHandleAnnotations,
      kObjc3SourceOnlyFeatureClaimCleanupResourceSugar,
      kObjc3SourceOnlyFeatureClaimExplicitBlockCaptureLists,
  };
  std::size_t cleanup_attribute_sites = 0;
  std::size_t cleanup_sugar_sites = 0;
  std::size_t resource_attribute_sites = 0;
  std::size_t resource_sugar_sites = 0;
  std::size_t resource_close_clause_sites = 0;
  std::size_t resource_invalid_clause_sites = 0;
  std::size_t explicit_capture_list_sites = 0;
  std::size_t explicit_capture_item_sites = 0;
  std::size_t explicit_capture_weak_sites = 0;
  std::size_t explicit_capture_unowned_sites = 0;
  std::size_t explicit_capture_move_sites = 0;
  std::size_t explicit_capture_plain_sites = 0;
  bool cleanup_attribute_source_supported = false;
  bool resource_sugar_source_supported = false;
  bool explicit_capture_list_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary {
  std::string contract_id =
      kObjc3OwnershipRetainableCFamilySourceCompletionContractId;
  std::string frontend_surface_path =
      kObjc3OwnershipRetainableCFamilySourceCompletionSurfacePath;
  std::string source_model =
      kObjc3OwnershipRetainableCFamilySourceCompletionSourceModel;
  std::string failure_model =
      kObjc3OwnershipRetainableCFamilySourceCompletionFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimRetainableCFamilyCallableAnnotations,
      kObjc3SourceOnlyFeatureClaimRetainableCFamilyCompatibilityAliases,
  };
  std::size_t family_retain_sites = 0;
  std::size_t family_release_sites = 0;
  std::size_t family_autorelease_sites = 0;
  std::size_t compatibility_returns_retained_sites = 0;
  std::size_t compatibility_returns_not_retained_sites = 0;
  std::size_t compatibility_consumed_sites = 0;
  bool callable_annotation_source_supported = false;
  bool compatibility_alias_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendDispatchDispatchIntentSourceClosureSummary {
  std::string contract_id = kObjc3DispatchDispatchIntentSourceClosureContractId;
  std::string frontend_surface_path =
      kObjc3DispatchDispatchIntentSourceClosureSurfacePath;
  std::string source_model = kObjc3DispatchDispatchIntentSourceClosureSourceModel;
  std::string failure_model = kObjc3DispatchDispatchIntentSourceClosureFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimDirectMethodAnnotations,
      kObjc3SourceOnlyFeatureClaimDirectMembersClassAnnotations,
      kObjc3SourceOnlyFeatureClaimFinalAnnotations,
      kObjc3SourceOnlyFeatureClaimSealedAnnotations,
      kObjc3SourceOnlyFeatureClaimDynamicMethodAnnotations,
  };
  std::size_t direct_callable_sites = 0;
  std::size_t final_callable_sites = 0;
  std::size_t dynamic_callable_sites = 0;
  std::size_t direct_members_container_sites = 0;
  std::size_t final_container_sites = 0;
  std::size_t sealed_container_sites = 0;
  std::size_t actor_container_sites = 0;
  bool callable_annotation_source_supported = false;
  bool container_annotation_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendDispatchDispatchIntentSourceCompletionSummary {
  std::string contract_id = kObjc3DispatchDispatchIntentSourceCompletionContractId;
  std::string frontend_surface_path =
      kObjc3DispatchDispatchIntentSourceCompletionSurfacePath;
  std::string source_model =
      kObjc3DispatchDispatchIntentSourceCompletionSourceModel;
  std::string failure_model =
      kObjc3DispatchDispatchIntentSourceCompletionFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimPrefixedDispatchIntentAttributes,
      kObjc3SourceOnlyFeatureClaimDirectMembersDefaultingSurfaces,
      kObjc3SourceOnlyFeatureClaimDynamicOptOutDefaultingSurfaces,
      kObjc3SourceOnlyFeatureClaimFinalAnnotations,
      kObjc3SourceOnlyFeatureClaimSealedAnnotations,
  };
  std::size_t prefixed_container_attribute_sites = 0;
  std::size_t direct_members_container_sites = 0;
  std::size_t final_container_sites = 0;
  std::size_t sealed_container_sites = 0;
  std::size_t effective_direct_member_sites = 0;
  std::size_t direct_members_defaulted_method_sites = 0;
  std::size_t direct_members_dynamic_opt_out_sites = 0;
  bool prefixed_attribute_source_supported = false;
  bool defaulting_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendMetaprogrammingMetaprogrammingSourceClosureSummary {
  std::string contract_id = kObjc3MetaprogrammingMetaprogrammingSourceClosureContractId;
  std::string frontend_surface_path =
      kObjc3MetaprogrammingMetaprogrammingSourceClosureSurfacePath;
  std::string source_model =
      kObjc3MetaprogrammingMetaprogrammingSourceClosureSourceModel;
  std::string failure_model =
      kObjc3MetaprogrammingMetaprogrammingSourceClosureFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimDeriveMarkers,
      kObjc3SourceOnlyFeatureClaimMacroMarkers,
      kObjc3SourceOnlyFeatureClaimPropertyBehaviorMarkers,
  };
  std::size_t derive_marker_sites = 0;
  std::size_t macro_marker_sites = 0;
  std::size_t property_behavior_sites = 0;
  bool derive_marker_source_supported = false;
  bool macro_marker_source_supported = false;
  bool property_behavior_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendMetaprogrammingMacroPackageProvenanceSourceCompletionSummary {
  std::string contract_id =
      kObjc3MetaprogrammingMacroPackageProvenanceSourceCompletionContractId;
  std::string frontend_surface_path =
      kObjc3MetaprogrammingMacroPackageProvenanceSourceCompletionSurfacePath;
  std::string source_model =
      kObjc3MetaprogrammingMacroPackageProvenanceSourceCompletionSourceModel;
  std::string failure_model =
      kObjc3MetaprogrammingMacroPackageProvenanceSourceCompletionFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimMacroMarkers,
      kObjc3SourceOnlyFeatureClaimMacroPackageMarkers,
      kObjc3SourceOnlyFeatureClaimMacroProvenanceMarkers,
      kObjc3SourceOnlyFeatureClaimMacroCacheKeyMarkers,
      kObjc3SourceOnlyFeatureClaimMacroSandboxPolicyMarkers,
      kObjc3SourceOnlyFeatureClaimMacroExpansionVisibleState,
  };
  std::size_t macro_marker_sites = 0;
  std::size_t macro_package_sites = 0;
  std::size_t macro_provenance_sites = 0;
  std::size_t macro_cache_key_sites = 0;
  std::size_t macro_sandbox_policy_sites = 0;
  std::size_t expansion_visible_macro_sites = 0;
  bool macro_package_source_supported = false;
  bool macro_provenance_source_supported = false;
  bool macro_cache_key_source_supported = false;
  bool macro_sandbox_policy_source_supported = false;
  bool expansion_visible_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary {
  std::string contract_id =
      kObjc3MetaprogrammingPropertyBehaviorSourceCompletionContractId;
  std::string frontend_surface_path =
      kObjc3MetaprogrammingPropertyBehaviorSourceCompletionSurfacePath;
  std::string source_model =
      kObjc3MetaprogrammingPropertyBehaviorSourceCompletionSourceModel;
  std::string failure_model =
      kObjc3MetaprogrammingPropertyBehaviorSourceCompletionFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimPropertyBehaviorMarkers,
      kObjc3SourceOnlyFeatureClaimPropertyBehaviorSynthesisVisibility,
  };
  std::size_t property_behavior_sites = 0;
  std::size_t interface_property_behavior_sites = 0;
  std::size_t implementation_property_behavior_sites = 0;
  std::size_t protocol_property_behavior_sites = 0;
  std::size_t synthesized_binding_visible_sites = 0;
  std::size_t synthesized_getter_visible_sites = 0;
  std::size_t synthesized_setter_visible_sites = 0;
  bool property_behavior_source_supported = false;
  bool synthesized_declaration_visibility_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendInteropForeignImportSourceClosureSummary {
  std::string contract_id = kObjc3InteropForeignImportSourceClosureContractId;
  std::string frontend_surface_path =
      kObjc3InteropForeignImportSourceClosureSurfacePath;
  std::string source_model =
      kObjc3InteropForeignImportSourceClosureSourceModel;
  std::string failure_model =
      kObjc3InteropForeignImportSourceClosureFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimForeignDeclarationMarkers,
      kObjc3SourceOnlyFeatureClaimImportedModuleAnnotations,
      kObjc3SourceOnlyFeatureClaimInteropAnnotationMarkers,
  };
  std::size_t foreign_callable_sites = 0;
  std::size_t extern_foreign_callable_sites = 0;
  std::size_t import_module_annotation_sites = 0;
  std::size_t imported_module_name_sites = 0;
  std::size_t export_header_annotation_sites = 0;
  std::size_t export_header_name_sites = 0;
  std::size_t mixed_image_annotation_sites = 0;
  std::size_t mixed_image_name_sites = 0;
  std::size_t package_entry_annotation_sites = 0;
  std::size_t package_entry_name_sites = 0;
  std::size_t interop_annotation_sites = 0;
  bool foreign_declaration_source_supported = false;
  bool imported_surface_source_supported = false;
  bool interop_annotation_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendInteropCppSwiftInteropAnnotationSourceCompletionSummary {
  std::string contract_id =
      kObjc3InteropCppSwiftInteropAnnotationSourceCompletionContractId;
  std::string frontend_surface_path =
      kObjc3InteropCppSwiftInteropAnnotationSourceCompletionSurfacePath;
  std::string source_model =
      kObjc3InteropCppSwiftInteropAnnotationSourceCompletionSourceModel;
  std::string failure_model =
      kObjc3InteropCppSwiftInteropAnnotationSourceCompletionFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimSwiftFacingAnnotationMarkers,
      kObjc3SourceOnlyFeatureClaimCppFacingAnnotationMarkers,
      kObjc3SourceOnlyFeatureClaimInteropMetadataAnnotationMarkers,
      kObjc3SourceOnlyFeatureClaimInteropNamedMetadataPayloads,
  };
  std::size_t swift_name_annotation_sites = 0;
  std::size_t swift_private_annotation_sites = 0;
  std::size_t cpp_name_annotation_sites = 0;
  std::size_t header_name_annotation_sites = 0;
  std::size_t abi_alignment_annotation_sites = 0;
  std::size_t foreign_type_annotation_sites = 0;
  std::size_t interop_metadata_annotation_sites = 0;
  std::size_t named_annotation_payload_sites = 0;
  bool swift_annotation_source_supported = false;
  bool cpp_annotation_source_supported = false;
  bool interop_metadata_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendToolingDiagnosticsMigratorSourceInventorySummary {
  std::string contract_id =
      kObjc3ToolingDiagnosticsMigratorSourceInventoryContractId;
  std::string frontend_surface_path =
      kObjc3ToolingDiagnosticsMigratorSourceInventorySurfacePath;
  std::string source_model =
      kObjc3ToolingDiagnosticsMigratorSourceInventorySourceModel;
  std::string failure_model =
      kObjc3ToolingDiagnosticsMigratorSourceInventoryFailureModel;
  std::vector<std::string> dependency_contract_ids = {
      kObjc3ErrorHandlingErrorSourceClosureContractId,
      kObjc3ConcurrencyAsyncSourceClosureContractId,
      kObjc3ActorMemberIsolationSourceClosureContractId,
      kObjc3ConcurrencyTaskGroupCancellationSourceClosureContractId,
      kObjc3OwnershipSystemExtensionSourceClosureContractId,
      kObjc3OwnershipCleanupResourceCaptureSurfaceCompletionContractId,
      kObjc3OwnershipRetainableCFamilySourceCompletionContractId,
      kObjc3DispatchDispatchIntentSourceClosureContractId,
      kObjc3DispatchDispatchIntentSourceCompletionContractId,
      kObjc3MetaprogrammingMetaprogrammingSourceClosureContractId,
      kObjc3MetaprogrammingMacroPackageProvenanceSourceCompletionContractId,
      kObjc3MetaprogrammingPropertyBehaviorSourceCompletionContractId,
      kObjc3InteropForeignImportSourceClosureContractId,
      kObjc3InteropCppSwiftInteropAnnotationSourceCompletionContractId,
  };
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimAdvancedDiagnosticInventory,
      kObjc3SourceOnlyFeatureClaimAdvancedFixItInventory,
      kObjc3SourceOnlyFeatureClaimAdvancedMigratorInventory,
  };
  std::size_t advanced_feature_family_count = 0;
  std::size_t dependency_surface_count = 0;
  std::size_t aggregated_source_only_claim_count = 0;
  std::size_t fail_closed_construct_count = 0;
  std::size_t diagnostic_surface_sites = 0;
  std::size_t fixit_surface_sites = 0;
  std::size_t migrator_surface_sites = 0;
  std::size_t canonicalization_hint_sites = 0;
  std::size_t error_surface_sites = 0;
  std::size_t concurrency_surface_sites = 0;
  std::size_t system_surface_sites = 0;
  std::size_t dispatch_surface_sites = 0;
  std::size_t metaprogramming_surface_sites = 0;
  std::size_t interop_surface_sites = 0;
  bool diagnostics_inventory_source_supported = false;
  bool fixit_inventory_source_supported = false;
  bool migrator_inventory_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendToolingMigrationCanonicalizationSourceCompletionSummary {
  std::string contract_id =
      kObjc3ToolingMigrationCanonicalizationSourceCompletionContractId;
  std::string dependency_contract_id =
      kObjc3ToolingDiagnosticsMigratorSourceInventoryContractId;
  std::string frontend_surface_path =
      kObjc3ToolingMigrationCanonicalizationSourceCompletionSurfacePath;
  std::string source_model =
      kObjc3ToolingMigrationCanonicalizationSourceCompletionSourceModel;
  std::string completion_model =
      kObjc3ToolingMigrationCanonicalizationSourceCompletionCompletionModel;
  std::string failure_model =
      kObjc3ToolingMigrationCanonicalizationSourceCompletionFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimAdvancedCanonicalizationInventory,
      kObjc3SourceOnlyFeatureClaimAdvancedMigrationAssistFlow,
  };
  std::string language_profile = "canonical";
  bool canonical_literal_rejection_diagnostics_enabled = false;
  bool canonical_literal_rejection_diagnostics_required = true;
  std::size_t legacy_yes_sites = 0;
  std::size_t legacy_no_sites = 0;
  std::size_t legacy_null_sites = 0;
  std::size_t legacy_total_sites = 0;
  std::size_t canonical_true_rewrite_sites = 0;
  std::size_t canonical_false_rewrite_sites = 0;
  std::size_t canonical_nil_rewrite_sites = 0;
  std::size_t canonicalization_candidate_sites = 0;
  std::size_t fixit_candidate_sites = 0;
  std::size_t migrator_candidate_sites = 0;
  bool dependency_inventory_ready = false;
  bool canonicalization_surface_supported = false;
  bool fixit_migration_surface_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};
