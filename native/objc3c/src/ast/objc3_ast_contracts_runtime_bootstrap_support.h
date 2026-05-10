#pragma once

#include <cstdint>

inline constexpr const char *kObjc3RuntimeBootstrapSemanticsContractId =
    "objc3c.runtime.startup.bootstrap.semantics.v1";
inline constexpr const char *kObjc3RuntimeBootstrapSemanticsSurfacePath =
    "frontend.pipeline.semantic_surface.objc_runtime_startup_bootstrap_semantics";
inline constexpr const char *kObjc3RuntimeBootstrapResultModel =
    "zero-success-negative-fail-closed";
inline constexpr const char
    *kObjc3RuntimeBootstrapRegistrationOrderOrdinalModel =
        "strictly-monotonic-positive-registration-order-ordinal";
inline constexpr const char *kObjc3RuntimeBootstrapStateSnapshotSymbol =
    "objc3_runtime_copy_registration_state_for_testing";
inline constexpr const char
    *kObjc3RuntimeBootstrapDuplicateInstallDiagnosticModel =
        "duplicate-install-rejections-publish-the-rejected-module-identity-and-registration-ordinal-without-advancing-runtime-installation-state";
inline constexpr const char
    *kObjc3RuntimeBootstrapOutOfOrderDiagnosticModel =
        "out-of-order-install-rejections-publish-the-rejected-module-identity-and-registration-ordinal-without-advancing-runtime-installation-state";
inline constexpr const char
    *kObjc3RuntimeBootstrapRejectedModuleNameField =
        "last_rejected_module_name";
inline constexpr const char
    *kObjc3RuntimeBootstrapRejectedTranslationUnitIdentityKeyField =
        "last_rejected_translation_unit_identity_key";
inline constexpr const char
    *kObjc3RuntimeBootstrapNextExpectedRegistrationOrderField =
        "next_expected_registration_order_ordinal";
inline constexpr const char
    *kObjc3RuntimeBootstrapLastSuccessfulRegistrationOrderField =
        "last_successful_registration_order_ordinal";
inline constexpr const char
    *kObjc3RuntimeBootstrapLastRejectedRegistrationOrderField =
        "last_rejected_registration_order_ordinal";
inline constexpr int kObjc3RuntimeBootstrapSuccessStatusCode = 0;
inline constexpr int kObjc3RuntimeBootstrapInvalidDescriptorStatusCode = -1;
inline constexpr int
    kObjc3RuntimeBootstrapDuplicateRegistrationStatusCode = -2;
inline constexpr int kObjc3RuntimeBootstrapOutOfOrderStatusCode = -3;
inline constexpr int
    kObjc3RuntimeBootstrapInvalidRegistrationRootsStatusCode = -4;
inline constexpr std::uint64_t
    kObjc3RuntimeBootstrapTranslationUnitRegistrationOrderOrdinal = 1u;
inline constexpr const char *kObjc3RuntimeBootstrapApiContractId =
    "objc3c.runtime.bootstrap.api.freeze.v1";
inline constexpr const char *kObjc3RuntimeBootstrapApiSurfacePath =
    "frontend.pipeline.semantic_surface.objc_runtime_bootstrap_api_contract";
inline constexpr const char *kObjc3RuntimeBootstrapApiStatusEnumType =
    "objc3_runtime_registration_status_code";
inline constexpr const char *kObjc3RuntimeBootstrapApiImageDescriptorType =
    "objc3_runtime_image_descriptor";
inline constexpr const char *kObjc3RuntimeBootstrapApiSelectorHandleType =
    "objc3_runtime_selector_handle";
inline constexpr const char *kObjc3RuntimeBootstrapApiRegistrationSnapshotType =
    "objc3_runtime_registration_state_snapshot";
inline constexpr const char *kObjc3RuntimeBootstrapApiStateLockingModel =
    "process-global-mutex-serialized-runtime-state";
inline constexpr const char *kObjc3RuntimeBootstrapApiStartupInvocationModel =
    "generated-init-stub-calls-runtime-register-image";
inline constexpr const char *kObjc3RuntimeBootstrapApiImageWalkLifecycleModel =
    "deferred-until-next-runtime-phase";
inline constexpr const char
    *kObjc3RuntimeBootstrapApiDeterministicResetLifecycleModel =
        "deferred-until-next-runtime-phase";
inline constexpr const char
    *kObjc3RuntimeBootstrapRegistrationSourceSurfaceContractId =
        "objc3c.runtime.bootstrap.registration.source.surface.v1";
inline constexpr const char
    *kObjc3RuntimeBootstrapLoweringRegistrationArtifactSurfaceContractId =
        "objc3c.runtime.bootstrap.lowering.registration.artifact.surface.v1";
inline constexpr const char
    *kObjc3RuntimeMultiImageStartupOrderingSourceSurfaceContractId =
        "objc3c.runtime.multi.image.startup.ordering.source.surface.v1";
inline constexpr const char
    *kObjc3RuntimeObjectModelRealizationSourceSurfaceContractId =
        "objc3c.runtime.object.model.realization.source.surface.v1";
inline constexpr const char
    *kObjc3RuntimeRealizationLoweringReflectionArtifactSurfaceContractId =
        "objc3c.runtime.realization.lowering.reflection.artifact.surface.v1";
inline constexpr const char
    *kObjc3RuntimeDispatchTableReflectionRecordLoweringSurfaceContractId =
        "objc3c.runtime.dispatch.table.reflection.record.lowering.surface.v1";
inline constexpr const char *kObjc3RuntimeObjectModelQueryBoundaryModel =
    "public-runtime-header-plus-private-testing-snapshots-freeze-the-object-model-lookup-and-reflection-query-surface-without-widening-the-public-abi";
inline constexpr const char
    *kObjc3RuntimeRealizationLookupReflectionImplementationModel =
        "runtime-owned-class-property-protocol-and-method-cache-query-snapshots-publish-coherent-last-query-state-and-live-object-model-counts";
inline constexpr const char *kObjc3RuntimeReflectionQuerySurfaceContractId =
    "objc3c.runtime.reflection.query.surface.v1";
inline constexpr const char *kObjc3RuntimeRealizationLookupSemanticsSurfaceContractId =
    "objc3c.runtime.realization.lookup.semantics.v1";
inline constexpr const char *kObjc3RuntimeClassMetaclassProtocolRealizationSurfaceContractId =
    "objc3c.runtime.class.metaclass.protocol.realization.v1";
inline constexpr const char *kObjc3RuntimeCategoryAttachmentMergedDispatchSurfaceContractId =
    "objc3c.runtime.category.attachment.merged.dispatch.surface.v1";
inline constexpr const char
    *kObjc3RuntimeReflectionVisibilityCoherenceDiagnosticsSurfaceContractId =
        "objc3c.runtime.reflection.visibility.coherence.diagnostics.surface.v1";
inline constexpr const char *kObjc3RuntimeUnifiedConcurrencySourceSurfaceContractId =
    "objc3c.runtime.unified.concurrency.source.surface.v1";
inline constexpr const char
    *kObjc3RuntimeAsyncTaskActorNormalizationCompletionSurfaceContractId =
        "objc3c.runtime.async.task.actor.normalization.completion.surface.v1";
inline constexpr const char
    *kObjc3RuntimeUnifiedConcurrencyLoweringMetadataSurfaceContractId =
        "objc3c.runtime.unified.concurrency.lowering.metadata.surface.v1";
inline constexpr const char
    *kObjc3RuntimeUnifiedConcurrencyRuntimeAbiSurfaceContractId =
        "objc3c.runtime.unified.concurrency.runtime.abi.surface.v1";
inline constexpr const char
    *kObjc3RuntimeUnifiedConcurrencyRuntimeAbiBoundaryModel =
        "private-async-task-and-actor-helper-entrypoints-plus-testing-snapshots-define-the-live-runtime-abi-without-widening-the-public-runtime-header";
inline constexpr const char
    *kObjc3RuntimeUnifiedConcurrencyContinuationRuntimeModel =
        "continuation-allocation-handoff-resume-and-testing-snapshots-stay-on-bootstrap-internal-runtime-entrypoints";
inline constexpr const char
    *kObjc3RuntimeUnifiedConcurrencyTaskRuntimeModel =
        "task-spawn-group-cancellation-executor-hop-and-testing-snapshots-stay-on-bootstrap-internal-runtime-entrypoints";
inline constexpr const char
    *kObjc3RuntimeUnifiedConcurrencyActorRuntimeModel =
        "actor-isolation-nonisolated-hop-replay-race-guard-mailbox-and-testing-snapshots-stay-on-bootstrap-internal-runtime-entrypoints";
inline constexpr const char
    *kObjc3RuntimeUnifiedConcurrencyRuntimeAbiFailClosedModel =
        "public-runtime-header-remains-registration-lookup-dispatch-only-until-deliberate-concurrency-runtime-abi-widening";
inline constexpr const char *kObjc3RuntimeInstallationAbiSurfaceContractId =
    "objc3c.runtime.installation.abi.surface.v1";
inline constexpr const char *kObjc3RuntimeLoaderLifecycleSurfaceContractId =
    "objc3c.runtime.loader.lifecycle.surface.v1";
inline constexpr const char *kObjc3RuntimeReleaseCandidateClaimAbiSurfaceContractId =
    "objc3c.runtime.release.candidate.claim.abi.surface.v1";
inline constexpr const char *kObjc3RuntimeReleaseCandidateClaimSnapshotSymbol =
    "objc3_runtime_copy_release_candidate_claim_snapshot_for_testing";
inline constexpr const char *kObjc3RuntimeReleaseCandidateClaimSnapshotType =
    "objc3_runtime_release_candidate_claim_snapshot";
inline constexpr const char *kObjc3RuntimeReleaseCandidateClaimProbePath =
    "tests/tooling/runtime/release_candidate_claim_runtime_probe.cpp";
inline constexpr const char *kObjc3RuntimeReleaseCandidateClaimBoundaryModel =
    "private-release-candidate-claim-snapshot-freezes-the-final-claim-publication-contract-set-and-deprecated-path-shutdown-without-widening-the-public-runtime-header";
inline constexpr const char
    *kObjc3RuntimeFinalReleaseEvidenceDescaffoldingImplementationSurfaceContractId =
        "objc3c.runtime.final.release.evidence.descaffolding.implementation.surface.v1";
inline constexpr const char *kObjc3RuntimeReleaseCandidateEvidenceSnapshotSymbol =
    "objc3_runtime_copy_release_candidate_evidence_state_for_testing";
inline constexpr const char *kObjc3RuntimeReleaseCandidateEvidenceSnapshotType =
    "objc3_runtime_release_candidate_evidence_state_snapshot";
inline constexpr const char *kObjc3RuntimeReleaseCandidateEvidenceProbePath =
    "tests/tooling/runtime/release_candidate_evidence_runtime_probe.cpp";
inline constexpr const char *kObjc3RuntimeReleaseCandidateEvidenceImplementationModel =
    "private-release-candidate-evidence-snapshot-freezes-the-live-validation-release-evidence-dashboard-gate-matrix-and-deprecated-path-shutdown-implementation-boundary";
inline constexpr const char *kObjc3RuntimeBootstrapRegistrarContractId =
    "objc3c.runtime.bootstrap.registrar.image.walk.v1";
inline constexpr const char *kObjc3RuntimeBootstrapRegistrarSurfacePath =
    "frontend.pipeline.semantic_surface.objc_runtime_bootstrap_registrar_contract";
inline constexpr const char *kObjc3RuntimeBootstrapInternalHeaderPath =
    "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h";
inline constexpr const char
    *kObjc3RuntimeBootstrapStageRegistrationTableSymbol =
        "objc3_runtime_stage_registration_table_for_bootstrap";
inline constexpr const char
    *kObjc3RuntimeBootstrapImageWalkSnapshotSymbol =
        "objc3_runtime_copy_image_walk_state_for_testing";
inline constexpr const char *kObjc3RuntimeBootstrapImageWalkModel =
    "registration-table-roots-validated-and-staged-before-realization";
inline constexpr const char
    *kObjc3RuntimeBootstrapDiscoveryRootValidationModel =
        "linker-anchor-must-point-at-discovery-root-and-discovery-root-must-close-over-registration-roots";
inline constexpr const char
    *kObjc3RuntimeBootstrapSelectorPoolInterningModel =
        "canonical-selector-pool-preinterned-during-startup-image-walk";
inline constexpr const char *kObjc3RuntimeBootstrapRealizationStagingModel =
    "registration-table-roots-retained-for-later-realization";
inline constexpr const char *kObjc3RuntimeBootstrapResetContractId =
    "objc3c.runtime.bootstrap.reset.replay.v1";
inline constexpr const char *kObjc3RuntimeBootstrapResetSurfacePath =
    "frontend.pipeline.semantic_surface.objc_runtime_bootstrap_reset_contract";
inline constexpr const char
    *kObjc3RuntimeBootstrapReplayRegisteredImagesSymbol =
        "objc3_runtime_replay_registered_images_for_testing";
inline constexpr const char
    *kObjc3RuntimeBootstrapResetReplayStateSnapshotSymbol =
        "objc3_runtime_copy_reset_replay_state_for_testing";
inline constexpr const char *kObjc3RuntimeInstallationLifecycleProbePath =
    "tests/tooling/runtime/runtime_installation_loader_lifecycle_probe.cpp";
inline constexpr const char *kObjc3RuntimeBootstrapResetLifecycleModel =
    "reset-clears-live-runtime-state-and-zeroes-image-local-init-cells";
inline constexpr const char *kObjc3RuntimeBootstrapReplayOrderModel =
    "replay-re-registers-retained-images-in-original-registration-order";
inline constexpr const char
    *kObjc3RuntimeBootstrapImageLocalInitStateResetModel =
        "retained-bootstrap-image-local-init-cells-reset-to-zero-before-replay";
inline constexpr const char *kObjc3RuntimeBootstrapCatalogRetentionModel =
    "bootstrap-catalog-retained-across-reset-for-deterministic-replay";
inline constexpr const char *kObjc3RuntimeSupportLibraryTargetName =
    "objc3_runtime";
inline constexpr const char *kObjc3RuntimeSupportLibrarySourceRoot =
    "native/objc3c/src/runtime";
inline constexpr const char *kObjc3RuntimeSupportLibraryPublicHeaderPath =
    "native/objc3c/src/runtime/public/objc3_runtime_api.h";
inline constexpr const char *kObjc3RuntimeSupportLibraryKind = "static";
inline constexpr const char *kObjc3RuntimeSupportLibraryArchiveBasename =
    "objc3_runtime";
inline constexpr const char *kObjc3RuntimeSupportLibraryArchiveRelativePath =
    "artifacts/lib/objc3_runtime.lib";
inline constexpr const char *kObjc3RuntimeSupportLibraryImplementationSourcePath =
    "native/objc3c/src/runtime/objc3_runtime.cpp";
inline constexpr const char *kObjc3RuntimeSupportLibraryProbeSourcePath =
    "tests/tooling/runtime/runtime_library_probe.cpp";
inline constexpr const char *kObjc3RuntimeSupportLibraryRegisterImageSymbol =
    "objc3_runtime_register_image";
inline constexpr const char *kObjc3RuntimeSupportLibraryLookupSelectorSymbol =
    "objc3_runtime_lookup_selector";
inline constexpr const char *kObjc3RuntimeSupportLibraryDispatchI32Symbol =
    "objc3_runtime_dispatch_i32";
inline constexpr const char *kObjc3RuntimeSupportLibraryResetForTestingSymbol =
    "objc3_runtime_reset_for_testing";
inline constexpr const char *kObjc3RuntimeSupportLibraryDriverLinkMode =
    "not-linked-until-next-runtime-phase";
inline constexpr const char *kObjc3RuntimeSupportLibraryLinkWiringContractId =
    "objc3c.runtime.support.library.link.wiring.v1";
inline constexpr const char
    *kObjc3RuntimeSupportLibraryExecutionSmokeScriptPath =
        "scripts/check_objc3c_native_execution_smoke.ps1";
inline constexpr const char *kObjc3RuntimeSupportLibraryLinkWiringMode =
    "emitted-object-links-against-objc3_runtime-lib";
inline constexpr const char
    *kObjc3RuntimeSupportLibraryCompilerOwnershipBoundary =
        "compiler-emits-metadata-runtime-does-not-own-source-records";
inline constexpr const char
    *kObjc3RuntimeSupportLibraryRuntimeOwnershipBoundary =
        "runtime-owns-registration-lookup-and-dispatch-state";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorImageRootSourceSurfaceContractId =
        "objc3c.bootstrap.registration.descriptor.image.root.source.surface.v1";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorImageRootSourceSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_runtime_registration_descriptor_image_root_source_surface";
inline constexpr const char
    *kObjc3RuntimeBootstrapModuleIdentitySourceModel =
        "module-declaration-or-default";
inline constexpr const char
    *kObjc3RuntimeBootstrapDerivedIdentitySourcePragma =
        "source-pragma";
inline constexpr const char
    *kObjc3RuntimeBootstrapDerivedIdentitySourceModuleDefault =
        "module-derived-default";
inline constexpr const char
    *kObjc3RuntimeBootstrapVisibleMetadataOwnershipModel =
        "image-root-owns-registration-descriptor-runtime-owns-bootstrap-state";
inline constexpr const char
    *kObjc3RuntimeBootstrapRegistrationDescriptorDefaultSuffix =
        "_registration_descriptor";
inline constexpr const char
    *kObjc3RuntimeBootstrapImageRootDefaultSuffix = "_image_root";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorFrontendClosureContractId =
        "objc3c.runtime.registration.descriptor.frontend.closure.v1";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorFrontendClosureSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_runtime_registration_descriptor_frontend_closure";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorFrontendClosurePayloadModel =
        "runtime-registration-descriptor-json-v1";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorFrontendClosureArtifactSuffix =
        ".runtime-registration-descriptor.json";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorFrontendClosureArtifactRelativePath =
        "module.runtime-registration-descriptor.json";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorFrontendAuthorityModel =
        "registration-descriptor-artifact-derived-from-source-surface-and-registration-manifest";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorPayloadOwnershipModel =
        "compiler-emits-registration-descriptor-artifact-runtime-consumes-bootstrap-identity";
inline constexpr const char
    *kObjc3RuntimeBlockArcUnifiedSourceSurfaceContractId =
        "objc3c.runtime.block.arc.unified.source.surface.v1";
inline constexpr const char
    *kObjc3RuntimeBlockArcUnifiedSourceSurfaceModel =
        "block-arc-unified-source-surface-freezes-live-frontend-sema-ir-and-runtime-entrypoints-before-generalized-ownership-automation-or-public-abi-widening";
inline constexpr const char
    *kObjc3RuntimeOwnershipTransferCaptureFamilySourceSurfaceContractId =
        "objc3c.runtime.ownership.transfer.capture.family.source.surface.v1";
inline constexpr const char
    *kObjc3RuntimeOwnershipTransferCaptureFamilySourceSurfaceModel =
        "ownership-transfer-and-capture-family-source-surface-freezes-sema-level-move-capture-explicit-capture-mode-and-retainable-family-truth-before-lowering-or-runtime-lifetime-expansion";
inline constexpr const char
    *kObjc3RuntimeBlockArcLoweringHelperSurfaceContractId =
        "objc3c.runtime.block.arc.lowering.helper.surface.v1";
inline constexpr const char
    *kObjc3RuntimeBlockArcLoweringHelperSurfaceModel =
        "block-arc-lowering-helper-surface-freezes-live-semantic-lowering-packets-manifest-replay-keys-llvm-helper-summaries-and-private-runtime-hooks-before-cross-module-or-public-abi-expansion";
inline constexpr const char
    *kObjc3RuntimeBlockArcRuntimeAbiSurfaceContractId =
        "objc3c.runtime.block.arc.runtime.abi.surface.v1";
inline constexpr const char
    *kObjc3RuntimeBlockArcRuntimeAbiBoundaryModel =
        "private-block-and-arc-helper-entrypoints-plus-testing-snapshots-define-the-live-runtime-abi-without-widening-the-public-runtime-header";
inline constexpr const char
    *kObjc3RuntimeBlockArcRuntimeAbiBlockModel =
        "promote-invoke-and-handle-lifetime-for-supported-block-records-stay-on-bootstrap-internal-runtime-entrypoints";
inline constexpr const char
    *kObjc3RuntimeBlockArcRuntimeAbiArcModel =
        "retain-release-autorelease-autoreleasepool-and-current-property-weak-helper-traffic-stays-on-bootstrap-internal-runtime-entrypoints";
inline constexpr const char
    *kObjc3RuntimeBlockArcRuntimeAbiFailClosedModel =
        "public-runtime-header-remains-registration-lookup-dispatch-only-until-deliberate-runtime-abi-widening";
