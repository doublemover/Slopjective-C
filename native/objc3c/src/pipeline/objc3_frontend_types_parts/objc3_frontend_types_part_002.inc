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
        "tooling-now-publishes-one-deterministic-compatibility-packet-over-the-live-canonical-mode-legacy-literal-diagnostics-and-feature-specific-fixit-baseline";
inline constexpr const char
    *kObjc3ToolingLegacyCanonicalMigrationSemanticsLanguageProfileModel =
        "legacy-mode-keeps-inventory-only-while-canonical-mode-fails-closed-on-legacy-yes-no-null-literals-with-o3s216-and-preserves-the-canonical-literal-happy-path";
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
  bool legacy_literal_diagnostics_enabled = false;
  bool legacy_literal_diagnostics_required = true;
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

struct Objc3ToolingDiagnosticTaxonomyPortabilityContractSummary {
  std::string contract_id = kObjc3ToolingDiagnosticTaxonomyPortabilityContractId;
  std::string dependency_contract_id =
      kObjc3ToolingMigrationCanonicalizationSourceCompletionContractId;
  std::string frontend_surface_path =
      kObjc3ToolingDiagnosticTaxonomyPortabilitySurfacePath;
  std::string semantic_model =
      kObjc3ToolingDiagnosticTaxonomyPortabilitySemanticModel;
  std::string portability_model =
      kObjc3ToolingDiagnosticTaxonomyPortabilityPortabilityModel;
  std::vector<std::string> portability_dependency_surface_ids = {
      "objc3c.releaseclaims.publicationmatrix.surface.v1", "objc3c.releaseclaims.compatibility.closeout.v1", "objc3c.releaseclaims.abiobjectmodel.closeout.v1", "objc3c.errors.closeout.surface.v1", "objc3c.concurrency.continuation.closeout.surface.v1",
      "objc3c.concurrency.taskruntime.closeout.surface.v1", "objc3c.concurrency.actor.closeout.surface.v1", "objc3c.ownership.closeout.surface.v1", "objc3c.dispatch.closeout.surface.v1", "objc3c.metaprogramming.closeout.surface.v1",
      "objc3c.interop.closeout.surface.v1",
  };
  std::string diagnostic_namespace =
      kObjc3ToolingDiagnosticTaxonomyPortabilityDiagnosticNamespace;
  std::size_t portability_dependency_count = 0;
  std::size_t diagnostics_total = 0;
  std::size_t diagnostics_after_pass_final = 0;
  std::size_t diagnostics_emitted_total = 0;
  std::size_t ownership_arc_diagnostic_candidate_sites = 0;
  std::size_t ownership_arc_fixit_available_sites = 0;
  std::size_t ownership_arc_profiled_sites = 0;
  std::size_t ownership_arc_weak_unowned_conflict_diagnostic_sites = 0;
  std::size_t ownership_arc_empty_fixit_hint_sites = 0;
  std::size_t ownership_arc_contract_violation_sites = 0;
  std::size_t migration_canonicalization_candidate_sites = 0;
  bool migration_surface_ready = false;
  bool portability_dependencies_frozen = false;
  bool deterministic_handoff = false;
  bool ready_for_lowering_and_runtime = false;
  std::string recovery_replay_key;
  std::string arc_diagnostics_fixit_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3ToolingFeatureSpecificFixitSynthesisSummary {
  std::string contract_id = kObjc3ToolingFeatureSpecificFixitSynthesisContractId;
  std::string dependency_contract_id =
      kObjc3ToolingDiagnosticTaxonomyPortabilityContractId;
  std::string frontend_surface_path =
      kObjc3ToolingFeatureSpecificFixitSynthesisSurfacePath;
  std::string semantic_model =
      kObjc3ToolingFeatureSpecificFixitSynthesisSemanticModel;
  std::vector<std::string> fixit_family_ids = {
      "migration-canonicalization",
      "ownership-arc",
  };
  std::size_t fixit_family_count = 0;
  std::size_t migration_fixit_candidate_sites = 0;
  std::size_t migrator_candidate_sites = 0;
  std::size_t ownership_arc_fixit_available_sites = 0;
  std::size_t ownership_arc_empty_fixit_hint_sites = 0;
  bool diagnostic_taxonomy_ready = false;
  bool deterministic_handoff = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3ToolingLegacyCanonicalMigrationSemanticsSummary {
  std::string contract_id =
      kObjc3ToolingLegacyCanonicalMigrationSemanticsContractId;
  std::string dependency_contract_id =
      kObjc3ToolingFeatureSpecificFixitSynthesisContractId;
  std::string compatibility_semantics_contract_id =
      kObjc3CompatibilityStrictnessClaimSemanticsContractId;
  std::string frontend_surface_path =
      kObjc3ToolingLegacyCanonicalMigrationSemanticsSurfacePath;
  std::string semantic_model =
      kObjc3ToolingLegacyCanonicalMigrationSemanticsSemanticModel;
  std::string language_profile_model =
      kObjc3ToolingLegacyCanonicalMigrationSemanticsLanguageProfileModel;
  std::string effective_language_profile = "canonical";
  bool legacy_literal_diagnostics_enabled = false;
  std::size_t current_run_legacy_literal_sites = 0;
  std::size_t current_run_canonicalization_candidate_sites = 0;
  std::size_t fixit_family_count = 0;
  bool fail_closed = false;
  bool selected_configuration_valid = false;
  bool language_profile_semantics_landed = false;
  bool legacy_literal_diagnostics_semantics_landed = false;
  std::string canonical_mode_rejection_code =
      kObjc3ToolingLegacyCanonicalMigrationDiagnosticCode;
  bool canonical_mode_rejection_ready = false;
  bool feature_specific_fixit_surface_ready = false;
  bool deterministic_handoff = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3InteropForeignSurfaceInterfacePreservationSummary {
  std::string contract_id =
      kObjc3InteropForeignSurfaceInterfacePreservationContractId;
  std::string foreign_import_source_contract_id =
      kObjc3InteropForeignImportSourceClosureContractId;
  std::string cpp_swift_source_contract_id =
      kObjc3InteropCppSwiftInteropAnnotationSourceCompletionContractId;
  std::string surface_path =
      kObjc3InteropForeignSurfaceInterfacePreservationSurfacePath;
  std::string import_artifact_member_name =
      kObjc3InteropForeignSurfaceInterfacePreservationImportArtifactMemberName;
  std::string source_model =
      kObjc3InteropForeignSurfaceInterfacePreservationSourceModel;
  std::string preservation_model =
      kObjc3InteropForeignSurfaceInterfacePreservationModel;
  std::string fail_closed_model =
      kObjc3InteropForeignSurfaceInterfacePreservationFailClosedModel;
  std::string foreign_import_source_replay_key;
  std::string cpp_swift_source_replay_key;
  std::vector<std::string> local_import_module_names_lexicographic;
  std::vector<std::string> imported_provider_module_names_lexicographic;
  std::size_t local_foreign_callable_count = 0;
  std::size_t local_import_module_annotation_count = 0;
  std::size_t local_imported_module_name_count = 0;
  std::size_t local_swift_name_annotation_count = 0;
  std::size_t local_swift_private_annotation_count = 0;
  std::size_t local_cpp_name_annotation_count = 0;
  std::size_t local_header_name_annotation_count = 0;
  std::size_t local_named_annotation_payload_count = 0;
  std::size_t imported_module_count = 0;
  std::size_t imported_foreign_callable_count = 0;
  std::size_t imported_import_module_annotation_count = 0;
  std::size_t imported_imported_module_name_count = 0;
  std::size_t imported_swift_name_annotation_count = 0;
  std::size_t imported_swift_private_annotation_count = 0;
  std::size_t imported_cpp_name_annotation_count = 0;
  std::size_t imported_header_name_annotation_count = 0;
  std::size_t imported_named_annotation_payload_count = 0;
  bool runtime_import_artifact_ready = false;
  bool separate_compilation_preservation_ready = false;
  bool deterministic = false;
  std::string replay_key;
};

struct Objc3InteropHeaderModuleBridgeGenerationSummary {
  std::string contract_id = kObjc3InteropHeaderModuleBridgeGenerationContractId;
  std::string source_contract_id =
      kObjc3InteropHeaderModuleBridgeGenerationSourceContractId;
  std::string preservation_contract_id =
      kObjc3InteropHeaderModuleBridgeGenerationPreservationContractId;
  std::string surface_path =
      kObjc3InteropHeaderModuleBridgeGenerationSurfacePath;
  std::string import_artifact_member_name =
      kObjc3InteropHeaderModuleBridgeGenerationImportArtifactMemberName;
  std::string generation_model = kObjc3InteropHeaderModuleBridgeGenerationModel;
  std::string packaging_model =
      kObjc3InteropHeaderModuleBridgeGenerationPackagingModel;
  std::string fail_closed_model =
      kObjc3InteropHeaderModuleBridgeGenerationFailClosedModel;
  std::string header_artifact_relative_path =
      kObjc3InteropBridgeHeaderArtifactRelativePath;
  std::string module_artifact_relative_path =
      kObjc3InteropBridgeModuleArtifactRelativePath;
  std::string bridge_artifact_relative_path =
      kObjc3InteropBridgeArtifactRelativePath;
  std::vector<std::string> local_import_module_names_lexicographic;
  std::vector<std::string> imported_provider_module_names_lexicographic;
  std::size_t local_foreign_callable_count = 0;
  std::size_t local_import_module_name_count = 0;
  std::size_t local_cpp_name_annotation_count = 0;
  std::size_t local_header_name_annotation_count = 0;
  std::size_t local_swift_name_annotation_count = 0;
  std::size_t imported_module_count = 0;
  bool runtime_generation_ready = false;
  bool cross_module_packaging_ready = false;
  bool deterministic = false;
  std::string preservation_replay_key;
  std::string replay_key;
};

struct Objc3FrontendConcurrencyTaskGroupCancellationSourceClosureSummary {
  std::string contract_id = kObjc3ConcurrencyTaskGroupCancellationSourceClosureContractId;
  std::string frontend_surface_path =
      kObjc3ConcurrencyTaskGroupCancellationSourceClosureSurfacePath;
  std::string source_model = kObjc3ConcurrencyTaskGroupCancellationSourceClosureSourceModel;
  std::string failure_model = kObjc3ConcurrencyTaskGroupCancellationSourceClosureFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimTaskCreationSites,
      kObjc3SourceOnlyFeatureClaimSupportedTaskGroupSurface,
      kObjc3SourceOnlyFeatureClaimTaskExecutorCancellationProfiles,
  };
  std::size_t async_callable_sites = 0;
  std::size_t executor_attribute_sites = 0;
  std::size_t task_creation_sites = 0;
  std::size_t task_group_scope_sites = 0;
  std::size_t task_group_add_task_sites = 0;
  std::size_t task_group_wait_next_sites = 0;
  std::size_t task_group_cancel_all_sites = 0;
  std::size_t cancellation_check_sites = 0;
  std::size_t cancellation_handler_sites = 0;
  bool task_creation_source_supported = false;
  bool task_group_source_supported = false;
  bool cancellation_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendSymbolGraphScopeResolutionSummary {
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
  bool deterministic_symbol_graph_handoff = true;
  bool deterministic_scope_resolution_handoff = true;
  std::string deterministic_handoff_key;

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

struct Objc3RuntimeMetadataClassSourceRecord {
  std::string record_kind;
  std::string name;
  std::string super_name;
  std::vector<std::string> adopted_protocols_lexicographic;
  bool has_super = false;
  bool objc_final_declared = false;
  bool objc_sealed_declared = false;
  std::size_t property_count = 0;
  std::size_t method_count = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3RuntimeMetadataProtocolSourceRecord {
  std::string name;
  std::vector<std::string> inherited_protocols_lexicographic;
  bool is_forward_declaration = false;
  std::size_t property_count = 0;
  std::size_t method_count = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3RuntimeMetadataCategorySourceRecord {
  std::string record_kind;
  std::string class_name;
  std::string category_name;
  std::vector<std::string> adopted_protocols_lexicographic;
  std::size_t property_count = 0;
  std::size_t method_count = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3RuntimeMetadataPropertySourceRecord {
  std::string owner_kind;
  std::string owner_name;
  std::string property_name;
  std::string type_name;
  bool has_getter = false;
  std::string getter_selector;
  bool has_setter = false;
  std::string setter_selector;
  std::string ivar_binding_symbol;
  std::string executable_synthesized_binding_kind;
  std::string executable_synthesized_binding_symbol;
  std::string property_attribute_profile;
  std::string ownership_lifetime_profile;
  std::string ownership_runtime_hook_profile;
  std::string effective_getter_selector;
  bool effective_setter_available = false;
  std::string effective_setter_selector;
  std::string accessor_ownership_profile;
  bool synthesizes_executable_accessors = false;
  std::string getter_storage_runtime_helper_symbol;
  std::string setter_storage_runtime_helper_symbol;
  std::string executable_ivar_layout_symbol;
  std::size_t executable_ivar_layout_slot_index = 0;
  std::size_t executable_ivar_layout_size_bytes = 0;
  std::size_t executable_ivar_layout_alignment_bytes = 0;
  std::size_t executable_ivar_layout_offset_bytes = 0;
  std::size_t executable_ivar_layout_padding_bytes = 0;
  std::size_t executable_ivar_layout_inherited_slot_count = 0;
  std::size_t executable_ivar_layout_inherited_size_bytes = 0;
  std::size_t executable_ivar_layout_owner_size_bytes = 0;
  std::size_t executable_ivar_init_order_index = 0;
  std::size_t executable_ivar_destroy_order_index = 0;
  bool executable_ivar_layout_valid = false;
  std::string executable_ivar_layout_replay_key;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3RuntimeMetadataMethodSourceRecord {
  std::string owner_kind;
  std::string owner_name;
  std::string selector;
  bool is_class_method = false;
  bool has_body = false;
  bool effective_direct_dispatch = false;
  bool objc_final_declared = false;
  std::size_t parameter_count = 0;
  std::string return_type_name;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3RuntimeMetadataIvarSourceRecord {
  std::string owner_kind;
  std::string owner_name;
  std::string property_name;
  std::string ivar_binding_symbol;
  std::string executable_synthesized_binding_kind;
  std::string executable_synthesized_binding_symbol;
  std::string executable_ivar_layout_symbol;
  std::size_t executable_ivar_layout_slot_index = 0;
  std::size_t executable_ivar_layout_size_bytes = 0;
  std::size_t executable_ivar_layout_alignment_bytes = 0;
  std::size_t executable_ivar_layout_offset_bytes = 0;
  std::size_t executable_ivar_layout_padding_bytes = 0;
  std::size_t executable_ivar_layout_inherited_slot_count = 0;
  std::size_t executable_ivar_layout_inherited_size_bytes = 0;
  std::size_t executable_ivar_layout_owner_size_bytes = 0;
  std::size_t executable_ivar_init_order_index = 0;
  std::size_t executable_ivar_destroy_order_index = 0;
  bool executable_ivar_layout_valid = false;
  std::string executable_ivar_layout_replay_key;
  std::string source_model = kObjc3RuntimeMetadataIvarSourceModel;
  unsigned line = 1;
  unsigned column = 1;
};

inline constexpr const char *kObjc3RuntimeExportLegalityContractId =
    "objc3c.runtime.export.legality.freeze.v1";
inline constexpr const char *kObjc3RuntimeExportEnforcementContractId =
    "objc3c.runtime.export.enforcement.v1";

struct Objc3RuntimeMetadataSourceRecordSet {
  std::vector<Objc3RuntimeMetadataClassSourceRecord> classes_lexicographic;
  std::vector<Objc3RuntimeMetadataProtocolSourceRecord> protocols_lexicographic;
  std::vector<Objc3RuntimeMetadataCategorySourceRecord> categories_lexicographic;
  std::vector<Objc3RuntimeMetadataPropertySourceRecord> properties_lexicographic;
  std::vector<Objc3RuntimeMetadataMethodSourceRecord> methods_lexicographic;
  std::vector<Objc3RuntimeMetadataIvarSourceRecord> ivars_lexicographic;
  bool deterministic = false;
};

inline bool IsReadyObjc3RuntimeMetadataSourceRecordSet(
    const Objc3RuntimeMetadataSourceRecordSet &records) {
  return records.deterministic;
}

struct Objc3RuntimeSupportLibraryLinkWiringSummary;
bool IsReadyObjc3RuntimeSupportLibraryLinkWiringSummary(
    const Objc3RuntimeSupportLibraryLinkWiringSummary &summary);

struct Objc3RuntimeStorageReflectionArtifactPreservationSummary {
  std::string contract_id =
      kObjc3RuntimeStorageReflectionArtifactPreservationContractId;
  std::string source_contract_id =
      kObjc3RuntimePropertyIvarStorageAccessorSourceSurfaceContractId;
  std::string dispatch_and_synthesized_accessor_lowering_surface_contract_id =
      kObjc3DispatchAndSynthesizedAccessorLoweringSurfaceContractId;
  std::string executable_property_accessor_layout_lowering_contract_id =
      kObjc3ExecutablePropertyAccessorLayoutLoweringContractId;
  std::string executable_ivar_layout_emission_contract_id =
      kObjc3ExecutableIvarLayoutEmissionContractId;
  std::string executable_synthesized_accessor_property_lowering_contract_id =
      kObjc3ExecutableSynthesizedAccessorPropertyLoweringContractId;
  std::string surface_path =
      kObjc3RuntimeStorageReflectionArtifactPreservationSurfacePath;
  std::string import_artifact_member_name =
      kObjc3RuntimeStorageReflectionArtifactPreservationImportArtifactMemberName;
  std::string source_model =
      kObjc3RuntimeStorageReflectionArtifactPreservationSourceModel;
  std::string preservation_model =
      kObjc3RuntimeStorageReflectionArtifactPreservationModel;
  std::string fail_closed_model =
      kObjc3RuntimeStorageReflectionArtifactPreservationFailClosedModel;
  std::size_t local_property_descriptor_count = 0;
  std::size_t local_ivar_descriptor_count = 0;
  std::size_t implementation_owned_property_entries = 0;
  std::size_t synthesized_accessor_owner_entries = 0;
  std::size_t synthesized_getter_entries = 0;
  std::size_t synthesized_setter_entries = 0;
  std::size_t synthesized_accessor_entries = 0;
  std::size_t current_property_read_entries = 0;
  std::size_t current_property_write_entries = 0;
  std::size_t current_property_exchange_entries = 0;
  std::size_t weak_current_property_load_entries = 0;
  std::size_t weak_current_property_store_entries = 0;
  std::size_t ivar_layout_entries = 0;
  std::size_t ivar_layout_owner_entries = 0;
  bool runtime_import_artifact_ready = false;
  bool separate_compilation_preservation_ready = false;
  bool deterministic = false;
  std::string replay_key;
};

inline Objc3RuntimeStorageReflectionArtifactPreservationSummary
BuildObjc3RuntimeStorageReflectionArtifactPreservationSummary(
    const Objc3RuntimeMetadataSourceRecordSet &records) {
  Objc3RuntimeStorageReflectionArtifactPreservationSummary summary;
  summary.local_property_descriptor_count =
      records.properties_lexicographic.size();
  summary.local_ivar_descriptor_count = records.ivars_lexicographic.size();
  summary.deterministic = records.deterministic;

  for (const auto &property_record : records.properties_lexicographic) {
    if (!property_record.synthesizes_executable_accessors) {
      continue;
    }

    ++summary.synthesized_accessor_owner_entries;
    if (property_record.owner_kind == "class-implementation") {
      ++summary.implementation_owned_property_entries;
    }
    if (!property_record.getter_storage_runtime_helper_symbol.empty()) {
      ++summary.synthesized_getter_entries;
      ++summary.synthesized_accessor_entries;
      if (property_record.getter_storage_runtime_helper_symbol ==
          kObjc3RuntimeReadCurrentPropertyI32Symbol) {
        ++summary.current_property_read_entries;
      } else if (property_record.getter_storage_runtime_helper_symbol ==
                 kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol) {
        ++summary.weak_current_property_load_entries;
      }
    }
    if (!property_record.setter_storage_runtime_helper_symbol.empty()) {
      ++summary.synthesized_setter_entries;
      ++summary.synthesized_accessor_entries;
      if (property_record.setter_storage_runtime_helper_symbol ==
          kObjc3RuntimeWriteCurrentPropertyI32Symbol) {
        ++summary.current_property_write_entries;
      } else if (property_record.setter_storage_runtime_helper_symbol ==
                 kObjc3RuntimeExchangeCurrentPropertyI32Symbol) {
        ++summary.current_property_exchange_entries;
      } else if (property_record.setter_storage_runtime_helper_symbol ==
                 kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol) {
        ++summary.weak_current_property_store_entries;
      }
    }
  }

  std::unordered_set<std::string> ivar_layout_owner_keys;
  for (const auto &ivar_record : records.ivars_lexicographic) {
    if (ivar_record.executable_ivar_layout_symbol.empty()) {
      continue;
    }
    ++summary.ivar_layout_entries;
    ivar_layout_owner_keys.insert(ivar_record.owner_kind + "|" +
                                  ivar_record.owner_name);
  }
  summary.ivar_layout_owner_entries = ivar_layout_owner_keys.size();

  const bool helpers_complete =
      summary.synthesized_accessor_entries ==
      summary.synthesized_getter_entries + summary.synthesized_setter_entries;
  const bool layouts_complete =
      summary.ivar_layout_entries == summary.local_ivar_descriptor_count;
  summary.runtime_import_artifact_ready =
      summary.deterministic && !summary.contract_id.empty() &&
      !summary.source_contract_id.empty() &&
      !summary.dispatch_and_synthesized_accessor_lowering_surface_contract_id
           .empty() &&
      !summary.executable_property_accessor_layout_lowering_contract_id.empty() &&
      !summary.executable_ivar_layout_emission_contract_id.empty() &&
      !summary
           .executable_synthesized_accessor_property_lowering_contract_id
           .empty() &&
      !summary.surface_path.empty() &&
      !summary.import_artifact_member_name.empty() &&
      !summary.source_model.empty() && !summary.preservation_model.empty() &&
      !summary.fail_closed_model.empty() && helpers_complete &&
      layouts_complete;
  summary.separate_compilation_preservation_ready =
      summary.runtime_import_artifact_ready;

  std::ostringstream replay_key;
  replay_key << summary.contract_id
             << "|properties=" << summary.local_property_descriptor_count
             << "|ivars=" << summary.local_ivar_descriptor_count
             << "|impl_owned=" << summary.implementation_owned_property_entries
             << "|owners=" << summary.synthesized_accessor_owner_entries
             << "|accessors=" << summary.synthesized_accessor_entries
             << "|reads=" << summary.current_property_read_entries
             << "|writes=" << summary.current_property_write_entries
             << "|exchanges=" << summary.current_property_exchange_entries
             << "|weak_loads=" << summary.weak_current_property_load_entries
             << "|weak_stores=" << summary.weak_current_property_store_entries
             << "|layouts=" << summary.ivar_layout_entries
             << "|layout_owners=" << summary.ivar_layout_owner_entries;
  summary.replay_key = replay_key.str();
  return summary;
}

struct Objc3RuntimeBlockOwnershipArtifactPreservationSummary {
  std::string contract_id =
      kObjc3RuntimeBlockOwnershipArtifactPreservationContractId;
  std::string source_contract_id =
      kObjc3RuntimeBlockArcLoweringHelperSurfaceContractId;
  std::string block_object_invoke_thunk_lowering_contract_id =
      Expr::kObjc3ExecutableBlockObjectInvokeThunkLoweringContractId;
  std::string block_byref_helper_lowering_contract_id =
      Expr::kObjc3ExecutableBlockByrefHelperLoweringContractId;
  std::string block_escape_runtime_hook_lowering_contract_id =
      Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId;
  std::string runtime_support_library_link_wiring_contract_id =
      kObjc3RuntimeSupportLibraryLinkWiringContractId;
  std::string surface_path =
      kObjc3RuntimeBlockOwnershipArtifactPreservationSurfacePath;
  std::string import_artifact_member_name =
      kObjc3RuntimeBlockOwnershipArtifactPreservationImportArtifactMemberName;
  std::string source_model =
      kObjc3RuntimeBlockOwnershipArtifactPreservationSourceModel;
  std::string preservation_model =
      kObjc3RuntimeBlockOwnershipArtifactPreservationModel;
  std::string fail_closed_model =
      kObjc3RuntimeBlockOwnershipArtifactPreservationFailClosedModel;
  std::size_t local_block_literal_sites = 0;
  std::size_t local_invoke_trampoline_symbolized_sites = 0;
  std::size_t local_copy_helper_required_sites = 0;
  std::size_t local_dispose_helper_required_sites = 0;
  std::size_t local_copy_helper_symbolized_sites = 0;
  std::size_t local_dispose_helper_symbolized_sites = 0;
  std::size_t local_escape_to_heap_sites = 0;
  std::size_t local_byref_layout_symbolized_sites = 0;
  bool runtime_import_artifact_ready = false;
  bool separate_compilation_preservation_ready = false;
  bool runtime_support_library_link_wiring_ready = false;
  bool deterministic = false;
  std::string replay_key;
};

inline Objc3RuntimeBlockOwnershipArtifactPreservationSummary
BuildObjc3RuntimeBlockOwnershipArtifactPreservationSummary(
    const Objc3BlockAbiInvokeTrampolineLoweringContract
        &block_abi_invoke_trampoline_lowering_contract,
    const Objc3BlockStorageEscapeLoweringContract
        &block_storage_escape_lowering_contract,
    const Objc3BlockCopyDisposeLoweringContract
        &block_copy_dispose_lowering_contract,
    const Objc3RuntimeSupportLibraryLinkWiringSummary
        &runtime_support_library_link_wiring) {
  Objc3RuntimeBlockOwnershipArtifactPreservationSummary summary;
  summary.local_block_literal_sites =
      block_abi_invoke_trampoline_lowering_contract.block_literal_sites;
  summary.local_invoke_trampoline_symbolized_sites =
      block_abi_invoke_trampoline_lowering_contract
          .invoke_trampoline_symbolized_sites;
  summary.local_copy_helper_required_sites =
      block_copy_dispose_lowering_contract.copy_helper_required_sites;
  summary.local_dispose_helper_required_sites =
      block_copy_dispose_lowering_contract.dispose_helper_required_sites;
  summary.local_copy_helper_symbolized_sites =
      block_copy_dispose_lowering_contract.copy_helper_symbolized_sites;
  summary.local_dispose_helper_symbolized_sites =
      block_copy_dispose_lowering_contract.dispose_helper_symbolized_sites;
  summary.local_escape_to_heap_sites =
      block_storage_escape_lowering_contract.escape_to_heap_sites;
  summary.local_byref_layout_symbolized_sites =
      block_storage_escape_lowering_contract.byref_layout_symbolized_sites;
  summary.runtime_support_library_link_wiring_ready =
      IsReadyObjc3RuntimeSupportLibraryLinkWiringSummary(
          runtime_support_library_link_wiring);
  summary.deterministic =
      block_abi_invoke_trampoline_lowering_contract.deterministic &&
      block_storage_escape_lowering_contract.deterministic &&
      block_copy_dispose_lowering_contract.deterministic;

  const bool invoke_sites_complete =
      summary.local_invoke_trampoline_symbolized_sites <=
      summary.local_block_literal_sites;
  const bool helper_sites_complete =
      summary.local_copy_helper_symbolized_sites <=
          summary.local_copy_helper_required_sites &&
      summary.local_dispose_helper_symbolized_sites <=
          summary.local_dispose_helper_required_sites;
  const bool byref_sites_complete =
      summary.local_byref_layout_symbolized_sites <=
      block_storage_escape_lowering_contract.byref_slot_count_total;
  const bool escape_sites_complete =
      summary.local_escape_to_heap_sites <=
      block_storage_escape_lowering_contract.block_literal_sites;
  summary.runtime_import_artifact_ready =
      summary.deterministic && !summary.contract_id.empty() &&
      !summary.source_contract_id.empty() &&
      !summary.block_object_invoke_thunk_lowering_contract_id.empty() &&
      !summary.block_byref_helper_lowering_contract_id.empty() &&
      !summary.block_escape_runtime_hook_lowering_contract_id.empty() &&
      !summary.runtime_support_library_link_wiring_contract_id.empty() &&
      !summary.surface_path.empty() &&
      !summary.import_artifact_member_name.empty() &&
      !summary.source_model.empty() && !summary.preservation_model.empty() &&
      !summary.fail_closed_model.empty() &&
      summary.runtime_support_library_link_wiring_ready &&
      invoke_sites_complete && helper_sites_complete && byref_sites_complete &&
      escape_sites_complete;
  summary.separate_compilation_preservation_ready =
      summary.runtime_import_artifact_ready;

  std::ostringstream replay_key;
  replay_key << summary.contract_id
             << "|blocks=" << summary.local_block_literal_sites
             << "|invoke=" << summary.local_invoke_trampoline_symbolized_sites
             << "|copy_required=" << summary.local_copy_helper_required_sites
             << "|dispose_required="
             << summary.local_dispose_helper_required_sites
             << "|copy_symbolized="
             << summary.local_copy_helper_symbolized_sites
             << "|dispose_symbolized="
             << summary.local_dispose_helper_symbolized_sites
             << "|escape=" << summary.local_escape_to_heap_sites
             << "|byref_layout="
             << summary.local_byref_layout_symbolized_sites;
  summary.replay_key = replay_key.str();
  return summary;
}

inline constexpr const char
    *kObjc3RuntimeAwareImportModuleSurfaceContractId =
        "objc3c.runtime.aware.import.module.surface.v1";
inline constexpr const char
    *kObjc3RuntimeAwareImportModuleFrontendClosureContractId =
        "objc3c.runtime.aware.import.module.frontend.closure.v1";
inline constexpr const char
    *kObjc3RuntimeAwareImportModuleFrontendClosureSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_runtime_aware_import_module_frontend_closure";
inline constexpr const char
    *kObjc3RuntimeAwareImportModuleFrontendClosurePayloadModel =
        "runtime-aware-import-module-surface-json-v1";
inline constexpr const char
    *kObjc3RuntimeAwareImportModuleFrontendClosureArtifactSuffix =
        ".runtime-import-surface.json";
inline constexpr const char
    *kObjc3RuntimeAwareImportModuleFrontendClosureArtifactRelativePath =
        "module.runtime-import-surface.json";
inline constexpr const char
    *kObjc3RuntimeAwareImportModuleFrontendAuthorityModel =
        "runtime-import-surface-artifact-derived-from-frozen-import-surface-and-runtime-metadata-source-records";
inline constexpr const char
    *kObjc3RuntimeAwareImportModuleFrontendPayloadOwnershipModel =
        "compiler-emits-runtime-import-surface-artifact-frontend-and-later-module-consumers-own-cross-translation-unit-handoff";

struct Objc3RuntimeAwareImportModuleFrontendClosureSummary {
  std::string contract_id =
      kObjc3RuntimeAwareImportModuleFrontendClosureContractId;
  std::string source_surface_contract_id =
      kObjc3RuntimeAwareImportModuleSurfaceContractId;
  std::string frontend_surface_path =
      kObjc3RuntimeAwareImportModuleFrontendClosureSurfacePath;
  std::string payload_model =
      kObjc3RuntimeAwareImportModuleFrontendClosurePayloadModel;
  std::string artifact_relative_path =
      kObjc3RuntimeAwareImportModuleFrontendClosureArtifactRelativePath;
  std::string authority_model =
      kObjc3RuntimeAwareImportModuleFrontendAuthorityModel;
  std::string payload_ownership_model =
      kObjc3RuntimeAwareImportModuleFrontendPayloadOwnershipModel;
  std::string module_name;
  std::size_t protocol_decl_count = 0;
  std::size_t interface_decl_count = 0;
  std::size_t implementation_decl_count = 0;
  std::size_t interface_category_decl_count = 0;
  std::size_t implementation_category_decl_count = 0;
  std::size_t function_decl_count = 0;
  std::size_t module_import_graph_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  std::size_t class_record_count = 0;
  std::size_t protocol_record_count = 0;
  std::size_t category_record_count = 0;
  std::size_t property_record_count = 0;
  std::size_t method_record_count = 0;
  std::size_t ivar_record_count = 0;
  std::size_t runtime_owned_declaration_count = 0;
  std::size_t superclass_reference_count = 0;
