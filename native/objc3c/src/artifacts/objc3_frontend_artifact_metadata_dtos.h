#pragma once

#include "artifacts/identity/artifact_identity.h"

struct Objc3IRFrontendMetadata;
struct Objc3Program;
struct Objc3FrontendOptions;
struct Objc3FrontendPipelineResult;
struct Objc3SemaParityContractSurface;
struct Objc3ImportedRuntimeModuleSurface;

struct Objc3RuntimeMetadataObjectInspectionHarnessSummary;

struct Objc3OwnershipAwareLoweringBehaviorScaffold;
struct Objc3IREmissionCompletenessScaffold;
struct Objc3LoweringPipelinePassGraphCoreFeatureSurface;
struct Objc3IREmissionCoreFeatureImplementationSurface;

struct Objc3RuntimeBootstrapLoweringSummary;
struct Objc3RuntimeRegistrationDescriptorFrontendClosureSummary;
struct Objc3RuntimeTranslationUnitRegistrationManifestSummary;

struct Objc3RuntimeMetadataSourceOwnershipBoundary;
struct Objc3RuntimeExportLegalityBoundary;
struct Objc3RuntimeExportEnforcementSummary;
struct Objc3RuntimeMetadataSectionAbiFreezeSummary;
struct Objc3RuntimeMetadataSectionPublicationSummary;
struct Objc3ExecutableMetadataTypedLoweringHandoff;

struct Objc3RuntimeSupportLibraryContractSummary;
struct Objc3RuntimeSupportLibraryCoreFeatureSummary;
struct Objc3RuntimeSupportLibraryLinkWiringSummary;

struct Objc3LightweightGenericsConstraintLoweringContract;
struct Objc3NullabilityFlowWarningPrecisionLoweringContract;
struct Objc3ProtocolQualifiedObjectTypeLoweringContract;
struct Objc3VarianceBridgeCastLoweringContract;
struct Objc3GenericMetadataAbiLoweringContract;

struct Objc3ControlFlowControlFlowSafetyLoweringContract;
struct Objc3ControlFlowControlFlowSemanticModelSummary;

struct Objc3OwnershipQualifierLoweringContract;
struct Objc3RetainReleaseOperationLoweringContract;
struct Objc3AutoreleasePoolScopeLoweringContract;
struct Objc3WeakUnownedSemanticsLoweringContract;
struct Objc3ArcDiagnosticsFixitLoweringContract;
struct Objc3OwnershipSystemExtensionLoweringContract;
struct Objc3FrontendOwnershipSystemExtensionSourceClosureSummary;
struct Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary;
struct Objc3EffectsOwnershipSemanticModelSummary;
struct Objc3OwnershipSystemExtensionSemanticModelSummary;
struct Objc3OwnershipResourceMoveUseAfterMoveSemanticsSummary;
struct Objc3OwnershipBorrowedPointerEscapeAnalysisSummary;
struct Objc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary;

struct Objc3BlockLiteralCaptureLoweringContract;
struct Objc3BlockSourceModelCompletionContract;
struct Objc3BlockSourceStorageAnnotationContract;
struct Objc3BlockAbiInvokeTrampolineLoweringContract;
struct Objc3BlockStorageEscapeLoweringContract;
struct Objc3BlockCopyDisposeLoweringContract;
struct Objc3BlockDeterminismPerfBaselineLoweringContract;

struct Objc3AsyncContinuationLoweringContract;
struct Objc3AwaitLoweringSuspensionStateLoweringContract;
struct Objc3ActorIsolationSendabilityLoweringContract;
struct Objc3ActorLoweringMetadataContract;
struct Objc3TaskRuntimeInteropCancellationLoweringContract;
struct Objc3ConcurrencyReplayRaceGuardLoweringContract;
struct Objc3ConcurrencyActorIsolationSendableSemanticModelSummary;
struct Objc3ConcurrencyActorIsolationSendabilityEnforcementSummary;
struct Objc3ConcurrencyActorRaceHazardEscapeDiagnosticsSummary;
struct Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary;
struct Objc3ConcurrencyAsyncEffectSuspensionSemanticModelSummary;
struct Objc3ConcurrencyAsyncDiagnosticsCompatibilitySummary;
struct Objc3ConcurrencyAwaitSuspensionResumeSemanticSummary;
struct Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary;
struct Objc3ConcurrencyStructuredTaskCancellationSemanticSummary;
struct Objc3ConcurrencyExecutorHopAffinityCompatibilitySummary;
struct Objc3FrontendConcurrencyAsyncSourceClosureSummary;

struct Objc3ExecutableMetadataDebugProjectionSummary;

struct Objc3ThrowsPropagationLoweringContract;
struct Objc3ResultLikeLoweringContract;
struct Objc3NSErrorBridgingLoweringContract;
struct Objc3UnwindCleanupLoweringContract;
struct Objc3ErrorHandlingErrorSemanticModelSummary;
struct Objc3ErrorHandlingTryDoCatchSemanticSummary;
struct Objc3ErrorHandlingErrorBridgeLegalitySummary;

struct Objc3InteropInteropLoweringContract;
struct Objc3InteropForeignCallLifetimeLoweringContract;
struct Objc3InteropFfiMetadataInterfacePreservationContract;
struct Objc3InteropHeaderModuleBridgeGenerationSummary;
struct Objc3InteropInteropSemanticModelSummary;
struct Objc3InteropInteropRuntimeParitySummary;
struct Objc3InteropCppInteropInteractionSummary;
struct Objc3InteropSwiftInteropIsolationSummary;
struct Objc3InteropForeignSurfaceInterfacePreservationSummary;
struct Objc3FrontendInteropForeignImportSourceClosureSummary;
struct Objc3FrontendInteropCppSwiftInteropAnnotationSourceCompletionSummary;

struct Objc3MetaprogrammingExpansionLoweringContract;
struct Objc3MetaprogrammingSynthesizedArtifactEmissionContract;
struct Objc3IRMetaprogrammingDerivedMethodBundle;
struct Objc3IRMetaprogrammingMacroArtifactBundle;
struct Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle;
struct Objc3MetaprogrammingModuleInterfaceReplayPreservationSurfaceSummary;
struct Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary;
struct Objc3MetaprogrammingExpansionBehaviorSemanticModelSummary;
struct Objc3MetaprogrammingDeriveExpansionInventorySummary;
struct Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary;
struct Objc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary;

struct Objc3ModuleImportGraphLoweringContract;
struct Objc3NamespaceCollisionShadowingLoweringContract;
struct Objc3PublicPrivateApiPartitionLoweringContract;
struct Objc3IncrementalModuleCacheInvalidationLoweringContract;
struct Objc3CrossModuleConformanceLoweringContract;
struct Objc3CrossModuleSemanticContractsDiagnosticsSummary;

namespace objc3::artifacts::frontend {

inline constexpr const char
    *kObjc3ArtifactMetaprogrammingModuleInterfaceReplayPreservationContractId =
        "objc3c.metaprogramming.module.interface.replay.preservation.v1";
inline constexpr const char
    *kObjc3ArtifactMetaprogrammingModuleInterfaceReplayPreservationSourceContractId =
        "objc3c.metaprogramming.synthesized.ast.ir.emission.v1";
inline constexpr const char
    *kObjc3ArtifactMetaprogrammingModuleInterfaceReplayPreservationSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_metaprogramming_module_interface_and_replay_preservation";
inline constexpr const char
    *kObjc3ArtifactMetaprogrammingModuleInterfaceReplayPreservationImportArtifactMemberName =
        "objc_metaprogramming_module_interface_and_replay_preservation";
inline constexpr const char
    *kObjc3ArtifactMetaprogrammingModuleInterfaceReplayPreservationSourceModel =
        "runtime-import-surface-artifacts-preserve-metaprogramming-derived-method-macro-and-property-behavior-replay-facts-for-separate-compilation-and-interface-inspection";
inline constexpr const char
    *kObjc3ArtifactMetaprogrammingModuleInterfaceReplayPreservationModel =
        "provider-and-consumer-import-surfaces-preserve-metaprogramming-synthesized-emission-counts-replay-keys-and-interface-vs-implementation-property-behavior-splits-beyond-local-ir-object-emission";
inline constexpr const char
    *kObjc3ArtifactMetaprogrammingModuleInterfaceReplayPreservationFailClosedModel =
        "missing-or-drifted-metaprogramming-preservation-packets-disable-cross-module-metaprogramming-preservation-claims";

inline constexpr const char
    *kObjc3ArtifactMetaprogrammingMacroHostProcessCacheRuntimeIntegrationContractId =
        "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1";
inline constexpr const char
    *kObjc3ArtifactMetaprogrammingMacroHostProcessCacheRuntimeIntegrationSourceContractId =
        "objc3c.metaprogramming.expansion.host.runtime.boundary.v1";
inline constexpr const char
    *kObjc3ArtifactMetaprogrammingMacroHostProcessCacheRuntimeIntegrationSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_metaprogramming_macro_host_process_and_cache_runtime_integration";
inline constexpr const char
    *kObjc3ArtifactMetaprogrammingMacroHostProcessCacheRuntimeIntegrationImportArtifactMemberName =
        "objc_metaprogramming_macro_host_process_and_cache_runtime_integration";
inline constexpr const char
    *kObjc3ArtifactMetaprogrammingMacroHostProcessCacheRuntimeIntegrationHostExecutableRelativePath =
        objc3::artifacts::identity::kObjc3NativeFrontendRunnerRelativePath;
inline constexpr const char
    *kObjc3ArtifactMetaprogrammingMacroHostProcessCacheRuntimeIntegrationCacheRootRelativePath =
        "tmp/artifacts/objc3c-native/cache/metaprogramming";
inline constexpr const char
    *kObjc3ArtifactMetaprogrammingMacroHostProcessCacheRuntimeIntegrationHostModel =
        "native-driver-launches-objc3c-frontend-c-api-runner-for-supported-metaprogramming-expansion-cache-materialization";
inline constexpr const char
    *kObjc3ArtifactMetaprogrammingMacroHostProcessCacheRuntimeIntegrationToolchainModel =
        "frontend-runner-executes-with-manifest-enabled-and-ir-object-emission-disabled-for-deterministic-cache-materialization";
inline constexpr const char
    *kObjc3ArtifactMetaprogrammingMacroHostProcessCacheRuntimeIntegrationCacheModel =
        "cache-entry-path-is-derived-from-a-stable-fnv1a64-key-over-the-metaprogramming-replay-surface-explicit-macro-cache-keys-and-policy-version-and-reused-on-subsequent-runs";
inline constexpr const char
    *kObjc3ArtifactMetaprogrammingMacroHostProcessCacheRuntimeIntegrationInvalidationModel =
        "metaprogramming-replay-key-explicit-macro-cache-key-or-sandbox-policy-drift-invalidates-the-entry-while-corrupt-or-incomplete-cache-artifacts-fail-closed";
inline constexpr const char
    *kObjc3ArtifactMetaprogrammingMacroHostProcessCacheRuntimeIntegrationSandboxPolicyModel =
        "macro-host-materialization-is-deny-by-default-and-only-admits-pure-free-functions-with-objc_macro_sandbox-named-deterministic";
inline constexpr const char
    *kObjc3ArtifactMetaprogrammingMacroHostProcessCacheRuntimeIntegrationDiagnosticsModel =
        "stable-O3S331-and-O3S332-diagnostics-gate-missing-or-invalid-macro-cache-key-and-sandbox-policy-metadata";
inline constexpr const char
    *kObjc3ArtifactMetaprogrammingMacroHostProcessCacheRuntimeIntegrationFailClosedModel =
        "missing-runner-corrupt-cache-or-import-surface-drift-disables-metaprogramming-host-process-cache-claims";

}  // namespace objc3::artifacts::frontend

namespace objc3::artifacts::evidence {

struct ErrorHandlingResultAndBridgingArtifactReplayEvidence;

}  // namespace objc3::artifacts::evidence
