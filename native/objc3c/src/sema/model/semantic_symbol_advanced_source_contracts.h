#pragma once

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
        "runtime-block-lowering-helper-surfaces-preserve-invoke-thunk-byref-copy-dispose-escape-runtime-link-and-arc-cleanup-facts-for-separate-compilation";
inline constexpr const char
    *kObjc3RuntimeBlockOwnershipArtifactPreservationModel =
        "provider-and-consumer-runtime-import-surfaces-and-cross-module-link-plans-preserve-block-ownership-lowering-helper-runtime-link-and-arc-cleanup-facts-beyond-local-ir-object-emission";
inline constexpr const char
    *kObjc3RuntimeBlockOwnershipArtifactPreservationFailClosedModel =
        "missing-or-drifted-block-ownership-preservation-packets-disable-cross-module-block-ownership-claims";
