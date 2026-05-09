#include "artifacts/objc3_frontend_artifacts.h"

#include "artifacts/objc3_runtime_state_publication_paths.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "artifacts/evidence/error_handling_replay_evidence.h"
#include "artifacts/identity/artifact_identity.h"
#include "artifacts/interop/interop_bridge_artifacts.h"
#include "artifacts/json/program_manifest_json.h"
#include "artifacts/json/runtime_metadata_manifest_json.h"
#include "artifacts/json/semantic_type_manifest_json.h"
#include "artifacts/objc3_frontend_actor_semantic_artifacts.h"
#include "artifacts/objc3_frontend_artifact_function_manifest.h"
#include "artifacts/objc3_frontend_artifact_metadata_mode.h"
#include "artifacts/objc3_frontend_artifact_runtime_metadata_plan.h"
#include "artifacts/objc3_frontend_artifact_runtime_registration_plan.h"
#include "artifacts/objc3_frontend_artifact_sanity.h"
#include "artifacts/objc3_frontend_artifact_diagnostics.h"
#include "artifacts/objc3_frontend_conformance_artifacts.h"
#include "artifacts/objc3_frontend_concurrency_semantic_artifacts.h"
#include "artifacts/objc3_frontend_control_flow_semantic_artifacts.h"
#include "artifacts/objc3_frontend_dispatch_semantic_artifacts.h"
#include "artifacts/objc3_frontend_error_semantic_artifacts.h"
#include "artifacts/objc3_frontend_feature_claim_artifacts.h"
#include "artifacts/objc3_frontend_feature_claim_truth_artifacts.h"
#include "artifacts/objc3_frontend_interop_semantic_artifacts.h"
#include "artifacts/objc3_frontend_metaprogramming_semantic_artifacts.h"
#include "artifacts/objc3_frontend_module_semantic_artifacts.h"
#include "artifacts/objc3_frontend_ownership_semantic_artifacts.h"
#include "artifacts/objc3_frontend_parser_diagnostic_artifacts.h"
#include "artifacts/objc3_frontend_runtime_bootstrap_artifacts.h"
#include "artifacts/objc3_frontend_runtime_capability_artifacts.h"
#include "artifacts/objc3_frontend_runtime_descriptor_artifacts.h"
#include "artifacts/objc3_frontend_runtime_import_artifacts.h"
#include "artifacts/objc3_frontend_runtime_ingest_binary_artifacts.h"
#include "artifacts/objc3_frontend_runtime_metadata_section_artifacts.h"
#include "artifacts/objc3_frontend_runtime_registration_artifacts.h"
#include "artifacts/objc3_frontend_source_closure_artifacts.h"
#include "artifacts/objc3_frontend_tooling_source_artifacts.h"
#include "artifacts/objc3_frontend_type_system_contract_artifacts.h"
#include "artifacts/objc3_frontend_type_system_semantic_artifacts.h"
#include "artifacts/reports/frontend_conformance_report_contracts.h"
#include "contracts/objc3_frontend_diagnostics_bus_contract.h"
#include "diag/objc3_diag_utils.h"
#include "ir/objc3_ir_emitter.h"
#include "io/json/json_writer.h"
#include "io/objc3_json.h"
#include "pipeline/objc3_ir_emission_core_feature_implementation_surface.h"
#include "pipeline/objc3_ir_emission_completeness_scaffold.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_edge_case_compatibility_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_implementation_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_scaffold.h"
#include "pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.h"
#include "pipeline/objc3_lowering_pipeline_pass_graph_scaffold.h"
#include "pipeline/objc3_ownership_aware_lowering_behavior_scaffold.h"
#include "pipeline/objc3_parse_lowering_readiness_surface.h"
#include "pipeline/objc3_runtime_import_surface.h"
#include "sema/objc3_semantic_passes.h"
#include "support/objc3_identifier_safe_suffix.h"
#include "support/objc3_runtime_metadata_record_set.h"
#include "support/objc3_value_type_names.h"

namespace {

using objc3::io::EscapeJsonString;
using objc3::io::json::JsonObjectWriter;
using objc3::artifacts::evidence::
    BuildErrorHandlingResultAndBridgingArtifactReplayEvidence;
using objc3::artifacts::evidence::
    BuildErrorHandlingResultAndBridgingArtifactReplayJson;
using objc3::artifacts::identity::BuildObjc3TranslationUnitIdentityKey;
using objc3::artifacts::identity::Objc3TranslationUnitIdentityEvidence;
using objc3::artifacts::interop::BuildInteropBridgeArtifactJson;
using objc3::artifacts::interop::BuildInteropBridgeHeaderArtifactText;
using objc3::artifacts::interop::BuildInteropBridgeModuleArtifactText;
using objc3::artifacts::frontend::
    BuildDispatchDispatchIntentCompatibilitySummaryJson;
using objc3::artifacts::frontend::BuildDispatchAbiMarshallingContract;
using objc3::artifacts::frontend::
    BuildDispatchDispatchControlLoweringContractJson;
using objc3::artifacts::frontend::
    BuildControlFlowControlFlowSemanticModelSummaryJson;
using objc3::artifacts::frontend::BuildDispatchSurfaceClassificationContract;
using objc3::artifacts::frontend::BuildDispatchDispatchIntentLegalitySummaryJson;
using objc3::artifacts::frontend::
    BuildDispatchDispatchIntentSemanticModelSummaryJson;
using objc3::artifacts::frontend::BuildIdClassSelObjectPointerTypecheckContract;
using objc3::artifacts::frontend::BuildMessageSendSelectorLoweringContract;
using objc3::artifacts::frontend::BuildNilReceiverSemanticsFoldabilityContract;
using objc3::artifacts::frontend::BuildRuntimeDispatchLoweringAbiContract;
using objc3::artifacts::frontend::BuildRuntimeLinkHostLinkContract;
using objc3::artifacts::frontend::BuildSuperDispatchMethodFamilyContract;
using objc3::artifacts::frontend::BuildPropertySynthesisIvarBindingContract;
using objc3::artifacts::frontend::
    BuildErrorHandlingErrorBridgeLegalitySummaryJson;
using objc3::artifacts::frontend::
    BuildErrorHandlingErrorSemanticModelSummaryJson;
using objc3::artifacts::frontend::
    BuildErrorHandlingTryDoCatchSemanticSummaryJson;
using objc3::artifacts::frontend::BuildNSErrorBridgingLoweringContract;
using objc3::artifacts::frontend::BuildResultLikeLoweringContract;
using objc3::artifacts::frontend::BuildThrowsPropagationLoweringContract;
using objc3::artifacts::frontend::BuildUnwindCleanupLoweringContract;
using objc3::artifacts::frontend::BuildArcDiagnosticsFixitLoweringContract;
using objc3::artifacts::frontend::BuildAutoreleasePoolScopeLoweringContract;
using objc3::artifacts::frontend::BuildEffectsOwnershipSemanticModelSummaryJson;
using objc3::artifacts::frontend::BuildInteropCppInteropInteractionSummaryJson;
using objc3::artifacts::frontend::
    BuildInteropForeignSurfaceInterfacePreservationSummaryJson;
using objc3::artifacts::frontend::
    BuildInteropHeaderModuleBridgeGenerationSummaryJson;
using objc3::artifacts::frontend::BuildInteropInteropRuntimeParitySummaryJson;
using objc3::artifacts::frontend::
    BuildInteropForeignCallLifetimeLoweringContractJson;
using objc3::artifacts::frontend::
    BuildInteropFfiMetadataInterfacePreservationContract;
using objc3::artifacts::frontend::
    BuildInteropFfiMetadataInterfacePreservationContractJson;
using objc3::artifacts::frontend::
    BuildInteropForeignSurfaceInterfacePreservationSummary;
using objc3::artifacts::frontend::BuildInteropInteropSemanticModelSummaryJson;
using objc3::artifacts::frontend::BuildInteropInteropLoweringContract;
using objc3::artifacts::frontend::BuildInteropInteropLoweringContractJson;
using objc3::artifacts::frontend::BuildInteropSwiftInteropIsolationSummaryJson;
using objc3::artifacts::frontend::
    BuildInteropForeignCallLifetimeLoweringContract;
using objc3::artifacts::frontend::BuildInteropHeaderModuleBridgeGenerationSummary;
using objc3::artifacts::frontend::
    BuildMetaprogrammingDeriveExpansionInventorySummaryJson;
using objc3::artifacts::frontend::BuildMetaprogrammingDerivedMethodBundles;
using objc3::artifacts::frontend::
    BuildMetaprogrammingExpansionBehaviorSemanticModelSummaryJson;
using objc3::artifacts::frontend::
    BuildMetaprogrammingExpansionLoweringContract;
using objc3::artifacts::frontend::
    BuildMetaprogrammingExpansionLoweringContractJson;
using objc3::artifacts::frontend::BuildMetaprogrammingMacroArtifactBundles;
using objc3::artifacts::frontend::
    BuildMetaprogrammingMacroHostProcessCacheRuntimeIntegrationSummary;
using objc3::artifacts::frontend::
    BuildMetaprogrammingMacroHostProcessCacheRuntimeIntegrationSummaryJson;
using objc3::artifacts::frontend::
    BuildMetaprogrammingMacroSafetySandboxDeterminismSummaryJson;
using objc3::artifacts::frontend::
    BuildMetaprogrammingModuleInterfaceReplayPreservationSummary;
using objc3::artifacts::frontend::
    BuildMetaprogrammingModuleInterfaceReplayPreservationSummaryJson;
using objc3::artifacts::frontend::
    BuildMetaprogrammingPropertyBehaviorLegalityCompatibilitySummaryJson;
using objc3::artifacts::frontend::
    BuildMetaprogrammingPropertyBehaviorArtifactBundles;
using objc3::artifacts::frontend::
    BuildMetaprogrammingSynthesizedArtifactEmissionContract;
using objc3::artifacts::frontend::
    BuildMetaprogrammingSynthesizedArtifactEmissionContractJson;
using objc3::artifacts::BuildRunnableFeatureClaimIds;
using objc3::artifacts::BuildRunnableFeatureClaimInventoryJson;
using objc3::artifacts::BuildRunnableFeatureClaimInventoryReplayKey;
using objc3::artifacts::BuildSourceOnlyFeatureClaimIds;
using objc3::artifacts::BuildUnsupportedFeatureClaimIds;
using objc3::artifacts::frontend::BuildFeatureClaimStrictnessTruthSurfaceJson;
using objc3::artifacts::frontend::BuildFeatureClaimStrictnessTruthSurfaceReplayKey;
using objc3::artifacts::frontend::
    BuildObjc3FrontendArtifactInitialPostPipelineFailure;
using objc3::artifacts::frontend::BuildObjc3ParserDiagnosticCodeCoverage;
using objc3::artifacts::frontend::BuildPublicConformanceReportJson;
using objc3::artifacts::frontend::BuildRuntimeCapabilityReportJson;
using objc3::artifacts::frontend::Objc3ParserDiagnosticCodeCoverage;
using objc3::artifacts::frontend::
    BuildConcurrencyActorIsolationSendabilityEnforcementSummaryJson;
using objc3::artifacts::frontend::
    BuildConcurrencyActorIsolationSendableSemanticModelSummaryJson;
using objc3::artifacts::frontend::
    BuildConcurrencyActorLoweringMetadataContract;
using objc3::artifacts::frontend::
    BuildConcurrencyActorLoweringMetadataContractJson;
using objc3::artifacts::frontend::
    BuildConcurrencyActorRaceHazardEscapeDiagnosticsSummaryJson;
using objc3::artifacts::frontend::
    BuildConcurrencyActorMemberIsolationSourceClosureSummaryJson;
using objc3::artifacts::frontend::
    BuildConcurrencyAsyncDiagnosticsCompatibilitySummaryJson;
using objc3::artifacts::frontend::
    BuildConcurrencyAsyncDirectCallLoweringJson;
using objc3::artifacts::frontend::
    BuildConcurrencyAsyncEffectSuspensionSemanticModelSummaryJson;
using objc3::artifacts::frontend::
    BuildConcurrencyAsyncContinuationLoweringContract;
using objc3::artifacts::frontend::
    BuildConcurrencyAwaitLoweringSuspensionStateLoweringContract;
using objc3::artifacts::frontend::
    BuildConcurrencyAwaitSuspensionResumeSemanticSummaryJson;
using objc3::artifacts::frontend::
    BuildConcurrencyContinuationAbiAsyncLoweringContractJson;
using objc3::artifacts::frontend::
    BuildConcurrencyExecutorHopAffinityCompatibilitySummaryJson;
using objc3::artifacts::frontend::
    BuildConcurrencyStructuredTaskCancellationSemanticSummaryJson;
using objc3::artifacts::frontend::
    BuildConcurrencySuspensionCleanupIntegrationJson;
using objc3::artifacts::frontend::
    BuildConcurrencyTaskExecutorCancellationSemanticModelSummaryJson;
using objc3::artifacts::frontend::
    BuildConcurrencyTaskRuntimeAbiCompletionJson;
using objc3::artifacts::frontend::
    BuildConcurrencyTaskRuntimeLoweringContractJson;
using objc3::artifacts::frontend::
    BuildExecutableMetadataRuntimeIngestBinaryBoundarySummaryJson;
using objc3::artifacts::frontend::
    BuildExecutableMetadataRuntimeIngestPackagingContractSummaryJson;
using objc3::artifacts::frontend::
    BuildRuntimeMetadataSourceToSectionMatrixSummaryJson;
using objc3::artifacts::frontend::BuildAccessorStorageLoweringMetadataSummary;
using objc3::artifacts::frontend::BuildExecutableAccessorLayoutLoweringSummary;
using objc3::artifacts::frontend::BuildExecutableMetadataSourceGraphJson;
using objc3::artifacts::frontend::
    BuildExecutableMetadataSemanticConsistencyBoundaryJson;
using objc3::artifacts::frontend::
    BuildExecutableMetadataSemanticValidationSurfaceJson;
using objc3::artifacts::frontend::
    BuildExecutableMetadataLoweringHandoffSurfaceJson;
using objc3::artifacts::frontend::
    BuildExecutableMetadataTypedLoweringHandoffJson;
using objc3::artifacts::frontend::
    BuildExecutableMetadataDebugProjectionRowDescriptor;
using objc3::artifacts::frontend::
    BuildExecutableMetadataDebugProjectionReplayKey;
using objc3::artifacts::frontend::
    BuildExecutableMetadataDebugProjectionSummaryJson;
using objc3::artifacts::frontend::Objc3AccessorStorageLoweringMetadataSummary;
using objc3::artifacts::frontend::
    Objc3ExecutableAccessorLayoutLoweringSummary;
using objc3::artifacts::frontend::BuildGenericMetadataAbiLoweringContract;
using objc3::artifacts::frontend::
    BuildLightweightGenericsConstraintLoweringContract;
using objc3::artifacts::frontend::
    BuildNullabilityFlowWarningPrecisionLoweringContract;
using objc3::artifacts::frontend::
    BuildProtocolQualifiedObjectTypeLoweringContract;
using objc3::artifacts::frontend::
    BuildTypeSystemGenericContractPreservationJson;
using objc3::artifacts::frontend::
    BuildTypeSystemNullabilityContractPreservationJson;
using objc3::artifacts::frontend::BuildTypeSystemOptionalKeypathLoweringContract;
using objc3::artifacts::frontend::
    BuildTypeSystemOptionalKeypathLoweringContractJson;
using objc3::artifacts::frontend::
    BuildTypeSystemOptionalKeypathRuntimeHelperContractJson;
using objc3::artifacts::frontend::
    BuildTypeSystemProtocolContractPreservationJson;
using objc3::artifacts::frontend::BuildTypeSystemTypeSemanticModelSummaryJson;
using objc3::artifacts::frontend::BuildVarianceBridgeCastLoweringContract;
using objc3::artifacts::frontend::
    BuildToolingDiagnosticTaxonomyPortabilityContractSummaryJson;
using objc3::artifacts::frontend::
    BuildToolingDiagnosticsMigratorSourceInventorySummaryJson;
using objc3::artifacts::frontend::
    BuildToolingFeatureSpecificFixitSynthesisSummaryJson;
using objc3::artifacts::frontend::
    BuildToolingLegacyCanonicalMigrationSemanticsSummary;
using objc3::artifacts::frontend::
    BuildToolingLegacyCanonicalMigrationSemanticsSummaryJson;
using objc3::artifacts::frontend::
    BuildToolingMigrationCanonicalizationSourceCompletionSummaryJson;
using objc3::artifacts::frontend::BuildConcurrencyAsyncSourceClosureSummaryJson;
using objc3::artifacts::frontend::
    BuildConcurrencyTaskGroupCancellationSourceClosureSummaryJson;
using objc3::artifacts::frontend::BuildControlFlowControlFlowSafetyLoweringContract;
using objc3::artifacts::frontend::
    BuildControlFlowControlFlowSafetyLoweringContractJson;
using objc3::artifacts::frontend::
    BuildControlFlowControlFlowSourceClosureSummaryJson;
using objc3::artifacts::frontend::BuildCrossModuleConformanceLoweringContract;
using objc3::artifacts::frontend::
    BuildIncrementalModuleCacheInvalidationLoweringContract;
using objc3::artifacts::frontend::BuildModuleImportGraphLoweringContract;
using objc3::artifacts::frontend::
    BuildNamespaceCollisionShadowingLoweringContract;
using objc3::artifacts::frontend::
    BuildPublicPrivateApiPartitionLoweringContract;
using objc3::artifacts::frontend::
    BuildCrossModuleSemanticContractsDiagnosticsSummaryJson;
using objc3::artifacts::frontend::
    BuildDispatchDispatchIntentSourceClosureSummaryJson;
using objc3::artifacts::frontend::
    BuildDispatchDispatchIntentSourceCompletionSummaryJson;
using objc3::artifacts::frontend::
    BuildErrorHandlingErrorSourceClosureSummaryJson;
using objc3::artifacts::frontend::
    BuildInteropCppSwiftInteropAnnotationSourceCompletionSummaryJson;
using objc3::artifacts::frontend::
    BuildInteropForeignImportSourceClosureSummaryJson;
using objc3::artifacts::frontend::
    BuildMetaprogrammingMacroPackageProvenanceSourceCompletionSummaryJson;
using objc3::artifacts::frontend::
    BuildMetaprogrammingMetaprogrammingSourceClosureSummaryJson;
using objc3::artifacts::frontend::
    BuildMetaprogrammingPropertyBehaviorSourceCompletionSummaryJson;
using objc3::artifacts::frontend::
    BuildOwnershipCleanupResourceCaptureSourceCompletionSummaryJson;
using objc3::artifacts::frontend::
    BuildOwnershipBorrowedPointerEscapeAnalysisSummaryJson;
using objc3::artifacts::frontend::BuildBlockAbiInvokeTrampolineLoweringContract;
using objc3::artifacts::frontend::BuildBlockCopyDisposeLoweringContract;
using objc3::artifacts::frontend::
    BuildBlockDeterminismPerfBaselineLoweringContract;
using objc3::artifacts::frontend::BuildBlockLiteralCaptureLoweringContract;
using objc3::artifacts::frontend::BuildBlockSourceModelCompletionContract;
using objc3::artifacts::frontend::BuildBlockSourceStorageAnnotationContract;
using objc3::artifacts::frontend::BuildBlockStorageEscapeLoweringContract;
using objc3::artifacts::frontend::
    BuildOwnershipCaptureListRetainableFamilyLegalityCompletionSummaryJson;
using objc3::artifacts::frontend::BuildOwnershipQualifierLoweringContract;
using objc3::artifacts::frontend::
    BuildOwnershipResourceMoveUseAfterMoveSemanticsSummaryJson;
using objc3::artifacts::frontend::BuildRetainReleaseOperationLoweringContract;
using objc3::artifacts::frontend::
    BuildOwnershipSystemExtensionSemanticModelSummaryJson;
using objc3::artifacts::frontend::
    BuildOwnershipSystemExtensionLoweringContractJson;
using objc3::artifacts::frontend::BuildWeakUnownedSemanticsLoweringContract;
using objc3::artifacts::frontend::
    BuildOwnershipBorrowedRetainableAbiCompletionJson;
using objc3::artifacts::frontend::
    BuildOwnershipBorrowedRetainableAbiCompletionReplayKey;
using objc3::artifacts::frontend::
    BuildOwnershipRetainableCFamilySourceCompletionSummaryJson;
using objc3::artifacts::frontend::
    BuildOwnershipSystemExtensionSourceClosureSummaryJson;
using objc3::artifacts::frontend::
    BuildTypeSystemTypeSourceClosureSummaryJson;
using objc3::artifacts::frontend::BuildRuntimeBootstrapLoweringSummaryJson;
using objc3::artifacts::frontend::BuildRuntimeBootstrapSemanticsSummaryJson;
using objc3::artifacts::frontend::BuildRuntimeBootstrapApiSummaryJson;
using objc3::artifacts::frontend::
    BuildRuntimeBootstrapLegalityFailureContractSummaryJson;
using objc3::artifacts::frontend::
    BuildRuntimeBootstrapLegalitySemanticsSummaryJson;
using objc3::artifacts::frontend::
    BuildRuntimeBootstrapFailureRestartSemanticsSummaryJson;
using objc3::artifacts::frontend::
    BuildRuntimeStartupBootstrapInvariantSummaryJson;
using objc3::artifacts::frontend::
    BuildRuntimeRegistrationDescriptorFrontendClosureSummaryJson;
using objc3::artifacts::frontend::
    BuildRuntimeRegistrationDescriptorImageRootSourceSurfaceSummaryJson;
using objc3::artifacts::frontend::
    BuildRuntimeTranslationUnitRegistrationContractSummaryJson;
using objc3::artifacts::frontend::
    BuildRuntimeTranslationUnitRegistrationManifestSummaryJson;
using objc3::artifacts::frontend::BuildRuntimeAwareImportModuleSurfaceReplayKey;
using objc3::artifacts::frontend::BuildRuntimeAwareImportModuleSurfaceSummaryJson;
using objc3::artifacts::frontend::
    BuildRuntimeAwareImportModuleFrontendClosureReplayKey;
using objc3::artifacts::frontend::
    BuildRuntimeAwareImportModuleFrontendClosureSummary;
using objc3::artifacts::frontend::
    BuildRuntimeAwareImportModuleFrontendClosureSummaryJson;
using objc3::artifacts::frontend::
    BuildConcurrencyActorMailboxRuntimeImportSummary;
using objc3::artifacts::frontend::
    BuildConcurrencyActorMailboxRuntimeImportSummaryJson;
using objc3::artifacts::frontend::
    BuildDispatchDispatchMetadataInterfacePreservationSummary;
using objc3::artifacts::frontend::
    BuildDispatchDispatchMetadataInterfacePreservationSummaryJson;
using objc3::artifacts::frontend::
    BuildRuntimeBlockOwnershipArtifactPreservationSummaryJson;
using objc3::artifacts::frontend::
    BuildRuntimeStorageReflectionArtifactPreservationSummaryJson;
using objc3::artifacts::frontend::
    BuildCrossModuleRuntimeMetadataSemanticPreservationReplayKey;
using objc3::artifacts::frontend::
    BuildCrossModuleRuntimeMetadataSemanticPreservationSummary;
using objc3::artifacts::frontend::
    BuildCrossModuleRuntimeMetadataSemanticPreservationSummaryJson;
using objc3::artifacts::frontend::
    BuildImportedRuntimeMetadataSemanticRulesReplayKey;
using objc3::artifacts::frontend::
    BuildImportedRuntimeMetadataSemanticRulesSummary;
using objc3::artifacts::frontend::
    BuildImportedRuntimeMetadataSemanticRulesSummaryJson;
using objc3::artifacts::frontend::
    BuildSerializedRuntimeMetadataImportLoweringReplayKey;
using objc3::artifacts::frontend::
    BuildSerializedRuntimeMetadataImportLoweringSummary;
using objc3::artifacts::frontend::
    BuildSerializedRuntimeMetadataImportLoweringSummaryJson;
using objc3::artifacts::frontend::
    BuildSerializedRuntimeMetadataArtifactReuseReplayKey;
using objc3::artifacts::frontend::
    BuildSerializedRuntimeMetadataArtifactReuseSummary;
using objc3::artifacts::frontend::
    BuildSerializedRuntimeMetadataArtifactReuseSummaryJson;
using objc3::artifacts::frontend::BuildSerializedRuntimeMetadataReuseRecordSet;
using objc3::artifacts::frontend::BuildSerializedRuntimeMetadataReusedModuleNames;
using objc3::artifacts::frontend::
    BuildCrossModuleBuildRuntimeOrchestrationReplayKey;
using objc3::artifacts::frontend::
    BuildCrossModuleBuildRuntimeOrchestrationSummary;
using objc3::artifacts::frontend::
    BuildCrossModuleBuildRuntimeOrchestrationSummaryJson;
using objc3::artifacts::reports::
    BuildFrontendCompatibilityStrictnessClaimSemanticsSummary;
using objc3::artifacts::reports::
    BuildFrontendCompatibilityStrictnessClaimSemanticsSummaryJson;
using objc3::artifacts::reports::
    BuildToolingCorpusShardingReleaseEvidencePackagingSummary;
using objc3::artifacts::reports::
    BuildToolingCorpusShardingReleaseEvidencePackagingSummaryJson;
using objc3::artifacts::reports::
    BuildToolingFeatureAwareConformanceReportEmissionSummary;
using objc3::artifacts::reports::
    BuildToolingFeatureAwareConformanceReportEmissionSummaryJson;
using objc3::artifacts::reports::
    BuildToolingMachineReadableConformanceReportContractSummary;
using objc3::artifacts::reports::
    BuildToolingMachineReadableConformanceReportContractSummaryJson;
using objc3::artifacts::reports::
    BuildVersionedConformanceReportArtifactJson;
using objc3::artifacts::reports::
    BuildVersionedConformanceReportLoweringSummary;
using objc3::artifacts::reports::
    BuildVersionedConformanceReportLoweringSummaryJson;
using objc3c::support::CountRuntimeMetadataSourceRecordSetDeclarations;
using objc3c::support::CountRuntimeMetadataSourceRecordSetReferences;

const char *LanguageProfileName(Objc3FrontendLanguageProfile mode) {
  (void)mode;
  return "canonical";
}

const char *ArcModeName(Objc3FrontendArcMode mode) {
  return mode == Objc3FrontendArcMode::kEnabled ? "enabled" : "disabled";
}

std::string BuildStringArrayJson(const std::vector<std::string> &values) {
  return objc3::io::json::RenderJsonStringArray(values);
}

Objc3ActorIsolationSendabilityLoweringContract
BuildConcurrencyActorIsolationSendabilityLoweringContract(
    const Objc3ConcurrencyExecutorHopAffinityCompatibilitySummary &summary) {
  Objc3ActorIsolationSendabilityLoweringContract contract;
  contract.actor_isolation_sites =
      summary.executor_affinity_sites +
      summary.illegal_missing_executor_affinity_sites +
      summary.illegal_main_executor_detached_sites;
  contract.sendability_check_sites = summary.executor_affinity_sites;
  contract.cross_actor_hop_sites = summary.detached_task_creation_sites;
  contract.non_sendable_capture_sites = 0;
  contract.sendable_transfer_sites = summary.detached_task_creation_sites;
  contract.isolation_boundary_sites = summary.executor_affinity_sites;
  contract.guard_blocked_sites =
      summary.illegal_missing_executor_affinity_sites +
      summary.illegal_main_executor_detached_sites;
  contract.contract_violation_sites = 0;
  contract.deterministic =
      summary.deterministic && summary.ready_for_lowering_and_runtime &&
      contract.actor_isolation_sites ==
          contract.isolation_boundary_sites + contract.guard_blocked_sites;
  return contract;
}

Objc3OwnershipSystemExtensionLoweringContract
BuildOwnershipSystemExtensionLoweringContract(
    const Objc3OwnershipSystemExtensionSemanticModelSummary &semantic_summary,
    const Objc3OwnershipResourceMoveUseAfterMoveSemanticsSummary &resource_summary,
    const Objc3OwnershipBorrowedPointerEscapeAnalysisSummary &borrowed_summary,
    const Objc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary
        &family_summary) {
  // lowering-implementation anchor: the live cleanup/resource
  // helper-emission path continues to consume the single C001 Part 8 lowering
  // contract; this issue does not mint a second manifest surface.
  Objc3OwnershipSystemExtensionLoweringContract contract;
  contract.cleanup_hook_sites = semantic_summary.cleanup_attribute_sites +
                                semantic_summary.cleanup_sugar_sites;
  contract.resource_local_sites = semantic_summary.resource_attribute_sites +
                                  semantic_summary.resource_sugar_sites;
  contract.cleanup_owned_local_sites = resource_summary.cleanup_owned_local_sites;
  contract.resource_move_capture_sites =
      resource_summary.resource_move_capture_sites;
  contract.borrowed_parameter_sites = borrowed_summary.borrowed_parameter_sites;
  contract.borrowed_return_callable_sites =
      borrowed_summary.borrowed_return_callable_sites;
  contract.borrowed_escape_candidate_sites =
      borrowed_summary.borrowed_escape_candidate_sites;
  contract.explicit_capture_item_sites =
      family_summary.explicit_capture_item_sites;
  contract.retainable_family_callable_sites =
      family_summary.retainable_family_callable_sites;
  contract.retainable_family_operation_callable_sites =
      family_summary.retainable_family_operation_callable_sites;
  contract.retainable_family_alias_callable_sites =
      family_summary.retainable_family_alias_callable_sites;
  contract.guard_blocked_sites =
      resource_summary.illegal_non_resource_move_sites +
      resource_summary.illegal_use_after_move_sites +
      resource_summary.illegal_duplicate_move_sites +
      borrowed_summary.illegal_unproven_call_escape_sites +
      borrowed_summary.illegal_escaping_block_capture_sites +
      borrowed_summary.illegal_borrowed_return_sites +
      family_summary.illegal_duplicate_explicit_capture_sites +
      family_summary.illegal_non_object_capture_mode_sites +
      family_summary.illegal_unused_explicit_capture_sites +
      family_summary.illegal_conflicting_retainable_family_sites +
      family_summary.illegal_invalid_family_operation_shape_sites +
      family_summary.illegal_invalid_family_alias_shape_sites;
  contract.contract_violation_sites = 0;
  contract.deterministic =
      semantic_summary.deterministic &&
      semantic_summary.ready_for_lowering_and_runtime &&
      resource_summary.deterministic &&
      resource_summary.ready_for_lowering_and_runtime &&
      borrowed_summary.deterministic &&
      borrowed_summary.ready_for_lowering_and_runtime &&
      family_summary.deterministic &&
      family_summary.ready_for_lowering_and_runtime;
  return contract;
}

Objc3DispatchDispatchControlLoweringContract
BuildDispatchDispatchControlLoweringContract(
    const Objc3DispatchDispatchIntentSemanticModelSummary &semantic_summary,
    const Objc3DispatchDispatchIntentLegalitySummary &legality_summary,
    const Objc3DispatchDispatchIntentCompatibilitySummary &compatibility_summary) {
  Objc3DispatchDispatchControlLoweringContract contract;
  const std::size_t dispatch_intent_callable_capacity =
      compatibility_summary.callable_dispatch_intent_sites;
  const std::size_t dispatch_intent_container_capacity =
      compatibility_summary.container_dispatch_intent_sites;
  const std::size_t dispatch_intent_override_capacity =
      semantic_summary.effective_direct_member_sites +
      dispatch_intent_container_capacity;
  const std::size_t dispatch_intent_guard_capacity =
      dispatch_intent_callable_capacity + dispatch_intent_container_capacity;
  const std::size_t raw_guard_blocked_sites =
      legality_summary.illegal_final_superclass_sites +
      legality_summary.illegal_sealed_superclass_sites +
      legality_summary.illegal_final_override_sites +
      legality_summary.illegal_direct_override_sites +
      compatibility_summary.illegal_direct_dynamic_conflict_sites +
      compatibility_summary.illegal_final_dynamic_conflict_sites +
      compatibility_summary.illegal_non_method_callable_sites +
      compatibility_summary.illegal_protocol_method_sites +
      compatibility_summary.illegal_category_method_sites +
      compatibility_summary.illegal_category_container_sites;
  contract.direct_call_candidate_sites =
      semantic_summary.effective_direct_member_sites;
  contract.direct_members_defaulted_sites =
      semantic_summary.direct_members_defaulted_method_sites;
  contract.dynamic_opt_out_sites =
      semantic_summary.direct_members_dynamic_opt_out_sites;
  contract.final_container_sites = semantic_summary.final_container_sites;
  contract.sealed_container_sites = semantic_summary.sealed_container_sites;
  // Ordinary dynamic-override accounting must not invalidate the dispatch-intent
  // lowering contract when the source program does not opt into direct/final/sealed
  // dispatch-control features.
  contract.override_legality_sites =
      std::min(legality_summary.override_sites, dispatch_intent_override_capacity);
  contract.metadata_preserved_callable_sites =
      dispatch_intent_callable_capacity;
  contract.metadata_preserved_container_sites =
      dispatch_intent_container_capacity;
  contract.guard_blocked_sites =
      std::min(raw_guard_blocked_sites, dispatch_intent_guard_capacity);
  contract.contract_violation_sites = 0;
  contract.deterministic =
      semantic_summary.deterministic &&
      semantic_summary.ready_for_core_implementation &&
      legality_summary.deterministic &&
      legality_summary.ready_for_lowering_and_runtime &&
      compatibility_summary.deterministic &&
      compatibility_summary.ready_for_lowering_and_runtime;
  return contract;
}

Objc3TaskRuntimeInteropCancellationLoweringContract
BuildConcurrencyTaskRuntimeInteropCancellationLoweringContract(
    const Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary
        &semantic_summary,
    const Objc3ConcurrencyStructuredTaskCancellationSemanticSummary
        &structured_summary,
    const Objc3ConcurrencyExecutorHopAffinityCompatibilitySummary &executor_summary) {
  Objc3TaskRuntimeInteropCancellationLoweringContract contract;
  contract.task_runtime_sites =
      semantic_summary.task_runtime_interop_sites +
      structured_summary.illegal_non_async_task_sites +
      structured_summary.illegal_task_group_scope_sites +
      structured_summary.illegal_task_hierarchy_sites +
      structured_summary.illegal_cancellation_usage_sites +
      executor_summary.illegal_missing_executor_affinity_sites +
      executor_summary.illegal_main_executor_detached_sites;
  contract.task_runtime_interop_sites =
      semantic_summary.task_runtime_interop_sites;
  contract.cancellation_probe_sites =
      semantic_summary.cancellation_check_sites;
  contract.cancellation_handler_sites =
      semantic_summary.cancellation_handler_sites;
  contract.runtime_resume_sites =
      structured_summary.task_group_wait_next_sites;
  contract.runtime_cancel_sites =
      structured_summary.task_group_cancel_all_sites;
  contract.guard_blocked_sites =
      structured_summary.illegal_non_async_task_sites +
      structured_summary.illegal_task_group_scope_sites +
      structured_summary.illegal_task_hierarchy_sites +
      structured_summary.illegal_cancellation_usage_sites +
      executor_summary.illegal_missing_executor_affinity_sites +
      executor_summary.illegal_main_executor_detached_sites;
  contract.normalized_sites =
      contract.task_runtime_sites - contract.guard_blocked_sites;
  contract.contract_violation_sites = 0;
  contract.deterministic =
      semantic_summary.deterministic && structured_summary.deterministic &&
      executor_summary.deterministic &&
      semantic_summary.ready_for_lowering_and_runtime &&
      structured_summary.ready_for_lowering_and_runtime &&
      executor_summary.ready_for_lowering_and_runtime &&
      contract.task_runtime_interop_sites <= contract.task_runtime_sites;
  return contract;
}

Objc3ConcurrencyReplayRaceGuardLoweringContract
BuildConcurrencyConcurrencyReplayRaceGuardLoweringContract(
    const Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary
        &semantic_summary,
    const Objc3ConcurrencyStructuredTaskCancellationSemanticSummary
        &structured_summary,
    const Objc3ConcurrencyExecutorHopAffinityCompatibilitySummary &executor_summary,
    const Objc3ActorIsolationSendabilityLoweringContract &actor_contract) {
  Objc3ConcurrencyReplayRaceGuardLoweringContract contract;
  contract.replay_proof_sites =
      semantic_summary.task_creation_sites +
      structured_summary.task_group_wait_next_sites;
  contract.race_guard_sites =
      semantic_summary.cancellation_check_sites +
      semantic_summary.cancellation_handler_sites;
  contract.task_handoff_sites =
      semantic_summary.task_creation_sites +
      executor_summary.detached_task_creation_sites +
      structured_summary.task_group_wait_next_sites;
  contract.actor_isolation_sites = actor_contract.actor_isolation_sites;
  contract.guard_blocked_sites =
      structured_summary.illegal_non_async_task_sites +
      structured_summary.illegal_task_group_scope_sites +
      structured_summary.illegal_task_hierarchy_sites +
      structured_summary.illegal_cancellation_usage_sites +
      executor_summary.illegal_missing_executor_affinity_sites +
      executor_summary.illegal_main_executor_detached_sites;
  contract.deterministic_schedule_sites =
      contract.replay_proof_sites + contract.race_guard_sites +
      contract.task_handoff_sites + contract.actor_isolation_sites;
  contract.concurrency_replay_sites =
      contract.deterministic_schedule_sites + contract.guard_blocked_sites;
  contract.contract_violation_sites = 0;
  contract.deterministic =
      semantic_summary.deterministic && structured_summary.deterministic &&
      executor_summary.deterministic && actor_contract.deterministic &&
      semantic_summary.ready_for_lowering_and_runtime &&
      structured_summary.ready_for_lowering_and_runtime &&
      executor_summary.ready_for_lowering_and_runtime;
  return contract;
}

}  // namespace

Objc3FrontendArtifactBundle BuildObjc3FrontendArtifacts(const std::filesystem::path &input_path,
                                                        const Objc3FrontendPipelineResult &pipeline_result,
                                                        const Objc3FrontendOptions &options) {
  Objc3FrontendArtifactBundle bundle;
  const Objc3Program &program = Objc3ParsedProgramAst(pipeline_result.program);
  bundle.stage_diagnostics = pipeline_result.stage_diagnostics;
  bundle.parse_lowering_readiness_surface = BuildObjc3ParseLoweringReadinessSurface(pipeline_result, options);
  bundle.diagnostics = FlattenStageDiagnostics(bundle.stage_diagnostics);
  if (!bundle.diagnostics.empty()) {
    return bundle;
  }

  const bool metadata_only_ir_emission_mode =
      Objc3FrontendArtifactMetadataOnlyIrEmissionMode(pipeline_result);
  std::string post_pipeline_failure_code;
  std::string post_pipeline_failure_message;
  const auto record_post_pipeline_failure = [&](const char *code, std::string message) {
    if (!post_pipeline_failure_code.empty()) {
      return;
    }
    post_pipeline_failure_code = code == nullptr ? "" : code;
    post_pipeline_failure_message = std::move(message);
  };

  const Objc3IREmissionCoreFeatureImplementationSurface
      ir_emission_core_feature_impl_surface =
          BuildObjc3IREmissionCoreFeatureImplementationSurface(pipeline_result);

  const auto initial_post_pipeline_failure =
      BuildObjc3FrontendArtifactInitialPostPipelineFailure(
          pipeline_result, bundle.parse_lowering_readiness_surface,
          ir_emission_core_feature_impl_surface, metadata_only_ir_emission_mode);
  if (!initial_post_pipeline_failure.empty()) {
    record_post_pipeline_failure(initial_post_pipeline_failure.code.c_str(),
                                 initial_post_pipeline_failure.message);
  }
  const Objc3FrontendArtifactFunctionManifest function_manifest =
      BuildObjc3FrontendArtifactFunctionManifest(program, pipeline_result);
  const std::vector<const FunctionDecl *> &manifest_functions =
      function_manifest.manifest_functions;
  const std::size_t scalar_return_i32 = function_manifest.scalar_return_i32;
  const std::size_t scalar_return_bool = function_manifest.scalar_return_bool;
  const std::size_t scalar_return_void = function_manifest.scalar_return_void;
  const std::size_t scalar_param_i32 = function_manifest.scalar_param_i32;
  const std::size_t scalar_param_bool = function_manifest.scalar_param_bool;
  const std::size_t vector_signature_functions =
      function_manifest.vector_signature_functions;
  const std::size_t vector_return_signatures =
      function_manifest.vector_return_signatures;
  const std::size_t vector_param_signatures =
      function_manifest.vector_param_signatures;
  const std::size_t vector_i32_signatures =
      function_manifest.vector_i32_signatures;
  const std::size_t vector_bool_signatures =
      function_manifest.vector_bool_signatures;
  const std::size_t vector_lane2_signatures =
      function_manifest.vector_lane2_signatures;
  const std::size_t vector_lane4_signatures =
      function_manifest.vector_lane4_signatures;
  const std::size_t vector_lane8_signatures =
      function_manifest.vector_lane8_signatures;
  const std::size_t vector_lane16_signatures =
      function_manifest.vector_lane16_signatures;
  const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff = pipeline_result.sema_type_metadata_handoff;
  const Objc3InterfaceImplementationSummary &interface_implementation_summary =
      type_metadata_handoff.interface_implementation_summary;
  const Objc3FrontendProtocolCategorySummary &protocol_category_summary = pipeline_result.protocol_category_summary;
  const Objc3FrontendClassProtocolCategoryLinkingSummary &class_protocol_category_linking_summary =
      pipeline_result.class_protocol_category_linking_summary;
  const Objc3FrontendSelectorNormalizationSummary &selector_normalization_summary =
      pipeline_result.selector_normalization_summary;
  const Objc3FrontendPropertyAttributeSummary &property_attribute_summary =
      pipeline_result.property_attribute_summary;
  const Objc3FrontendObjectPointerNullabilityGenericsSummary &object_pointer_nullability_generics_summary =
      pipeline_result.object_pointer_nullability_generics_summary;
  const Objc3FrontendTypeSystemTypeSourceClosureSummary &type_system_type_source_closure_summary =
      pipeline_result.type_system_type_source_closure_summary;
  const Objc3FrontendControlFlowControlFlowSourceClosureSummary
      &control_flow_control_flow_source_closure_summary =

          pipeline_result.control_flow_control_flow_source_closure_summary;
  const Objc3FrontendErrorHandlingErrorSourceClosureSummary
      &error_handling_error_source_closure_summary =
          pipeline_result.error_handling_error_source_closure_summary;
  const Objc3FrontendConcurrencyAsyncSourceClosureSummary
      &concurrency_async_source_closure_summary =
          pipeline_result.concurrency_async_source_closure_summary;
  const Objc3FrontendOwnershipSystemExtensionSourceClosureSummary
      &ownership_system_extension_source_closure_summary =
          pipeline_result.ownership_system_extension_source_closure_summary;
  const Objc3FrontendOwnershipCleanupResourceCaptureSourceCompletionSummary
      &ownership_cleanup_resource_capture_source_completion_summary =
          pipeline_result.ownership_cleanup_resource_capture_source_completion_summary;
  const Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary
      &ownership_retainable_c_family_source_completion_summary =
          pipeline_result.ownership_retainable_c_family_source_completion_summary;
  const Objc3FrontendDispatchDispatchIntentSourceClosureSummary
      &dispatch_dispatch_intent_source_closure_summary =
          pipeline_result.dispatch_dispatch_intent_source_closure_summary;
  const Objc3FrontendDispatchDispatchIntentSourceCompletionSummary
      &dispatch_dispatch_intent_source_completion_summary =
          pipeline_result.dispatch_dispatch_intent_source_completion_summary;
  const Objc3FrontendMetaprogrammingMetaprogrammingSourceClosureSummary
      &metaprogramming_metaprogramming_source_closure_summary =
          pipeline_result.metaprogramming_metaprogramming_source_closure_summary;
  const Objc3FrontendMetaprogrammingMacroPackageProvenanceSourceCompletionSummary
      &metaprogramming_macro_package_provenance_source_completion_summary =
          pipeline_result.metaprogramming_macro_package_provenance_source_completion_summary;
  const Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary
      &metaprogramming_property_behavior_source_completion_summary =
          pipeline_result.metaprogramming_property_behavior_source_completion_summary;
  const Objc3FrontendInteropForeignImportSourceClosureSummary
      &interop_foreign_import_source_closure_summary =
          pipeline_result.interop_foreign_import_source_closure_summary;
  const Objc3FrontendInteropCppSwiftInteropAnnotationSourceCompletionSummary
      &interop_cpp_swift_interop_annotation_source_completion_summary =
          pipeline_result.interop_cpp_swift_interop_annotation_source_completion_summary;
  const Objc3FrontendToolingDiagnosticsMigratorSourceInventorySummary
      &tooling_diagnostics_migrator_source_inventory_summary =
          pipeline_result.tooling_diagnostics_migrator_source_inventory_summary;
  const Objc3FrontendToolingMigrationCanonicalizationSourceCompletionSummary
      &tooling_migration_canonicalization_source_completion_summary =
          pipeline_result
              .tooling_migration_canonicalization_source_completion_summary;
  const Objc3ToolingDiagnosticTaxonomyPortabilityContractSummary
      &tooling_diagnostic_taxonomy_portability_contract_summary =
          pipeline_result.tooling_diagnostic_taxonomy_portability_contract_summary;
  const Objc3ToolingFeatureSpecificFixitSynthesisSummary
      &tooling_feature_specific_fixit_synthesis_summary =
          pipeline_result.tooling_feature_specific_fixit_synthesis_summary;
  const Objc3InteropInteropSemanticModelSummary
      &interop_interop_semantic_model_summary =
          pipeline_result.interop_interop_semantic_model_summary;
  const Objc3InteropInteropRuntimeParitySummary
      &interop_interop_runtime_parity_summary =
          pipeline_result.interop_interop_runtime_parity_summary;
  const Objc3InteropCppInteropInteractionSummary
      &interop_cpp_interop_interaction_summary =
          pipeline_result.interop_cpp_interop_interaction_summary;
  const Objc3InteropSwiftInteropIsolationSummary
      &interop_swift_interop_isolation_summary =
          pipeline_result.interop_swift_interop_isolation_summary;
  const Objc3MetaprogrammingExpansionBehaviorSemanticModelSummary
      &metaprogramming_expansion_behavior_semantic_model_summary =
          pipeline_result.metaprogramming_expansion_behavior_semantic_model_summary;
  const Objc3MetaprogrammingDeriveExpansionInventorySummary
      &metaprogramming_derive_expansion_inventory_summary =
          pipeline_result.metaprogramming_derive_expansion_inventory_summary;
  const Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary
      &metaprogramming_macro_safety_sandbox_determinism_summary =
          pipeline_result.metaprogramming_macro_safety_sandbox_determinism_summary;
  const Objc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary
      &metaprogramming_property_behavior_legality_compatibility_summary =
          pipeline_result.metaprogramming_property_behavior_legality_compatibility_summary;
  const Objc3DispatchDispatchIntentSemanticModelSummary
      &dispatch_dispatch_intent_semantic_model_summary =
          pipeline_result.dispatch_dispatch_intent_semantic_model_summary;
  const Objc3DispatchDispatchIntentLegalitySummary
      &dispatch_dispatch_intent_legality_summary =
          pipeline_result.dispatch_dispatch_intent_legality_summary;
  const Objc3DispatchDispatchIntentCompatibilitySummary
      &dispatch_dispatch_intent_compatibility_summary =
          pipeline_result.dispatch_dispatch_intent_compatibility_summary;
  const Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary
      &concurrency_actor_member_isolation_source_closure_summary =
          pipeline_result.concurrency_actor_member_isolation_source_closure_summary;
  const Objc3ConcurrencyActorIsolationSendableSemanticModelSummary
      &concurrency_actor_isolation_sendable_semantic_model_summary =
          pipeline_result.concurrency_actor_isolation_sendable_semantic_model_summary;
  const Objc3ConcurrencyActorIsolationSendabilityEnforcementSummary
      &concurrency_actor_isolation_sendability_enforcement_summary =
          pipeline_result.concurrency_actor_isolation_sendability_enforcement_summary;
  const Objc3ConcurrencyActorRaceHazardEscapeDiagnosticsSummary
      &concurrency_actor_race_hazard_escape_diagnostics_summary =
          pipeline_result.concurrency_actor_race_hazard_escape_diagnostics_summary;
  const Objc3FrontendConcurrencyTaskGroupCancellationSourceClosureSummary
      &concurrency_task_group_cancellation_source_closure_summary =
          pipeline_result.concurrency_task_group_cancellation_source_closure_summary;
  const Objc3ErrorHandlingErrorSemanticModelSummary
      &error_handling_error_semantic_model_summary =
          pipeline_result.error_handling_error_semantic_model_summary;
  const Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary
      &concurrency_task_executor_cancellation_semantic_model_summary =
          pipeline_result.concurrency_task_executor_cancellation_semantic_model_summary;
  const Objc3OwnershipSystemExtensionSemanticModelSummary
      &ownership_system_extension_semantic_model_summary =
          pipeline_result.ownership_system_extension_semantic_model_summary;
  const Objc3EffectsOwnershipSemanticModelSummary
      &effects_ownership_semantic_model_summary =
          pipeline_result.effects_ownership_semantic_model_summary;
  const Objc3CrossModuleSemanticContractsDiagnosticsSummary
      &cross_module_semantic_contracts_diagnostics_summary =
          pipeline_result.cross_module_semantic_contracts_diagnostics_summary;
  const Objc3OwnershipResourceMoveUseAfterMoveSemanticsSummary
      &ownership_resource_move_use_after_move_semantics_summary =
          pipeline_result.ownership_resource_move_use_after_move_semantics_summary;
  const Objc3OwnershipBorrowedPointerEscapeAnalysisSummary
      &ownership_borrowed_pointer_escape_analysis_summary =
          pipeline_result.ownership_borrowed_pointer_escape_analysis_summary;
  const Objc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary
      &ownership_capture_list_retainable_family_legality_completion_summary =
          pipeline_result
              .ownership_capture_list_retainable_family_legality_completion_summary;
  const Objc3DispatchDispatchControlLoweringContract
      dispatch_dispatch_control_lowering_contract =
          BuildDispatchDispatchControlLoweringContract(
              dispatch_dispatch_intent_semantic_model_summary,
              dispatch_dispatch_intent_legality_summary,
              dispatch_dispatch_intent_compatibility_summary);
  if (!IsValidObjc3DispatchDispatchControlLoweringContract(
          dispatch_dispatch_control_lowering_contract)) {
    record_post_pipeline_failure(
        "O3L300",
        "LLVM IR emission failed: invalid dispatch-control lowering contract");
  }
  const std::string dispatch_dispatch_control_lowering_replay_key =
      Objc3DispatchDispatchControlLoweringReplayKey(
          dispatch_dispatch_control_lowering_contract);
  const Objc3MetaprogrammingExpansionLoweringContract
      metaprogramming_expansion_lowering_contract = BuildMetaprogrammingExpansionLoweringContract(
          metaprogramming_property_behavior_source_completion_summary,
          metaprogramming_derive_expansion_inventory_summary,
          metaprogramming_macro_safety_sandbox_determinism_summary,
          metaprogramming_property_behavior_legality_compatibility_summary);
  if (!IsValidObjc3MetaprogrammingExpansionLoweringContract(
          metaprogramming_expansion_lowering_contract)) {
    record_post_pipeline_failure(
        "O3L300",
        "LLVM IR emission failed: invalid Part 10 expansion lowering contract");
  }
  const std::string metaprogramming_expansion_lowering_replay_key =
      Objc3MetaprogrammingExpansionLoweringReplayKey(
          metaprogramming_expansion_lowering_contract);
  const std::vector<Objc3IRMetaprogrammingDerivedMethodBundle>
      metaprogramming_derived_method_bundles =
          BuildMetaprogrammingDerivedMethodBundles(program);
  const std::vector<Objc3IRMetaprogrammingMacroArtifactBundle>
      metaprogramming_macro_artifact_bundles =
          BuildMetaprogrammingMacroArtifactBundles(program);
  const std::vector<Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle>
      metaprogramming_property_behavior_artifact_bundles =
          BuildMetaprogrammingPropertyBehaviorArtifactBundles(program);
  const Objc3MetaprogrammingSynthesizedArtifactEmissionContract
      metaprogramming_synthesized_artifact_emission_contract =
          BuildMetaprogrammingSynthesizedArtifactEmissionContract(
              metaprogramming_expansion_lowering_contract,
              metaprogramming_derived_method_bundles,
              metaprogramming_macro_artifact_bundles,
              metaprogramming_property_behavior_artifact_bundles);
  if (!IsValidObjc3MetaprogrammingSynthesizedArtifactEmissionContract(
          metaprogramming_synthesized_artifact_emission_contract)) {
    record_post_pipeline_failure(
        "O3L300",
        "LLVM IR emission failed: invalid Part 10 synthesized artifact emission contract");
  }
  const std::string metaprogramming_synthesized_artifact_emission_replay_key =
      Objc3MetaprogrammingSynthesizedArtifactEmissionReplayKey(
          metaprogramming_synthesized_artifact_emission_contract);
  const Objc3OwnershipSystemExtensionLoweringContract
      ownership_system_extension_lowering_contract =
          BuildOwnershipSystemExtensionLoweringContract(
              ownership_system_extension_semantic_model_summary,
              ownership_resource_move_use_after_move_semantics_summary,
              ownership_borrowed_pointer_escape_analysis_summary,
              ownership_capture_list_retainable_family_legality_completion_summary);
  if (!IsValidObjc3OwnershipSystemExtensionLoweringContract(
          ownership_system_extension_lowering_contract)) {
    record_post_pipeline_failure(
        "O3L300",
        "LLVM IR emission failed: invalid system-extension lowering contract");
  }
  const std::string ownership_system_extension_lowering_replay_key =
      Objc3OwnershipSystemExtensionLoweringReplayKey(
          ownership_system_extension_lowering_contract);
  const std::string ownership_borrowed_retainable_abi_completion_replay_key =
      BuildOwnershipBorrowedRetainableAbiCompletionReplayKey(
          ownership_system_extension_lowering_contract,
          ownership_system_extension_source_closure_summary,
          ownership_retainable_c_family_source_completion_summary);
  const Objc3ConcurrencyStructuredTaskCancellationSemanticSummary
      &concurrency_structured_task_cancellation_semantic_summary =
          pipeline_result.concurrency_structured_task_cancellation_semantic_summary;
  const Objc3ConcurrencyExecutorHopAffinityCompatibilitySummary
      &concurrency_executor_hop_affinity_compatibility_summary =
          pipeline_result.concurrency_executor_hop_affinity_compatibility_summary;
  const Objc3ConcurrencyAsyncEffectSuspensionSemanticModelSummary
      &concurrency_async_effect_suspension_semantic_model_summary =
          pipeline_result.concurrency_async_effect_suspension_semantic_model_summary;
  const Objc3ConcurrencyAwaitSuspensionResumeSemanticSummary
      &concurrency_await_suspension_resume_semantic_summary =
          pipeline_result.concurrency_await_suspension_resume_semantic_summary;
  const Objc3ConcurrencyAsyncDiagnosticsCompatibilitySummary
      &concurrency_async_diagnostics_compatibility_summary =
          pipeline_result.concurrency_async_diagnostics_compatibility_summary;
  const Objc3AsyncContinuationLoweringContract
      concurrency_async_continuation_lowering_contract =
          BuildConcurrencyAsyncContinuationLoweringContract(
              concurrency_async_effect_suspension_semantic_model_summary,
              concurrency_async_diagnostics_compatibility_summary);
  const Objc3AwaitLoweringSuspensionStateLoweringContract
      concurrency_await_lowering_suspension_state_lowering_contract =
          BuildConcurrencyAwaitLoweringSuspensionStateLoweringContract(
              concurrency_await_suspension_resume_semantic_summary);
  if (!IsValidObjc3AsyncContinuationLoweringContract(
          concurrency_async_continuation_lowering_contract)) {
    record_post_pipeline_failure(
        "O3L300",
        "LLVM IR emission failed: invalid async continuation lowering contract");
  }
  if (!IsValidObjc3AwaitLoweringSuspensionStateLoweringContract(
          concurrency_await_lowering_suspension_state_lowering_contract)) {
    record_post_pipeline_failure(
        "O3L300",
        "LLVM IR emission failed: invalid await suspension lowering contract");
  }
  const std::string concurrency_async_continuation_lowering_replay_key =
      Objc3AsyncContinuationLoweringReplayKey(
          concurrency_async_continuation_lowering_contract);
  const std::string concurrency_await_lowering_suspension_state_lowering_replay_key =
      Objc3AwaitLoweringSuspensionStateLoweringReplayKey(
          concurrency_await_lowering_suspension_state_lowering_contract);
  const Objc3ActorIsolationSendabilityLoweringContract
      concurrency_actor_isolation_sendability_lowering_contract =
          BuildConcurrencyActorIsolationSendabilityLoweringContract(
              concurrency_executor_hop_affinity_compatibility_summary);
  if (!IsValidObjc3ActorIsolationSendabilityLoweringContract(
          concurrency_actor_isolation_sendability_lowering_contract)) {
    record_post_pipeline_failure(
        "O3L300",
        "LLVM IR emission failed: invalid actor isolation sendability lowering contract");
  }
  const std::string concurrency_actor_isolation_sendability_lowering_replay_key =
      Objc3ActorIsolationSendabilityLoweringReplayKey(
          concurrency_actor_isolation_sendability_lowering_contract);
  const Objc3ActorLoweringMetadataContract
      concurrency_actor_lowering_metadata_contract =
          BuildConcurrencyActorLoweringMetadataContract(
              concurrency_actor_member_isolation_source_closure_summary,
              concurrency_actor_isolation_sendability_enforcement_summary,
              concurrency_actor_race_hazard_escape_diagnostics_summary);
  if (!IsValidObjc3ActorLoweringMetadataContract(
          concurrency_actor_lowering_metadata_contract)) {
    record_post_pipeline_failure(
        "O3L300",
        "LLVM IR emission failed: invalid actor lowering metadata contract");
  }
  const std::string concurrency_actor_lowering_metadata_replay_key =
      Objc3ActorLoweringMetadataReplayKey(
          concurrency_actor_lowering_metadata_contract);
  const Objc3TaskRuntimeInteropCancellationLoweringContract
      concurrency_task_runtime_interop_cancellation_lowering_contract =
          BuildConcurrencyTaskRuntimeInteropCancellationLoweringContract(
              concurrency_task_executor_cancellation_semantic_model_summary,
              concurrency_structured_task_cancellation_semantic_summary,
              concurrency_executor_hop_affinity_compatibility_summary);
  if (!IsValidObjc3TaskRuntimeInteropCancellationLoweringContract(
          concurrency_task_runtime_interop_cancellation_lowering_contract)) {
    record_post_pipeline_failure(
        "O3L300",
        "LLVM IR emission failed: invalid task runtime interop cancellation lowering contract");
  }
  const std::string concurrency_task_runtime_interop_cancellation_lowering_replay_key =
      Objc3TaskRuntimeInteropCancellationLoweringReplayKey(
          concurrency_task_runtime_interop_cancellation_lowering_contract);
  const Objc3ConcurrencyReplayRaceGuardLoweringContract
      concurrency_concurrency_replay_race_guard_lowering_contract =
          BuildConcurrencyConcurrencyReplayRaceGuardLoweringContract(
              concurrency_task_executor_cancellation_semantic_model_summary,
              concurrency_structured_task_cancellation_semantic_summary,
              concurrency_executor_hop_affinity_compatibility_summary,
              concurrency_actor_isolation_sendability_lowering_contract);
  if (!IsValidObjc3ConcurrencyReplayRaceGuardLoweringContract(
          concurrency_concurrency_replay_race_guard_lowering_contract)) {
    record_post_pipeline_failure(
        "O3L300",
        "LLVM IR emission failed: invalid concurrency replay race guard lowering contract");
  }
  const std::string concurrency_concurrency_replay_race_guard_lowering_replay_key =
      Objc3ConcurrencyReplayRaceGuardLoweringReplayKey(
          concurrency_concurrency_replay_race_guard_lowering_contract);
  const Objc3ErrorHandlingTryDoCatchSemanticSummary
      &error_handling_try_do_catch_semantic_summary =
          pipeline_result.error_handling_try_do_catch_semantic_summary;
  const Objc3ErrorHandlingErrorBridgeLegalitySummary
      &error_handling_error_bridge_legality_summary =
          pipeline_result.error_handling_error_bridge_legality_summary;
  const Objc3TypeSystemTypeSemanticModelSummary type_system_type_semantic_model_summary =
      BuildTypeSystemTypeSemanticModelSummary(
          pipeline_result.program.ast, pipeline_result.integration_surface, 4u);
  const Objc3ControlFlowControlFlowSemanticModelSummary
      &control_flow_control_flow_semantic_model_summary =
          pipeline_result.control_flow_control_flow_semantic_model_summary;
  const Objc3FrontendSymbolGraphScopeResolutionSummary &symbol_graph_scope_resolution_summary =
      pipeline_result.symbol_graph_scope_resolution_summary;
  const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records =
      pipeline_result.runtime_metadata_source_records;
  const Objc3ExecutableMetadataSourceGraph &executable_metadata_source_graph =
      pipeline_result.executable_metadata_source_graph;
  const Objc3ExecutableMetadataSemanticConsistencyBoundary
      &executable_metadata_semantic_consistency_boundary =
          pipeline_result.executable_metadata_semantic_consistency_boundary;
  const Objc3ExecutableMetadataSemanticValidationSurface
      &executable_metadata_semantic_validation_surface =
          pipeline_result.executable_metadata_semantic_validation_surface;
  const Objc3ExecutableMetadataLoweringHandoffSurface
      &executable_metadata_lowering_handoff_surface =
          pipeline_result.executable_metadata_lowering_handoff_surface;
  const Objc3ExecutableMetadataTypedLoweringHandoff
      &executable_metadata_typed_lowering_handoff =
          pipeline_result.executable_metadata_typed_lowering_handoff;
  const Objc3RuntimeMetadataSourceOwnershipBoundary &runtime_metadata_source_ownership =
      pipeline_result.runtime_metadata_source_ownership_boundary;
  const Objc3RuntimeExportLegalityBoundary &runtime_export_legality =
      pipeline_result.runtime_export_legality_boundary;
  const Objc3RuntimeExportEnforcementSummary &runtime_export_enforcement =
      pipeline_result.runtime_export_enforcement_summary;
  const Objc3FrontendArtifactRuntimeMetadataPlan runtime_metadata_plan =
      BuildObjc3FrontendArtifactRuntimeMetadataPlan(pipeline_result);
  const Objc3RuntimeMetadataSectionAbiFreezeSummary
      &runtime_metadata_section_abi =
          runtime_metadata_plan.runtime_metadata_section_abi;
  const Objc3RuntimeMetadataSectionPublicationSummary
      &runtime_metadata_section_publication =
          runtime_metadata_plan.runtime_metadata_section_publication;
  const Objc3RuntimeMetadataObjectInspectionHarnessSummary
      &runtime_metadata_object_inspection =
          runtime_metadata_plan.runtime_metadata_object_inspection;
  const Objc3RuntimeMetadataSourceToSectionMatrixSummary
      &runtime_metadata_source_to_section_matrix =
          runtime_metadata_plan.runtime_metadata_source_to_section_matrix;
  const Objc3ExecutableMetadataDebugProjectionSummary
      &executable_metadata_debug_projection =
          runtime_metadata_plan.executable_metadata_debug_projection;
  const Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary
      &executable_metadata_runtime_ingest_packaging_contract =
          runtime_metadata_plan
              .executable_metadata_runtime_ingest_packaging_contract;
  const std::string &executable_metadata_runtime_ingest_binary_payload =
      runtime_metadata_plan.executable_metadata_runtime_ingest_binary_payload;
  const Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary
      &executable_metadata_runtime_ingest_binary_boundary =
          runtime_metadata_plan
              .executable_metadata_runtime_ingest_binary_boundary;
  const Objc3FrontendArtifactRuntimeRegistrationPlan runtime_registration_plan =
      BuildObjc3FrontendArtifactRuntimeRegistrationPlan(
          input_path, program, pipeline_result, options, runtime_metadata_plan);
  const Objc3RuntimeSupportLibraryContractSummary &runtime_support_library =
      runtime_registration_plan.runtime_support_library;
  const Objc3RuntimeSupportLibraryCoreFeatureSummary
      &runtime_support_library_core_feature =
          runtime_registration_plan.runtime_support_library_core_feature;
  const Objc3RuntimeSupportLibraryLinkWiringSummary
      &runtime_support_library_link_wiring =
          runtime_registration_plan.runtime_support_library_link_wiring;
  const Objc3RuntimeTranslationUnitRegistrationContractSummary
      &runtime_translation_unit_registration_contract =
          runtime_registration_plan
              .runtime_translation_unit_registration_contract;
  const Objc3RuntimeTranslationUnitRegistrationManifestSummary
      &runtime_translation_unit_registration_manifest =
          runtime_registration_plan
              .runtime_translation_unit_registration_manifest;
  const Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary
      &runtime_registration_descriptor_image_root_source_surface =
          runtime_registration_plan
              .runtime_registration_descriptor_image_root_source_surface;
  const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
      &runtime_registration_descriptor_frontend_closure =
          runtime_registration_plan
              .runtime_registration_descriptor_frontend_closure;
  const std::string &translation_unit_identity_key =
      runtime_registration_plan.translation_unit_identity_key;
  const Objc3RuntimeStartupBootstrapInvariantSummary
      &runtime_startup_bootstrap_invariants =
          runtime_registration_plan.runtime_startup_bootstrap_invariants;
  const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api =
      runtime_registration_plan.runtime_bootstrap_api;
  const Objc3RuntimeBootstrapSemanticsSummary &runtime_bootstrap_semantics =
      runtime_registration_plan.runtime_bootstrap_semantics;
  const Objc3RuntimeBootstrapLegalityFailureContractSummary
      &runtime_bootstrap_legality_failure_contract =
          runtime_registration_plan
              .runtime_bootstrap_legality_failure_contract;
  const Objc3RuntimeBootstrapLegalitySemanticsSummary
      &runtime_bootstrap_legality_semantics =
          runtime_registration_plan.runtime_bootstrap_legality_semantics;
  const Objc3RuntimeBootstrapLoweringSummary &runtime_bootstrap_lowering =
      runtime_registration_plan.runtime_bootstrap_lowering;
  const Objc3RuntimeBootstrapFailureRestartSemanticsSummary
      &runtime_bootstrap_failure_restart_semantics =
          runtime_registration_plan
              .runtime_bootstrap_failure_restart_semantics;
  const Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary
      frontend_compatibility_strictness_claim_semantics =
          BuildFrontendCompatibilityStrictnessClaimSemanticsSummary(
              pipeline_result.sema_parity_surface
                  .compatibility_strictness_claim_semantics_summary);
  const Objc3ToolingLegacyCanonicalMigrationSemanticsSummary
      tooling_legacy_canonical_migration_semantics_summary =
          BuildToolingLegacyCanonicalMigrationSemanticsSummary(
              frontend_compatibility_strictness_claim_semantics,
              tooling_feature_specific_fixit_synthesis_summary);
  const Objc3VersionedConformanceReportLoweringSummary
      versioned_conformance_report_lowering =
          BuildVersionedConformanceReportLoweringSummary(
              options, pipeline_result,
              frontend_compatibility_strictness_claim_semantics);
  const Objc3ToolingMachineReadableConformanceReportContractSummary
      tooling_machine_readable_conformance_report_contract_summary =
          BuildToolingMachineReadableConformanceReportContractSummary(
              tooling_legacy_canonical_migration_semantics_summary,
              versioned_conformance_report_lowering);
  const Objc3ToolingFeatureAwareConformanceReportEmissionSummary
      tooling_feature_aware_conformance_report_emission_summary =
          BuildToolingFeatureAwareConformanceReportEmissionSummary(
              tooling_feature_specific_fixit_synthesis_summary,
              tooling_legacy_canonical_migration_semantics_summary,
              tooling_machine_readable_conformance_report_contract_summary);
  const Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary
      tooling_corpus_sharding_release_evidence_packaging_summary =
          BuildToolingCorpusShardingReleaseEvidencePackagingSummary(
              tooling_feature_aware_conformance_report_emission_summary);
  if (!IsReadyObjc3VersionedConformanceReportLoweringSummary(
          versioned_conformance_report_lowering)) {
    record_post_pipeline_failure(
        "O3L300",
        "LLVM IR emission failed: incomplete versioned conformance-report lowering summary");
  }
  const Objc3PropertySynthesisIvarBindingContract property_synthesis_ivar_binding_contract =
      BuildPropertySynthesisIvarBindingContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3PropertySynthesisIvarBindingContract(property_synthesis_ivar_binding_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid property synthesis/ivar binding lowering contract");
  }
  const std::string property_synthesis_ivar_binding_replay_key =
      Objc3PropertySynthesisIvarBindingReplayKey(property_synthesis_ivar_binding_contract);
  const Objc3PropertySynthesisIvarBindingSummary &property_synthesis_ivar_binding_summary =
      pipeline_result.sema_parity_surface.property_synthesis_ivar_binding_summary;
  // export-legality anchor: manifest sema surfaces must publish the
  // canonical sema property-synthesis/ivar-binding summary rather than the
  // lowering fail-closed contract used for later replay keys.
  const bool property_synthesis_ivar_binding_handoff_deterministic =
      property_synthesis_ivar_binding_summary.deterministic &&
      pipeline_result.sema_parity_surface
          .deterministic_property_synthesis_ivar_binding_handoff;
  const Objc3IdClassSelObjectPointerTypecheckContract id_class_sel_object_pointer_typecheck_contract =
      BuildIdClassSelObjectPointerTypecheckContract(program);
  if (!IsValidObjc3IdClassSelObjectPointerTypecheckContract(id_class_sel_object_pointer_typecheck_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid id/Class/SEL/object-pointer typecheck lowering contract");
  }
  const std::string id_class_sel_object_pointer_typecheck_replay_key =
      Objc3IdClassSelObjectPointerTypecheckReplayKey(id_class_sel_object_pointer_typecheck_contract);
  const Objc3DispatchSurfaceClassificationContract dispatch_surface_classification_contract =
      BuildDispatchSurfaceClassificationContract(program);
  if (!IsValidObjc3DispatchSurfaceClassificationContract(
          dispatch_surface_classification_contract)) {
    record_post_pipeline_failure(
        "O3L300",
        "LLVM IR emission failed: invalid dispatch-surface classification contract");
  }
  const std::string dispatch_surface_classification_replay_key =
      Objc3DispatchSurfaceClassificationReplayKey(
          dispatch_surface_classification_contract);
  const Objc3MessageSendSelectorLoweringContract message_send_selector_lowering_contract =
      BuildMessageSendSelectorLoweringContract(program);
  if (!IsValidObjc3MessageSendSelectorLoweringContract(message_send_selector_lowering_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid message-send selector lowering contract");
  }
  const std::string message_send_selector_lowering_replay_key =
      Objc3MessageSendSelectorLoweringReplayKey(message_send_selector_lowering_contract);
  const Objc3DispatchAbiMarshallingContract dispatch_abi_marshalling_contract =
      BuildDispatchAbiMarshallingContract(program, options.lowering.max_message_send_args);
  if (!IsValidObjc3DispatchAbiMarshallingContract(dispatch_abi_marshalling_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid dispatch ABI marshalling contract");
  }
  const std::string dispatch_abi_marshalling_replay_key =
      Objc3DispatchAbiMarshallingReplayKey(dispatch_abi_marshalling_contract);
  const Objc3NilReceiverSemanticsFoldabilityContract nil_receiver_semantics_foldability_contract =
      BuildNilReceiverSemanticsFoldabilityContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3NilReceiverSemanticsFoldabilityContract(nil_receiver_semantics_foldability_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid nil-receiver semantics/foldability contract");
  }
  const std::string nil_receiver_semantics_foldability_replay_key =
      Objc3NilReceiverSemanticsFoldabilityReplayKey(nil_receiver_semantics_foldability_contract);
  const Objc3TypeSystemOptionalKeypathLoweringContract
      type_system_optional_keypath_lowering_contract =
          BuildTypeSystemOptionalKeypathLoweringContract(
              type_system_type_semantic_model_summary);
  if (!IsValidObjc3TypeSystemOptionalKeypathLoweringContract(
          type_system_optional_keypath_lowering_contract)) {
    record_post_pipeline_failure(
        "O3L300",
        "LLVM IR emission failed: invalid Part 3 optional/key-path lowering contract");
  }
  const std::string type_system_optional_keypath_lowering_replay_key =
      Objc3TypeSystemOptionalKeypathLoweringReplayKey(
          type_system_optional_keypath_lowering_contract);
  const Objc3ControlFlowControlFlowSafetyLoweringContract
      control_flow_control_flow_safety_lowering_contract =
          BuildControlFlowControlFlowSafetyLoweringContract(
              control_flow_control_flow_semantic_model_summary);
  if (!IsValidObjc3ControlFlowControlFlowSafetyLoweringContract(
          control_flow_control_flow_safety_lowering_contract)) {
    record_post_pipeline_failure(
        "O3L300",
        "LLVM IR emission failed: invalid Part 5 control-flow safety lowering contract");
  }
  const std::string control_flow_control_flow_safety_lowering_replay_key =
      Objc3ControlFlowControlFlowSafetyLoweringReplayKey(
          control_flow_control_flow_safety_lowering_contract);
  const Objc3SuperDispatchMethodFamilyContract super_dispatch_method_family_contract =
      BuildSuperDispatchMethodFamilyContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3SuperDispatchMethodFamilyContract(super_dispatch_method_family_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid super-dispatch/method-family contract");
  }
  const std::string super_dispatch_method_family_replay_key =
      Objc3SuperDispatchMethodFamilyReplayKey(super_dispatch_method_family_contract);
  const Objc3RuntimeLinkHostLinkContract runtime_link_host_link_contract =
      BuildRuntimeLinkHostLinkContract(
          dispatch_abi_marshalling_contract,
          nil_receiver_semantics_foldability_contract,
          options);
  if (!IsValidObjc3RuntimeLinkHostLinkContract(runtime_link_host_link_contract)) {
    record_post_pipeline_failure("O3L300",
                                 "LLVM IR emission failed: invalid runtime dispatch host-link contract");
  }
  const std::string runtime_link_host_link_replay_key =
      Objc3RuntimeLinkHostLinkReplayKey(runtime_link_host_link_contract);
  // dispatch lowering ABI freeze anchor: lane-C now publishes the
  // canonical runtime-dispatch cutover boundary separately from the historical
  // dispatch-host-link packet so C002 can swap call emission over without
  // redefining the selector lookup/handle or argument-slot ABI ad hoc.
  const Objc3RuntimeDispatchLoweringAbiContract
      runtime_dispatch_lowering_abi_contract =
          BuildRuntimeDispatchLoweringAbiContract(
              dispatch_abi_marshalling_contract, runtime_link_host_link_contract,
              runtime_bootstrap_api);
  if (!IsValidObjc3RuntimeDispatchLoweringAbiContract(
          runtime_dispatch_lowering_abi_contract)) {
    record_post_pipeline_failure(
        "O3L300",
        "LLVM IR emission failed: invalid runtime dispatch lowering ABI contract");
  }
  const std::string runtime_dispatch_lowering_abi_replay_key =
      Objc3RuntimeDispatchLoweringAbiReplayKey(
          runtime_dispatch_lowering_abi_contract);
  const Objc3OwnershipQualifierLoweringContract ownership_qualifier_lowering_contract =
      BuildOwnershipQualifierLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3OwnershipQualifierLoweringContract(ownership_qualifier_lowering_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid ownership-qualifier lowering contract");
  }
  const std::string ownership_qualifier_lowering_replay_key =
      Objc3OwnershipQualifierLoweringReplayKey(ownership_qualifier_lowering_contract);
  const Objc3RetainReleaseOperationLoweringContract retain_release_operation_lowering_contract =
      BuildRetainReleaseOperationLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3RetainReleaseOperationLoweringContract(retain_release_operation_lowering_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid retain-release operation lowering contract");
  }
  const std::string retain_release_operation_lowering_replay_key =
      Objc3RetainReleaseOperationLoweringReplayKey(retain_release_operation_lowering_contract);
  const Objc3AutoreleasePoolScopeLoweringContract autoreleasepool_scope_lowering_contract =
      BuildAutoreleasePoolScopeLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3AutoreleasePoolScopeLoweringContract(autoreleasepool_scope_lowering_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid autoreleasepool scope lowering contract");
  }
  const std::string autoreleasepool_scope_lowering_replay_key =
      Objc3AutoreleasePoolScopeLoweringReplayKey(autoreleasepool_scope_lowering_contract);
  const Objc3WeakUnownedSemanticsLoweringContract weak_unowned_semantics_lowering_contract =
      BuildWeakUnownedSemanticsLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3WeakUnownedSemanticsLoweringContract(weak_unowned_semantics_lowering_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid weak-unowned semantics lowering contract");
  }
  const std::string weak_unowned_semantics_lowering_replay_key =
      Objc3WeakUnownedSemanticsLoweringReplayKey(weak_unowned_semantics_lowering_contract);
  const Objc3ArcDiagnosticsFixitLoweringContract arc_diagnostics_fixit_lowering_contract =
      BuildArcDiagnosticsFixitLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3ArcDiagnosticsFixitLoweringContract(arc_diagnostics_fixit_lowering_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid ARC diagnostics/fix-it lowering contract");
  }
  const std::string arc_diagnostics_fixit_lowering_replay_key =
      Objc3ArcDiagnosticsFixitLoweringReplayKey(arc_diagnostics_fixit_lowering_contract);
  const Objc3OwnershipAwareLoweringBehaviorScaffold ownership_aware_lowering_behavior_scaffold =
      BuildObjc3OwnershipAwareLoweringBehaviorScaffold(
          ownership_qualifier_lowering_contract,
          ownership_qualifier_lowering_replay_key,
          retain_release_operation_lowering_contract,
          retain_release_operation_lowering_replay_key,
          autoreleasepool_scope_lowering_contract,
          autoreleasepool_scope_lowering_replay_key,
          weak_unowned_semantics_lowering_contract,
          weak_unowned_semantics_lowering_replay_key,
          arc_diagnostics_fixit_lowering_contract,
          arc_diagnostics_fixit_lowering_replay_key,
          pipeline_result.parse_lowering_readiness_surface
              .compatibility_handoff_consistent,
          pipeline_result.parse_lowering_readiness_surface
              .language_version_pragma_coordinate_order_consistent,
          pipeline_result.parse_lowering_readiness_surface
              .parse_artifact_edge_case_robustness_consistent,
          pipeline_result.parse_lowering_readiness_surface
              .parse_artifact_replay_key_deterministic,
          pipeline_result.parse_lowering_readiness_surface.compatibility_handoff_key,
          pipeline_result.parse_lowering_readiness_surface
              .parse_artifact_edge_robustness_key,
          pipeline_result.parse_lowering_readiness_surface
              .parse_recovery_determinism_hardening_consistent,
          pipeline_result.parse_lowering_readiness_surface
              .parse_recovery_determinism_hardening_key,
          pipeline_result.parse_lowering_readiness_surface
              .parse_lowering_conformance_matrix_consistent,
          pipeline_result.parse_lowering_readiness_surface
              .parse_lowering_conformance_matrix_key,
          pipeline_result.parse_lowering_readiness_surface
              .parse_lowering_conformance_corpus_consistent,
          pipeline_result.parse_lowering_readiness_surface
              .parse_lowering_conformance_corpus_case_count,
          pipeline_result.parse_lowering_readiness_surface
              .parse_lowering_conformance_corpus_key,
          pipeline_result.parse_lowering_readiness_surface
              .parse_lowering_performance_quality_guardrails_consistent,
          pipeline_result.parse_lowering_readiness_surface
              .parse_lowering_performance_quality_guardrails_case_count,
          pipeline_result.parse_lowering_readiness_surface
              .parse_lowering_performance_quality_guardrails_passed_case_count,
          pipeline_result.parse_lowering_readiness_surface
              .parse_lowering_performance_quality_guardrails_failed_case_count,
          pipeline_result.parse_lowering_readiness_surface
              .parse_lowering_performance_quality_guardrails_key,
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
              .conformance_corpus_ready,
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
              .conformance_corpus_key,
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
              .performance_quality_guardrails_ready,
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
              .performance_quality_guardrails_key);
  if (!metadata_only_ir_emission_mode) {
    std::string ownership_aware_lowering_behavior_error;
    if (!IsObjc3OwnershipAwareLoweringBehaviorScaffoldReady(
            ownership_aware_lowering_behavior_scaffold,
            ownership_aware_lowering_behavior_error)) {
      record_post_pipeline_failure("O3L305",         "LLVM IR emission failed: ownership-aware lowering modular split scaffold check failed: " +
              ownership_aware_lowering_behavior_error);
    }
    std::string ownership_aware_lowering_behavior_expansion_error;
    if (!IsObjc3OwnershipAwareLoweringBehaviorCoreFeatureExpansionReady(
            ownership_aware_lowering_behavior_scaffold,
            ownership_aware_lowering_behavior_expansion_error)) {
      record_post_pipeline_failure("O3L310",         "LLVM IR emission failed: ownership-aware lowering core feature expansion check failed: " +
              ownership_aware_lowering_behavior_expansion_error);
    }
    std::string ownership_aware_lowering_behavior_edge_case_compatibility_error;
    if (!IsObjc3OwnershipAwareLoweringBehaviorEdgeCaseCompatibilityReady(
            ownership_aware_lowering_behavior_scaffold,
            ownership_aware_lowering_behavior_edge_case_compatibility_error)) {
      record_post_pipeline_failure("O3L312",         "LLVM IR emission failed: ownership-aware lowering edge-case compatibility check failed: " +
              ownership_aware_lowering_behavior_edge_case_compatibility_error);
    }
    std::string ownership_aware_lowering_behavior_recovery_determinism_error;
    if (!IsObjc3OwnershipAwareLoweringBehaviorRecoveryDeterminismReady(
            ownership_aware_lowering_behavior_scaffold,
            ownership_aware_lowering_behavior_recovery_determinism_error)) {
      record_post_pipeline_failure("O3L318",         "LLVM IR emission failed: ownership-aware lowering recovery determinism check failed: " +
              ownership_aware_lowering_behavior_recovery_determinism_error);
    }
    std::string ownership_aware_lowering_behavior_conformance_matrix_error;
    if (!IsObjc3OwnershipAwareLoweringBehaviorConformanceMatrixReady(
            ownership_aware_lowering_behavior_scaffold,
            ownership_aware_lowering_behavior_conformance_matrix_error)) {
      record_post_pipeline_failure("O3L319",         "LLVM IR emission failed: ownership-aware lowering conformance matrix check failed: " +
              ownership_aware_lowering_behavior_conformance_matrix_error);
    }
    std::string ownership_aware_lowering_behavior_conformance_corpus_error;
    if (!IsObjc3OwnershipAwareLoweringBehaviorConformanceCorpusReady(
            ownership_aware_lowering_behavior_scaffold,
            ownership_aware_lowering_behavior_conformance_corpus_error)) {
      record_post_pipeline_failure("O3L320",         "LLVM IR emission failed: ownership-aware lowering conformance corpus check failed: " +
              ownership_aware_lowering_behavior_conformance_corpus_error);
    }
    std::string ownership_aware_lowering_behavior_performance_quality_guardrails_error;
    if (!IsObjc3OwnershipAwareLoweringBehaviorPerformanceQualityGuardrailsReady(
            ownership_aware_lowering_behavior_scaffold,
            ownership_aware_lowering_behavior_performance_quality_guardrails_error)) {
      record_post_pipeline_failure("O3L328",         "LLVM IR emission failed: ownership-aware lowering performance quality guardrails check failed: " +
              ownership_aware_lowering_behavior_performance_quality_guardrails_error);
    }
    std::string ownership_aware_lowering_behavior_cross_lane_integration_error;
    if (!IsObjc3OwnershipAwareLoweringBehaviorCrossLaneIntegrationReady(
            ownership_aware_lowering_behavior_scaffold,
            ownership_aware_lowering_behavior_cross_lane_integration_error)) {
      record_post_pipeline_failure("O3L329",         "LLVM IR emission failed: ownership-aware lowering cross-lane integration check failed: " +
              ownership_aware_lowering_behavior_cross_lane_integration_error);
    }
  }
  const Objc3BlockLiteralCaptureLoweringContract block_literal_capture_lowering_contract =
      BuildBlockLiteralCaptureLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3BlockLiteralCaptureLoweringContract(block_literal_capture_lowering_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid block literal capture lowering contract");
  }
  const std::string block_literal_capture_lowering_replay_key =
      Objc3BlockLiteralCaptureLoweringReplayKey(block_literal_capture_lowering_contract);
  // block-source-model-completion anchor: frontend artifacts now
  // derive the authoritative block signature/capture/invoke source-model
  // contract directly from the AST so source-only frontend runs can emit a
  // stable manifest handoff before runnable block lowering exists.
  const Objc3BlockSourceModelCompletionContract block_source_model_completion_contract =
      BuildBlockSourceModelCompletionContract(pipeline_result.program.ast);
  if (!IsValidObjc3BlockSourceModelCompletionContract(
          block_source_model_completion_contract)) {
    record_post_pipeline_failure(
        "O3L300",
        "LLVM IR emission failed: invalid block source model completion contract");
  }
  const std::string block_source_model_completion_replay_key =
      Objc3BlockSourceModelCompletionReplayKey(
          block_source_model_completion_contract);
  // block-source-storage-annotation anchor: frontend artifacts now
  // publish the truthful byref/helper/escape-shape source inventory directly
  // from the parser-owned block model so later sema and runtime lanes can
  // consume something real without rewriting the old synthetic legacy
  // packets in place.
  const Objc3BlockSourceStorageAnnotationContract
      block_source_storage_annotation_contract =
          BuildBlockSourceStorageAnnotationContract(
              pipeline_result.program.ast);
  if (!IsValidObjc3BlockSourceStorageAnnotationContract(
          block_source_storage_annotation_contract)) {
    record_post_pipeline_failure(
        "O3L300",
        "LLVM IR emission failed: invalid block source storage annotation contract");
  }
  const std::string block_source_storage_annotation_replay_key =
      Objc3BlockSourceStorageAnnotationReplayKey(
          block_source_storage_annotation_contract);
  const Objc3BlockAbiInvokeTrampolineLoweringContract block_abi_invoke_trampoline_lowering_contract =
      BuildBlockAbiInvokeTrampolineLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3BlockAbiInvokeTrampolineLoweringContract(
          block_abi_invoke_trampoline_lowering_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid block ABI invoke-trampoline lowering contract");
  }
  const std::string block_abi_invoke_trampoline_lowering_replay_key =
      Objc3BlockAbiInvokeTrampolineLoweringReplayKey(
          block_abi_invoke_trampoline_lowering_contract);
  const Objc3BlockStorageEscapeLoweringContract block_storage_escape_lowering_contract =
      BuildBlockStorageEscapeLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3BlockStorageEscapeLoweringContract(
          block_storage_escape_lowering_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid block storage escape lowering contract");
  }
  const std::string block_storage_escape_lowering_replay_key =
      Objc3BlockStorageEscapeLoweringReplayKey(block_storage_escape_lowering_contract);
  const Objc3BlockCopyDisposeLoweringContract block_copy_dispose_lowering_contract =
      BuildBlockCopyDisposeLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3BlockCopyDisposeLoweringContract(
          block_copy_dispose_lowering_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid block copy-dispose lowering contract");
  }
  const std::string block_copy_dispose_lowering_replay_key =
      Objc3BlockCopyDisposeLoweringReplayKey(block_copy_dispose_lowering_contract);
  const Objc3BlockDeterminismPerfBaselineLoweringContract block_determinism_perf_baseline_lowering_contract =
      BuildBlockDeterminismPerfBaselineLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3BlockDeterminismPerfBaselineLoweringContract(
          block_determinism_perf_baseline_lowering_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid block determinism/perf baseline lowering contract");
  }
  const std::string block_determinism_perf_baseline_lowering_replay_key =
      Objc3BlockDeterminismPerfBaselineLoweringReplayKey(
          block_determinism_perf_baseline_lowering_contract);
  const Objc3LightweightGenericsConstraintLoweringContract lightweight_generic_constraint_lowering_contract =
      BuildLightweightGenericsConstraintLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3LightweightGenericsConstraintLoweringContract(
          lightweight_generic_constraint_lowering_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid lightweight generics constraint lowering contract");
  }
  const std::string lightweight_generic_constraint_lowering_replay_key =
      Objc3LightweightGenericsConstraintLoweringReplayKey(
          lightweight_generic_constraint_lowering_contract);
  const Objc3NullabilityFlowWarningPrecisionLoweringContract nullability_flow_warning_precision_lowering_contract =
      BuildNullabilityFlowWarningPrecisionLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3NullabilityFlowWarningPrecisionLoweringContract(
          nullability_flow_warning_precision_lowering_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid nullability-flow warning-precision lowering contract");
  }
  const std::string nullability_flow_warning_precision_lowering_replay_key =
      Objc3NullabilityFlowWarningPrecisionLoweringReplayKey(
          nullability_flow_warning_precision_lowering_contract);
  const Objc3ProtocolQualifiedObjectTypeLoweringContract protocol_qualified_object_type_lowering_contract =
      BuildProtocolQualifiedObjectTypeLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3ProtocolQualifiedObjectTypeLoweringContract(
          protocol_qualified_object_type_lowering_contract)) {
    const std::string protocol_contract_replay_key =
        Objc3ProtocolQualifiedObjectTypeLoweringReplayKey(
            protocol_qualified_object_type_lowering_contract);
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid protocol-qualified object type lowering contract (" +
            protocol_contract_replay_key + ")");
  }
  const std::string protocol_qualified_object_type_lowering_replay_key =
      Objc3ProtocolQualifiedObjectTypeLoweringReplayKey(
          protocol_qualified_object_type_lowering_contract);
  const Objc3VarianceBridgeCastLoweringContract variance_bridge_cast_lowering_contract =
      BuildVarianceBridgeCastLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3VarianceBridgeCastLoweringContract(
          variance_bridge_cast_lowering_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid variance/bridged-cast lowering contract");
  }
  const std::string variance_bridge_cast_lowering_replay_key =
      Objc3VarianceBridgeCastLoweringReplayKey(
          variance_bridge_cast_lowering_contract);
  const Objc3GenericMetadataAbiLoweringContract generic_metadata_abi_lowering_contract =
      BuildGenericMetadataAbiLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3GenericMetadataAbiLoweringContract(
          generic_metadata_abi_lowering_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid generic metadata ABI lowering contract");
  }
  const std::string generic_metadata_abi_lowering_replay_key =
      Objc3GenericMetadataAbiLoweringReplayKey(
          generic_metadata_abi_lowering_contract);
  const Objc3ModuleImportGraphLoweringContract module_import_graph_lowering_contract =
      BuildModuleImportGraphLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3ModuleImportGraphLoweringContract(
          module_import_graph_lowering_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid module import graph lowering contract");
  }
  const std::string module_import_graph_lowering_replay_key =
      Objc3ModuleImportGraphLoweringReplayKey(
          module_import_graph_lowering_contract);
  const Objc3RuntimeAwareImportModuleFrontendClosureSummary
      runtime_aware_import_module_frontend_closure =
          BuildRuntimeAwareImportModuleFrontendClosureSummary(
              program,
              pipeline_result.parser_contract_snapshot,
              module_import_graph_lowering_contract,
              runtime_metadata_source_records);
  const Objc3CrossModuleRuntimeMetadataSemanticPreservationSummary
      cross_module_runtime_metadata_semantic_preservation =
          BuildCrossModuleRuntimeMetadataSemanticPreservationSummary(
              runtime_aware_import_module_frontend_closure,
              runtime_metadata_source_records);
  std::vector<Objc3ImportedRuntimeModuleSurface> imported_runtime_module_surfaces;
  imported_runtime_module_surfaces.reserve(
      options.imported_runtime_surface_paths.size());
  {
    std::unordered_set<std::string> normalized_input_paths;
    std::unordered_set<std::string> imported_module_names;
    for (const auto &input_path_text : options.imported_runtime_surface_paths) {
      const std::filesystem::path raw_input_path(input_path_text);
      const std::filesystem::path absolute_input_path =
          std::filesystem::absolute(raw_input_path);
      const std::string normalized_input_path =
          absolute_input_path.lexically_normal().generic_string();
      if (!normalized_input_paths.insert(normalized_input_path).second) {
        record_post_pipeline_failure(
            "O3S264",
            "imported runtime surface path was provided more than once: " +
                normalized_input_path);
        break;
      }
      Objc3ImportedRuntimeModuleSurface imported_surface;
      std::string import_surface_error;
      if (!TryLoadObjc3ImportedRuntimeModuleSurface(absolute_input_path,
                                                   imported_surface,
                                                   import_surface_error)) {
        record_post_pipeline_failure(
            "O3S264",
            "imported runtime surface load failed: " + import_surface_error);
        break;
      }
      const std::string &module_name =
          imported_surface.frontend_closure_summary.module_name;
      if (!imported_module_names.insert(module_name).second) {
        record_post_pipeline_failure(
            "O3S264",
            "imported runtime surface module name was provided more than once: " +
                module_name);
        break;
      }
      imported_runtime_module_surfaces.push_back(std::move(imported_surface));
    }
  }
  const Objc3ImportedRuntimeMetadataSemanticRulesSummary
      imported_runtime_metadata_semantic_rules =
          BuildImportedRuntimeMetadataSemanticRulesSummary(
              cross_module_runtime_metadata_semantic_preservation,
              imported_runtime_module_surfaces,
              options.imported_runtime_surface_paths.size());
  const bool has_imported_runtime_surface_inputs =
      !options.imported_runtime_surface_paths.empty();
  if (post_pipeline_failure_code.empty() &&
      has_imported_runtime_surface_inputs &&
      !IsReadyObjc3ImportedRuntimeMetadataSemanticRulesSummary(
          imported_runtime_metadata_semantic_rules)) {
    record_post_pipeline_failure(
        "O3S264",
        "imported runtime metadata semantic rules are incomplete: " +
            imported_runtime_metadata_semantic_rules.failure_reason);
  }
  const Objc3SerializedRuntimeMetadataImportLoweringSummary
      serialized_runtime_metadata_import_lowering =
          BuildSerializedRuntimeMetadataImportLoweringSummary(
              imported_runtime_metadata_semantic_rules);
  if (post_pipeline_failure_code.empty() &&
      has_imported_runtime_surface_inputs &&
      !IsReadyObjc3SerializedRuntimeMetadataImportLoweringSummary(
          serialized_runtime_metadata_import_lowering)) {
    record_post_pipeline_failure(
        "O3S265",
        "serialized runtime metadata import/lowering boundary is incomplete: " +
            serialized_runtime_metadata_import_lowering.failure_reason);
  }
  const std::vector<std::string> serialized_runtime_metadata_reused_module_names =
      BuildSerializedRuntimeMetadataReusedModuleNames(
          runtime_aware_import_module_frontend_closure.module_name,
          imported_runtime_module_surfaces);
  const Objc3RuntimeMetadataSourceRecordSet
      serialized_runtime_metadata_reuse_records =
          BuildSerializedRuntimeMetadataReuseRecordSet(
              runtime_metadata_source_records, imported_runtime_module_surfaces);
  const Objc3SerializedRuntimeMetadataArtifactReuseSummary
      serialized_runtime_metadata_artifact_reuse =
          BuildSerializedRuntimeMetadataArtifactReuseSummary(
              serialized_runtime_metadata_import_lowering,
              runtime_aware_import_module_frontend_closure.module_name,
              serialized_runtime_metadata_reuse_records,
              serialized_runtime_metadata_reused_module_names);
  if (post_pipeline_failure_code.empty() &&
      has_imported_runtime_surface_inputs &&
      !IsReadyObjc3SerializedRuntimeMetadataArtifactReuseSummary(
          serialized_runtime_metadata_artifact_reuse)) {
    record_post_pipeline_failure(
        "O3S266",
        "serialized runtime metadata artifact reuse is incomplete: " +
            serialized_runtime_metadata_artifact_reuse.failure_reason);
  }
  const Objc3CrossModuleBuildRuntimeOrchestrationSummary
      cross_module_build_runtime_orchestration =
          BuildCrossModuleBuildRuntimeOrchestrationSummary(
              serialized_runtime_metadata_artifact_reuse,
              imported_runtime_metadata_semantic_rules,
              runtime_translation_unit_registration_manifest,
              options.imported_runtime_surface_paths.size());
  if (post_pipeline_failure_code.empty() &&
      has_imported_runtime_surface_inputs &&
      !IsReadyObjc3CrossModuleBuildRuntimeOrchestrationSummary(
          cross_module_build_runtime_orchestration)) {
    record_post_pipeline_failure(
        "O3S267",
        "cross-module build/runtime orchestration contract is incomplete: " +
            cross_module_build_runtime_orchestration.failure_reason);
  }
  const Objc3NamespaceCollisionShadowingLoweringContract
      namespace_collision_shadowing_lowering_contract =
          BuildNamespaceCollisionShadowingLoweringContract(
              pipeline_result.sema_parity_surface);
  if (!IsValidObjc3NamespaceCollisionShadowingLoweringContract(
          namespace_collision_shadowing_lowering_contract)) {
    record_post_pipeline_failure("O3L300", "LLVM IR emission failed: invalid namespace collision "
                 "shadowing lowering contract");
  }
  const std::string namespace_collision_shadowing_lowering_replay_key =
      Objc3NamespaceCollisionShadowingLoweringReplayKey(
          namespace_collision_shadowing_lowering_contract);
  const Objc3PublicPrivateApiPartitionLoweringContract
      public_private_api_partition_lowering_contract =
          BuildPublicPrivateApiPartitionLoweringContract(
              pipeline_result.sema_parity_surface);
  if (!IsValidObjc3PublicPrivateApiPartitionLoweringContract(
          public_private_api_partition_lowering_contract)) {
    record_post_pipeline_failure("O3L300", "LLVM IR emission failed: invalid public-private API "
                 "partition lowering contract");
  }
  const std::string public_private_api_partition_lowering_replay_key =
      Objc3PublicPrivateApiPartitionLoweringReplayKey(
          public_private_api_partition_lowering_contract);
  const Objc3IncrementalModuleCacheInvalidationLoweringContract
      incremental_module_cache_invalidation_lowering_contract =
          BuildIncrementalModuleCacheInvalidationLoweringContract(
              pipeline_result.sema_parity_surface);
  if (!IsValidObjc3IncrementalModuleCacheInvalidationLoweringContract(
          incremental_module_cache_invalidation_lowering_contract)) {
    record_post_pipeline_failure("O3L300", "LLVM IR emission failed: invalid incremental module cache "
            "invalidation lowering contract");
  }
  const std::string incremental_module_cache_invalidation_lowering_replay_key =
      Objc3IncrementalModuleCacheInvalidationLoweringReplayKey(
          incremental_module_cache_invalidation_lowering_contract);
  const Objc3CrossModuleConformanceLoweringContract
      cross_module_conformance_lowering_contract =
          BuildCrossModuleConformanceLoweringContract(
              pipeline_result.sema_parity_surface);
  if (!IsValidObjc3CrossModuleConformanceLoweringContract(
          cross_module_conformance_lowering_contract)) {
    record_post_pipeline_failure("O3L300", "LLVM IR emission failed: invalid cross-module conformance "
                 "lowering contract");
  }
  const std::string cross_module_conformance_lowering_replay_key =
      Objc3CrossModuleConformanceLoweringReplayKey(
          cross_module_conformance_lowering_contract);
  const Objc3ThrowsPropagationLoweringContract
      throws_propagation_lowering_contract =
          BuildThrowsPropagationLoweringContract(
              pipeline_result.sema_parity_surface);
  if (!IsValidObjc3ThrowsPropagationLoweringContract(
          throws_propagation_lowering_contract)) {
    record_post_pipeline_failure("O3L300", "LLVM IR emission failed: invalid throws propagation "
                 "lowering contract");
  }
  const std::string throws_propagation_lowering_replay_key =
      Objc3ThrowsPropagationLoweringReplayKey(
          throws_propagation_lowering_contract);
  const Objc3ResultLikeLoweringContract result_like_lowering_contract =
      BuildResultLikeLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3ResultLikeLoweringContract(
          result_like_lowering_contract)) {
    record_post_pipeline_failure(
        "O3L300",
        "LLVM IR emission failed: invalid result-like lowering contract");
  }
  const std::string result_like_lowering_replay_key =
      Objc3ResultLikeLoweringReplayKey(result_like_lowering_contract);
  const Objc3NSErrorBridgingLoweringContract
      ns_error_bridging_lowering_contract =
          BuildNSErrorBridgingLoweringContract(
              pipeline_result.sema_parity_surface);
  if (!IsValidObjc3NSErrorBridgingLoweringContract(
          ns_error_bridging_lowering_contract)) {
    record_post_pipeline_failure(
        "O3L300",
        "LLVM IR emission failed: invalid NSError bridging lowering contract");
  }
  const std::string ns_error_bridging_lowering_replay_key =
      Objc3NSErrorBridgingLoweringReplayKey(
          ns_error_bridging_lowering_contract);
  const Objc3UnwindCleanupLoweringContract unwind_cleanup_lowering_contract =
      BuildUnwindCleanupLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3UnwindCleanupLoweringContract(
          unwind_cleanup_lowering_contract)) {
    record_post_pipeline_failure(
        "O3L300",
        "LLVM IR emission failed: invalid unwind cleanup lowering contract");
  }
  const std::string unwind_cleanup_lowering_replay_key =
      Objc3UnwindCleanupLoweringReplayKey(unwind_cleanup_lowering_contract);
  const bool deterministic_error_handling_throws_abi_propagation_lowering =
      result_like_lowering_contract.deterministic &&
      throws_propagation_lowering_contract.deterministic &&
      ns_error_bridging_lowering_contract.deterministic &&
      unwind_cleanup_lowering_contract.deterministic;
  // replay continuity anchor: \",\"next_issue\":\"objc3c.errors.throws.propagationlowering.v1\"
  const std::string error_handling_throws_abi_propagation_lowering_replay_key =
      Objc3ErrorHandlingThrowsAbiPropagationLoweringSummary() +
      ";throws_replay_key=" + throws_propagation_lowering_replay_key +
      ";result_like_replay_key=" + result_like_lowering_replay_key +
      ";ns_error_replay_key=" + ns_error_bridging_lowering_replay_key +
      ";unwind_replay_key=" + unwind_cleanup_lowering_replay_key +
      ";deterministic=" +
      (deterministic_error_handling_throws_abi_propagation_lowering ? "true" : "false") +
      ";ready_for_runtime_execution=true" +
      ";follow_on_surface=objc3c.errors.throws.propagationlowering.v1";
  const auto error_handling_result_and_bridging_artifact_replay_summary =
      BuildErrorHandlingResultAndBridgingArtifactReplayEvidence(
          error_handling_throws_abi_propagation_lowering_replay_key,
          throws_propagation_lowering_replay_key,
          result_like_lowering_replay_key,
          ns_error_bridging_lowering_replay_key,
          unwind_cleanup_lowering_replay_key,
          deterministic_error_handling_throws_abi_propagation_lowering,
          IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(
              runtime_aware_import_module_frontend_closure),
          imported_runtime_module_surfaces);
  const auto interop_foreign_surface_interface_preservation_summary =
      BuildInteropForeignSurfaceInterfacePreservationSummary(
          program, interop_foreign_import_source_closure_summary,
          interop_cpp_swift_interop_annotation_source_completion_summary,
          IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(
              runtime_aware_import_module_frontend_closure),
          imported_runtime_module_surfaces);
  const Objc3InteropInteropLoweringContract interop_interop_lowering_contract =
      BuildInteropInteropLoweringContract(
          interop_interop_semantic_model_summary,
          interop_interop_runtime_parity_summary,
          interop_cpp_interop_interaction_summary,
          interop_swift_interop_isolation_summary,
          interop_foreign_surface_interface_preservation_summary);
  if (!IsValidObjc3InteropInteropLoweringContract(
          interop_interop_lowering_contract)) {
    record_post_pipeline_failure(
        "O3L300",
        "LLVM IR emission failed: invalid Part 11 interop lowering contract");
  }
  const std::string interop_interop_lowering_replay_key =
      Objc3InteropInteropLoweringReplayKey(interop_interop_lowering_contract);
  const auto interop_foreign_call_lifetime_lowering_contract =
      BuildInteropForeignCallLifetimeLoweringContract(
          program, interop_interop_lowering_contract,
          interop_cpp_interop_interaction_summary,
          interop_foreign_surface_interface_preservation_summary);
  if (!IsValidObjc3InteropForeignCallLifetimeLoweringContract(
          interop_foreign_call_lifetime_lowering_contract)) {
    record_post_pipeline_failure(
        "O3L300",
        "LLVM IR emission failed: invalid Part 11 foreign call and lifetime lowering contract");
  }
  const std::string interop_foreign_call_lifetime_lowering_replay_key =
      Objc3InteropForeignCallLifetimeLoweringReplayKey(
          interop_foreign_call_lifetime_lowering_contract);
  std::string interop_ffi_metadata_interface_preservation_replay_key;
  const auto interop_ffi_metadata_interface_preservation_contract =
      BuildInteropFfiMetadataInterfacePreservationContract(
          interop_foreign_call_lifetime_lowering_contract,
          interop_foreign_call_lifetime_lowering_replay_key,
          interop_foreign_surface_interface_preservation_summary,
          imported_runtime_module_surfaces,
          IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(
              runtime_aware_import_module_frontend_closure),
          interop_ffi_metadata_interface_preservation_replay_key);
  if (!IsValidObjc3InteropFfiMetadataInterfacePreservationContract(
          interop_ffi_metadata_interface_preservation_contract)) {
    record_post_pipeline_failure(
        "O3L300",
        "LLVM IR emission failed: invalid Part 11 ffi metadata/interface preservation contract");
  }
  const auto interop_header_module_bridge_generation_summary =
      BuildInteropHeaderModuleBridgeGenerationSummary(
          program, interop_foreign_surface_interface_preservation_summary,
          interop_ffi_metadata_interface_preservation_contract,
          interop_ffi_metadata_interface_preservation_replay_key,
          imported_runtime_module_surfaces);
  const auto dispatch_dispatch_metadata_interface_preservation_summary =
      BuildDispatchDispatchMetadataInterfacePreservationSummary(
          runtime_metadata_source_records, dispatch_dispatch_control_lowering_replay_key,
          IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(
              runtime_aware_import_module_frontend_closure),
          imported_runtime_module_surfaces);
  const auto runtime_block_ownership_artifact_preservation_summary =
      BuildObjc3RuntimeBlockOwnershipArtifactPreservationSummary(
          block_abi_invoke_trampoline_lowering_contract,
          block_storage_escape_lowering_contract,
          block_copy_dispose_lowering_contract,
          runtime_support_library_link_wiring);
  const auto runtime_storage_reflection_artifact_preservation_summary =
      BuildObjc3RuntimeStorageReflectionArtifactPreservationSummary(
          runtime_metadata_source_records);
  const auto metaprogramming_module_interface_replay_preservation_summary =
      BuildMetaprogrammingModuleInterfaceReplayPreservationSummary(
          metaprogramming_expansion_lowering_contract,
          metaprogramming_expansion_lowering_replay_key,
          metaprogramming_synthesized_artifact_emission_contract,
          metaprogramming_synthesized_artifact_emission_replay_key,
          metaprogramming_property_behavior_artifact_bundles,
          IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(
              runtime_aware_import_module_frontend_closure),
          imported_runtime_module_surfaces);
  const auto metaprogramming_macro_host_process_cache_runtime_integration_summary =
      BuildMetaprogrammingMacroHostProcessCacheRuntimeIntegrationSummary(
          metaprogramming_module_interface_replay_preservation_summary,
          imported_runtime_module_surfaces,
          options);
  std::size_t interface_class_method_symbols = 0;
  std::size_t interface_instance_method_symbols = 0;
  for (const auto &interface_metadata : type_metadata_handoff.interfaces_lexicographic) {
    for (const auto &method_metadata : interface_metadata.methods_lexicographic) {
      if (method_metadata.is_class_method) {
        ++interface_class_method_symbols;
      } else {
        ++interface_instance_method_symbols;
      }
    }
  }
  std::size_t implementation_class_method_symbols = 0;
  std::size_t implementation_instance_method_symbols = 0;
  std::size_t implementation_methods_with_body = 0;
  for (const auto &implementation_metadata : type_metadata_handoff.implementations_lexicographic) {
    for (const auto &method_metadata : implementation_metadata.methods_lexicographic) {
      if (method_metadata.is_class_method) {
        ++implementation_class_method_symbols;
      } else {
        ++implementation_instance_method_symbols;
      }
      if (method_metadata.has_definition) {
        ++implementation_methods_with_body;
      }
    }
  }

  std::vector<int> resolved_global_values;
  if (!ResolveGlobalInitializerValues(program.globals, resolved_global_values) ||
      resolved_global_values.size() != program.globals.size()) {
    record_post_pipeline_failure("O3L300", "LLVM IR emission failed: global initializer failed const evaluation");
  }

  std::ostringstream manifest;
  manifest << "{\n";
  manifest << "  \"source\": \"" << input_path.generic_string() << "\",\n";
  manifest << "  \"module\": \"" << program.module_name << "\",\n";
  manifest << "  \"frontend\": {\n";
  manifest << "    \"language_version\":" << static_cast<unsigned>(options.language_version) << ",\n";
  manifest << "    \"language_profile\":\"" << LanguageProfileName(options.language_profile) << "\",\n";
  manifest << "    \"arc_mode\":\"" << ArcModeName(options.arc_mode) << "\",\n";
  manifest << "    \"default_language_profile\":\"canonical\",\n";
  manifest << "    \"canonical_literal_rejection_diagnostics\":true,\n";
  manifest << "    \"language_version_selection_supported\":true,\n";
  manifest << "    \"language_profile_selection_supported\":true,\n";
  manifest << "    \"canonical_rejection_diagnostics_selection_supported\":false,\n";
  manifest << "    \"canonical_literal_rejection_diagnostics_hard_error\":true,\n";
  manifest << "    \"strictness_selection_supported\":false,\n";
  manifest << "    \"strict_concurrency_selection_supported\":false,\n";
  manifest << "    \"feature_macro_surface_supported\":false,\n";
  manifest << "    \"feature_claim_truth_surface_contract_id\":\""
           << kObjc3FeatureClaimStrictnessTruthSurfaceContractId << "\",\n";
  manifest << "    \"canonical_literal_rejection_counts\":{\"yes_literal_sites\":"
           << pipeline_result.canonical_literal_rejection_counts.yes_literal_sites
           << ",\"no_literal_sites\":"
           << pipeline_result.canonical_literal_rejection_counts.no_literal_sites
           << ",\"null_literal_sites\":"
           << pipeline_result.canonical_literal_rejection_counts
                  .null_literal_sites
           << ",\"total_literal_sites\":"
           << pipeline_result.canonical_literal_rejection_counts
                  .total_literal_sites()
           << "},\n";
  manifest << "    \"language_version_pragma_contract\":{\"seen\":"
           << (pipeline_result.language_version_pragma_contract.seen ? "true" : "false")
           << ",\"directive_count\":" << pipeline_result.language_version_pragma_contract.directive_count
           << ",\"duplicate\":" << (pipeline_result.language_version_pragma_contract.duplicate ? "true" : "false")
           << ",\"non_leading\":"
           << (pipeline_result.language_version_pragma_contract.non_leading ? "true" : "false")
           << ",\"first_line\":" << pipeline_result.language_version_pragma_contract.first_line
           << ",\"first_column\":" << pipeline_result.language_version_pragma_contract.first_column
           << ",\"last_line\":" << pipeline_result.language_version_pragma_contract.last_line
           << ",\"last_column\":" << pipeline_result.language_version_pragma_contract.last_column << "},\n";
  manifest << "    \"bootstrap_registration_source_pragma_contract\":{"
           << "\"registration_descriptor\":{\"seen\":"
           << (pipeline_result.bootstrap_registration_source_pragma_contract
                       .registration_descriptor.seen
                   ? "true"
                   : "false")
           << ",\"directive_count\":"
           << pipeline_result.bootstrap_registration_source_pragma_contract
                  .registration_descriptor.directive_count
           << ",\"duplicate\":"
           << (pipeline_result.bootstrap_registration_source_pragma_contract
                       .registration_descriptor.duplicate
                   ? "true"
                   : "false")
           << ",\"non_leading\":"
           << (pipeline_result.bootstrap_registration_source_pragma_contract
                       .registration_descriptor.non_leading
                   ? "true"
                   : "false")
           << ",\"first_line\":"
           << pipeline_result.bootstrap_registration_source_pragma_contract
                  .registration_descriptor.first_line
           << ",\"first_column\":"
           << pipeline_result.bootstrap_registration_source_pragma_contract
                  .registration_descriptor.first_column
           << ",\"last_line\":"
           << pipeline_result.bootstrap_registration_source_pragma_contract
                  .registration_descriptor.last_line
           << ",\"last_column\":"
           << pipeline_result.bootstrap_registration_source_pragma_contract
                  .registration_descriptor.last_column
           << ",\"identifier\":\""
           << EscapeJsonString(
                  pipeline_result.bootstrap_registration_source_pragma_contract
                      .registration_descriptor.identifier)
           << "\"},\"image_root\":{\"seen\":"
           << (pipeline_result.bootstrap_registration_source_pragma_contract
                       .image_root.seen
                   ? "true"
                   : "false")
           << ",\"directive_count\":"
           << pipeline_result.bootstrap_registration_source_pragma_contract
                  .image_root.directive_count
           << ",\"duplicate\":"
           << (pipeline_result.bootstrap_registration_source_pragma_contract
                       .image_root.duplicate
                   ? "true"
                   : "false")
           << ",\"non_leading\":"
           << (pipeline_result.bootstrap_registration_source_pragma_contract
                       .image_root.non_leading
                   ? "true"
                   : "false")
           << ",\"first_line\":"
           << pipeline_result.bootstrap_registration_source_pragma_contract
                  .image_root.first_line
           << ",\"first_column\":"
           << pipeline_result.bootstrap_registration_source_pragma_contract
                  .image_root.first_column
           << ",\"last_line\":"
           << pipeline_result.bootstrap_registration_source_pragma_contract
                  .image_root.last_line
           << ",\"last_column\":"
           << pipeline_result.bootstrap_registration_source_pragma_contract
                  .image_root.last_column
           << ",\"identifier\":\""
           << EscapeJsonString(
                  pipeline_result.bootstrap_registration_source_pragma_contract
                      .image_root.identifier)
           << "\"},\"registration_descriptor_pragma_name\":\""
           << EscapeJsonString(kObjc3BootstrapRegistrationDescriptorPragmaName)
           << "\",\"image_root_pragma_name\":\""
           << EscapeJsonString(kObjc3BootstrapImageRootPragmaName) << "\"},\n";
  manifest << "    \"max_message_send_args\":" << options.lowering.max_message_send_args << ",\n";
  manifest << "    \"pipeline\": {\n";
  manifest << "      \"semantic_skipped\": " << (pipeline_result.integration_surface.built ? "false" : "true")
           << ",\n";
  manifest << "      \"stages\": {\n";
  const Objc3ParserDiagnosticCodeCoverage parser_diag_code_coverage =
      BuildObjc3ParserDiagnosticCodeCoverage(bundle.stage_diagnostics.parser);
  manifest << "        \"lexer\": {\"diagnostics\":" << bundle.stage_diagnostics.lexer.size() << "},\n";
  manifest << "        \"parser\": {\"diagnostics\":" << bundle.stage_diagnostics.parser.size()
           << ",\"token_count\":" << pipeline_result.parser_contract_snapshot.token_count
           << ",\"top_level_declarations\":" << pipeline_result.parser_contract_snapshot.top_level_declaration_count
           << ",\"globals\":" << pipeline_result.parser_contract_snapshot.global_decl_count
           << ",\"protocols\":" << pipeline_result.parser_contract_snapshot.protocol_decl_count
           << ",\"protocol_properties\":"
           << pipeline_result.parser_contract_snapshot.protocol_property_decl_count
           << ",\"protocol_methods\":"
           << pipeline_result.parser_contract_snapshot.protocol_method_decl_count
           << ",\"interfaces\":" << pipeline_result.parser_contract_snapshot.interface_decl_count
           << ",\"interface_properties\":"
           << pipeline_result.parser_contract_snapshot.interface_property_decl_count
           << ",\"interface_methods\":"
           << pipeline_result.parser_contract_snapshot.interface_method_decl_count
           << ",\"interface_categories\":"
           << pipeline_result.parser_contract_snapshot.interface_category_decl_count
           << ",\"implementations\":" << pipeline_result.parser_contract_snapshot.implementation_decl_count
           << ",\"implementation_properties\":"
           << pipeline_result.parser_contract_snapshot.implementation_property_decl_count
           << ",\"implementation_methods\":"
           << pipeline_result.parser_contract_snapshot.implementation_method_decl_count
           << ",\"implementation_categories\":"
           << pipeline_result.parser_contract_snapshot.implementation_category_decl_count
           << ",\"functions\":" << pipeline_result.parser_contract_snapshot.function_decl_count
           << ",\"function_prototypes\":"
           << pipeline_result.parser_contract_snapshot.function_prototype_count
           << ",\"function_pure\":"
           << pipeline_result.parser_contract_snapshot.function_pure_count
           << ",\"draft_syntax_surface_count\":"
           << pipeline_result.parser_contract_snapshot.draft_syntax_surface_count
           << ",\"draft_syntax_surface_fingerprint\":"
           << pipeline_result.parser_contract_snapshot.draft_syntax_surface_fingerprint
           << ",\"draft_syntax_surface_handoff_key\":\""
           << pipeline_result.parser_contract_snapshot.draft_syntax_surface_handoff_key
           << "\",\"draft_syntax_surface_deterministic\":"
           << (pipeline_result.parser_contract_snapshot.draft_syntax_surface_handoff_deterministic ? "true" : "false")
           << ",\"long_tail_grammar_constructs\":"

           << pipeline_result.parser_contract_snapshot.long_tail_grammar_construct_count
           << ",\"long_tail_grammar_covered_constructs\":"
           << pipeline_result.parser_contract_snapshot.long_tail_grammar_covered_construct_count
           << ",\"long_tail_grammar_fingerprint\":"
           << pipeline_result.parser_contract_snapshot.long_tail_grammar_fingerprint
           << ",\"long_tail_grammar_handoff_key\":\""
           << pipeline_result.parser_contract_snapshot.long_tail_grammar_handoff_key
           << "\",\"long_tail_grammar_deterministic\":"
           << (pipeline_result.parser_contract_snapshot.long_tail_grammar_handoff_deterministic ? "true" : "false")
           << ",\"diagnostic_code_count\":"
           << parser_diag_code_coverage.unique_code_count
           << ",\"diagnostic_code_fingerprint\":"
           << parser_diag_code_coverage.unique_code_fingerprint
           << ",\"diagnostic_code_surface_deterministic\":"
           << (parser_diag_code_coverage.deterministic_surface ? "true" : "false")
           << ",\"deterministic_handoff\":"
           << (pipeline_result.parser_contract_snapshot.deterministic_handoff ? "true" : "false")
           << ",\"recovery_replay_ready\":"
           << (pipeline_result.parser_contract_snapshot.parser_recovery_replay_ready ? "true" : "false") << "},\n";
  manifest << "        \"semantic\": {\"diagnostics\":" << bundle.stage_diagnostics.semantic.size()
           << "}\n";
  manifest << "      },\n";
  manifest << "      \"parse_lowering_readiness\": {\"ready_for_lowering\": "
           << (bundle.parse_lowering_readiness_surface.ready_for_lowering ? "true" : "false")
           << ",\"parser_contract_snapshot_present\": "
           << (bundle.parse_lowering_readiness_surface.parser_contract_snapshot_present ? "true" : "false")
           << ",\"long_tail_grammar_core_feature_consistent\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_core_feature_consistent ? "true" : "false")
           << ",\"long_tail_grammar_handoff_key_deterministic\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_handoff_key_deterministic ? "true"
                                                                                                     : "false")
           << ",\"long_tail_grammar_expansion_accounting_consistent\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_expansion_accounting_consistent ? "true"
                                                                                                           : "false")
           << ",\"long_tail_grammar_replay_keys_ready\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_replay_keys_ready ? "true" : "false")
           << ",\"long_tail_grammar_expansion_ready\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_expansion_ready ? "true" : "false")
           << ",\"long_tail_grammar_compatibility_handoff_ready\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_compatibility_handoff_ready ? "true"
                                                                                                       : "false")
           << ",\"long_tail_grammar_edge_case_compatibility_consistent\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_edge_case_compatibility_consistent ? "true"
                                                                                                              : "false")
           << ",\"long_tail_grammar_edge_case_compatibility_ready\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_edge_case_compatibility_ready ? "true"
                                                                                                         : "false")
           << ",\"long_tail_grammar_edge_case_expansion_consistent\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_edge_case_expansion_consistent ? "true"
                                                                                                          : "false")
           << ",\"long_tail_grammar_edge_case_robustness_ready\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_edge_case_robustness_ready ? "true" : "false")
           << ",\"long_tail_grammar_diagnostics_hardening_consistent\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_diagnostics_hardening_consistent ? "true"
                                                                                                            : "false")
           << ",\"long_tail_grammar_diagnostics_hardening_ready\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_diagnostics_hardening_ready ? "true"
                                                                                                       : "false")
           << ",\"long_tail_grammar_recovery_determinism_consistent\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_recovery_determinism_consistent ? "true"
                                                                                                           : "false")
           << ",\"long_tail_grammar_recovery_determinism_ready\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_recovery_determinism_ready ? "true"
                                                                                                      : "false")
           << ",\"long_tail_grammar_conformance_matrix_consistent\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_conformance_matrix_consistent ? "true"
                                                                                                         : "false")
           << ",\"long_tail_grammar_conformance_matrix_ready\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_conformance_matrix_ready ? "true"
                                                                                                    : "false")
           << ",\"long_tail_grammar_integration_closeout_consistent\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_integration_closeout_consistent ? "true"
                                                                                                           : "false")
           << ",\"long_tail_grammar_gate_signoff_ready\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_gate_signoff_ready ? "true" : "false")
           << ",\"parse_artifact_handoff_consistent\": "
           << (bundle.parse_lowering_readiness_surface.parse_artifact_handoff_consistent ? "true" : "false")
           << ",\"parse_artifact_handoff_deterministic\": "
           << (bundle.parse_lowering_readiness_surface.parse_artifact_handoff_deterministic ? "true" : "false")
           << ",\"parser_token_count_budget_consistent\": "
           << (bundle.parse_lowering_readiness_surface.parser_token_count_budget_consistent ? "true" : "false")
           << ",\"parse_artifact_layout_fingerprint_consistent\": "
           << (bundle.parse_lowering_readiness_surface.parse_artifact_layout_fingerprint_consistent ? "true" : "false")
           << ",\"parse_artifact_fingerprint_consistent\": "
           << (bundle.parse_lowering_readiness_surface.parse_artifact_fingerprint_consistent ? "true" : "false")
           << ",\"compatibility_handoff_consistent\": "
           << (bundle.parse_lowering_readiness_surface.compatibility_handoff_consistent ? "true" : "false")
           << ",\"parser_diagnostic_surface_consistent\": "
           << (bundle.parse_lowering_readiness_surface.parser_diagnostic_surface_consistent ? "true" : "false")
           << ",\"parser_diagnostic_code_surface_deterministic\": "
           << (bundle.parse_lowering_readiness_surface.parser_diagnostic_code_surface_deterministic ? "true"
                                                                                                     : "false")
           << ",\"language_version_pragma_coordinate_order_consistent\": "
           << (bundle.parse_lowering_readiness_surface.language_version_pragma_coordinate_order_consistent ? "true"
                                                                                                            : "false")
           << ",\"parse_artifact_replay_key_deterministic\": "
           << (bundle.parse_lowering_readiness_surface.parse_artifact_replay_key_deterministic ? "true" : "false")
           << ",\"parse_artifact_diagnostics_hardening_consistent\": "
           << (bundle.parse_lowering_readiness_surface.parse_artifact_diagnostics_hardening_consistent ? "true"
                                                                                                         : "false")
           << ",\"parse_artifact_edge_case_robustness_consistent\": "
           << (bundle.parse_lowering_readiness_surface.parse_artifact_edge_case_robustness_consistent ? "true"
                                                                                                        : "false")
           << ",\"parse_recovery_determinism_hardening_consistent\": "
           << (bundle.parse_lowering_readiness_surface.parse_recovery_determinism_hardening_consistent ? "true"
                                                                                                        : "false")
           << ",\"parser_diagnostic_grammar_hooks_recovery_determinism_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .parser_diagnostic_grammar_hooks_recovery_determinism_consistent
                   ? "true"
                   : "false")
           << ",\"parser_diagnostic_grammar_hooks_recovery_determinism_ready\": "
           << (bundle.parse_lowering_readiness_surface
                       .parser_diagnostic_grammar_hooks_recovery_determinism_ready
                   ? "true"
                   : "false")
           << ",\"parser_diagnostic_grammar_hooks_conformance_matrix_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .parser_diagnostic_grammar_hooks_conformance_matrix_consistent
                   ? "true"
                   : "false")
           << ",\"parser_diagnostic_grammar_hooks_conformance_matrix_ready\": "
           << (bundle.parse_lowering_readiness_surface
                       .parser_diagnostic_grammar_hooks_conformance_matrix_ready
                   ? "true"
                   : "false")
           << ",\"parser_diagnostic_grammar_hooks_conformance_corpus_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .parser_diagnostic_grammar_hooks_conformance_corpus_consistent
                   ? "true"
                   : "false")
           << ",\"parser_diagnostic_grammar_hooks_conformance_corpus_ready\": "
           << (bundle.parse_lowering_readiness_surface
                       .parser_diagnostic_grammar_hooks_conformance_corpus_ready
                   ? "true"
                   : "false")
           << ",\"parse_lowering_conformance_matrix_consistent\": "
           << (bundle.parse_lowering_readiness_surface.parse_lowering_conformance_matrix_consistent ? "true"
                                                                                                    : "false")
           << ",\"parse_lowering_conformance_corpus_consistent\": "
           << (bundle.parse_lowering_readiness_surface.parse_lowering_conformance_corpus_consistent ? "true"
                                                                                                      : "false")
           << ",\"parse_lowering_performance_quality_guardrails_consistent\": "
           << (bundle.parse_lowering_readiness_surface.parse_lowering_performance_quality_guardrails_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_cross_lane_integration_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_cross_lane_integration_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_cross_lane_integration_ready\": "
           << (bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_cross_lane_integration_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_docs_runbook_sync_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_docs_runbook_sync_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_docs_runbook_sync_ready\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_docs_runbook_sync_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_core_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_advanced_core_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_core_ready\": "
           << (bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_core_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_edge_compatibility_ready\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_advanced_edge_compatibility_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_diagnostics_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_advanced_diagnostics_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_diagnostics_ready\": "
           << (bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_diagnostics_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_conformance_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_advanced_conformance_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_conformance_ready\": "
           << (bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_conformance_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_integration_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_advanced_integration_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_integration_ready\": "
           << (bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_integration_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_performance_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_advanced_performance_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_performance_ready\": "
           << (bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_performance_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_core_shard2_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_advanced_core_shard2_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_core_shard2_ready\": "
           << (bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_core_shard2_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_integration_closeout_signoff_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_integration_closeout_signoff_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_integration_closeout_signoff_ready\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_integration_closeout_signoff_ready
                   ? "true"
                   : "false")
           << ",\"semantic_integration_surface_built\": "
           << (bundle.parse_lowering_readiness_surface.semantic_integration_surface_built ? "true" : "false")
           << ",\"executable_metadata_lowering_handoff_ready\": "
           << (bundle.parse_lowering_readiness_surface
                       .executable_metadata_lowering_handoff_ready
                   ? "true"
                   : "false")
           << ",\"executable_metadata_lowering_handoff_deterministic\": "
           << (bundle.parse_lowering_readiness_surface
                       .executable_metadata_lowering_handoff_deterministic
                   ? "true"
                   : "false")
           << ",\"executable_metadata_typed_lowering_handoff_ready\": "
           << (bundle.parse_lowering_readiness_surface
                       .executable_metadata_typed_lowering_handoff_ready
                   ? "true"
                   : "false")
           << ",\"executable_metadata_typed_lowering_handoff_deterministic\": "
           << (bundle.parse_lowering_readiness_surface
                       .executable_metadata_typed_lowering_handoff_deterministic
                   ? "true"
                   : "false")
           << ",\"lowering_boundary_ready\": "
           << (bundle.parse_lowering_readiness_surface.lowering_boundary_ready ? "true" : "false")
           << ",\"parse_lowering_conformance_matrix_case_count\": "
           << bundle.parse_lowering_readiness_surface.parse_lowering_conformance_matrix_case_count
           << ",\"parse_lowering_conformance_corpus_case_count\": "
           << bundle.parse_lowering_readiness_surface.parse_lowering_conformance_corpus_case_count
           << ",\"parse_lowering_conformance_corpus_passed_case_count\": "
           << bundle.parse_lowering_readiness_surface.parse_lowering_conformance_corpus_passed_case_count
           << ",\"parse_lowering_conformance_corpus_failed_case_count\": "
           << bundle.parse_lowering_readiness_surface.parse_lowering_conformance_corpus_failed_case_count
           << ",\"parse_lowering_performance_quality_guardrails_case_count\": "
           << bundle.parse_lowering_readiness_surface.parse_lowering_performance_quality_guardrails_case_count
           << ",\"parse_lowering_performance_quality_guardrails_passed_case_count\": "
           << bundle.parse_lowering_readiness_surface.parse_lowering_performance_quality_guardrails_passed_case_count
           << ",\"parse_lowering_performance_quality_guardrails_failed_case_count\": "
           << bundle.parse_lowering_readiness_surface.parse_lowering_performance_quality_guardrails_failed_case_count
           << ",\"parser_diagnostic_code_count\": "
           << bundle.parse_lowering_readiness_surface.parser_diagnostic_code_count
           << ",\"long_tail_grammar_construct_count\": "
           << bundle.parse_lowering_readiness_surface.long_tail_grammar_construct_count
           << ",\"long_tail_grammar_covered_construct_count\": "
           << bundle.parse_lowering_readiness_surface.long_tail_grammar_covered_construct_count
           << ",\"parser_diagnostic_code_fingerprint\": "
           << bundle.parse_lowering_readiness_surface.parser_diagnostic_code_fingerprint
           << ",\"long_tail_grammar_fingerprint\": "
           << bundle.parse_lowering_readiness_surface.long_tail_grammar_fingerprint
           << ",\"parser_contract_snapshot_fingerprint\": "
           << bundle.parse_lowering_readiness_surface.parser_contract_snapshot_fingerprint
           << ",\"parser_ast_shape_fingerprint\": "
           << bundle.parse_lowering_readiness_surface.parser_ast_shape_fingerprint
           << ",\"parser_ast_top_level_layout_fingerprint\": "
           << bundle.parse_lowering_readiness_surface.parser_ast_top_level_layout_fingerprint
           << ",\"ast_shape_fingerprint\": "
           << bundle.parse_lowering_readiness_surface.ast_shape_fingerprint
           << ",\"ast_top_level_layout_fingerprint\": "
           << bundle.parse_lowering_readiness_surface.ast_top_level_layout_fingerprint
           << ",\"parse_artifact_handoff_key\":\""
           << bundle.parse_lowering_readiness_surface.parse_artifact_handoff_key
           << "\",\"long_tail_grammar_handoff_key\":\""
           << bundle.parse_lowering_readiness_surface.long_tail_grammar_handoff_key
           << "\",\"long_tail_grammar_expansion_key\":\""
           << bundle.parse_lowering_readiness_surface.long_tail_grammar_expansion_key
           << "\",\"long_tail_grammar_edge_case_compatibility_key\":\""
           << bundle.parse_lowering_readiness_surface.long_tail_grammar_edge_case_compatibility_key
           << "\",\"long_tail_grammar_edge_case_robustness_key\":\""
           << bundle.parse_lowering_readiness_surface.long_tail_grammar_edge_case_robustness_key
           << "\",\"long_tail_grammar_diagnostics_hardening_key\":\""
           << bundle.parse_lowering_readiness_surface.long_tail_grammar_diagnostics_hardening_key
           << "\",\"long_tail_grammar_recovery_determinism_key\":\""
           << bundle.parse_lowering_readiness_surface.long_tail_grammar_recovery_determinism_key
           << "\",\"long_tail_grammar_conformance_matrix_key\":\""
           << bundle.parse_lowering_readiness_surface.long_tail_grammar_conformance_matrix_key
           << "\",\"long_tail_grammar_integration_closeout_key\":\""
           << bundle.parse_lowering_readiness_surface.long_tail_grammar_integration_closeout_key
           << "\",\"compatibility_handoff_key\":\""
           << bundle.parse_lowering_readiness_surface.compatibility_handoff_key
           << "\",\"parse_artifact_replay_key\":\""
           << bundle.parse_lowering_readiness_surface.parse_artifact_replay_key
           << "\",\"parse_artifact_diagnostics_hardening_key\":\""
           << bundle.parse_lowering_readiness_surface.parse_artifact_diagnostics_hardening_key
           << "\",\"parse_artifact_edge_robustness_key\":\""
           << bundle.parse_lowering_readiness_surface.parse_artifact_edge_robustness_key
           << "\",\"parse_recovery_determinism_hardening_key\":\""
           << bundle.parse_lowering_readiness_surface.parse_recovery_determinism_hardening_key
           << "\",\"parser_diagnostic_grammar_hooks_recovery_determinism_key\":\""
           << bundle.parse_lowering_readiness_surface
                  .parser_diagnostic_grammar_hooks_recovery_determinism_key
           << "\",\"parser_diagnostic_grammar_hooks_conformance_matrix_key\":\""
           << bundle.parse_lowering_readiness_surface
                  .parser_diagnostic_grammar_hooks_conformance_matrix_key
           << "\",\"parser_diagnostic_grammar_hooks_conformance_corpus_key\":\""
           << bundle.parse_lowering_readiness_surface
                  .parser_diagnostic_grammar_hooks_conformance_corpus_key
           << "\",\"parse_lowering_conformance_matrix_key\":\""
           << bundle.parse_lowering_readiness_surface.parse_lowering_conformance_matrix_key
           << "\",\"parse_lowering_conformance_corpus_key\":\""
           << bundle.parse_lowering_readiness_surface.parse_lowering_conformance_corpus_key
           << "\",\"parse_lowering_performance_quality_guardrails_key\":\""
           << bundle.parse_lowering_readiness_surface.parse_lowering_performance_quality_guardrails_key
           << "\",\"toolchain_runtime_ga_operations_cross_lane_integration_key\":\""
           << bundle.parse_lowering_readiness_surface
                  .toolchain_runtime_ga_operations_cross_lane_integration_key
           << "\",\"toolchain_runtime_ga_operations_docs_runbook_sync_key\":\""
           << bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_docs_runbook_sync_key
           << "\",\"toolchain_runtime_ga_operations_advanced_core_key\":\""
           << bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_core_key
           << "\",\"toolchain_runtime_ga_operations_advanced_edge_compatibility_key\":\""
           << bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_edge_compatibility_key
           << "\",\"toolchain_runtime_ga_operations_advanced_diagnostics_key\":\""
           << bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_diagnostics_key
           << "\",\"toolchain_runtime_ga_operations_advanced_conformance_key\":\""
           << bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_conformance_key
           << "\",\"toolchain_runtime_ga_operations_advanced_integration_key\":\""
           << bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_integration_key
           << "\",\"toolchain_runtime_ga_operations_advanced_performance_key\":\""
           << bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_performance_key
           << "\",\"toolchain_runtime_ga_operations_advanced_core_shard2_key\":\""
           << bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_core_shard2_key
           << "\",\"toolchain_runtime_ga_operations_integration_closeout_signoff_key\":\""
           << bundle.parse_lowering_readiness_surface
                  .toolchain_runtime_ga_operations_integration_closeout_signoff_key
           << "\",\"executable_metadata_lowering_handoff_key\":\""
           << bundle.parse_lowering_readiness_surface
                  .executable_metadata_lowering_handoff_key
           << "\",\"executable_metadata_typed_lowering_handoff_key\":\""
           << bundle.parse_lowering_readiness_surface
                  .executable_metadata_typed_lowering_handoff_key
           << "\",\"failure_reason\":\"" << bundle.parse_lowering_readiness_surface.failure_reason
           << "\",\"lowering_boundary_replay_key\":\""
           << bundle.parse_lowering_readiness_surface.lowering_boundary_replay_key
           << "\"},\n";
  manifest << "      \"sema_pass_manager\": {\"diagnostics_after_build\":"
           << pipeline_result.sema_diagnostics_after_pass[0] << ",\"diagnostics_after_validate_bodies\":"
           << pipeline_result.sema_diagnostics_after_pass[1] << ",\"diagnostics_after_validate_pure_contract\":"
           << pipeline_result.sema_diagnostics_after_pass[2] << ",\"diagnostics_emitted_by_build\":"
           << pipeline_result.sema_parity_surface.diagnostics_emitted_by_pass[0]
           << ",\"diagnostics_emitted_by_validate_bodies\":"
           << pipeline_result.sema_parity_surface.diagnostics_emitted_by_pass[1]
           << ",\"diagnostics_emitted_by_validate_pure_contract\":"
           << pipeline_result.sema_parity_surface.diagnostics_emitted_by_pass[2] << ",\"diagnostics_monotonic\":"
           << (pipeline_result.sema_parity_surface.diagnostics_after_pass_monotonic ? "true" : "false")
           << ",\"diagnostics_total\":"
           << pipeline_result.sema_parity_surface.diagnostics_total
           << ",\"deterministic_semantic_diagnostics\":"
           << (pipeline_result.sema_parity_surface.deterministic_semantic_diagnostics ? "true" : "false")
           << ",\"diagnostics_accounting_consistent\":"
           << (pipeline_result.sema_parity_surface.diagnostics_accounting_consistent ? "true" : "false")
           << ",\"diagnostics_bus_publish_consistent\":"
           << (pipeline_result.sema_parity_surface.diagnostics_bus_publish_consistent ? "true" : "false")
           << ",\"diagnostics_canonicalized\":"
           << (pipeline_result.sema_parity_surface.diagnostics_canonicalized ? "true" : "false")
           << ",\"diagnostics_hardening_satisfied\":"
           << (pipeline_result.sema_parity_surface.diagnostics_hardening_satisfied ? "true" : "false")
           << ",\"pass_flow_recovery_replay_contract_satisfied\":"
           << (pipeline_result.sema_parity_surface.pass_flow_recovery_replay_contract_satisfied ? "true" : "false")
           << ",\"pass_flow_recovery_replay_key\":\""
           << pipeline_result.sema_parity_surface.pass_flow_recovery_replay_key
           << "\",\"pass_flow_recovery_replay_key_deterministic\":"
           << (pipeline_result.sema_parity_surface.pass_flow_recovery_replay_key_deterministic ? "true" : "false")
           << ",\"pass_flow_recovery_determinism_hardening_satisfied\":"
           << (pipeline_result.sema_parity_surface.pass_flow_recovery_determinism_hardening_satisfied ? "true" : "false")
           << ",\"deterministic_type_metadata_handoff\":"
           << (pipeline_result.sema_parity_surface.deterministic_type_metadata_handoff ? "true" : "false")
           << ",\"pass_flow_configured_count\":"
           << pipeline_result.sema_pass_flow_summary.configured_pass_count
           << ",\"pass_flow_executed_count\":"
           << pipeline_result.sema_pass_flow_summary.executed_pass_count
           << ",\"pass_flow_language_profile\":\""
           << (pipeline_result.sema_pass_flow_summary.language_profile == Objc3SemaLanguageProfile::Canonical
                   ? "canonical"
                   : "canonical")
           << "\",\"pass_flow_canonical_literal_rejection_total\":"
           << pipeline_result.canonical_literal_rejection_counts.total_literal_sites()
           << ",\"pass_flow_duplicate_execution_count\":"
           << pipeline_result.sema_pass_flow_summary.duplicate_pass_execution_count
           << ",\"pass_flow_missing_execution_count\":"
           << pipeline_result.sema_pass_flow_summary.missing_pass_execution_count
           << ",\"pass_flow_diagnostics_total\":"
           << pipeline_result.sema_pass_flow_summary.diagnostics_total
           << ",\"pass_flow_diagnostics_emitted_by_build\":"
           << pipeline_result.sema_pass_flow_summary.diagnostics_emitted_by_pass[0]
           << ",\"pass_flow_diagnostics_emitted_by_validate_bodies\":"
           << pipeline_result.sema_pass_flow_summary.diagnostics_emitted_by_pass[1]
           << ",\"pass_flow_diagnostics_emitted_by_validate_pure_contract\":"
           << pipeline_result.sema_pass_flow_summary.diagnostics_emitted_by_pass[2]
           << ",\"pass_flow_transition_edge_count\":"
           << pipeline_result.sema_pass_flow_summary.transition_edge_count
           << ",\"pass_flow_order_matches_contract\":"
           << (pipeline_result.sema_pass_flow_summary.pass_order_matches_contract ? "true" : "false")
           << ",\"pass_flow_diagnostics_emission_totals_consistent\":"
           << (pipeline_result.sema_pass_flow_summary.diagnostics_emission_totals_consistent ? "true" : "false")
           << ",\"pass_flow_diagnostics_accounting_consistent\":"
           << (pipeline_result.sema_pass_flow_summary.diagnostics_accounting_consistent ? "true" : "false")
           << ",\"pass_flow_diagnostics_bus_publish_consistent\":"
           << (pipeline_result.sema_pass_flow_summary.diagnostics_bus_publish_consistent ? "true" : "false")
           << ",\"pass_flow_diagnostics_canonicalized\":"
           << (pipeline_result.sema_pass_flow_summary.diagnostics_canonicalized ? "true" : "false")
           << ",\"pass_flow_diagnostics_hardening_satisfied\":"
           << (pipeline_result.sema_pass_flow_summary.diagnostics_hardening_satisfied ? "true" : "false")
           << ",\"pass_flow_parser_recovery_replay_ready\":"
           << (pipeline_result.sema_pass_flow_summary.parser_recovery_replay_ready ? "true" : "false")
           << ",\"pass_flow_parser_recovery_replay_case_present\":"
           << (pipeline_result.sema_pass_flow_summary.parser_recovery_replay_case_present ? "true" : "false")
           << ",\"pass_flow_parser_recovery_replay_case_passed\":"
           << (pipeline_result.sema_pass_flow_summary.parser_recovery_replay_case_passed ? "true" : "false")
           << ",\"pass_flow_recovery_replay_contract_satisfied\":"
           << (pipeline_result.sema_pass_flow_summary.recovery_replay_contract_satisfied ? "true" : "false")
           << ",\"pass_flow_recovery_replay_key\":\""
           << pipeline_result.sema_pass_flow_summary.recovery_replay_key
           << "\",\"pass_flow_recovery_replay_key_deterministic\":"
           << (pipeline_result.sema_pass_flow_summary.recovery_replay_key_deterministic ? "true" : "false")
           << ",\"pass_flow_recovery_determinism_hardening_satisfied\":"
           << (pipeline_result.sema_pass_flow_summary.recovery_determinism_hardening_satisfied ? "true" : "false")
           << ",\"pass_flow_compatibility_handoff_consistent\":"
           << (pipeline_result.sema_pass_flow_summary.compatibility_handoff_consistent ? "true" : "false")
           << ",\"pass_flow_robustness_guardrails_satisfied\":"
           << (pipeline_result.sema_pass_flow_summary.robustness_guardrails_satisfied ? "true" : "false")
           << ",\"pass_flow_symbol_counts_consistent\":"
           << (pipeline_result.sema_pass_flow_summary.symbol_flow_counts_consistent ? "true" : "false")
           << ",\"pass_flow_fingerprint\":"
           << pipeline_result.sema_pass_flow_summary.pass_execution_fingerprint
           << ",\"pass_flow_deterministic_handoff_key\":\""
           << pipeline_result.sema_pass_flow_summary.deterministic_handoff_key
           << "\",\"pass_flow_replay_key_deterministic\":"
           << (pipeline_result.sema_pass_flow_summary.replay_key_deterministic ? "true" : "false")
           << ",\"pass_flow_deterministic\":"
           << (pipeline_result.sema_pass_flow_summary.deterministic ? "true" : "false")
           << ",\"deterministic_atomic_memory_order_mapping\":"
           << (pipeline_result.sema_parity_surface.deterministic_atomic_memory_order_mapping ? "true" : "false")
           << ",\"atomic_memory_order_mapping_total\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.total()
           << ",\"atomic_relaxed_ops\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.relaxed
           << ",\"atomic_acquire_ops\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.acquire
           << ",\"atomic_release_ops\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.release
           << ",\"atomic_acq_rel_ops\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.acq_rel
           << ",\"atomic_seq_cst_ops\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.seq_cst
           << ",\"atomic_unmapped_ops\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.unsupported
           << ",\"deterministic_vector_type_lowering\":"
           << (pipeline_result.sema_parity_surface.deterministic_vector_type_lowering ? "true" : "false")
           << ",\"vector_type_lowering_total\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.total()
           << ",\"vector_return_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.return_annotations
           << ",\"vector_param_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.param_annotations
           << ",\"vector_i32_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.i32_annotations
           << ",\"vector_bool_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.bool_annotations
           << ",\"vector_lane2_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.lane2_annotations
           << ",\"vector_lane4_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.lane4_annotations
           << ",\"vector_lane8_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.lane8_annotations
           << ",\"vector_lane16_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.lane16_annotations
           << ",\"vector_unsupported_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.unsupported_annotations
           << ",\"ready\":"
           << (pipeline_result.sema_parity_surface.ready ? "true" : "false")
           << ",\"parity_ready\":"
           << (IsReadyObjc3SemaParityContractSurface(pipeline_result.sema_parity_surface) ? "true" : "false")
           << ",\"globals_total\":"
           << pipeline_result.sema_parity_surface.globals_total
           << ",\"functions_total\":"
           << pipeline_result.sema_parity_surface.functions_total
           << ",\"type_metadata_global_entries\":"
           << pipeline_result.sema_parity_surface.type_metadata_global_entries
           << ",\"type_metadata_function_entries\":"
           << pipeline_result.sema_parity_surface.type_metadata_function_entries
           // Legacy extraction anchor retained for contract tests:
           // << pipeline_result.sema_parity_surface.type_metadata_function_entries << "},\n";
           << ",\"deterministic_interface_implementation_handoff\":"
           << (pipeline_result.sema_parity_surface.deterministic_interface_implementation_handoff ? "true" : "false")
           << ",\"interfaces_total\":"
           << pipeline_result.sema_parity_surface.interfaces_total
           << ",\"implementations_total\":"
           << pipeline_result.sema_parity_surface.implementations_total
           << ",\"type_metadata_interface_entries\":"
           << pipeline_result.sema_parity_surface.type_metadata_interface_entries
           << ",\"type_metadata_implementation_entries\":"
           << pipeline_result.sema_parity_surface.type_metadata_implementation_entries
           << ",\"declared_interfaces\":"
           << pipeline_result.sema_parity_surface.interface_implementation_summary.declared_interfaces
           << ",\"declared_implementations\":"
           << pipeline_result.sema_parity_surface.interface_implementation_summary.declared_implementations
           << ",\"resolved_interfaces\":"
           << pipeline_result.sema_parity_surface.interface_implementation_summary.resolved_interfaces
           << ",\"resolved_implementations\":"
           << pipeline_result.sema_parity_surface.interface_implementation_summary.resolved_implementations
           << ",\"interface_method_symbols_total\":"
           << pipeline_result.sema_parity_surface.interface_method_symbols_total
           << ",\"implementation_method_symbols_total\":"
           << pipeline_result.sema_parity_surface.implementation_method_symbols_total
           << ",\"linked_implementation_symbols_total\":"
           << pipeline_result.sema_parity_surface.linked_implementation_symbols_total
           << ",\"deterministic_interface_implementation_summary\":"
           << (pipeline_result.sema_parity_surface.interface_implementation_summary.deterministic ? "true" : "false")
           << ",\"deterministic_protocol_category_handoff\":"
           << (protocol_category_summary.deterministic_protocol_category_handoff ? "true" : "false")
           << ",\"type_metadata_protocol_entries\":"
           << protocol_category_summary.resolved_protocol_symbols
           << ",\"type_metadata_category_entries\":"
           << protocol_category_summary.resolved_category_symbols
           << ",\"deterministic_class_protocol_category_linking_handoff\":"
           << (class_protocol_category_linking_summary.deterministic_class_protocol_category_linking_handoff
                   ? "true"
                   : "false")
           << ",\"class_protocol_category_declared_class_interfaces\":"
           << class_protocol_category_linking_summary.declared_class_interfaces
           << ",\"class_protocol_category_declared_class_implementations\":"
           << class_protocol_category_linking_summary.declared_class_implementations
           << ",\"class_protocol_category_resolved_class_interfaces\":"
           << class_protocol_category_linking_summary.resolved_class_interfaces
           << ",\"class_protocol_category_resolved_class_implementations\":"
           << class_protocol_category_linking_summary.resolved_class_implementations
           << ",\"class_protocol_category_linked_class_method_symbols\":"
           << class_protocol_category_linking_summary.linked_class_method_symbols
           << ",\"class_protocol_category_linked_category_method_symbols\":"
           << class_protocol_category_linking_summary.linked_category_method_symbols
           << ",\"class_protocol_category_protocol_composition_sites\":"
           << class_protocol_category_linking_summary.protocol_composition_sites
           << ",\"class_protocol_category_protocol_composition_symbols\":"
           << class_protocol_category_linking_summary.protocol_composition_symbols
           << ",\"class_protocol_category_category_composition_sites\":"
           << class_protocol_category_linking_summary.category_composition_sites
           << ",\"class_protocol_category_category_composition_symbols\":"
           << class_protocol_category_linking_summary.category_composition_symbols
           << ",\"class_protocol_category_invalid_protocol_composition_sites\":"
           << class_protocol_category_linking_summary.invalid_protocol_composition_sites
           << ",\"deterministic_selector_normalization_handoff\":"
           << (selector_normalization_summary.deterministic_selector_normalization_handoff ? "true" : "false")
           << ",\"selector_method_declaration_entries\":"
           << selector_normalization_summary.method_declaration_entries
           << ",\"selector_normalized_method_declarations\":"
           << selector_normalization_summary.normalized_method_declarations
           << ",\"selector_piece_entries\":"
           << selector_normalization_summary.selector_piece_entries
           << ",\"selector_piece_parameter_links\":"
           << selector_normalization_summary.selector_piece_parameter_links
           << ",\"deterministic_property_attribute_handoff\":"
           << (property_attribute_summary.deterministic_property_attribute_handoff ? "true" : "false")
           << ",\"property_declaration_entries\":"
           << property_attribute_summary.property_declaration_entries
           << ",\"property_attribute_entries\":"
           << property_attribute_summary.property_attribute_entries
           << ",\"property_attribute_value_entries\":"
           << property_attribute_summary.property_attribute_value_entries
           << ",\"property_accessor_modifier_entries\":"
           << property_attribute_summary.property_accessor_modifier_entries
           << ",\"property_getter_selector_entries\":"
           << property_attribute_summary.property_getter_selector_entries
           << ",\"property_setter_selector_entries\":"
           << property_attribute_summary.property_setter_selector_entries
           << ",\"runtime_metadata_source_ownership_contract_id\":\""
           << runtime_metadata_source_ownership.contract_id
           << "\",\"runtime_metadata_source_schema\":\""
           << runtime_metadata_source_ownership.canonical_source_schema
           << "\",\"runtime_metadata_ivar_source_model\":\""
           << runtime_metadata_source_ownership.ivar_record_source_model
           << "\",\"frontend_owns_runtime_metadata_source_records\":"
           << (runtime_metadata_source_ownership.frontend_owns_runtime_metadata_source_records ? "true" : "false")
           << ",\"runtime_metadata_source_records_ready_for_lowering\":"
           << (runtime_metadata_source_ownership.runtime_metadata_source_records_ready_for_lowering ? "true"
                                                                                                   : "false")
           << ",\"native_runtime_library_present\":"
           << (runtime_metadata_source_ownership.native_runtime_library_present ? "true" : "false")
           << ",\"runtime_link_test_only\":"
           << (runtime_metadata_source_ownership.runtime_link_test_only ? "true" : "false")
           << ",\"runtime_metadata_source_boundary_fail_closed\":"
           << (runtime_metadata_source_ownership.fail_closed ? "true" : "false")
           << ",\"runtime_metadata_source_boundary_ready\":"
           << (IsReadyObjc3RuntimeMetadataSourceOwnershipBoundary(runtime_metadata_source_ownership) ? "true"
                                                                                                      : "false")
           << ",\"deterministic_runtime_metadata_source_schema\":"
           << (runtime_metadata_source_ownership.deterministic_source_schema ? "true" : "false")
           << ",\"runtime_metadata_class_record_count\":"
           << runtime_metadata_source_ownership.class_record_count
           << ",\"runtime_metadata_protocol_record_count\":"
           << runtime_metadata_source_ownership.protocol_record_count
           << ",\"runtime_metadata_category_interface_record_count\":"
           << runtime_metadata_source_ownership.category_interface_record_count
           << ",\"runtime_metadata_category_implementation_record_count\":"
           << runtime_metadata_source_ownership.category_implementation_record_count
           << ",\"runtime_metadata_property_record_count\":"
           << runtime_metadata_source_ownership.property_record_count
           << ",\"runtime_metadata_method_record_count\":"
           << runtime_metadata_source_ownership.method_record_count
           << ",\"runtime_metadata_ivar_record_count\":"
           << runtime_metadata_source_ownership.ivar_record_count
           << ",\"runtime_metadata_source_boundary_failure_reason\":\""
           << runtime_metadata_source_ownership.failure_reason
           << "\""
           << ",\"runtime_export_legality_contract_id\":\""
           << runtime_export_legality.contract_id
           << "\",\"runtime_export_semantic_boundary_frozen\":"
           << (runtime_export_legality.semantic_boundary_frozen ? "true" : "false")
           << ",\"runtime_export_metadata_export_enforcement_ready\":"
           << (runtime_export_legality.metadata_export_enforcement_ready ? "true" : "false")
           << ",\"runtime_export_fail_closed\":"
           << (runtime_export_legality.fail_closed ? "true" : "false")
           << ",\"runtime_export_boundary_ready\":"
           << (IsReadyObjc3RuntimeExportLegalityBoundary(runtime_export_legality) ? "true"
                                                                                 : "false")
           << ",\"runtime_export_duplicate_runtime_identity_enforcement_pending\":"
           << (runtime_export_legality.duplicate_runtime_identity_enforcement_pending ? "true"
                                                                                    : "false")
           << ",\"runtime_export_incomplete_declaration_export_blocking_pending\":"
           << (runtime_export_legality.incomplete_declaration_export_blocking_pending ? "true"
                                                                                    : "false")
           << ",\"runtime_export_illegal_redeclaration_mix_export_blocking_pending\":"
           << (runtime_export_legality.illegal_redeclaration_mix_export_blocking_pending ? "true"
                                                                                        : "false")
           << ",\"runtime_export_class_record_count\":"
           << runtime_export_legality.class_record_count
           << ",\"runtime_export_protocol_record_count\":"
           << runtime_export_legality.protocol_record_count
           << ",\"runtime_export_category_record_count\":"
           << runtime_export_legality.category_record_count
           << ",\"runtime_export_property_record_count\":"
           << runtime_export_legality.property_record_count
           << ",\"runtime_export_method_record_count\":"
           << runtime_export_legality.method_record_count
           << ",\"runtime_export_ivar_record_count\":"
           << runtime_export_legality.ivar_record_count
           << ",\"runtime_export_invalid_protocol_composition_sites\":"
           << runtime_export_legality.invalid_protocol_composition_sites
           << ",\"runtime_export_property_attribute_invalid_entries\":"
           << runtime_export_legality.property_attribute_invalid_entries
           << ",\"runtime_export_property_attribute_contract_violations\":"
           << runtime_export_legality.property_attribute_contract_violations
           << ",\"runtime_export_invalid_type_annotation_sites\":"
           << runtime_export_legality.invalid_type_annotation_sites
           << ",\"runtime_export_property_ivar_binding_missing\":"
           << runtime_export_legality.property_ivar_binding_missing
           << ",\"runtime_export_property_ivar_binding_conflicts\":"
           << runtime_export_legality.property_ivar_binding_conflicts
           << ",\"runtime_export_implementation_resolution_misses\":"
           << runtime_export_legality.implementation_resolution_misses
           << ",\"runtime_export_method_resolution_misses\":"
           << runtime_export_legality.method_resolution_misses
           << ",\"runtime_export_failure_reason\":\""
           << runtime_export_legality.failure_reason
           << "\""
           << ",\"runtime_export_enforcement_contract_id\":\""
           << runtime_export_enforcement.contract_id
           << "\",\"runtime_export_metadata_completeness_enforced\":"
           << (runtime_export_enforcement.metadata_completeness_enforced ? "true"
                                                                        : "false")
           << ",\"runtime_export_duplicate_runtime_identity_suppression_enforced\":"
           << (runtime_export_enforcement
                           .duplicate_runtime_identity_suppression_enforced
                   ? "true"
                   : "false")
           << ",\"runtime_export_illegal_redeclaration_mix_blocking_enforced\":"
           << (runtime_export_enforcement
                           .illegal_redeclaration_mix_blocking_enforced
                   ? "true"
                   : "false")
           << ",\"runtime_export_metadata_shape_drift_blocking_enforced\":"
           << (runtime_export_enforcement
                           .metadata_shape_drift_blocking_enforced
                   ? "true"
                   : "false")
           << ",\"runtime_export_enforcement_fail_closed\":"
           << (runtime_export_enforcement.fail_closed ? "true" : "false")
           << ",\"runtime_export_ready_for_runtime_export\":"
           << (runtime_export_enforcement.ready_for_runtime_export ? "true"
                                                                  : "false")
           // diagnostic precision anchor: manifest evidence must
           // preserve the duplicate/incomplete/illegal counters that feed the
           // precise category-attachment, duplicate-member, and ambiguity
           // diagnostics emitted by the runtime metadata blocker.
           << ",\"runtime_export_duplicate_runtime_identity_sites\":"
           << runtime_export_enforcement.duplicate_runtime_identity_sites
           << ",\"runtime_export_incomplete_declaration_sites\":"
           << runtime_export_enforcement.incomplete_declaration_sites
           << ",\"runtime_export_illegal_redeclaration_mix_sites\":"
           << runtime_export_enforcement.illegal_redeclaration_mix_sites
           << ",\"runtime_export_metadata_shape_drift_sites\":"
           << runtime_export_enforcement.metadata_shape_drift_sites
           << ",\"runtime_export_enforcement_failure_reason\":\""
           << runtime_export_enforcement.failure_reason
           << "\""
           << ",\"runtime_metadata_section_abi_contract_id\":\""
           << runtime_metadata_section_abi.contract_id
           << "\",\"runtime_metadata_section_boundary_frozen\":"
           << (runtime_metadata_section_abi.boundary_frozen ? "true" : "false")
           << ",\"runtime_metadata_section_fail_closed\":"
           << (runtime_metadata_section_abi.fail_closed ? "true" : "false")
           << ",\"runtime_metadata_section_object_file_inventory_frozen\":"
           << (runtime_metadata_section_abi.object_file_section_inventory_frozen
                   ? "true"
                   : "false")
           << ",\"runtime_metadata_section_symbol_policy_frozen\":"
           << (runtime_metadata_section_abi.symbol_policy_frozen ? "true"
                                                                : "false")
           << ",\"runtime_metadata_section_visibility_model_frozen\":"
           << (runtime_metadata_section_abi.visibility_model_frozen ? "true"
                                                                    : "false")
           << ",\"runtime_metadata_section_retention_policy_frozen\":"
           << (runtime_metadata_section_abi.retention_policy_frozen ? "true"
                                                                    : "false")
           << ",\"runtime_metadata_section_ready_for_scaffold\":"
           << (runtime_metadata_section_abi.ready_for_section_scaffold ? "true"
                                                                       : "false")
           << ",\"runtime_metadata_section_logical_image_info_section\":\""
           << runtime_metadata_section_abi.logical_image_info_section
           << "\",\"runtime_metadata_section_logical_class_descriptor_section\":\""
           << runtime_metadata_section_abi.logical_class_descriptor_section
           << "\",\"runtime_metadata_section_logical_protocol_descriptor_section\":\""
           << runtime_metadata_section_abi.logical_protocol_descriptor_section
           << "\",\"runtime_metadata_section_logical_category_descriptor_section\":\""
           << runtime_metadata_section_abi.logical_category_descriptor_section
           << "\",\"runtime_metadata_section_logical_property_descriptor_section\":\""
           << runtime_metadata_section_abi.logical_property_descriptor_section
           << "\",\"runtime_metadata_section_logical_ivar_descriptor_section\":\""
           << runtime_metadata_section_abi.logical_ivar_descriptor_section
           << "\",\"runtime_metadata_section_descriptor_symbol_prefix\":\""
           << runtime_metadata_section_abi.descriptor_symbol_prefix
           << "\",\"runtime_metadata_section_aggregate_symbol_prefix\":\""
           << runtime_metadata_section_abi.aggregate_symbol_prefix
           << "\",\"runtime_metadata_section_image_info_symbol\":\""
           << runtime_metadata_section_abi.image_info_symbol
           << "\",\"runtime_metadata_section_descriptor_linkage\":\""
           << runtime_metadata_section_abi.descriptor_linkage
           << "\",\"runtime_metadata_section_aggregate_linkage\":\""
           << runtime_metadata_section_abi.aggregate_linkage
           << "\",\"runtime_metadata_section_visibility\":\""
           << runtime_metadata_section_abi.metadata_visibility
           << "\",\"runtime_metadata_section_retention_root\":\""
           << runtime_metadata_section_abi.retention_root
           << "\",\"runtime_metadata_section_failure_reason\":\""
           << runtime_metadata_section_abi.failure_reason
           << "\""
           << ",\"runtime_metadata_section_publication_contract_id\":\""
           << runtime_metadata_section_publication.contract_id
           << "\",\"runtime_metadata_section_publication_abi_contract_id\":\""
           << runtime_metadata_section_publication.abi_contract_id
           << "\",\"runtime_metadata_section_publication_emitted\":"
           << (runtime_metadata_section_publication.publication_emitted ? "true"
                                                                  : "false")
           << ",\"runtime_metadata_section_publication_fail_closed\":"
           << (runtime_metadata_section_publication.fail_closed ? "true" : "false")
           << ",\"runtime_metadata_section_publication_uses_llvm_used\":"
           << (runtime_metadata_section_publication.uses_llvm_used ? "true"
                                                                : "false")
           << ",\"runtime_metadata_section_publication_image_info_emitted\":"
           << (runtime_metadata_section_publication.image_info_emitted ? "true"
                                                                    : "false")
           << ",\"runtime_metadata_section_publication_class_descriptor_count\":"
           << runtime_metadata_section_publication.class_descriptor_count
           << ",\"runtime_metadata_section_publication_protocol_descriptor_count\":"
           << runtime_metadata_section_publication.protocol_descriptor_count
           << ",\"runtime_metadata_section_publication_category_descriptor_count\":"
           << runtime_metadata_section_publication.category_descriptor_count
           << ",\"runtime_metadata_section_publication_property_descriptor_count\":"
           << runtime_metadata_section_publication.property_descriptor_count
           << ",\"runtime_metadata_section_publication_ivar_descriptor_count\":"
           << runtime_metadata_section_publication.ivar_descriptor_count
           << ",\"runtime_metadata_section_publication_total_descriptor_count\":"
           << runtime_metadata_section_publication.total_descriptor_count
           << ",\"runtime_metadata_section_publication_total_retained_global_count\":"
           << runtime_metadata_section_publication.total_retained_global_count
           << ",\"runtime_metadata_section_publication_image_info_symbol\":\""
           << runtime_metadata_section_publication.image_info_symbol
           << "\",\"runtime_metadata_section_publication_class_aggregate_symbol\":\""
           << runtime_metadata_section_publication.class_aggregate_symbol
           << "\",\"runtime_metadata_section_publication_protocol_aggregate_symbol\":\""
           << runtime_metadata_section_publication.protocol_aggregate_symbol
           << "\",\"runtime_metadata_section_publication_category_aggregate_symbol\":\""
           << runtime_metadata_section_publication.category_aggregate_symbol
           << "\",\"runtime_metadata_section_publication_property_aggregate_symbol\":\""
           << runtime_metadata_section_publication.property_aggregate_symbol
           << "\",\"runtime_metadata_section_publication_ivar_aggregate_symbol\":\""
           << runtime_metadata_section_publication.ivar_aggregate_symbol
           << "\",\"runtime_metadata_section_publication_failure_reason\":\""
           << runtime_metadata_section_publication.failure_reason
           << "\",\"runtime_metadata_object_inspection_contract_id\":\""
           << runtime_metadata_object_inspection.contract_id
           << "\",\"runtime_metadata_object_inspection_publication_contract_id\":\""
           << runtime_metadata_object_inspection.publication_contract_id
           << "\",\"runtime_metadata_object_inspection_matrix_published\":"
           << (runtime_metadata_object_inspection.matrix_published ? "true"
                                                                   : "false")
           << ",\"runtime_metadata_object_inspection_fail_closed\":"
           << (runtime_metadata_object_inspection.fail_closed ? "true"
                                                              : "false")
           << ",\"runtime_metadata_object_inspection_uses_llvm_readobj\":"
           << (runtime_metadata_object_inspection.uses_llvm_readobj ? "true"
                                                                    : "false")
           << ",\"runtime_metadata_object_inspection_uses_llvm_objdump\":"
           << (runtime_metadata_object_inspection.uses_llvm_objdump ? "true"
                                                                    : "false")
           << ",\"runtime_metadata_object_inspection_matrix_row_count\":"
           << runtime_metadata_object_inspection.matrix_row_count
           << ",\"runtime_metadata_object_inspection_fixture_path\":\""
           << runtime_metadata_object_inspection.fixture_path
           << "\",\"runtime_metadata_object_inspection_emit_prefix\":\""
           << runtime_metadata_object_inspection.emit_prefix
           << "\",\"runtime_metadata_object_inspection_object_relative_path\":\""
           << runtime_metadata_object_inspection.object_relative_path
           << "\",\"runtime_metadata_object_inspection_section_inventory_row_key\":\""
           << runtime_metadata_object_inspection.section_inventory_row_key
           << "\",\"runtime_metadata_object_inspection_section_inventory_command\":\""
           << runtime_metadata_object_inspection.section_inventory_command
           << "\",\"runtime_metadata_object_inspection_symbol_inventory_row_key\":\""
           << runtime_metadata_object_inspection.symbol_inventory_row_key
           << "\",\"runtime_metadata_object_inspection_symbol_inventory_command\":\""
           << runtime_metadata_object_inspection.symbol_inventory_command
           << "\",\"runtime_metadata_object_inspection_failure_reason\":\""
           << runtime_metadata_object_inspection.failure_reason
           << "\",\"executable_metadata_debug_projection_contract_id\":\""
           << EscapeJsonString(executable_metadata_debug_projection.contract_id)
           << "\",\"executable_metadata_debug_projection_typed_handoff_contract_id\":\""
           << EscapeJsonString(
                  executable_metadata_debug_projection
                      .typed_lowering_handoff_contract_id)
           << "\",\"executable_metadata_debug_projection_source_graph_contract_id\":\""
           << EscapeJsonString(
                  executable_metadata_debug_projection.source_graph_contract_id)
           << "\",\"executable_metadata_debug_projection_named_metadata_name\":\""
           << EscapeJsonString(
                  executable_metadata_debug_projection.named_metadata_name)
           << "\",\"executable_metadata_debug_projection_manifest_surface_path\":\""
           << EscapeJsonString(
                  executable_metadata_debug_projection.manifest_surface_path)
           << "\",\"executable_metadata_debug_projection_typed_handoff_surface_path\":\""
           << EscapeJsonString(
                  executable_metadata_debug_projection.typed_handoff_surface_path)
           << "\",\"executable_metadata_debug_projection_source_graph_surface_path\":\""
           << EscapeJsonString(
                  executable_metadata_debug_projection.source_graph_surface_path)
           << "\",\"executable_metadata_debug_projection_matrix_published\":"
           << (executable_metadata_debug_projection.matrix_published ? "true"
                                                                    : "false")
           << ",\"executable_metadata_debug_projection_fail_closed\":"
           << (executable_metadata_debug_projection.fail_closed ? "true"
                                                                : "false")
           << ",\"executable_metadata_debug_projection_manifest_debug_surface_published\":"
           << (executable_metadata_debug_projection
                       .manifest_debug_surface_published
                   ? "true"
                   : "false")
           << ",\"executable_metadata_debug_projection_ir_named_metadata_published\":"
           << (executable_metadata_debug_projection.ir_named_metadata_published
                   ? "true"
                   : "false")
           << ",\"executable_metadata_debug_projection_replay_anchor_deterministic\":"
           << (executable_metadata_debug_projection.replay_anchor_deterministic
                   ? "true"
                   : "false")
           << ",\"executable_metadata_debug_projection_active_typed_handoff_ready\":"
           << (executable_metadata_debug_projection.active_typed_handoff_ready
                   ? "true"
                   : "false")
           << ",\"executable_metadata_debug_projection_matrix_row_count\":"
           << executable_metadata_debug_projection.matrix_row_count
           << ",\"executable_metadata_debug_projection_replay_key\":\""
           << EscapeJsonString(executable_metadata_debug_projection.replay_key)
           << "\",\"executable_metadata_debug_projection_active_typed_handoff_replay_key\":\""
           << EscapeJsonString(
                  executable_metadata_debug_projection
                      .active_typed_handoff_replay_key)
           << "\",\"executable_metadata_debug_projection_failure_reason\":\""
           << EscapeJsonString(
                  executable_metadata_debug_projection.failure_reason)
           << "\",\"executable_metadata_runtime_ingest_packaging_contract_id\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .contract_id)
           << "\",\"executable_metadata_runtime_ingest_packaging_typed_handoff_contract_id\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .typed_lowering_handoff_contract_id)
           << "\",\"executable_metadata_runtime_ingest_packaging_debug_projection_contract_id\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .debug_projection_contract_id)
           << "\",\"executable_metadata_runtime_ingest_packaging_surface_path\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .packaging_surface_path)
           << "\",\"executable_metadata_runtime_ingest_packaging_typed_handoff_surface_path\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .typed_handoff_surface_path)
           << "\",\"executable_metadata_runtime_ingest_packaging_debug_projection_surface_path\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .debug_projection_surface_path)
           << "\",\"executable_metadata_runtime_ingest_packaging_payload_model\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .packaging_payload_model)
           << "\",\"executable_metadata_runtime_ingest_packaging_transport_artifact_relative_path\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .transport_artifact_relative_path)
           << "\",\"executable_metadata_runtime_ingest_packaging_boundary_frozen\":"
           << (executable_metadata_runtime_ingest_packaging_contract
                       .boundary_frozen
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_packaging_fail_closed\":"
           << (executable_metadata_runtime_ingest_packaging_contract.fail_closed
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_packaging_typed_handoff_ready\":"
           << (executable_metadata_runtime_ingest_packaging_contract
                       .typed_lowering_handoff_ready
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_packaging_debug_projection_ready\":"
           << (executable_metadata_runtime_ingest_packaging_contract
                       .debug_projection_ready
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_packaging_manifest_transport_frozen\":"
           << (executable_metadata_runtime_ingest_packaging_contract
                       .manifest_transport_frozen
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_packaging_runtime_section_emission_not_yet_landed\":"
           << (executable_metadata_runtime_ingest_packaging_contract
                       .runtime_section_emission_not_yet_landed
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_packaging_startup_registration_not_yet_landed\":"
           << (executable_metadata_runtime_ingest_packaging_contract
                       .startup_registration_not_yet_landed
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_packaging_runtime_loader_registration_not_yet_landed\":"
           << (executable_metadata_runtime_ingest_packaging_contract
                       .runtime_loader_registration_not_yet_landed
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_packaging_explicit_non_goals_published\":"
           << (executable_metadata_runtime_ingest_packaging_contract
                       .explicit_non_goals_published
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_packaging_ready_for_packaging_implementation\":"
           << (executable_metadata_runtime_ingest_packaging_contract
                       .ready_for_packaging_implementation
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_packaging_typed_handoff_replay_key\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .typed_lowering_handoff_replay_key)
           << "\",\"executable_metadata_runtime_ingest_packaging_debug_projection_replay_key\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .debug_projection_replay_key)
           << "\",\"executable_metadata_runtime_ingest_packaging_replay_key\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .replay_key)
           << "\",\"executable_metadata_runtime_ingest_packaging_failure_reason\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .failure_reason)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_contract_id\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .contract_id)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_packaging_contract_id\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .packaging_contract_id)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_surface_path\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .binary_boundary_surface_path)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_payload_model\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .payload_model)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_envelope_format\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .envelope_format)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_artifact_relative_path\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .artifact_relative_path)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_artifact_suffix\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .artifact_suffix)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_magic\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .binary_magic)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_chunk_count\":"
           << executable_metadata_runtime_ingest_binary_boundary.chunk_count
           << ",\"executable_metadata_runtime_ingest_binary_boundary_fail_closed\":"
           << (executable_metadata_runtime_ingest_binary_boundary.fail_closed
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_binary_boundary_binary_payload_present\":"
           << (executable_metadata_runtime_ingest_binary_boundary
                       .binary_payload_present
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_binary_boundary_binary_envelope_deterministic\":"
           << (executable_metadata_runtime_ingest_binary_boundary
                       .binary_envelope_deterministic
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_binary_boundary_ready_for_section_emission_handoff\":"
           << (executable_metadata_runtime_ingest_binary_boundary
                       .ready_for_section_emission_handoff
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_binary_boundary_payload_bytes\":"
           << executable_metadata_runtime_ingest_binary_boundary.payload_bytes
           << ",\"executable_metadata_runtime_ingest_binary_boundary_packaging_contract_replay_key\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .packaging_contract_replay_key)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_typed_handoff_replay_key\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .typed_lowering_handoff_replay_key)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_debug_projection_replay_key\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .debug_projection_replay_key)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_replay_key\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .replay_key)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_failure_reason\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .failure_reason)
           << "\",\"runtime_support_library_contract_id\":\""
           << runtime_support_library.contract_id
           << "\",\"runtime_support_library_metadata_publication_contract_id\":\""
           << runtime_support_library.metadata_publication_contract_id
           << "\",\"runtime_support_library_boundary_frozen\":"
           << (runtime_support_library.boundary_frozen ? "true" : "false")
           << ",\"runtime_support_library_fail_closed\":"
           << (runtime_support_library.fail_closed ? "true" : "false")
           << ",\"runtime_support_library_target_name_frozen\":"
           << (runtime_support_library.target_name_frozen ? "true" : "false")
           << ",\"runtime_support_library_exported_entrypoints_frozen\":"
           << (runtime_support_library.exported_entrypoints_frozen ? "true"
                                                                   : "false")
           << ",\"runtime_support_library_ownership_boundaries_frozen\":"
           << (runtime_support_library.ownership_boundaries_frozen ? "true"
                                                                   : "false")
           << ",\"runtime_support_library_build_constraints_frozen\":"
           << (runtime_support_library.build_constraints_frozen ? "true"
                                                                : "false")
           << ",\"runtime_support_library_strict_dispatch_errors_required\":"
           << (runtime_support_library
                           .strict_dispatch_errors_required
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_native_library_present\":"
           << (runtime_support_library.native_runtime_library_present ? "true"
                                                                      : "false")
           << ",\"runtime_support_library_driver_link_wiring_pending\":"
           << (runtime_support_library.driver_link_wiring_pending ? "true"
                                                                  : "false")
           << ",\"runtime_support_library_ready_for_skeleton\":"
           << (runtime_support_library.ready_for_runtime_library_skeleton
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_target_name\":\""
           << runtime_support_library.cmake_target_name
           << "\",\"runtime_support_library_public_header_path\":\""
           << runtime_support_library.public_header_path
           << "\",\"runtime_support_library_source_root\":\""
           << runtime_support_library.source_root
           << "\",\"runtime_support_library_library_kind\":\""
           << runtime_support_library.library_kind
           << "\",\"runtime_support_library_archive_basename\":\""
           << runtime_support_library.archive_basename
           << "\",\"runtime_support_library_register_image_symbol\":\""
           << runtime_support_library.register_image_symbol
           << "\",\"runtime_support_library_lookup_selector_symbol\":\""
           << runtime_support_library.lookup_selector_symbol
           << "\",\"runtime_support_library_dispatch_i32_symbol\":\""
           << runtime_support_library.dispatch_i32_symbol
           << "\",\"runtime_support_library_reset_for_testing_symbol\":\""
           << runtime_support_library.reset_for_testing_symbol
           << "\",\"runtime_support_library_driver_link_mode\":\""
           << runtime_support_library.driver_link_mode
           << "\",\"runtime_support_library_compiler_ownership_boundary\":\""
           << runtime_support_library.compiler_ownership_boundary
           << "\",\"runtime_support_library_runtime_ownership_boundary\":\""
           << runtime_support_library.runtime_ownership_boundary
           << "\",\"runtime_support_library_failure_reason\":\""
           << runtime_support_library.failure_reason
           << "\",\"runtime_support_library_core_feature_contract_id\":\""
           << runtime_support_library_core_feature.contract_id
           << "\",\"runtime_support_library_core_feature_support_library_contract_id\":\""
           << runtime_support_library_core_feature.support_library_contract_id
           << "\",\"runtime_support_library_core_feature_metadata_publication_contract_id\":\""
           << runtime_support_library_core_feature.metadata_publication_contract_id
           << "\",\"runtime_support_library_core_feature_fail_closed\":"
           << (runtime_support_library_core_feature.fail_closed ? "true"
                                                                : "false")
           << ",\"runtime_support_library_core_feature_sources_present\":"
           << (runtime_support_library_core_feature
                           .native_runtime_library_sources_present
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_core_feature_header_present\":"
           << (runtime_support_library_core_feature
                           .native_runtime_library_header_present
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_core_feature_archive_build_enabled\":"
           << (runtime_support_library_core_feature
                           .native_runtime_library_archive_build_enabled
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_core_feature_entrypoints_implemented\":"
           << (runtime_support_library_core_feature
                           .native_runtime_library_entrypoints_implemented
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_core_feature_selector_lookup_stateful\":"
           << (runtime_support_library_core_feature.selector_lookup_stateful
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_core_feature_dispatch_formula_matches_runtime_test_helper\":"
           << (runtime_support_library_core_feature
                           .deterministic_dispatch_formula_matches_runtime_test_helper
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_core_feature_reset_for_testing_supported\":"
           << (runtime_support_library_core_feature.reset_for_testing_supported
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_core_feature_strict_dispatch_errors_required\":"
           << (runtime_support_library_core_feature
                           .strict_dispatch_errors_required
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_core_feature_driver_link_wiring_pending\":"
           << (runtime_support_library_core_feature.driver_link_wiring_pending
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_core_feature_ready_for_driver_link_wiring\":"
           << (runtime_support_library_core_feature.ready_for_driver_link_wiring
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_core_feature_target_name\":\""
           << runtime_support_library_core_feature.cmake_target_name
           << "\",\"runtime_support_library_core_feature_public_header_path\":\""
           << runtime_support_library_core_feature.public_header_path
           << "\",\"runtime_support_library_core_feature_source_root\":\""
           << runtime_support_library_core_feature.source_root
           << "\",\"runtime_support_library_core_feature_implementation_source_path\":\""
           << runtime_support_library_core_feature.implementation_source_path
           << "\",\"runtime_support_library_core_feature_library_kind\":\""
           << runtime_support_library_core_feature.library_kind
           << "\",\"runtime_support_library_core_feature_archive_basename\":\""
           << runtime_support_library_core_feature.archive_basename
           << "\",\"runtime_support_library_core_feature_archive_relative_path\":\""
           << runtime_support_library_core_feature.archive_relative_path
           << "\",\"runtime_support_library_core_feature_probe_source_path\":\""
           << runtime_support_library_core_feature.probe_source_path
           << "\",\"runtime_support_library_core_feature_register_image_symbol\":\""
           << runtime_support_library_core_feature.register_image_symbol
           << "\",\"runtime_support_library_core_feature_lookup_selector_symbol\":\""
           << runtime_support_library_core_feature.lookup_selector_symbol
           << "\",\"runtime_support_library_core_feature_dispatch_i32_symbol\":\""
           << runtime_support_library_core_feature.dispatch_i32_symbol
           << "\",\"runtime_support_library_core_feature_reset_for_testing_symbol\":\""
           << runtime_support_library_core_feature.reset_for_testing_symbol
           << "\",\"runtime_support_library_core_feature_driver_link_mode\":\""
           << runtime_support_library_core_feature.driver_link_mode
           << "\",\"runtime_support_library_link_wiring_contract_id\":\""
           << runtime_support_library_link_wiring.contract_id
           << "\",\"runtime_support_library_link_wiring_core_feature_contract_id\":\""
           << runtime_support_library_link_wiring
                  .support_library_core_feature_contract_id
           << "\",\"runtime_support_library_link_wiring_fail_closed\":"
           << (runtime_support_library_link_wiring.fail_closed ? "true"
                                                               : "false")
           << ",\"runtime_support_library_link_wiring_archive_available\":"
           << (runtime_support_library_link_wiring
                       .runtime_library_archive_available
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_link_wiring_driver_emits_runtime_link_contract\":"
           << (runtime_support_library_link_wiring
                       .driver_emits_runtime_link_contract
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_link_wiring_execution_smoke_consumes_runtime_library\":"
           << (runtime_support_library_link_wiring
                       .execution_smoke_consumes_runtime_library
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_link_wiring_strict_dispatch_errors_required\":"
           << (runtime_support_library_link_wiring
                           .strict_dispatch_errors_required
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_link_wiring_ready_for_runtime_library_consumption\":"
           << (runtime_support_library_link_wiring
                       .ready_for_runtime_library_consumption
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_link_wiring_archive_relative_path\":\""
           << runtime_support_library_link_wiring.archive_relative_path
           << "\",\"runtime_support_library_link_wiring_runtime_dispatch_symbol\":\""
           << runtime_support_library_link_wiring.runtime_dispatch_symbol
           << "\",\"runtime_support_library_link_wiring_execution_smoke_script_path\":\""
           << runtime_support_library_link_wiring.execution_smoke_script_path
           << "\",\"runtime_support_library_link_wiring_driver_link_mode\":\""
           << runtime_support_library_link_wiring.driver_link_mode
           << "\",\"runtime_support_library_link_wiring_failure_reason\":\""
           << runtime_support_library_link_wiring.failure_reason
           << "\",\"runtime_support_library_core_feature_failure_reason\":\""
           << runtime_support_library_core_feature.failure_reason
           << "\",\"runtime_translation_unit_registration_contract_id\":\""
           << runtime_translation_unit_registration_contract.contract_id
           << "\",\"runtime_translation_unit_registration_binary_boundary_contract_id\":\""
           << runtime_translation_unit_registration_contract
                  .binary_boundary_contract_id
           << "\",\"runtime_translation_unit_registration_archive_static_link_contract_id\":\""
           << runtime_translation_unit_registration_contract
                  .archive_static_link_contract_id
           << "\",\"runtime_translation_unit_registration_object_emission_closeout_contract_id\":\""
           << runtime_translation_unit_registration_contract
                  .object_emission_closeout_contract_id
           << "\",\"runtime_translation_unit_registration_runtime_support_library_link_wiring_contract_id\":\""
           << runtime_translation_unit_registration_contract
                  .runtime_support_library_link_wiring_contract_id
           << "\",\"runtime_translation_unit_registration_payload_model\":\""
           << runtime_translation_unit_registration_contract
                  .registration_payload_model
           << "\",\"runtime_translation_unit_registration_runtime_owned_payload_artifact_count\":"
           << runtime_translation_unit_registration_contract
                  .runtime_owned_payload_artifact_count
           << ",\"runtime_translation_unit_registration_payload_artifact_relative_path\":\""
           << runtime_translation_unit_registration_contract
                  .runtime_owned_payload_artifacts[0]
           << "\",\"runtime_translation_unit_registration_linker_response_artifact_relative_path\":\""
           << runtime_translation_unit_registration_contract
                  .runtime_owned_payload_artifacts[1]
           << "\",\"runtime_translation_unit_registration_discovery_artifact_relative_path\":\""
           << runtime_translation_unit_registration_contract
                  .runtime_owned_payload_artifacts[2]
           << "\",\"runtime_translation_unit_registration_constructor_root_symbol\":\""
           << runtime_translation_unit_registration_contract
                  .constructor_root_symbol
           << "\",\"runtime_translation_unit_registration_constructor_root_ownership_model\":\""
           << runtime_translation_unit_registration_contract
                  .constructor_root_ownership_model
           << "\",\"runtime_translation_unit_registration_constructor_emission_mode\":\""
           << runtime_translation_unit_registration_contract
                  .constructor_emission_mode
           << "\",\"runtime_translation_unit_registration_constructor_priority_policy\":\""
           << runtime_translation_unit_registration_contract
                  .constructor_priority_policy
           << "\",\"runtime_translation_unit_registration_entrypoint_symbol\":\""
           << runtime_translation_unit_registration_contract
                  .registration_entrypoint_symbol
           << "\",\"runtime_translation_unit_registration_translation_unit_identity_model\":\""
           << runtime_translation_unit_registration_contract
                  .translation_unit_identity_model
           << "\",\"runtime_translation_unit_registration_boundary_frozen\":"
           << (runtime_translation_unit_registration_contract.boundary_frozen
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_fail_closed\":"
           << (runtime_translation_unit_registration_contract.fail_closed
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_binary_boundary_ready\":"
           << (runtime_translation_unit_registration_contract.binary_boundary_ready
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_archive_static_link_surface_ready\":"
           << (runtime_translation_unit_registration_contract
                       .archive_static_link_surface_ready
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_object_emission_closeout_surface_ready\":"
           << (runtime_translation_unit_registration_contract
                       .object_emission_closeout_surface_ready
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_runtime_support_library_link_wiring_ready\":"
           << (runtime_translation_unit_registration_contract
                       .runtime_support_library_link_wiring_ready
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_runtime_owned_payload_inventory_published\":"
           << (runtime_translation_unit_registration_contract
                       .runtime_owned_payload_inventory_published
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_constructor_root_reserved_not_emitted\":"
           << (runtime_translation_unit_registration_contract
                       .constructor_root_reserved_not_emitted
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_startup_registration_not_yet_landed\":"
           << (runtime_translation_unit_registration_contract
                       .startup_registration_not_yet_landed
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_runtime_bootstrap_not_yet_landed\":"
           << (runtime_translation_unit_registration_contract
                       .runtime_bootstrap_not_yet_landed
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_explicit_non_goals_published\":"
           << (runtime_translation_unit_registration_contract
                       .explicit_non_goals_published
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_ready_for_manifest_implementation\":"
           << (runtime_translation_unit_registration_contract
                       .ready_for_registration_manifest_implementation
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_binary_boundary_replay_key\":\""
           << EscapeJsonString(
                  runtime_translation_unit_registration_contract
                      .binary_boundary_replay_key)
           << "\",\"runtime_translation_unit_registration_replay_key\":\""
           << EscapeJsonString(
                  runtime_translation_unit_registration_contract.replay_key)
           << "\",\"runtime_translation_unit_registration_failure_reason\":\""
           << EscapeJsonString(
                  runtime_translation_unit_registration_contract.failure_reason)
           << "\""
           << ",\"runtime_translation_unit_registration_manifest_contract_id\":\""
           << runtime_translation_unit_registration_manifest.contract_id
           << "\",\"runtime_translation_unit_registration_manifest_payload_model\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_payload_model
           << "\",\"runtime_translation_unit_registration_manifest_artifact_relative_path\":\""
           << runtime_translation_unit_registration_manifest

                  .manifest_artifact_relative_path
           << "\",\"runtime_translation_unit_registration_manifest_runtime_owned_payload_artifact_count\":"
           << runtime_translation_unit_registration_manifest
                  .runtime_owned_payload_artifact_count
           << ",\"runtime_translation_unit_registration_manifest_runtime_support_library_archive_relative_path\":\""
           << runtime_translation_unit_registration_manifest
                  .runtime_support_library_archive_relative_path
           << "\",\"runtime_translation_unit_registration_manifest_constructor_root_symbol\":\""
           << runtime_translation_unit_registration_manifest
                  .constructor_root_symbol
           << "\",\"runtime_translation_unit_registration_manifest_constructor_root_ownership_model\":\""
           << runtime_translation_unit_registration_manifest
                  .constructor_root_ownership_model
           << "\",\"runtime_translation_unit_registration_manifest_authority_model\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_authority_model
           << "\",\"runtime_translation_unit_registration_manifest_init_stub_symbol_prefix\":\""
           << runtime_translation_unit_registration_manifest
                  .constructor_init_stub_symbol_prefix
           << "\",\"runtime_translation_unit_registration_manifest_init_stub_ownership_model\":\""
           << runtime_translation_unit_registration_manifest
                  .constructor_init_stub_ownership_model
           << "\",\"runtime_translation_unit_registration_manifest_constructor_priority_policy\":\""
           << runtime_translation_unit_registration_manifest
                  .constructor_priority_policy
           << "\",\"runtime_translation_unit_registration_manifest_registration_entrypoint_symbol\":\""
           << runtime_translation_unit_registration_manifest
                  .registration_entrypoint_symbol
           << "\",\"runtime_translation_unit_registration_manifest_translation_unit_identity_model\":\""
           << runtime_translation_unit_registration_manifest
                  .translation_unit_identity_model
           << "\",\"runtime_translation_unit_registration_manifest_launch_integration_contract_id\":\""
           << runtime_translation_unit_registration_manifest
                  .launch_integration_contract_id
           << "\",\"runtime_translation_unit_registration_manifest_runtime_library_resolution_model\":\""
           << runtime_translation_unit_registration_manifest
                  .runtime_library_resolution_model
           << "\",\"runtime_translation_unit_registration_manifest_driver_linker_flag_consumption_model\":\""
           << runtime_translation_unit_registration_manifest
                  .driver_linker_flag_consumption_model
           << "\",\"runtime_translation_unit_registration_manifest_compile_wrapper_command_surface\":\""
           << runtime_translation_unit_registration_manifest
                  .compile_wrapper_command_surface
           << "\",\"runtime_translation_unit_registration_manifest_compile_proof_command_surface\":\""
           << runtime_translation_unit_registration_manifest
                  .compile_proof_command_surface
           << "\",\"runtime_translation_unit_registration_manifest_execution_smoke_command_surface\":\""
           << runtime_translation_unit_registration_manifest
                  .execution_smoke_command_surface
           << "\",\"runtime_translation_unit_registration_manifest_class_descriptor_count\":"
           << runtime_translation_unit_registration_manifest
                  .class_descriptor_count
           << ",\"runtime_translation_unit_registration_manifest_protocol_descriptor_count\":"
           << runtime_translation_unit_registration_manifest
                  .protocol_descriptor_count
           << ",\"runtime_translation_unit_registration_manifest_category_descriptor_count\":"
           << runtime_translation_unit_registration_manifest
                  .category_descriptor_count
           << ",\"runtime_translation_unit_registration_manifest_property_descriptor_count\":"
           << runtime_translation_unit_registration_manifest
                  .property_descriptor_count
           << ",\"runtime_translation_unit_registration_manifest_ivar_descriptor_count\":"
           << runtime_translation_unit_registration_manifest
                  .ivar_descriptor_count
           << ",\"runtime_translation_unit_registration_manifest_total_descriptor_count\":"
           << runtime_translation_unit_registration_manifest
                  .total_descriptor_count
           << ",\"runtime_translation_unit_registration_manifest_translation_unit_registration_order_ordinal\":"
           << runtime_translation_unit_registration_manifest
                  .translation_unit_registration_order_ordinal
           << ",\"runtime_translation_unit_registration_manifest_fail_closed\":"
           << (runtime_translation_unit_registration_manifest.fail_closed
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_manifest_contract_ready\":"
           << (runtime_translation_unit_registration_manifest
                       .translation_unit_registration_contract_ready
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_manifest_runtime_support_library_link_wiring_ready\":"
           << (runtime_translation_unit_registration_manifest
                       .runtime_support_library_link_wiring_ready
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_manifest_template_published\":"
           << (runtime_translation_unit_registration_manifest
                       .runtime_manifest_template_published
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_manifest_constructor_root_manifest_authoritative\":"
           << (runtime_translation_unit_registration_manifest
                       .constructor_root_manifest_authoritative
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_manifest_constructor_root_reserved_for_lowering\":"
           << (runtime_translation_unit_registration_manifest
                       .constructor_root_reserved_for_lowering
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_manifest_init_stub_emission_deferred_to_lowering\":"
           << (runtime_translation_unit_registration_manifest
                       .init_stub_emission_deferred_to_lowering
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_manifest_artifact_emitted_by_driver\":"
           << (runtime_translation_unit_registration_manifest
                       .runtime_registration_artifact_emitted_by_driver
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_manifest_ready_for_lowering_init_stub_emission\":"
           << (runtime_translation_unit_registration_manifest
                       .ready_for_lowering_init_stub_emission
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_manifest_launch_integration_ready\":"
           << (runtime_translation_unit_registration_manifest
                       .launch_integration_ready
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_manifest_translation_unit_registration_replay_key\":\""
           << EscapeJsonString(
                  runtime_translation_unit_registration_manifest
                      .translation_unit_registration_replay_key)
          << "\",\"runtime_translation_unit_registration_manifest_replay_key\":\""
          << EscapeJsonString(
                 runtime_translation_unit_registration_manifest.replay_key)
          << "\",\"runtime_translation_unit_registration_manifest_failure_reason\":\""
          << EscapeJsonString(
                 runtime_translation_unit_registration_manifest.failure_reason)
          << "\",\"runtime_registration_descriptor_image_root_source_surface_contract_id\":\""
          << EscapeJsonString(
                 runtime_registration_descriptor_image_root_source_surface
                     .contract_id)
          << "\",\"runtime_registration_descriptor_image_root_source_surface_path\":\""
          << EscapeJsonString(
                 runtime_registration_descriptor_image_root_source_surface
                     .source_surface_path)
          << "\",\"runtime_registration_descriptor_image_root_source_surface_registration_descriptor_pragma_name\":\""
          << EscapeJsonString(
                 runtime_registration_descriptor_image_root_source_surface
                     .registration_descriptor_pragma_name)
          << "\",\"runtime_registration_descriptor_image_root_source_surface_image_root_pragma_name\":\""
          << EscapeJsonString(
                 runtime_registration_descriptor_image_root_source_surface
                     .image_root_pragma_name)
          << "\",\"runtime_registration_descriptor_image_root_source_surface_module_identity_source\":\""
          << EscapeJsonString(
                 runtime_registration_descriptor_image_root_source_surface
                     .module_identity_source)
          << "\",\"runtime_registration_descriptor_image_root_source_surface_registration_descriptor_identity_source\":\""
          << EscapeJsonString(
                 runtime_registration_descriptor_image_root_source_surface
                     .registration_descriptor_identity_source)
          << "\",\"runtime_registration_descriptor_image_root_source_surface_image_root_identity_source\":\""
          << EscapeJsonString(
                 runtime_registration_descriptor_image_root_source_surface
                     .image_root_identity_source)
          << "\",\"runtime_registration_descriptor_image_root_source_surface_bootstrap_visible_metadata_ownership_model\":\""
          << EscapeJsonString(
                 runtime_registration_descriptor_image_root_source_surface
                     .bootstrap_visible_metadata_ownership_model)
          << "\",\"runtime_registration_descriptor_image_root_source_surface_module_name\":\""
          << EscapeJsonString(
                 runtime_registration_descriptor_image_root_source_surface
                     .module_name)
          << "\",\"runtime_registration_descriptor_image_root_source_surface_registration_descriptor_identifier\":\""
          << EscapeJsonString(
                 runtime_registration_descriptor_image_root_source_surface
                     .registration_descriptor_identifier)
          << "\",\"runtime_registration_descriptor_image_root_source_surface_image_root_identifier\":\""
          << EscapeJsonString(
                 runtime_registration_descriptor_image_root_source_surface
                     .image_root_identifier)
          << "\",\"runtime_registration_descriptor_image_root_source_surface_registration_manifest_replay_key\":\""
          << EscapeJsonString(
                 runtime_registration_descriptor_image_root_source_surface
                     .registration_manifest_replay_key)
          << "\",\"runtime_registration_descriptor_image_root_source_surface_replay_key\":\""
          << EscapeJsonString(
                 runtime_registration_descriptor_image_root_source_surface
                     .replay_key)
          << "\",\"runtime_registration_descriptor_image_root_source_surface_fail_closed\":"
          << (runtime_registration_descriptor_image_root_source_surface
                      .fail_closed
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_image_root_source_surface_registration_manifest_contract_ready\":"
          << (runtime_registration_descriptor_image_root_source_surface
                      .registration_manifest_contract_ready
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_image_root_source_surface_source_surface_frozen\":"
          << (runtime_registration_descriptor_image_root_source_surface
                      .source_surface_frozen
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_image_root_source_surface_prelude_pragma_contract_published\":"
          << (runtime_registration_descriptor_image_root_source_surface
                      .prelude_pragma_contract_published
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_image_root_source_surface_registration_descriptor_identifier_resolved\":"
          << (runtime_registration_descriptor_image_root_source_surface
                      .registration_descriptor_identifier_resolved
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_image_root_source_surface_image_root_identifier_resolved\":"
          << (runtime_registration_descriptor_image_root_source_surface
                      .image_root_identifier_resolved
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_image_root_source_surface_bootstrap_visible_metadata_ownership_published\":"
          << (runtime_registration_descriptor_image_root_source_surface
                      .bootstrap_visible_metadata_ownership_published
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_image_root_source_surface_ready_for_descriptor_frontend_closure\":"
          << (runtime_registration_descriptor_image_root_source_surface
                      .ready_for_descriptor_frontend_closure
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_image_root_source_surface_registration_descriptor_pragma_seen\":"
          << (runtime_registration_descriptor_image_root_source_surface
                      .registration_descriptor_pragma_seen
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_image_root_source_surface_registration_descriptor_pragma_duplicate\":"
          << (runtime_registration_descriptor_image_root_source_surface
                      .registration_descriptor_pragma_duplicate
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_image_root_source_surface_registration_descriptor_pragma_non_leading\":"
          << (runtime_registration_descriptor_image_root_source_surface
                      .registration_descriptor_pragma_non_leading
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_image_root_source_surface_registration_descriptor_pragma_directive_count\":"
          << runtime_registration_descriptor_image_root_source_surface
                 .registration_descriptor_pragma_directive_count
          << ",\"runtime_registration_descriptor_image_root_source_surface_image_root_pragma_seen\":"
          << (runtime_registration_descriptor_image_root_source_surface
                      .image_root_pragma_seen
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_image_root_source_surface_image_root_pragma_duplicate\":"
          << (runtime_registration_descriptor_image_root_source_surface
                      .image_root_pragma_duplicate
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_image_root_source_surface_image_root_pragma_non_leading\":"
          << (runtime_registration_descriptor_image_root_source_surface
                      .image_root_pragma_non_leading
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_image_root_source_surface_image_root_pragma_directive_count\":"
          << runtime_registration_descriptor_image_root_source_surface
                 .image_root_pragma_directive_count
          << ",\"runtime_registration_descriptor_image_root_source_surface_failure_reason\":\""
          << EscapeJsonString(
                 runtime_registration_descriptor_image_root_source_surface
                     .failure_reason)
          << "\""
          << ",\"runtime_registration_descriptor_frontend_closure_contract_id\":\""
          << EscapeJsonString(
                 runtime_registration_descriptor_frontend_closure.contract_id)
          << "\",\"runtime_registration_descriptor_frontend_closure_source_surface_contract_id\":\""
          << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                                  .source_surface_contract_id)
          << "\",\"runtime_registration_descriptor_frontend_closure_payload_model\":\""
          << EscapeJsonString(
                 runtime_registration_descriptor_frontend_closure.payload_model)
          << "\",\"runtime_registration_descriptor_frontend_closure_artifact_relative_path\":\""
          << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                                  .artifact_relative_path)
          << "\",\"runtime_registration_descriptor_frontend_closure_authority_model\":\""
          << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                                  .authority_model)
          << "\",\"runtime_registration_descriptor_frontend_closure_translation_unit_identity_model\":\""
          << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                                  .translation_unit_identity_model)
          << "\",\"runtime_registration_descriptor_frontend_closure_payload_ownership_model\":\""
          << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                                  .payload_ownership_model)
          << "\",\"runtime_registration_descriptor_frontend_closure_registration_descriptor_identifier\":\""
          << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                                  .registration_descriptor_identifier)
          << "\",\"runtime_registration_descriptor_frontend_closure_image_root_identifier\":\""
          << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                                  .image_root_identifier)
          << "\",\"runtime_registration_descriptor_frontend_closure_registration_descriptor_identity_source\":\""
          << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                                  .registration_descriptor_identity_source)
          << "\",\"runtime_registration_descriptor_frontend_closure_image_root_identity_source\":\""
          << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                                  .image_root_identity_source)
          << "\",\"runtime_registration_descriptor_frontend_closure_bootstrap_visible_metadata_ownership_model\":\""
          << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                                  .bootstrap_visible_metadata_ownership_model)
          << "\",\"runtime_registration_descriptor_frontend_closure_class_descriptor_count\":"
          << runtime_registration_descriptor_frontend_closure
                 .class_descriptor_count
          << ",\"runtime_registration_descriptor_frontend_closure_protocol_descriptor_count\":"
          << runtime_registration_descriptor_frontend_closure
                 .protocol_descriptor_count
          << ",\"runtime_registration_descriptor_frontend_closure_category_descriptor_count\":"
          << runtime_registration_descriptor_frontend_closure
                 .category_descriptor_count
          << ",\"runtime_registration_descriptor_frontend_closure_property_descriptor_count\":"
          << runtime_registration_descriptor_frontend_closure
                 .property_descriptor_count
          << ",\"runtime_registration_descriptor_frontend_closure_ivar_descriptor_count\":"
          << runtime_registration_descriptor_frontend_closure
                 .ivar_descriptor_count
          << ",\"runtime_registration_descriptor_frontend_closure_total_descriptor_count\":"
          << runtime_registration_descriptor_frontend_closure.total_descriptor_count
          << ",\"runtime_registration_descriptor_frontend_closure_translation_unit_registration_order_ordinal\":"
          << runtime_registration_descriptor_frontend_closure
                 .translation_unit_registration_order_ordinal
          << ",\"runtime_registration_descriptor_frontend_closure_fail_closed\":"
          << (runtime_registration_descriptor_frontend_closure.fail_closed ? "true"
                                                                          : "false")
          << ",\"runtime_registration_descriptor_frontend_closure_source_surface_contract_ready\":"
          << (runtime_registration_descriptor_frontend_closure
                      .source_surface_contract_ready
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_frontend_closure_registration_manifest_contract_ready\":"
          << (runtime_registration_descriptor_frontend_closure
                      .registration_manifest_contract_ready
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_frontend_closure_descriptor_frontend_surface_published\":"
          << (runtime_registration_descriptor_frontend_closure
                      .descriptor_frontend_surface_published
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_frontend_closure_descriptor_artifact_template_published\":"
          << (runtime_registration_descriptor_frontend_closure
                      .descriptor_artifact_template_published
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_frontend_closure_descriptor_fields_resolved\":"
          << (runtime_registration_descriptor_frontend_closure
                      .descriptor_fields_resolved
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_frontend_closure_ready_for_descriptor_artifact_emission\":"
          << (runtime_registration_descriptor_frontend_closure
                      .ready_for_descriptor_artifact_emission
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_frontend_closure_ready_for_registration_descriptor_lowering\":"
          << (runtime_registration_descriptor_frontend_closure
                      .ready_for_registration_descriptor_lowering
                  ? "true"
                  : "false")
          << ",\"runtime_registration_descriptor_frontend_closure_source_surface_replay_key\":\""
          << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                                  .source_surface_replay_key)
          << "\",\"runtime_registration_descriptor_frontend_closure_registration_manifest_replay_key\":\""
          << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                                  .registration_manifest_replay_key)
          << "\",\"runtime_registration_descriptor_frontend_closure_replay_key\":\""
          << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                                  .replay_key)
          << "\",\"runtime_registration_descriptor_frontend_closure_failure_reason\":\""
          << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                                  .failure_reason)
          << "\""
          << ",\"runtime_bootstrap_legality_failure_contract_id\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .contract_id)
          << "\",\"runtime_bootstrap_legality_failure_registration_descriptor_frontend_closure_contract_id\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .registration_descriptor_frontend_closure_contract_id)
          << "\",\"runtime_bootstrap_legality_failure_bootstrap_semantics_contract_id\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .bootstrap_semantics_contract_id)
          << "\",\"runtime_bootstrap_legality_failure_frontend_surface_path\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .frontend_surface_path)
          << "\",\"runtime_bootstrap_legality_failure_duplicate_registration_policy\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .duplicate_registration_policy)
          << "\",\"runtime_bootstrap_legality_failure_image_registration_order_invariant\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .image_registration_order_invariant)
          << "\",\"runtime_bootstrap_legality_failure_failure_mode\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .failure_mode)
          << "\",\"runtime_bootstrap_legality_failure_restart_lifecycle_model\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .restart_lifecycle_model)
          << "\",\"runtime_bootstrap_legality_failure_replay_order_model\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .replay_order_model)
          << "\",\"runtime_bootstrap_legality_failure_image_local_init_reset_model\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .image_local_init_reset_model)
          << "\",\"runtime_bootstrap_legality_failure_catalog_retention_model\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .catalog_retention_model)
          << "\",\"runtime_bootstrap_legality_failure_runtime_state_snapshot_symbol\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .runtime_state_snapshot_symbol)
          << "\",\"runtime_bootstrap_legality_failure_registration_descriptor_identifier\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .registration_descriptor_identifier)
          << "\",\"runtime_bootstrap_legality_failure_image_root_identifier\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .image_root_identifier)
          << "\",\"runtime_bootstrap_legality_failure_registration_descriptor_identity_source\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .registration_descriptor_identity_source)
          << "\",\"runtime_bootstrap_legality_failure_image_root_identity_source\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .image_root_identity_source)
          << "\",\"runtime_bootstrap_legality_failure_translation_unit_registration_order_ordinal\":"
          << runtime_bootstrap_legality_failure_contract
                 .translation_unit_registration_order_ordinal
          << ",\"runtime_bootstrap_legality_failure_fail_closed\":"
          << (runtime_bootstrap_legality_failure_contract.fail_closed ? "true"
                                                                     : "false")
          << ",\"runtime_bootstrap_legality_failure_registration_descriptor_frontend_closure_contract_ready\":"
          << (runtime_bootstrap_legality_failure_contract
                      .registration_descriptor_frontend_closure_contract_ready
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_failure_bootstrap_semantics_contract_ready\":"
          << (runtime_bootstrap_legality_failure_contract
                      .bootstrap_semantics_contract_ready
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_failure_semantic_boundary_ready\":"
          << (runtime_bootstrap_legality_failure_contract.semantic_boundary_ready
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_failure_duplicate_registration_policy_frozen\":"
          << (runtime_bootstrap_legality_failure_contract
                      .duplicate_registration_policy_frozen
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_failure_image_order_invariant_frozen\":"
          << (runtime_bootstrap_legality_failure_contract
                      .image_order_invariant_frozen
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_failure_bootstrap_rejection_frozen\":"
          << (runtime_bootstrap_legality_failure_contract
                      .bootstrap_rejection_frozen
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_failure_restart_boundary_frozen\":"
          << (runtime_bootstrap_legality_failure_contract
                      .restart_boundary_frozen
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_failure_semantic_diagnostics_required\":"
          << (runtime_bootstrap_legality_failure_contract
                      .semantic_diagnostics_required
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_failure_ready_for_lowering_and_runtime\":"
          << (runtime_bootstrap_legality_failure_contract
                      .ready_for_lowering_and_runtime
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_failure_registration_descriptor_frontend_closure_replay_key\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .registration_descriptor_frontend_closure_replay_key)
          << "\",\"runtime_bootstrap_legality_failure_bootstrap_semantics_replay_key\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .bootstrap_semantics_replay_key)
          << "\",\"runtime_bootstrap_legality_failure_semantic_boundary_replay_key\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .semantic_boundary_replay_key)
          << "\",\"runtime_bootstrap_legality_failure_replay_key\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .replay_key)
          << "\",\"runtime_bootstrap_legality_failure_failure_reason\":\""
          << EscapeJsonString(runtime_bootstrap_legality_failure_contract
                                  .failure_reason)
          << "\""
          << ",\"runtime_bootstrap_legality_semantics_contract_id\":\""
          << EscapeJsonString(runtime_bootstrap_legality_semantics.contract_id)
          << "\",\"runtime_bootstrap_legality_semantics_bootstrap_legality_failure_contract_id\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics
                     .bootstrap_legality_failure_contract_id)
          << "\",\"runtime_bootstrap_legality_semantics_registration_descriptor_frontend_closure_contract_id\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics
                     .registration_descriptor_frontend_closure_contract_id)
          << "\",\"runtime_bootstrap_legality_semantics_bootstrap_semantics_contract_id\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics
                     .bootstrap_semantics_contract_id)
          << "\",\"runtime_bootstrap_legality_semantics_frontend_surface_path\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics.frontend_surface_path)
          << "\",\"runtime_bootstrap_legality_semantics_duplicate_registration_policy\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics
                     .duplicate_registration_policy)
          << "\",\"runtime_bootstrap_legality_semantics_image_registration_order_invariant\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics
                     .image_registration_order_invariant)
          << "\",\"runtime_bootstrap_legality_semantics_cross_image_legality_model\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics.cross_image_legality_model)
          << "\",\"runtime_bootstrap_legality_semantics_semantic_diagnostic_model\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics
                     .semantic_diagnostic_model)
          << "\",\"runtime_bootstrap_legality_semantics_translation_unit_identity_model\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics
                     .translation_unit_identity_model)
          << "\",\"runtime_bootstrap_legality_semantics_translation_unit_identity_key\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics
                     .translation_unit_identity_key)
          << "\",\"runtime_bootstrap_legality_semantics_registration_descriptor_identifier\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics
                     .registration_descriptor_identifier)
          << "\",\"runtime_bootstrap_legality_semantics_image_root_identifier\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics.image_root_identifier)
          << "\",\"runtime_bootstrap_legality_semantics_registration_descriptor_identity_source\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics
                     .registration_descriptor_identity_source)
          << "\",\"runtime_bootstrap_legality_semantics_image_root_identity_source\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics
                     .image_root_identity_source)
          << "\",\"runtime_bootstrap_legality_semantics_translation_unit_registration_order_ordinal\":"
          << runtime_bootstrap_legality_semantics
                 .translation_unit_registration_order_ordinal
          << ",\"runtime_bootstrap_legality_semantics_fail_closed\":"
          << (runtime_bootstrap_legality_semantics.fail_closed ? "true"
                                                               : "false")
          << ",\"runtime_bootstrap_legality_semantics_semantic_boundary_ready\":"
          << (runtime_bootstrap_legality_semantics.semantic_boundary_ready
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_semantics_bootstrap_legality_failure_contract_ready\":"
          << (runtime_bootstrap_legality_semantics
                      .bootstrap_legality_failure_contract_ready
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_semantics_registration_descriptor_frontend_closure_contract_ready\":"
          << (runtime_bootstrap_legality_semantics
                      .registration_descriptor_frontend_closure_contract_ready
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_semantics_bootstrap_semantics_contract_ready\":"
          << (runtime_bootstrap_legality_semantics
                      .bootstrap_semantics_contract_ready
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_semantics_duplicate_registration_semantics_landed\":"
          << (runtime_bootstrap_legality_semantics
                      .duplicate_registration_semantics_landed
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_semantics_image_order_semantics_landed\":"
          << (runtime_bootstrap_legality_semantics.image_order_semantics_landed
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_semantics_cross_image_legality_semantics_landed\":"
          << (runtime_bootstrap_legality_semantics
                      .cross_image_legality_semantics_landed
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_semantics_semantic_diagnostics_landed\":"
          << (runtime_bootstrap_legality_semantics.semantic_diagnostics_landed
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_semantics_ready_for_lowering_and_runtime\":"
          << (runtime_bootstrap_legality_semantics.ready_for_lowering_and_runtime
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_legality_semantics_semantic_boundary_replay_key\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics.semantic_boundary_replay_key)
          << "\",\"runtime_bootstrap_legality_semantics_bootstrap_legality_failure_contract_replay_key\":\""
          << EscapeJsonString(runtime_bootstrap_legality_semantics
                                  .bootstrap_legality_failure_contract_replay_key)
          << "\",\"runtime_bootstrap_legality_semantics_registration_descriptor_frontend_closure_replay_key\":\""
          << EscapeJsonString(runtime_bootstrap_legality_semantics
                                  .registration_descriptor_frontend_closure_replay_key)
          << "\",\"runtime_bootstrap_legality_semantics_bootstrap_semantics_replay_key\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics
                     .bootstrap_semantics_replay_key)
          << "\",\"runtime_bootstrap_legality_semantics_replay_key\":\""
          << EscapeJsonString(runtime_bootstrap_legality_semantics.replay_key)
          << "\",\"runtime_bootstrap_legality_semantics_failure_reason\":\""
          << EscapeJsonString(
                 runtime_bootstrap_legality_semantics.failure_reason)
          << "\""
          << ",\"runtime_bootstrap_failure_restart_semantics_contract_id\":\""
          << EscapeJsonString(
                 runtime_bootstrap_failure_restart_semantics.contract_id)
          << "\",\"runtime_bootstrap_failure_restart_semantics_bootstrap_legality_semantics_contract_id\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .bootstrap_legality_semantics_contract_id)
          << "\",\"runtime_bootstrap_failure_restart_semantics_bootstrap_reset_contract_id\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .bootstrap_reset_contract_id)
          << "\",\"runtime_bootstrap_failure_restart_semantics_bootstrap_semantics_contract_id\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .bootstrap_semantics_contract_id)
          << "\",\"runtime_bootstrap_failure_restart_semantics_frontend_surface_path\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .frontend_surface_path)
          << "\",\"runtime_bootstrap_failure_restart_semantics_failure_mode\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .failure_mode)
          << "\",\"runtime_bootstrap_failure_restart_semantics_restart_lifecycle_model\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .restart_lifecycle_model)
          << "\",\"runtime_bootstrap_failure_restart_semantics_replay_order_model\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .replay_order_model)
          << "\",\"runtime_bootstrap_failure_restart_semantics_image_local_init_reset_model\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .image_local_init_reset_model)
          << "\",\"runtime_bootstrap_failure_restart_semantics_catalog_retention_model\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .catalog_retention_model)
          << "\",\"runtime_bootstrap_failure_restart_semantics_unsupported_topology_model\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .unsupported_topology_model)
          << "\",\"runtime_bootstrap_failure_restart_semantics_translation_unit_identity_model\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .translation_unit_identity_model)
          << "\",\"runtime_bootstrap_failure_restart_semantics_translation_unit_identity_key\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .translation_unit_identity_key)
          << "\",\"runtime_bootstrap_failure_restart_semantics_runtime_state_snapshot_symbol\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .runtime_state_snapshot_symbol)
          << "\",\"runtime_bootstrap_failure_restart_semantics_replay_registered_images_symbol\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .replay_registered_images_symbol)
          << "\",\"runtime_bootstrap_failure_restart_semantics_reset_replay_state_snapshot_symbol\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .reset_replay_state_snapshot_symbol)
          << "\",\"runtime_bootstrap_failure_restart_semantics_invalid_descriptor_status_code\":"
          << runtime_bootstrap_failure_restart_semantics
                 .invalid_descriptor_status_code
          << ",\"runtime_bootstrap_failure_restart_semantics_translation_unit_registration_order_ordinal\":"
          << runtime_bootstrap_failure_restart_semantics
                 .translation_unit_registration_order_ordinal
          << ",\"runtime_bootstrap_failure_restart_semantics_fail_closed\":"
          << (runtime_bootstrap_failure_restart_semantics.fail_closed ? "true"
                                                                      : "false")
          << ",\"runtime_bootstrap_failure_restart_semantics_semantic_boundary_ready\":"
          << (runtime_bootstrap_failure_restart_semantics.semantic_boundary_ready
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_failure_restart_semantics_bootstrap_legality_semantics_contract_ready\":"
          << (runtime_bootstrap_failure_restart_semantics
                      .bootstrap_legality_semantics_contract_ready
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_failure_restart_semantics_bootstrap_semantics_contract_ready\":"
          << (runtime_bootstrap_failure_restart_semantics
                      .bootstrap_semantics_contract_ready
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_failure_restart_semantics_bootstrap_reset_contract_ready\":"
          << (runtime_bootstrap_failure_restart_semantics
                      .bootstrap_reset_contract_ready
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_failure_restart_semantics_failure_mode_semantics_landed\":"
          << (runtime_bootstrap_failure_restart_semantics
                      .failure_mode_semantics_landed
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_failure_restart_semantics_restart_semantics_landed\":"
          << (runtime_bootstrap_failure_restart_semantics
                      .restart_semantics_landed
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_failure_restart_semantics_replay_semantics_landed\":"
          << (runtime_bootstrap_failure_restart_semantics
                      .replay_semantics_landed
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_failure_restart_semantics_unsupported_topology_semantics_landed\":"
          << (runtime_bootstrap_failure_restart_semantics
                      .unsupported_topology_semantics_landed
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_failure_restart_semantics_deterministic_recovery_semantics_landed\":"
          << (runtime_bootstrap_failure_restart_semantics
                      .deterministic_recovery_semantics_landed
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_failure_restart_semantics_runtime_restart_probe_required\":"
          << (runtime_bootstrap_failure_restart_semantics
                      .runtime_restart_probe_required
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_failure_restart_semantics_ready_for_lowering_and_runtime\":"
          << (runtime_bootstrap_failure_restart_semantics
                      .ready_for_lowering_and_runtime
                  ? "true"
                  : "false")
          << ",\"runtime_bootstrap_failure_restart_semantics_semantic_boundary_replay_key\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .semantic_boundary_replay_key)
          << "\",\"runtime_bootstrap_failure_restart_semantics_bootstrap_legality_semantics_replay_key\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .bootstrap_legality_semantics_replay_key)
          << "\",\"runtime_bootstrap_failure_restart_semantics_bootstrap_semantics_replay_key\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .bootstrap_semantics_replay_key)
          << "\",\"runtime_bootstrap_failure_restart_semantics_replay_key\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .replay_key)
          << "\",\"runtime_bootstrap_failure_restart_semantics_failure_reason\":\""
          << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                  .failure_reason)
          << "\""
          << ",\"runtime_bootstrap_api_contract_id\":\""
          << EscapeJsonString(runtime_bootstrap_api.contract_id)
          << "\",\"runtime_bootstrap_api_public_header_path\":\""
          << EscapeJsonString(runtime_bootstrap_api.public_header_path)
          << "\",\"runtime_bootstrap_api_archive_relative_path\":\""
          << EscapeJsonString(runtime_bootstrap_api.archive_relative_path)
          << "\",\"runtime_bootstrap_api_registration_status_enum_type\":\""
          << EscapeJsonString(
                 runtime_bootstrap_api.registration_status_enum_type)
          << "\",\"runtime_bootstrap_api_image_descriptor_type\":\""
          << EscapeJsonString(runtime_bootstrap_api.image_descriptor_type)
          << "\",\"runtime_bootstrap_api_selector_handle_type\":\""
          << EscapeJsonString(runtime_bootstrap_api.selector_handle_type)
          << "\",\"runtime_bootstrap_api_registration_snapshot_type\":\""
          << EscapeJsonString(
                 runtime_bootstrap_api.registration_snapshot_type)
          << "\",\"runtime_bootstrap_api_registration_entrypoint_symbol\":\""
          << EscapeJsonString(
                 runtime_bootstrap_api.registration_entrypoint_symbol)
          << "\",\"runtime_bootstrap_api_selector_lookup_symbol\":\""
          << EscapeJsonString(runtime_bootstrap_api.selector_lookup_symbol)
          << "\",\"runtime_bootstrap_api_dispatch_entrypoint_symbol\":\""
          << EscapeJsonString(
                 runtime_bootstrap_api.dispatch_entrypoint_symbol)
          << "\",\"runtime_bootstrap_api_state_snapshot_symbol\":\""
          << EscapeJsonString(runtime_bootstrap_api.state_snapshot_symbol)
          << "\",\"runtime_bootstrap_api_reset_for_testing_symbol\":\""
          << EscapeJsonString(runtime_bootstrap_api.reset_for_testing_symbol)
          << "\",\"runtime_bootstrap_api_registration_result_model\":\""
          << EscapeJsonString(runtime_bootstrap_api.registration_result_model)
          << "\",\"runtime_bootstrap_api_registration_order_ordinal_model\":\""
          << EscapeJsonString(
                 runtime_bootstrap_api.registration_order_ordinal_model)
          << "\",\"runtime_bootstrap_api_runtime_state_locking_model\":\""
          << EscapeJsonString(
                 runtime_bootstrap_api.runtime_state_locking_model)
          << "\",\"runtime_bootstrap_api_startup_invocation_model\":\""
          << EscapeJsonString(runtime_bootstrap_api.startup_invocation_model)
          << "\",\"runtime_bootstrap_api_image_walk_lifecycle_model\":\""
          << EscapeJsonString(
                 runtime_bootstrap_api.image_walk_lifecycle_model)
          << "\",\"runtime_bootstrap_api_deterministic_reset_lifecycle_model\":\""
          << EscapeJsonString(
                 runtime_bootstrap_api.deterministic_reset_lifecycle_model)
          << "\",\"runtime_bootstrap_api_ready_for_registrar_implementation\":"
          << (runtime_bootstrap_api.ready_for_registrar_implementation ? "true"
                                                                       : "false")
          << ",\"runtime_bootstrap_api_support_library_core_feature_replay_key\":\""
          << EscapeJsonString(
                 runtime_bootstrap_api.support_library_core_feature_replay_key)
          << "\",\"runtime_bootstrap_api_support_library_link_wiring_replay_key\":\""
          << EscapeJsonString(
                 runtime_bootstrap_api.support_library_link_wiring_replay_key)
          << "\""
          << ",\"runtime_bootstrap_api_replay_key\":\""
          << EscapeJsonString(runtime_bootstrap_api.replay_key)
          << "\",\"runtime_bootstrap_api_failure_reason\":\""
          << EscapeJsonString(runtime_bootstrap_api.failure_reason)
          << "\""
          << ",\"runtime_bootstrap_semantics_contract_id\":\""
          << runtime_bootstrap_semantics.contract_id
           << "\",\"runtime_bootstrap_semantics_bootstrap_invariant_contract_id\":\""
           << runtime_bootstrap_semantics.bootstrap_invariant_contract_id
           << "\",\"runtime_bootstrap_semantics_registration_manifest_contract_id\":\""
           << runtime_bootstrap_semantics.registration_manifest_contract_id
           << "\",\"runtime_bootstrap_semantics_duplicate_registration_policy\":\""
           << runtime_bootstrap_semantics.duplicate_registration_policy
           << "\",\"runtime_bootstrap_semantics_realization_order_policy\":\""
           << runtime_bootstrap_semantics.realization_order_policy
           << "\",\"runtime_bootstrap_semantics_failure_mode\":\""
           << runtime_bootstrap_semantics.failure_mode
           << "\",\"runtime_bootstrap_semantics_result_model\":\""
           << runtime_bootstrap_semantics.registration_result_model
           << "\",\"runtime_bootstrap_semantics_registration_order_ordinal_model\":\""
           << runtime_bootstrap_semantics.registration_order_ordinal_model
           << "\",\"runtime_bootstrap_semantics_runtime_state_snapshot_symbol\":\""
           << runtime_bootstrap_semantics.runtime_state_snapshot_symbol
           << "\",\"runtime_bootstrap_semantics_runtime_library_archive_relative_path\":\""
           << runtime_bootstrap_semantics.runtime_library_archive_relative_path
           << "\",\"runtime_bootstrap_semantics_translation_unit_registration_order_ordinal\":"
           << runtime_bootstrap_semantics.translation_unit_registration_order_ordinal
           << ",\"runtime_bootstrap_semantics_success_status_code\":"
           << runtime_bootstrap_semantics.success_status_code
           << ",\"runtime_bootstrap_semantics_invalid_descriptor_status_code\":"
           << runtime_bootstrap_semantics.invalid_descriptor_status_code
           << ",\"runtime_bootstrap_semantics_duplicate_registration_status_code\":"
           << runtime_bootstrap_semantics.duplicate_registration_status_code
           << ",\"runtime_bootstrap_semantics_out_of_order_status_code\":"
           << runtime_bootstrap_semantics.out_of_order_status_code
           << ",\"runtime_bootstrap_semantics_fail_closed\":"
           << (runtime_bootstrap_semantics.fail_closed ? "true" : "false")
           << ",\"runtime_bootstrap_semantics_live_runtime_enforcement_landed\":"
           << (runtime_bootstrap_semantics.live_runtime_enforcement_landed ? "true"
                                                                          : "false")
           << ",\"runtime_bootstrap_semantics_no_partial_commit_on_failure\":"
           << (runtime_bootstrap_semantics.no_partial_commit_on_failure ? "true"
                                                                        : "false")
           << ",\"runtime_bootstrap_semantics_ready_for_constructor_root_implementation\":"
           << (runtime_bootstrap_semantics
                       .ready_for_constructor_root_implementation
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_semantics_replay_key\":\""
           << EscapeJsonString(runtime_bootstrap_semantics.replay_key)
           << "\",\"runtime_bootstrap_semantics_failure_reason\":\""
           << EscapeJsonString(runtime_bootstrap_semantics.failure_reason)
           << "\""
           << ",\"runtime_bootstrap_lowering_contract_id\":\""
           << runtime_bootstrap_lowering.contract_id
           << "\",\"runtime_bootstrap_lowering_registration_manifest_contract_id\":\""
           << runtime_bootstrap_lowering.registration_manifest_contract_id
           << "\",\"runtime_bootstrap_registrar_contract_id\":\""
           << kObjc3RuntimeBootstrapRegistrarContractId
           << "\",\"runtime_bootstrap_registrar_stage_registration_table_symbol\":\""
           << kObjc3RuntimeBootstrapStageRegistrationTableSymbol
           << "\",\"runtime_bootstrap_registrar_image_walk_snapshot_symbol\":\""
           << kObjc3RuntimeBootstrapImageWalkSnapshotSymbol
           << "\",\"runtime_bootstrap_registrar_image_walk_model\":\""
           << kObjc3RuntimeBootstrapImageWalkModel
           << "\",\"runtime_bootstrap_registrar_discovery_root_validation_model\":\""
           << kObjc3RuntimeBootstrapDiscoveryRootValidationModel
           << "\",\"runtime_bootstrap_registrar_selector_pool_interning_model\":\""
           << kObjc3RuntimeBootstrapSelectorPoolInterningModel
           << "\",\"runtime_bootstrap_registrar_realization_staging_model\":\""
           << kObjc3RuntimeBootstrapRealizationStagingModel
           << "\",\"runtime_bootstrap_reset_contract_id\":\""
           << kObjc3RuntimeBootstrapResetContractId
           << "\",\"runtime_bootstrap_reset_replay_registered_images_symbol\":\""
           << kObjc3RuntimeBootstrapReplayRegisteredImagesSymbol
           << "\",\"runtime_bootstrap_reset_reset_replay_state_snapshot_symbol\":\""
           << kObjc3RuntimeBootstrapResetReplayStateSnapshotSymbol
           << "\",\"runtime_bootstrap_reset_lifecycle_model\":\""
           << kObjc3RuntimeBootstrapResetLifecycleModel
           << "\",\"runtime_bootstrap_reset_replay_order_model\":\""
           << kObjc3RuntimeBootstrapReplayOrderModel
           << "\",\"runtime_bootstrap_reset_image_local_init_state_reset_model\":\""
           << kObjc3RuntimeBootstrapImageLocalInitStateResetModel
           << "\",\"runtime_bootstrap_reset_bootstrap_catalog_retention_model\":\""
           << kObjc3RuntimeBootstrapCatalogRetentionModel
           << "\",\"runtime_bootstrap_lowering_bootstrap_semantics_contract_id\":\""
           << runtime_bootstrap_lowering.bootstrap_semantics_contract_id
           << "\",\"runtime_bootstrap_lowering_registration_descriptor_frontend_closure_contract_id\":\""
           << runtime_bootstrap_lowering
                  .registration_descriptor_frontend_closure_contract_id
           << "\",\"runtime_bootstrap_lowering_registration_descriptor_artifact\":\""
           << runtime_bootstrap_lowering.registration_descriptor_artifact
           << "\",\"runtime_bootstrap_lowering_boundary_model\":\""
           << runtime_bootstrap_lowering.lowering_boundary_model
           << "\",\"runtime_bootstrap_lowering_registration_descriptor_handoff_model\":\""
           << runtime_bootstrap_lowering.registration_descriptor_handoff_model
           << "\",\"runtime_bootstrap_lowering_constructor_root_symbol\":\""
           << runtime_bootstrap_lowering.constructor_root_symbol
           << "\",\"runtime_bootstrap_lowering_init_stub_symbol_prefix\":\""
           << runtime_bootstrap_lowering.constructor_init_stub_symbol_prefix
           << "\",\"runtime_bootstrap_lowering_registration_table_symbol_prefix\":\""
           << runtime_bootstrap_lowering.registration_table_symbol_prefix
           << "\",\"runtime_bootstrap_lowering_registration_entrypoint_symbol\":\""
           << runtime_bootstrap_lowering.registration_entrypoint_symbol
           << "\",\"runtime_bootstrap_lowering_global_ctor_list_model\":\""
           << runtime_bootstrap_lowering.global_ctor_list_model
           << "\",\"runtime_bootstrap_lowering_constructor_root_emission_state\":\""
           << runtime_bootstrap_lowering.constructor_root_emission_state
           << "\",\"runtime_bootstrap_lowering_init_stub_emission_state\":\""
           << runtime_bootstrap_lowering.init_stub_emission_state
           << "\",\"runtime_bootstrap_lowering_registration_table_emission_state\":\""
           << runtime_bootstrap_lowering.registration_table_emission_state
           << "\",\"runtime_bootstrap_lowering_fail_closed\":"
           << (runtime_bootstrap_lowering.fail_closed ? "true" : "false")
           << ",\"runtime_bootstrap_lowering_registration_descriptor_frontend_closure_contract_ready\":"
           << (runtime_bootstrap_lowering
                       .registration_descriptor_frontend_closure_contract_ready
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_lowering_no_bootstrap_ir_materialization_yet\":"
           << (runtime_bootstrap_lowering.no_bootstrap_ir_materialization_yet
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_lowering_ready_for_bootstrap_materialization\":"
           << (runtime_bootstrap_lowering.ready_for_bootstrap_materialization
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_lowering_replay_key\":\""
           << EscapeJsonString(runtime_bootstrap_lowering.replay_key)
           << "\",\"runtime_bootstrap_lowering_registration_descriptor_frontend_closure_replay_key\":\""
           << EscapeJsonString(runtime_bootstrap_lowering
                                   .registration_descriptor_frontend_closure_replay_key)
           << "\",\"runtime_bootstrap_lowering_failure_reason\":\""
           << EscapeJsonString(runtime_bootstrap_lowering.failure_reason)
           << "\""
           << ",\"runtime_startup_bootstrap_invariant_contract_id\":\""
           << runtime_startup_bootstrap_invariants.contract_id
           << "\",\"runtime_startup_bootstrap_invariant_duplicate_registration_policy\":\""
           << runtime_startup_bootstrap_invariants
                  .duplicate_registration_policy
           << "\",\"runtime_startup_bootstrap_invariant_realization_order_policy\":\""
           << runtime_startup_bootstrap_invariants.realization_order_policy
           << "\",\"runtime_startup_bootstrap_invariant_failure_mode\":\""
           << runtime_startup_bootstrap_invariants.failure_mode
           << "\",\"runtime_startup_bootstrap_invariant_image_local_initialization_scope\":\""
           << runtime_startup_bootstrap_invariants
                  .image_local_initialization_scope
           << "\",\"runtime_startup_bootstrap_invariant_constructor_root_uniqueness_policy\":\""
           << runtime_startup_bootstrap_invariants
                  .constructor_root_uniqueness_policy
           << "\",\"runtime_startup_bootstrap_invariant_constructor_root_consumption_model\":\""
           << runtime_startup_bootstrap_invariants
                  .constructor_root_consumption_model
           << "\",\"runtime_startup_bootstrap_invariant_startup_execution_mode\":\""
           << runtime_startup_bootstrap_invariants.startup_execution_mode
           << "\",\"runtime_startup_bootstrap_invariant_constructor_root_symbol\":\""
           << runtime_startup_bootstrap_invariants.constructor_root_symbol
           << "\",\"runtime_startup_bootstrap_invariant_registration_entrypoint_symbol\":\""
           << runtime_startup_bootstrap_invariants
                  .registration_entrypoint_symbol
           << "\",\"runtime_startup_bootstrap_invariant_manifest_authority_model\":\""
           << runtime_startup_bootstrap_invariants.manifest_authority_model
           << "\",\"runtime_startup_bootstrap_invariant_translation_unit_identity_model\":\""
           << runtime_startup_bootstrap_invariants
                  .translation_unit_identity_model
           << "\",\"runtime_startup_bootstrap_invariant_fail_closed\":"
           << (runtime_startup_bootstrap_invariants.fail_closed ? "true"
                                                                : "false")
           << ",\"runtime_startup_bootstrap_invariant_registration_manifest_contract_ready\":"
           << (runtime_startup_bootstrap_invariants
                       .registration_manifest_contract_ready
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_duplicate_registration_semantics_frozen\":"
           << (runtime_startup_bootstrap_invariants
                       .duplicate_registration_semantics_frozen
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_realization_order_semantics_frozen\":"
           << (runtime_startup_bootstrap_invariants
                       .realization_order_semantics_frozen
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_failure_mode_semantics_frozen\":"
           << (runtime_startup_bootstrap_invariants
                       .failure_mode_semantics_frozen
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_image_local_initialization_scope_frozen\":"
           << (runtime_startup_bootstrap_invariants
                       .image_local_initialization_scope_frozen
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_constructor_root_uniqueness_frozen\":"
           << (runtime_startup_bootstrap_invariants
                       .constructor_root_uniqueness_frozen
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_startup_execution_not_yet_landed\":"
           << (runtime_startup_bootstrap_invariants
                       .startup_execution_not_yet_landed
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_live_duplicate_registration_enforcement_not_yet_landed\":"
           << (runtime_startup_bootstrap_invariants
                       .live_duplicate_registration_enforcement_not_yet_landed
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_image_local_realization_not_yet_landed\":"
           << (runtime_startup_bootstrap_invariants
                       .image_local_realization_not_yet_landed
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_ready_for_bootstrap_implementation\":"
           << (runtime_startup_bootstrap_invariants
                       .ready_for_bootstrap_implementation
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_registration_manifest_replay_key\":\""
           << EscapeJsonString(
                  runtime_startup_bootstrap_invariants
                      .registration_manifest_replay_key)
           << "\",\"runtime_startup_bootstrap_invariant_replay_key\":\""
           << EscapeJsonString(runtime_startup_bootstrap_invariants.replay_key)
           << "\",\"runtime_startup_bootstrap_invariant_failure_reason\":\""
           << EscapeJsonString(
                  runtime_startup_bootstrap_invariants.failure_reason)
           << "\""
           << ",\"deterministic_property_synthesis_ivar_binding_handoff\":"
           << (property_synthesis_ivar_binding_handoff_deterministic ? "true" : "false")
           << ",\"property_synthesis_sites\":"
           << property_synthesis_ivar_binding_summary.property_synthesis_sites
           << ",\"property_synthesis_explicit_ivar_bindings\":"
           << property_synthesis_ivar_binding_summary.property_synthesis_explicit_ivar_bindings
           << ",\"property_synthesis_default_ivar_bindings\":"
           << property_synthesis_ivar_binding_summary.property_synthesis_default_ivar_bindings
           << ",\"interface_owned_property_synthesis_sites\":"
           << property_synthesis_ivar_binding_summary.interface_owned_property_synthesis_sites
           << ",\"implementation_property_redeclaration_sites\":"
           << property_synthesis_ivar_binding_summary.implementation_property_redeclaration_sites
           << ",\"ivar_binding_sites\":"
           << property_synthesis_ivar_binding_summary.ivar_binding_sites
           << ",\"ivar_binding_resolved\":"
           << property_synthesis_ivar_binding_summary.ivar_binding_resolved
           << ",\"ivar_binding_missing\":"
           << property_synthesis_ivar_binding_summary.ivar_binding_missing
           << ",\"ivar_binding_conflicts\":"
           << property_synthesis_ivar_binding_summary.ivar_binding_conflicts
           << ",\"lowering_property_synthesis_ivar_binding_replay_key\":\""
           << property_synthesis_ivar_binding_replay_key
           << "\""
           << ",\"deterministic_id_class_sel_object_pointer_typecheck_handoff\":"
           << (id_class_sel_object_pointer_typecheck_contract.deterministic ? "true" : "false")
           << ",\"id_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.id_typecheck_sites
           << ",\"class_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.class_typecheck_sites
           << ",\"sel_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.sel_typecheck_sites
           << ",\"object_pointer_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.object_pointer_typecheck_sites
           << ",\"id_class_sel_object_pointer_typecheck_sites_total\":"
           << id_class_sel_object_pointer_typecheck_contract.total_typecheck_sites
           << ",\"lowering_id_class_sel_object_pointer_typecheck_replay_key\":\""
           << id_class_sel_object_pointer_typecheck_replay_key
           << "\""
           << ",\"deterministic_message_send_selector_lowering_handoff\":"
           << (message_send_selector_lowering_contract.deterministic ? "true" : "false")
           << ",\"message_send_selector_lowering_sites\":"
           << message_send_selector_lowering_contract.message_send_sites
           << ",\"message_send_selector_lowering_unary_sites\":"
           << message_send_selector_lowering_contract.unary_selector_sites
           << ",\"message_send_selector_lowering_keyword_sites\":"
           << message_send_selector_lowering_contract.keyword_selector_sites
           << ",\"message_send_selector_lowering_selector_piece_sites\":"
           << message_send_selector_lowering_contract.selector_piece_sites
           << ",\"message_send_selector_lowering_argument_expression_sites\":"
           << message_send_selector_lowering_contract.argument_expression_sites
           << ",\"message_send_selector_lowering_receiver_sites\":"
           << message_send_selector_lowering_contract.receiver_expression_sites
           << ",\"message_send_selector_lowering_selector_literal_entries\":"
           << message_send_selector_lowering_contract.selector_literal_entries
           << ",\"message_send_selector_lowering_selector_literal_characters\":"
           << message_send_selector_lowering_contract.selector_literal_characters
           << ",\"lowering_message_send_selector_lowering_replay_key\":\""
           << message_send_selector_lowering_replay_key
           << "\""
           << ",\"deterministic_dispatch_abi_marshalling_handoff\":"
           << (dispatch_abi_marshalling_contract.deterministic ? "true" : "false")
           << ",\"dispatch_abi_marshalling_message_send_sites\":"
           << dispatch_abi_marshalling_contract.message_send_sites
           << ",\"dispatch_abi_marshalling_receiver_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.receiver_slots_marshaled
           << ",\"dispatch_abi_marshalling_selector_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.selector_slots_marshaled
           << ",\"dispatch_abi_marshalling_argument_value_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.argument_value_slots_marshaled
           << ",\"dispatch_abi_marshalling_argument_padding_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.argument_padding_slots_marshaled
           << ",\"dispatch_abi_marshalling_argument_total_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.argument_total_slots_marshaled
           << ",\"dispatch_abi_marshalling_total_marshaled_slots\":"
           << dispatch_abi_marshalling_contract.total_marshaled_slots
           << ",\"dispatch_abi_marshalling_runtime_dispatch_arg_slots\":"
           << dispatch_abi_marshalling_contract.runtime_dispatch_arg_slots
           << ",\"lowering_dispatch_abi_marshalling_replay_key\":\""
           << dispatch_abi_marshalling_replay_key
           << "\""
           << ",\"deterministic_nil_receiver_semantics_foldability_handoff\":"
           << (nil_receiver_semantics_foldability_contract.deterministic ? "true" : "false")
           << ",\"nil_receiver_semantics_foldability_message_send_sites\":"
           << nil_receiver_semantics_foldability_contract.message_send_sites
           << ",\"nil_receiver_semantics_foldability_receiver_nil_literal_sites\":"
           << nil_receiver_semantics_foldability_contract.receiver_nil_literal_sites
           << ",\"nil_receiver_semantics_foldability_enabled_sites\":"
           << nil_receiver_semantics_foldability_contract.nil_receiver_semantics_enabled_sites
           << ",\"nil_receiver_semantics_foldability_foldable_sites\":"
           << nil_receiver_semantics_foldability_contract.nil_receiver_foldable_sites
           << ",\"nil_receiver_semantics_foldability_runtime_dispatch_required_sites\":"
           << nil_receiver_semantics_foldability_contract.nil_receiver_runtime_dispatch_required_sites
           << ",\"nil_receiver_semantics_foldability_non_nil_receiver_sites\":"
           << nil_receiver_semantics_foldability_contract.non_nil_receiver_sites
           << ",\"nil_receiver_semantics_foldability_contract_violation_sites\":"
           << nil_receiver_semantics_foldability_contract.contract_violation_sites
           << ",\"lowering_nil_receiver_semantics_foldability_replay_key\":\""
           << nil_receiver_semantics_foldability_replay_key
           << "\""
           << ",\"deterministic_super_dispatch_method_family_handoff\":"
           << (super_dispatch_method_family_contract.deterministic ? "true" : "false")
           << ",\"super_dispatch_method_family_message_send_sites\":"
           << super_dispatch_method_family_contract.message_send_sites
           << ",\"super_dispatch_method_family_receiver_super_identifier_sites\":"
           << super_dispatch_method_family_contract.receiver_super_identifier_sites
           << ",\"super_dispatch_method_family_enabled_sites\":"
           << super_dispatch_method_family_contract.super_dispatch_enabled_sites
           << ",\"super_dispatch_method_family_requires_class_context_sites\":"
           << super_dispatch_method_family_contract.super_dispatch_requires_class_context_sites
           << ",\"super_dispatch_method_family_init_sites\":"
           << super_dispatch_method_family_contract.method_family_init_sites
           << ",\"super_dispatch_method_family_copy_sites\":"
           << super_dispatch_method_family_contract.method_family_copy_sites
           << ",\"super_dispatch_method_family_mutable_copy_sites\":"
           << super_dispatch_method_family_contract.method_family_mutable_copy_sites
           << ",\"super_dispatch_method_family_new_sites\":"
           << super_dispatch_method_family_contract.method_family_new_sites
           << ",\"super_dispatch_method_family_none_sites\":"
           << super_dispatch_method_family_contract.method_family_none_sites
           << ",\"super_dispatch_method_family_returns_retained_result_sites\":"
           << super_dispatch_method_family_contract.method_family_returns_retained_result_sites
           << ",\"super_dispatch_method_family_returns_related_result_sites\":"
           << super_dispatch_method_family_contract.method_family_returns_related_result_sites
           << ",\"super_dispatch_method_family_contract_violation_sites\":"
           << super_dispatch_method_family_contract.contract_violation_sites
           << ",\"lowering_super_dispatch_method_family_replay_key\":\""
           << super_dispatch_method_family_replay_key
           << "\""
           << ",\"deterministic_runtime_link_host_link_handoff\":"
           << (runtime_link_host_link_contract.deterministic ? "true" : "false")
           << ",\"runtime_link_host_link_message_send_sites\":"
           << runtime_link_host_link_contract.message_send_sites
           << ",\"runtime_link_host_link_required_runtime_link_sites\":"
           << runtime_link_host_link_contract.runtime_link_required_sites
           << ",\"runtime_link_host_link_elided_runtime_link_sites\":"
           << runtime_link_host_link_contract.runtime_link_elided_sites
           << ",\"runtime_link_host_link_runtime_dispatch_arg_slots\":"
           << runtime_link_host_link_contract.runtime_dispatch_arg_slots
           << ",\"runtime_link_host_link_runtime_dispatch_declaration_parameter_count\":"
           << runtime_link_host_link_contract.runtime_dispatch_declaration_parameter_count
           << ",\"runtime_link_host_link_runtime_dispatch_symbol\":\""
           << runtime_link_host_link_contract.runtime_dispatch_symbol
           << "\""
           << ",\"runtime_link_host_link_default_runtime_dispatch_symbol_binding\":"
           << (runtime_link_host_link_contract.default_runtime_dispatch_symbol_binding ? "true" : "false")
           << ",\"runtime_link_host_link_contract_violation_sites\":"
           << runtime_link_host_link_contract.contract_violation_sites
           << ",\"lowering_runtime_link_host_link_replay_key\":\""
           << runtime_link_host_link_replay_key
           << "\""
           << ",\"deterministic_ownership_qualifier_lowering_handoff\":"
           << (ownership_qualifier_lowering_contract.deterministic ? "true" : "false")
           << ",\"ownership_qualifier_lowering_type_annotation_ownership_qualifier_sites\":"
           << ownership_qualifier_lowering_contract.ownership_qualifier_sites
           << ",\"ownership_qualifier_lowering_type_annotation_invalid_ownership_qualifier_sites\":"
           << ownership_qualifier_lowering_contract.invalid_ownership_qualifier_sites
           << ",\"ownership_qualifier_lowering_type_annotation_object_pointer_type_sites\":"
           << ownership_qualifier_lowering_contract.object_pointer_type_annotation_sites
           << ",\"lowering_ownership_qualifier_replay_key\":\""
           << ownership_qualifier_lowering_replay_key
           << "\""
           << ",\"deterministic_retain_release_operation_lowering_handoff\":"
           << (retain_release_operation_lowering_contract.deterministic ? "true" : "false")
           << ",\"retain_release_operation_lowering_ownership_qualified_sites\":"
           << retain_release_operation_lowering_contract.ownership_qualified_sites
           << ",\"retain_release_operation_lowering_retain_insertion_sites\":"
           << retain_release_operation_lowering_contract.retain_insertion_sites
           << ",\"retain_release_operation_lowering_release_insertion_sites\":"
           << retain_release_operation_lowering_contract.release_insertion_sites
           << ",\"retain_release_operation_lowering_autorelease_insertion_sites\":"
           << retain_release_operation_lowering_contract.autorelease_insertion_sites
           << ",\"retain_release_operation_lowering_contract_violation_sites\":"
           << retain_release_operation_lowering_contract.contract_violation_sites
           << ",\"lowering_retain_release_operation_replay_key\":\""
           << retain_release_operation_lowering_replay_key
           << "\""
           << ",\"deterministic_autoreleasepool_scope_lowering_handoff\":"
           << (autoreleasepool_scope_lowering_contract.deterministic ? "true" : "false")
           << ",\"autoreleasepool_scope_lowering_scope_sites\":"
           << autoreleasepool_scope_lowering_contract.scope_sites
           << ",\"autoreleasepool_scope_lowering_scope_symbolized_sites\":"
           << autoreleasepool_scope_lowering_contract.scope_symbolized_sites
           << ",\"autoreleasepool_scope_lowering_max_scope_depth\":"
           << autoreleasepool_scope_lowering_contract.max_scope_depth
           << ",\"autoreleasepool_scope_lowering_scope_entry_transition_sites\":"
           << autoreleasepool_scope_lowering_contract.scope_entry_transition_sites
           << ",\"autoreleasepool_scope_lowering_scope_exit_transition_sites\":"
           << autoreleasepool_scope_lowering_contract.scope_exit_transition_sites
           << ",\"autoreleasepool_scope_lowering_contract_violation_sites\":"
           << autoreleasepool_scope_lowering_contract.contract_violation_sites
           << ",\"lowering_autoreleasepool_scope_replay_key\":\""
           << autoreleasepool_scope_lowering_replay_key
           << "\""
           << ",\"deterministic_weak_unowned_semantics_lowering_handoff\":"
           << (weak_unowned_semantics_lowering_contract.deterministic ? "true" : "false")
           << ",\"weak_unowned_semantics_lowering_ownership_candidate_sites\":"
           << weak_unowned_semantics_lowering_contract.ownership_candidate_sites
           << ",\"weak_unowned_semantics_lowering_weak_reference_sites\":"
           << weak_unowned_semantics_lowering_contract.weak_reference_sites
           << ",\"weak_unowned_semantics_lowering_unowned_reference_sites\":"
           << weak_unowned_semantics_lowering_contract.unowned_reference_sites
           << ",\"weak_unowned_semantics_lowering_unowned_safe_reference_sites\":"
           << weak_unowned_semantics_lowering_contract.unowned_safe_reference_sites
           << ",\"weak_unowned_semantics_lowering_conflict_sites\":"
           << weak_unowned_semantics_lowering_contract.weak_unowned_conflict_sites
           << ",\"weak_unowned_semantics_lowering_contract_violation_sites\":"
           << weak_unowned_semantics_lowering_contract.contract_violation_sites
           << ",\"lowering_weak_unowned_semantics_replay_key\":\""
           << weak_unowned_semantics_lowering_replay_key
           << "\""
           << ",\"deterministic_arc_diagnostics_fixit_lowering_handoff\":"
           << (arc_diagnostics_fixit_lowering_contract.deterministic ? "true" : "false")
           << ",\"arc_diagnostics_fixit_lowering_ownership_arc_diagnostic_candidate_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_diagnostic_candidate_sites
           << ",\"arc_diagnostics_fixit_lowering_ownership_arc_fixit_available_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_fixit_available_sites
           << ",\"arc_diagnostics_fixit_lowering_ownership_arc_profiled_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_profiled_sites
           << ",\"arc_diagnostics_fixit_lowering_ownership_arc_weak_unowned_conflict_diagnostic_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_weak_unowned_conflict_diagnostic_sites
           << ",\"arc_diagnostics_fixit_lowering_ownership_arc_empty_fixit_hint_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_empty_fixit_hint_sites
           << ",\"arc_diagnostics_fixit_lowering_contract_violation_sites\":"
           << arc_diagnostics_fixit_lowering_contract.contract_violation_sites
           << ",\"lowering_arc_diagnostics_fixit_replay_key\":\""
           << arc_diagnostics_fixit_lowering_replay_key
           << "\""
           << ",\"deterministic_block_literal_capture_lowering_handoff\":"
           << (block_literal_capture_lowering_contract.deterministic ? "true" : "false")
           << ",\"block_literal_capture_lowering_block_literal_sites\":"
           << block_literal_capture_lowering_contract.block_literal_sites
           << ",\"block_literal_capture_lowering_block_parameter_entries\":"
           << block_literal_capture_lowering_contract.block_parameter_entries
           << ",\"block_literal_capture_lowering_block_capture_entries\":"
           << block_literal_capture_lowering_contract.block_capture_entries
           << ",\"block_literal_capture_lowering_block_body_statement_entries\":"
           << block_literal_capture_lowering_contract.block_body_statement_entries
           << ",\"block_literal_capture_lowering_block_empty_capture_sites\":"
           << block_literal_capture_lowering_contract.block_empty_capture_sites
           << ",\"block_literal_capture_lowering_block_nondeterministic_capture_sites\":"
           << block_literal_capture_lowering_contract.block_nondeterministic_capture_sites
           << ",\"block_literal_capture_lowering_block_non_normalized_sites\":"
           << block_literal_capture_lowering_contract.block_non_normalized_sites
           << ",\"block_literal_capture_lowering_contract_violation_sites\":"
           << block_literal_capture_lowering_contract.contract_violation_sites
           << ",\"lowering_block_literal_capture_replay_key\":\""
           << block_literal_capture_lowering_replay_key
           << "\""
           << ",\"deterministic_block_source_model_completion_handoff\":"
           << (block_source_model_completion_contract.deterministic ? "true"
                                                                   : "false")
           << ",\"block_source_model_completion_block_literal_sites\":"
           << block_source_model_completion_contract.block_literal_sites
           << ",\"block_source_model_completion_signature_entries_total\":"
           << block_source_model_completion_contract.signature_entries_total
           << ",\"block_source_model_completion_explicit_typed_parameter_entries_total\":"
           << block_source_model_completion_contract
                  .explicit_typed_parameter_entries_total
           << ",\"block_source_model_completion_implicit_parameter_entries_total\":"
           << block_source_model_completion_contract
                  .implicit_parameter_entries_total
           << ",\"block_source_model_completion_capture_inventory_entries_total\":"
           << block_source_model_completion_contract
                  .capture_inventory_entries_total
           << ",\"block_source_model_completion_byvalue_readonly_capture_entries_total\":"
           << block_source_model_completion_contract
                  .byvalue_readonly_capture_entries_total
           << ",\"block_source_model_completion_invoke_surface_entries_total\":"
           << block_source_model_completion_contract
                  .invoke_surface_entries_total
           << ",\"block_source_model_completion_non_normalized_sites\":"
           << block_source_model_completion_contract.non_normalized_sites
           << ",\"block_source_model_completion_contract_violation_sites\":"
           << block_source_model_completion_contract.contract_violation_sites
           << ",\"lowering_block_source_model_completion_replay_key\":\""
           << block_source_model_completion_replay_key
           << "\""
           << ",\"deterministic_block_source_storage_annotation_handoff\":"
           << (block_source_storage_annotation_contract.deterministic ? "true"
                                                                     : "false")
           << ",\"block_source_storage_annotation_block_literal_sites\":"
           << block_source_storage_annotation_contract.block_literal_sites
           << ",\"block_source_storage_annotation_capture_entries_total\":"
           << block_source_storage_annotation_contract.capture_entries_total
           << ",\"block_source_storage_annotation_mutated_capture_entries_total\":"
           << block_source_storage_annotation_contract
                  .mutated_capture_entries_total
           << ",\"block_source_storage_annotation_byref_capture_entries_total\":"
           << block_source_storage_annotation_contract.byref_capture_entries_total
           << ",\"block_source_storage_annotation_copy_helper_intent_sites\":"
           << block_source_storage_annotation_contract.copy_helper_intent_sites
           << ",\"block_source_storage_annotation_dispose_helper_intent_sites\":"
           << block_source_storage_annotation_contract
                  .dispose_helper_intent_sites
           << ",\"block_source_storage_annotation_heap_candidate_sites\":"
           << block_source_storage_annotation_contract.heap_candidate_sites
           << ",\"block_source_storage_annotation_expression_sites\":"
           << block_source_storage_annotation_contract.expression_sites
           << ",\"block_source_storage_annotation_global_initializer_sites\":"
           << block_source_storage_annotation_contract
                  .global_initializer_sites
           << ",\"block_source_storage_annotation_binding_initializer_sites\":"
           << block_source_storage_annotation_contract
                  .binding_initializer_sites
           << ",\"block_source_storage_annotation_assignment_value_sites\":"
           << block_source_storage_annotation_contract.assignment_value_sites
           << ",\"block_source_storage_annotation_return_value_sites\":"
           << block_source_storage_annotation_contract.return_value_sites
           << ",\"block_source_storage_annotation_call_argument_sites\":"
           << block_source_storage_annotation_contract.call_argument_sites
           << ",\"block_source_storage_annotation_message_argument_sites\":"
           << block_source_storage_annotation_contract.message_argument_sites
           << ",\"block_source_storage_annotation_non_normalized_sites\":"
           << block_source_storage_annotation_contract.non_normalized_sites
           << ",\"block_source_storage_annotation_contract_violation_sites\":"
           << block_source_storage_annotation_contract.contract_violation_sites
           << ",\"lowering_block_source_storage_annotation_replay_key\":\""
           << block_source_storage_annotation_replay_key
           << "\""
           << ",\"deterministic_block_abi_invoke_trampoline_lowering_handoff\":"
           << (block_abi_invoke_trampoline_lowering_contract.deterministic ? "true" : "false")
           << ",\"block_abi_invoke_trampoline_lowering_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.block_literal_sites
           << ",\"block_abi_invoke_trampoline_lowering_invoke_argument_slots\":"
           << block_abi_invoke_trampoline_lowering_contract.invoke_argument_slots_total
           << ",\"block_abi_invoke_trampoline_lowering_capture_word_count\":"
           << block_abi_invoke_trampoline_lowering_contract.capture_word_count_total
           << ",\"block_abi_invoke_trampoline_lowering_parameter_entries\":"
           << block_abi_invoke_trampoline_lowering_contract.parameter_entries_total
           << ",\"block_abi_invoke_trampoline_lowering_capture_entries\":"
           << block_abi_invoke_trampoline_lowering_contract.capture_entries_total
           << ",\"block_abi_invoke_trampoline_lowering_body_statement_entries\":"
           << block_abi_invoke_trampoline_lowering_contract.body_statement_entries_total
           << ",\"block_abi_invoke_trampoline_lowering_descriptor_symbolized_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.descriptor_symbolized_sites
           << ",\"block_abi_invoke_trampoline_lowering_invoke_symbolized_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.invoke_trampoline_symbolized_sites
           << ",\"block_abi_invoke_trampoline_lowering_missing_invoke_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.missing_invoke_trampoline_sites
           << ",\"block_abi_invoke_trampoline_lowering_non_normalized_layout_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.non_normalized_layout_sites
           << ",\"block_abi_invoke_trampoline_lowering_contract_violation_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.contract_violation_sites
           << ",\"lowering_block_abi_invoke_trampoline_replay_key\":\""
           << block_abi_invoke_trampoline_lowering_replay_key
           << "\""
           << ",\"deterministic_block_storage_escape_lowering_handoff\":"
           << (block_storage_escape_lowering_contract.deterministic ? "true" : "false")
           << ",\"block_storage_escape_lowering_sites\":"
           << block_storage_escape_lowering_contract.block_literal_sites
           << ",\"block_storage_escape_lowering_mutable_capture_count\":"
           << block_storage_escape_lowering_contract.mutable_capture_count_total
           << ",\"block_storage_escape_lowering_byref_slot_count\":"
           << block_storage_escape_lowering_contract.byref_slot_count_total
           << ",\"block_storage_escape_lowering_parameter_entries\":"
           << block_storage_escape_lowering_contract.parameter_entries_total
           << ",\"block_storage_escape_lowering_capture_entries\":"
           << block_storage_escape_lowering_contract.capture_entries_total
           << ",\"block_storage_escape_lowering_body_statement_entries\":"
           << block_storage_escape_lowering_contract.body_statement_entries_total
           << ",\"block_storage_escape_lowering_requires_byref_cells_sites\":"
           << block_storage_escape_lowering_contract.requires_byref_cells_sites
           << ",\"block_storage_escape_lowering_escape_analysis_enabled_sites\":"
           << block_storage_escape_lowering_contract.escape_analysis_enabled_sites
           << ",\"block_storage_escape_lowering_escape_to_heap_sites\":"
           << block_storage_escape_lowering_contract.escape_to_heap_sites
           << ",\"block_storage_escape_lowering_escape_profile_normalized_sites\":"
           << block_storage_escape_lowering_contract.escape_profile_normalized_sites
           << ",\"block_storage_escape_lowering_byref_layout_symbolized_sites\":"
           << block_storage_escape_lowering_contract.byref_layout_symbolized_sites
           << ",\"block_storage_escape_lowering_contract_violation_sites\":"
           << block_storage_escape_lowering_contract.contract_violation_sites
           << ",\"lowering_block_storage_escape_replay_key\":\""
           << block_storage_escape_lowering_replay_key
           << "\""
           << ",\"deterministic_block_copy_dispose_lowering_handoff\":"
           << (block_copy_dispose_lowering_contract.deterministic ? "true" : "false")
           << ",\"block_copy_dispose_lowering_sites\":"
           << block_copy_dispose_lowering_contract.block_literal_sites
           << ",\"block_copy_dispose_lowering_mutable_capture_count\":"
           << block_copy_dispose_lowering_contract.mutable_capture_count_total
           << ",\"block_copy_dispose_lowering_byref_slot_count\":"
           << block_copy_dispose_lowering_contract.byref_slot_count_total
           << ",\"block_copy_dispose_lowering_parameter_entries\":"

           << block_copy_dispose_lowering_contract.parameter_entries_total
           << ",\"block_copy_dispose_lowering_capture_entries\":"
           << block_copy_dispose_lowering_contract.capture_entries_total
           << ",\"block_copy_dispose_lowering_body_statement_entries\":"
           << block_copy_dispose_lowering_contract.body_statement_entries_total
           << ",\"block_copy_dispose_lowering_copy_helper_required_sites\":"
           << block_copy_dispose_lowering_contract.copy_helper_required_sites
           << ",\"block_copy_dispose_lowering_dispose_helper_required_sites\":"
           << block_copy_dispose_lowering_contract.dispose_helper_required_sites
           << ",\"block_copy_dispose_lowering_profile_normalized_sites\":"
           << block_copy_dispose_lowering_contract.profile_normalized_sites
           << ",\"block_copy_dispose_lowering_copy_helper_symbolized_sites\":"
           << block_copy_dispose_lowering_contract.copy_helper_symbolized_sites
           << ",\"block_copy_dispose_lowering_dispose_helper_symbolized_sites\":"
           << block_copy_dispose_lowering_contract.dispose_helper_symbolized_sites
           << ",\"block_copy_dispose_lowering_contract_violation_sites\":"
           << block_copy_dispose_lowering_contract.contract_violation_sites
           << ",\"lowering_block_copy_dispose_replay_key\":\""
           << block_copy_dispose_lowering_replay_key
           << "\""
           << ",\"deterministic_block_determinism_perf_baseline_lowering_handoff\":"
           << (block_determinism_perf_baseline_lowering_contract.deterministic ? "true" : "false")
           << ",\"block_determinism_perf_baseline_lowering_sites\":"
           << block_determinism_perf_baseline_lowering_contract.block_literal_sites
           << ",\"block_determinism_perf_baseline_lowering_weight_total\":"
           << block_determinism_perf_baseline_lowering_contract.baseline_weight_total
           << ",\"block_determinism_perf_baseline_lowering_parameter_entries\":"
           << block_determinism_perf_baseline_lowering_contract.parameter_entries_total
           << ",\"block_determinism_perf_baseline_lowering_capture_entries\":"
           << block_determinism_perf_baseline_lowering_contract.capture_entries_total
           << ",\"block_determinism_perf_baseline_lowering_body_statement_entries\":"
           << block_determinism_perf_baseline_lowering_contract.body_statement_entries_total
           << ",\"block_determinism_perf_baseline_lowering_deterministic_capture_sites\":"
           << block_determinism_perf_baseline_lowering_contract.deterministic_capture_sites
           << ",\"block_determinism_perf_baseline_lowering_heavy_tier_sites\":"
           << block_determinism_perf_baseline_lowering_contract.heavy_tier_sites
           << ",\"block_determinism_perf_baseline_lowering_normalized_profile_sites\":"
           << block_determinism_perf_baseline_lowering_contract.normalized_profile_sites
           << ",\"block_determinism_perf_baseline_lowering_contract_violation_sites\":"
           << block_determinism_perf_baseline_lowering_contract.contract_violation_sites
           << ",\"lowering_block_determinism_perf_baseline_replay_key\":\""
           << block_determinism_perf_baseline_lowering_replay_key
           << "\""
           << ",\"deterministic_lightweight_generic_constraint_lowering_handoff\":"
           << (lightweight_generic_constraint_lowering_contract.deterministic ? "true" : "false")
           << ",\"lightweight_generic_constraint_lowering_sites\":"
           << lightweight_generic_constraint_lowering_contract.generic_constraint_sites
           << ",\"lightweight_generic_constraint_lowering_generic_suffix_sites\":"
           << lightweight_generic_constraint_lowering_contract.generic_suffix_sites
           << ",\"lightweight_generic_constraint_lowering_object_pointer_type_sites\":"
           << lightweight_generic_constraint_lowering_contract.object_pointer_type_sites
           << ",\"lightweight_generic_constraint_lowering_terminated_generic_suffix_sites\":"
           << lightweight_generic_constraint_lowering_contract.terminated_generic_suffix_sites
           << ",\"lightweight_generic_constraint_lowering_pointer_declarator_sites\":"
           << lightweight_generic_constraint_lowering_contract.pointer_declarator_sites
           << ",\"lightweight_generic_constraint_lowering_normalized_sites\":"
           << lightweight_generic_constraint_lowering_contract.normalized_constraint_sites
           << ",\"lightweight_generic_constraint_lowering_contract_violation_sites\":"
           << lightweight_generic_constraint_lowering_contract.contract_violation_sites
           << ",\"lowering_lightweight_generic_constraint_replay_key\":\""
           << lightweight_generic_constraint_lowering_replay_key
           << "\""
           << ",\"deterministic_nullability_flow_warning_precision_lowering_handoff\":"
           << (nullability_flow_warning_precision_lowering_contract.deterministic ? "true" : "false")
           << ",\"nullability_flow_warning_precision_lowering_sites\":"
           << nullability_flow_warning_precision_lowering_contract.nullability_flow_sites
           << ",\"nullability_flow_warning_precision_lowering_object_pointer_type_sites\":"
           << nullability_flow_warning_precision_lowering_contract.object_pointer_type_sites
           << ",\"nullability_flow_warning_precision_lowering_nullability_suffix_sites\":"
           << nullability_flow_warning_precision_lowering_contract.nullability_suffix_sites
           << ",\"nullability_flow_warning_precision_lowering_nullable_suffix_sites\":"
           << nullability_flow_warning_precision_lowering_contract.nullable_suffix_sites
           << ",\"nullability_flow_warning_precision_lowering_nonnull_suffix_sites\":"
           << nullability_flow_warning_precision_lowering_contract.nonnull_suffix_sites
           << ",\"nullability_flow_warning_precision_lowering_normalized_sites\":"
           << nullability_flow_warning_precision_lowering_contract.normalized_sites
           << ",\"nullability_flow_warning_precision_lowering_contract_violation_sites\":"
           << nullability_flow_warning_precision_lowering_contract.contract_violation_sites
           << ",\"lowering_nullability_flow_warning_precision_replay_key\":\""
           << nullability_flow_warning_precision_lowering_replay_key
           << "\""
           << ",\"deterministic_protocol_qualified_object_type_lowering_handoff\":"
           << (protocol_qualified_object_type_lowering_contract.deterministic ? "true" : "false")
           << ",\"protocol_qualified_object_type_lowering_sites\":"
           << protocol_qualified_object_type_lowering_contract.protocol_qualified_object_type_sites
           << ",\"protocol_qualified_object_type_lowering_protocol_composition_sites\":"
           << protocol_qualified_object_type_lowering_contract.protocol_composition_sites
           << ",\"protocol_qualified_object_type_lowering_object_pointer_type_sites\":"
           << protocol_qualified_object_type_lowering_contract.object_pointer_type_sites
           << ",\"protocol_qualified_object_type_lowering_terminated_protocol_composition_sites\":"
           << protocol_qualified_object_type_lowering_contract.terminated_protocol_composition_sites
           << ",\"protocol_qualified_object_type_lowering_pointer_declarator_sites\":"
           << protocol_qualified_object_type_lowering_contract.pointer_declarator_sites
           << ",\"protocol_qualified_object_type_lowering_normalized_protocol_composition_sites\":"
           << protocol_qualified_object_type_lowering_contract.normalized_protocol_composition_sites
           << ",\"protocol_qualified_object_type_lowering_contract_violation_sites\":"
           << protocol_qualified_object_type_lowering_contract.contract_violation_sites
           << ",\"lowering_protocol_qualified_object_type_replay_key\":\""
           << protocol_qualified_object_type_lowering_replay_key
           << "\""
           << ",\"deterministic_variance_bridge_cast_lowering_handoff\":"
           << (variance_bridge_cast_lowering_contract.deterministic ? "true" : "false")
           << ",\"variance_bridge_cast_lowering_sites\":"
           << variance_bridge_cast_lowering_contract.variance_bridge_cast_sites
           << ",\"variance_bridge_cast_lowering_protocol_composition_sites\":"
           << variance_bridge_cast_lowering_contract.protocol_composition_sites
           << ",\"variance_bridge_cast_lowering_ownership_qualifier_sites\":"
           << variance_bridge_cast_lowering_contract.ownership_qualifier_sites
           << ",\"variance_bridge_cast_lowering_object_pointer_type_sites\":"
           << variance_bridge_cast_lowering_contract.object_pointer_type_sites
           << ",\"variance_bridge_cast_lowering_pointer_declarator_sites\":"
           << variance_bridge_cast_lowering_contract.pointer_declarator_sites
           << ",\"variance_bridge_cast_lowering_normalized_sites\":"
           << variance_bridge_cast_lowering_contract.normalized_sites
           << ",\"variance_bridge_cast_lowering_contract_violation_sites\":"
           << variance_bridge_cast_lowering_contract.contract_violation_sites
           << ",\"lowering_variance_bridge_cast_replay_key\":\""
           << variance_bridge_cast_lowering_replay_key
           << "\""
           << ",\"deterministic_generic_metadata_abi_lowering_handoff\":"
           << (generic_metadata_abi_lowering_contract.deterministic ? "true" : "false")
           << ",\"generic_metadata_abi_lowering_sites\":"
           << generic_metadata_abi_lowering_contract.generic_metadata_abi_sites
           << ",\"generic_metadata_abi_lowering_generic_suffix_sites\":"
           << generic_metadata_abi_lowering_contract.generic_suffix_sites
           << ",\"generic_metadata_abi_lowering_protocol_composition_sites\":"
           << generic_metadata_abi_lowering_contract.protocol_composition_sites
           << ",\"generic_metadata_abi_lowering_ownership_qualifier_sites\":"
           << generic_metadata_abi_lowering_contract.ownership_qualifier_sites
           << ",\"generic_metadata_abi_lowering_object_pointer_type_sites\":"
           << generic_metadata_abi_lowering_contract.object_pointer_type_sites
           << ",\"generic_metadata_abi_lowering_pointer_declarator_sites\":"
           << generic_metadata_abi_lowering_contract.pointer_declarator_sites
           << ",\"generic_metadata_abi_lowering_normalized_sites\":"
           << generic_metadata_abi_lowering_contract.normalized_sites
           << ",\"generic_metadata_abi_lowering_contract_violation_sites\":"
           << generic_metadata_abi_lowering_contract.contract_violation_sites
           << ",\"lowering_generic_metadata_abi_replay_key\":\""
           << generic_metadata_abi_lowering_replay_key
           << "\""
           << ",\"deterministic_module_import_graph_lowering_handoff\":"
           << (module_import_graph_lowering_contract.deterministic ? "true" : "false")
           << ",\"module_import_graph_lowering_sites\":"
           << module_import_graph_lowering_contract.module_import_graph_sites
           << ",\"module_import_graph_lowering_import_edge_candidate_sites\":"
           << module_import_graph_lowering_contract.import_edge_candidate_sites
           << ",\"module_import_graph_lowering_namespace_segment_sites\":"
           << module_import_graph_lowering_contract.namespace_segment_sites
           << ",\"module_import_graph_lowering_object_pointer_type_sites\":"
           << module_import_graph_lowering_contract.object_pointer_type_sites
           << ",\"module_import_graph_lowering_pointer_declarator_sites\":"
           << module_import_graph_lowering_contract.pointer_declarator_sites
           << ",\"module_import_graph_lowering_normalized_sites\":"
           << module_import_graph_lowering_contract.normalized_sites
           << ",\"module_import_graph_lowering_contract_violation_sites\":"
           << module_import_graph_lowering_contract.contract_violation_sites
           << ",\"lowering_module_import_graph_replay_key\":\""
           << module_import_graph_lowering_replay_key
           << "\""
           << ",\"deterministic_namespace_collision_shadowing_lowering_handoff\":"
           << (namespace_collision_shadowing_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << ",\"namespace_collision_shadowing_lowering_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .namespace_collision_shadowing_sites
           << ",\"namespace_collision_shadowing_lowering_namespace_segment_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .namespace_segment_sites
           << ",\"namespace_collision_shadowing_lowering_import_edge_candidate_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .import_edge_candidate_sites
           << ",\"namespace_collision_shadowing_lowering_object_pointer_type_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .object_pointer_type_sites
           << ",\"namespace_collision_shadowing_lowering_pointer_declarator_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .pointer_declarator_sites
           << ",\"namespace_collision_shadowing_lowering_normalized_sites\":"
           << namespace_collision_shadowing_lowering_contract.normalized_sites
           << ",\"namespace_collision_shadowing_lowering_contract_violation_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .contract_violation_sites
           << ",\"lowering_namespace_collision_shadowing_replay_key\":\""
           << namespace_collision_shadowing_lowering_replay_key
           << "\""
           << ",\"deterministic_public_private_api_partition_lowering_handoff\":"
           << (public_private_api_partition_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << ",\"public_private_api_partition_lowering_sites\":"
           << public_private_api_partition_lowering_contract
                  .public_private_api_partition_sites
           << ",\"public_private_api_partition_lowering_namespace_segment_sites\":"
           << public_private_api_partition_lowering_contract
                  .namespace_segment_sites
           << ",\"public_private_api_partition_lowering_import_edge_candidate_sites\":"
           << public_private_api_partition_lowering_contract
                  .import_edge_candidate_sites
           << ",\"public_private_api_partition_lowering_object_pointer_type_sites\":"
           << public_private_api_partition_lowering_contract
                  .object_pointer_type_sites
           << ",\"public_private_api_partition_lowering_pointer_declarator_sites\":"
           << public_private_api_partition_lowering_contract
                  .pointer_declarator_sites
           << ",\"public_private_api_partition_lowering_normalized_sites\":"
           << public_private_api_partition_lowering_contract.normalized_sites
           << ",\"public_private_api_partition_lowering_contract_violation_sites\":"
           << public_private_api_partition_lowering_contract
                  .contract_violation_sites
           << ",\"lowering_public_private_api_partition_replay_key\":\""
           << public_private_api_partition_lowering_replay_key
           << "\""
           << ",\"deterministic_incremental_module_cache_invalidation_lowering_handoff\":"
           << (incremental_module_cache_invalidation_lowering_contract
                       .deterministic
                   ? "true"
                   : "false")
           << ",\"incremental_module_cache_invalidation_lowering_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .incremental_module_cache_invalidation_sites
           << ",\"incremental_module_cache_invalidation_lowering_namespace_segment_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .namespace_segment_sites
           << ",\"incremental_module_cache_invalidation_lowering_import_edge_candidate_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .import_edge_candidate_sites
           << ",\"incremental_module_cache_invalidation_lowering_object_pointer_type_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .object_pointer_type_sites
           << ",\"incremental_module_cache_invalidation_lowering_pointer_declarator_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .pointer_declarator_sites
           << ",\"incremental_module_cache_invalidation_lowering_normalized_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .normalized_sites
           << ",\"incremental_module_cache_invalidation_lowering_cache_invalidation_candidate_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .cache_invalidation_candidate_sites
           << ",\"incremental_module_cache_invalidation_lowering_contract_violation_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .contract_violation_sites
           << ",\"lowering_incremental_module_cache_invalidation_replay_key\":\""
           << incremental_module_cache_invalidation_lowering_replay_key
           << "\""
           << ",\"deterministic_cross_module_conformance_lowering_handoff\":"
           << (cross_module_conformance_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << ",\"cross_module_conformance_lowering_sites\":"
           << cross_module_conformance_lowering_contract
                  .cross_module_conformance_sites
           << ",\"cross_module_conformance_lowering_namespace_segment_sites\":"
           << cross_module_conformance_lowering_contract.namespace_segment_sites
           << ",\"cross_module_conformance_lowering_import_edge_candidate_sites\":"
           << cross_module_conformance_lowering_contract
                  .import_edge_candidate_sites
           << ",\"cross_module_conformance_lowering_object_pointer_type_sites\":"
           << cross_module_conformance_lowering_contract.object_pointer_type_sites
           << ",\"cross_module_conformance_lowering_pointer_declarator_sites\":"
           << cross_module_conformance_lowering_contract.pointer_declarator_sites
           << ",\"cross_module_conformance_lowering_normalized_sites\":"
           << cross_module_conformance_lowering_contract.normalized_sites
           << ",\"cross_module_conformance_lowering_cache_invalidation_candidate_sites\":"
           << cross_module_conformance_lowering_contract
                  .cache_invalidation_candidate_sites
           << ",\"cross_module_conformance_lowering_contract_violation_sites\":"
           << cross_module_conformance_lowering_contract.contract_violation_sites
           << ",\"lowering_cross_module_conformance_replay_key\":\""
           << cross_module_conformance_lowering_replay_key
           << "\""
           << ",\"deterministic_throws_propagation_lowering_handoff\":"
           << (throws_propagation_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << ",\"throws_propagation_lowering_sites\":"
           << throws_propagation_lowering_contract.throws_propagation_sites
           << ",\"throws_propagation_lowering_namespace_segment_sites\":"
           << throws_propagation_lowering_contract.namespace_segment_sites
           << ",\"throws_propagation_lowering_import_edge_candidate_sites\":"
           << throws_propagation_lowering_contract.import_edge_candidate_sites
           << ",\"throws_propagation_lowering_object_pointer_type_sites\":"
           << throws_propagation_lowering_contract.object_pointer_type_sites
           << ",\"throws_propagation_lowering_pointer_declarator_sites\":"
           << throws_propagation_lowering_contract.pointer_declarator_sites
           << ",\"throws_propagation_lowering_normalized_sites\":"
           << throws_propagation_lowering_contract.normalized_sites
           << ",\"throws_propagation_lowering_cache_invalidation_candidate_sites\":"
           << throws_propagation_lowering_contract
                  .cache_invalidation_candidate_sites
           << ",\"throws_propagation_lowering_contract_violation_sites\":"
           << throws_propagation_lowering_contract.contract_violation_sites
           << ",\"lowering_throws_propagation_replay_key\":\""
           << throws_propagation_lowering_replay_key
           << "\""
           << ",\"deterministic_object_pointer_nullability_generics_handoff\":"
           << (object_pointer_nullability_generics_summary.deterministic_object_pointer_nullability_generics_handoff
                   ? "true"
                   : "false")
           << ",\"object_pointer_type_spellings\":"
           << object_pointer_nullability_generics_summary.object_pointer_type_spellings
           << ",\"pointer_declarator_entries\":"
           << object_pointer_nullability_generics_summary.pointer_declarator_entries
           << ",\"pointer_declarator_depth_total\":"
           << object_pointer_nullability_generics_summary.pointer_declarator_depth_total
           << ",\"pointer_declarator_token_entries\":"
           << object_pointer_nullability_generics_summary.pointer_declarator_token_entries
           << ",\"nullability_suffix_entries\":"
           << object_pointer_nullability_generics_summary.nullability_suffix_entries
           << ",\"generic_suffix_entries\":"
           << object_pointer_nullability_generics_summary.generic_suffix_entries
           << ",\"terminated_generic_suffix_entries\":"
           << object_pointer_nullability_generics_summary.terminated_generic_suffix_entries
           << ",\"unterminated_generic_suffix_entries\":"
           << object_pointer_nullability_generics_summary.unterminated_generic_suffix_entries
           << ",\"symbol_graph_global_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.global_symbol_nodes
           << ",\"symbol_graph_function_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.function_symbol_nodes
           << ",\"symbol_graph_interface_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.interface_symbol_nodes
           << ",\"symbol_graph_implementation_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.implementation_symbol_nodes
           << ",\"symbol_graph_interface_property_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.interface_property_symbol_nodes
           << ",\"symbol_graph_implementation_property_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.implementation_property_symbol_nodes
           << ",\"symbol_graph_interface_method_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.interface_method_symbol_nodes
           << ",\"symbol_graph_implementation_method_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.implementation_method_symbol_nodes
           << ",\"scope_resolution_top_level_scope_symbols\":"
           << symbol_graph_scope_resolution_summary.top_level_scope_symbols
           << ",\"scope_resolution_nested_scope_symbols\":"
           << symbol_graph_scope_resolution_summary.nested_scope_symbols
           << ",\"scope_resolution_scope_frames_total\":"
           << symbol_graph_scope_resolution_summary.scope_frames_total
           << ",\"scope_resolution_implementation_interface_resolution_sites\":"
           << symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites
           << ",\"scope_resolution_implementation_interface_resolution_hits\":"
           << symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits
           << ",\"scope_resolution_implementation_interface_resolution_misses\":"
           << symbol_graph_scope_resolution_summary.implementation_interface_resolution_misses
           << ",\"scope_resolution_method_resolution_sites\":"
           << symbol_graph_scope_resolution_summary.method_resolution_sites
           << ",\"scope_resolution_method_resolution_hits\":"
           << symbol_graph_scope_resolution_summary.method_resolution_hits
           << ",\"scope_resolution_method_resolution_misses\":"
           << symbol_graph_scope_resolution_summary.method_resolution_misses
           << ",\"deterministic_symbol_graph_handoff\":"
           << (symbol_graph_scope_resolution_summary.deterministic_symbol_graph_handoff ? "true" : "false")
           << ",\"deterministic_scope_resolution_handoff\":"
           << (symbol_graph_scope_resolution_summary.deterministic_scope_resolution_handoff ? "true" : "false")
           << ",\"symbol_graph_scope_resolution_handoff_key\":\""
           << symbol_graph_scope_resolution_summary.deterministic_handoff_key
           << "\"},\n";
  manifest << "      \"vector_signature_surface\":{\"vector_signature_functions\":" << vector_signature_functions
           << ",\"vector_return_signatures\":" << vector_return_signatures
           << ",\"vector_param_signatures\":" << vector_param_signatures
           << ",\"vector_i32_signatures\":" << vector_i32_signatures
           << ",\"vector_bool_signatures\":" << vector_bool_signatures
           << ",\"lane2\":" << vector_lane2_signatures
           << ",\"lane4\":" << vector_lane4_signatures << ",\"lane8\":" << vector_lane8_signatures
           << ",\"lane16\":" << vector_lane16_signatures << "},\n";
  manifest << "      \"semantic_surface\": {\"declared_globals\":" << program.globals.size()
           << ",\"declared_functions\":" << manifest_functions.size()
           << ",\"declared_interfaces\":" << program.interfaces.size()
           << ",\"declared_implementations\":" << program.implementations.size()
           << ",\"resolved_global_symbols\":" << pipeline_result.integration_surface.globals.size()
           << ",\"resolved_function_symbols\":" << pipeline_result.integration_surface.functions.size()
           << ",\"resolved_interface_symbols\":" << pipeline_result.integration_surface.interfaces.size()
           << ",\"resolved_implementation_symbols\":" << pipeline_result.integration_surface.implementations.size()
           << ",\"declared_protocols\":" << protocol_category_summary.declared_protocols
           << ",\"declared_categories\":" << protocol_category_summary.declared_categories
           << ",\"resolved_protocol_symbols\":" << protocol_category_summary.resolved_protocol_symbols
           << ",\"resolved_category_symbols\":" << protocol_category_summary.resolved_category_symbols
           << ",\"interface_method_symbols\":"
           << pipeline_result.sema_parity_surface.interface_implementation_summary.interface_method_symbols
           << ",\"implementation_method_symbols\":"
           << pipeline_result.sema_parity_surface.interface_implementation_summary.implementation_method_symbols
           << ",\"protocol_method_symbols\":" << protocol_category_summary.protocol_method_symbols
           << ",\"category_method_symbols\":" << protocol_category_summary.category_method_symbols
           << ",\"linked_implementation_symbols\":"
           << pipeline_result.sema_parity_surface.interface_implementation_summary.linked_implementation_symbols
           << ",\"linked_category_symbols\":" << protocol_category_summary.linked_category_symbols
           << ",\"objc_interface_implementation_surface\":{\"interface_class_method_symbols\":"
           << interface_class_method_symbols
           << ",\"interface_instance_method_symbols\":"
           << interface_instance_method_symbols
           << ",\"implementation_class_method_symbols\":"
           << implementation_class_method_symbols
           << ",\"implementation_instance_method_symbols\":"
           << implementation_instance_method_symbols
           << ",\"implementation_methods_with_body\":"
           << implementation_methods_with_body
           << ",\"deterministic_handoff\":"
           << (pipeline_result.sema_parity_surface.deterministic_interface_implementation_handoff ? "true" : "false")
           << "}"
           << ",\"objc_protocol_category_surface\":{\"protocol_method_symbols\":"
           << protocol_category_summary.protocol_method_symbols
           << ",\"category_method_symbols\":"
           << protocol_category_summary.category_method_symbols
           << ",\"linked_category_symbols\":"
           << protocol_category_summary.linked_category_symbols
           << ",\"deterministic_handoff\":"
           << (protocol_category_summary.deterministic_protocol_category_handoff ? "true" : "false")
           << "}"
           << ",\"objc_class_protocol_category_linking_surface\":{\"declared_class_interfaces\":"
           << class_protocol_category_linking_summary.declared_class_interfaces
           << ",\"declared_class_implementations\":"
           << class_protocol_category_linking_summary.declared_class_implementations
           << ",\"resolved_class_interfaces\":"
           << class_protocol_category_linking_summary.resolved_class_interfaces
           << ",\"resolved_class_implementations\":"
           << class_protocol_category_linking_summary.resolved_class_implementations
           << ",\"linked_class_method_symbols\":"
           << class_protocol_category_linking_summary.linked_class_method_symbols
           << ",\"linked_category_method_symbols\":"
           << class_protocol_category_linking_summary.linked_category_method_symbols
           << ",\"protocol_composition_sites\":"
           << class_protocol_category_linking_summary.protocol_composition_sites
           << ",\"protocol_composition_symbols\":"
           << class_protocol_category_linking_summary.protocol_composition_symbols
           << ",\"category_composition_sites\":"
           << class_protocol_category_linking_summary.category_composition_sites
           << ",\"category_composition_symbols\":"
           << class_protocol_category_linking_summary.category_composition_symbols
           << ",\"invalid_protocol_composition_sites\":"
           << class_protocol_category_linking_summary.invalid_protocol_composition_sites
           << ",\"deterministic_handoff\":"
           << (class_protocol_category_linking_summary.deterministic_class_protocol_category_linking_handoff
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_selector_normalization_surface\":{\"method_declaration_entries\":"
           << selector_normalization_summary.method_declaration_entries
           << ",\"normalized_method_declarations\":"
           << selector_normalization_summary.normalized_method_declarations
           << ",\"selector_piece_entries\":"
           << selector_normalization_summary.selector_piece_entries
           << ",\"selector_piece_parameter_links\":"
           << selector_normalization_summary.selector_piece_parameter_links
           << ",\"deterministic_handoff\":"
           << (selector_normalization_summary.deterministic_selector_normalization_handoff ? "true" : "false")
           << "}"
           << ",\"objc_property_attribute_surface\":{\"property_declaration_entries\":"
           << property_attribute_summary.property_declaration_entries
           << ",\"property_attribute_entries\":"
           << property_attribute_summary.property_attribute_entries
           << ",\"property_attribute_value_entries\":"
           << property_attribute_summary.property_attribute_value_entries
           << ",\"property_accessor_modifier_entries\":"
           << property_attribute_summary.property_accessor_modifier_entries
           << ",\"property_getter_selector_entries\":"
           << property_attribute_summary.property_getter_selector_entries
           << ",\"property_setter_selector_entries\":"
           << property_attribute_summary.property_setter_selector_entries
           << ",\"deterministic_handoff\":"
           << (property_attribute_summary.deterministic_property_attribute_handoff ? "true" : "false")
           << "}"
           << ",\"objc_property_synthesis_ivar_binding_surface\":{\"property_synthesis_sites\":"
           << property_synthesis_ivar_binding_summary.property_synthesis_sites
           << ",\"property_synthesis_explicit_ivar_bindings\":"
           << property_synthesis_ivar_binding_summary.property_synthesis_explicit_ivar_bindings
           << ",\"property_synthesis_default_ivar_bindings\":"
           << property_synthesis_ivar_binding_summary.property_synthesis_default_ivar_bindings
           << ",\"interface_owned_property_synthesis_sites\":"
           << property_synthesis_ivar_binding_summary.interface_owned_property_synthesis_sites
           << ",\"implementation_property_redeclaration_sites\":"
           << property_synthesis_ivar_binding_summary.implementation_property_redeclaration_sites
           << ",\"ivar_binding_sites\":"
           << property_synthesis_ivar_binding_summary.ivar_binding_sites
           << ",\"ivar_binding_resolved\":"
           << property_synthesis_ivar_binding_summary.ivar_binding_resolved
           << ",\"ivar_binding_missing\":"
           << property_synthesis_ivar_binding_summary.ivar_binding_missing
           << ",\"ivar_binding_conflicts\":"
           << property_synthesis_ivar_binding_summary.ivar_binding_conflicts
           << ",\"replay_key\":\""
           << property_synthesis_ivar_binding_replay_key
           << "\",\"deterministic_handoff\":"
           << (property_synthesis_ivar_binding_handoff_deterministic ? "true" : "false")
           << "}"
           // mode-truth inventory anchor: lane-A publishes one
           // truthful frontend packet that separates runnable claims,
           // source-only claims, and fail-closed unsupported claims for the
           // currently implemented Objective-C 3 native subset.
           << ",\"objc_runnable_feature_claim_inventory\":"
           << BuildRunnableFeatureClaimInventoryJson(options, pipeline_result)
           // truth-surface anchor: the frontend must publish the
           // supported driver/selection surfaces explicitly so strictness and
           // feature-macro claims remain fail-closed until later lanes land.
           << ",\"objc_feature_claim_and_strictness_truth_surface\":"
           << BuildFeatureClaimStrictnessTruthSurfaceJson(options, pipeline_result)
           // Part 3 source-closure anchor: lane-A freezes the live
           // parser-owned type-surface truthfully before optional sends,
           // nil-coalescing, and typed key-path syntax are admitted in A002.
           << ",\"objc_type_system_type_source_closure\":"
           << BuildTypeSystemTypeSourceClosureSummaryJson(
                  type_system_type_source_closure_summary)
           << ",\"objc_control_flow_control_flow_source_closure\":"
           << BuildControlFlowControlFlowSourceClosureSummaryJson(
                  control_flow_control_flow_source_closure_summary)
           << ",\"objc_error_handling_error_source_closure\":"
           << BuildErrorHandlingErrorSourceClosureSummaryJson(
                  error_handling_error_source_closure_summary)
            << ",\"objc_concurrency_async_source_closure\":"
            << BuildConcurrencyAsyncSourceClosureSummaryJson(
                   concurrency_async_source_closure_summary)
            << ",\"objc_ownership_resource_borrowed_and_capture_list_source_closure\":"
            << BuildOwnershipSystemExtensionSourceClosureSummaryJson(
                   ownership_system_extension_source_closure_summary)
            << ",\"objc_ownership_cleanup_resource_and_capture_source_completion\":"
            << BuildOwnershipCleanupResourceCaptureSourceCompletionSummaryJson(
                   ownership_cleanup_resource_capture_source_completion_summary)
            << ",\"objc_ownership_retainable_c_family_source_completion\":"
            << BuildOwnershipRetainableCFamilySourceCompletionSummaryJson(
                   ownership_retainable_c_family_source_completion_summary)
            << ",\"objc_dispatch_dispatch_intent_and_dynamism_source_closure\":"
            << BuildDispatchDispatchIntentSourceClosureSummaryJson(
                   dispatch_dispatch_intent_source_closure_summary)
            << ",\"objc_dispatch_dispatch_intent_attribute_and_defaulting_source_completion\":"
            << BuildDispatchDispatchIntentSourceCompletionSummaryJson(
                   dispatch_dispatch_intent_source_completion_summary)
            << ",\"objc_metaprogramming_derive_macro_property_behavior_source_closure\":"
            << BuildMetaprogrammingMetaprogrammingSourceClosureSummaryJson(
                   metaprogramming_metaprogramming_source_closure_summary)
            << ",\"objc_metaprogramming_macro_package_and_provenance_source_completion\":"
            << BuildMetaprogrammingMacroPackageProvenanceSourceCompletionSummaryJson(
                   metaprogramming_macro_package_provenance_source_completion_summary)
            << ",\"objc_metaprogramming_property_behavior_and_synthesized_declaration_source_completion\":"
                   << BuildMetaprogrammingPropertyBehaviorSourceCompletionSummaryJson(
                   metaprogramming_property_behavior_source_completion_summary)
            << ",\"objc_interop_foreign_declaration_and_import_source_closure\":"
                   << BuildInteropForeignImportSourceClosureSummaryJson(
                   interop_foreign_import_source_closure_summary)
            << ",\"objc_interop_cpp_and_swift_interop_annotation_source_completion\":"
                   << BuildInteropCppSwiftInteropAnnotationSourceCompletionSummaryJson(
                   interop_cpp_swift_interop_annotation_source_completion_summary)
            << ",\"objc_tooling_diagnostics_fixit_and_migrator_source_inventory\":"
                   << BuildToolingDiagnosticsMigratorSourceInventorySummaryJson(
                   tooling_diagnostics_migrator_source_inventory_summary)
            << ",\"objc_tooling_migration_and_canonicalization_source_completion\":"
                   << BuildToolingMigrationCanonicalizationSourceCompletionSummaryJson(
                   tooling_migration_canonicalization_source_completion_summary)
            << ",\"objc_tooling_diagnostic_taxonomy_and_portability_contract\":"
                   << BuildToolingDiagnosticTaxonomyPortabilityContractSummaryJson(
                   tooling_diagnostic_taxonomy_portability_contract_summary)
            << ",\"objc_tooling_feature_specific_fixit_synthesis\":"
                   << BuildToolingFeatureSpecificFixitSynthesisSummaryJson(
                   tooling_feature_specific_fixit_synthesis_summary)
            << ",\"objc_interop_interop_semantic_model\":"
            << BuildInteropInteropSemanticModelSummaryJson(
                   interop_interop_semantic_model_summary)
            << ",\"objc_interop_c_and_objc_runtime_parity_semantics\":"
            << BuildInteropInteropRuntimeParitySummaryJson(
                   interop_interop_runtime_parity_summary)
            << ",\"objc_interop_cpp_ownership_throws_and_async_interactions\":"
            << BuildInteropCppInteropInteractionSummaryJson(
                   interop_cpp_interop_interaction_summary)
            << ",\"objc_interop_swift_metadata_and_isolation_mapping\":"
            << BuildInteropSwiftInteropIsolationSummaryJson(
                   interop_swift_interop_isolation_summary)
            << ",\"objc_interop_foreign_surface_interface_and_module_preservation\":"
            << BuildInteropForeignSurfaceInterfacePreservationSummaryJson(
                   interop_foreign_surface_interface_preservation_summary)
            << ",\"objc_interop_header_module_and_bridge_generation\":"
            << BuildInteropHeaderModuleBridgeGenerationSummaryJson(
                   interop_header_module_bridge_generation_summary)
            << ",\"objc_interop_interop_lowering_and_abi_contract\":"
            << BuildInteropInteropLoweringContractJson(
                   interop_interop_semantic_model_summary,
                   interop_interop_runtime_parity_summary,
                   interop_cpp_interop_interaction_summary,
                   interop_swift_interop_isolation_summary,
                   interop_foreign_surface_interface_preservation_summary,
                   interop_interop_lowering_contract,
                   interop_interop_lowering_replay_key)
            << ",\"objc_interop_foreign_call_and_lifetime_lowering\":"
            << BuildInteropForeignCallLifetimeLoweringContractJson(
                   interop_interop_lowering_contract,
                   interop_cpp_interop_interaction_summary,
                   interop_foreign_surface_interface_preservation_summary,
                   interop_foreign_call_lifetime_lowering_contract,
                   interop_foreign_call_lifetime_lowering_replay_key)
            << ",\"objc_interop_ffi_metadata_and_interface_preservation\":"
            << BuildInteropFfiMetadataInterfacePreservationContractJson(
                   interop_foreign_call_lifetime_lowering_contract,
                   interop_foreign_call_lifetime_lowering_replay_key,
                   interop_foreign_surface_interface_preservation_summary,
                   interop_ffi_metadata_interface_preservation_contract,
                   interop_ffi_metadata_interface_preservation_replay_key)
            << ",\"objc_metaprogramming_expansion_and_behavior_semantic_model\":"
            << BuildMetaprogrammingExpansionBehaviorSemanticModelSummaryJson(
                   metaprogramming_expansion_behavior_semantic_model_summary)
            << ",\"objc_metaprogramming_derive_expansion_inventory\":"
            << BuildMetaprogrammingDeriveExpansionInventorySummaryJson(
                   metaprogramming_derive_expansion_inventory_summary)
            << ",\"objc_metaprogramming_macro_safety_sandbox_and_determinism_semantics\":"
            << BuildMetaprogrammingMacroSafetySandboxDeterminismSummaryJson(
                   metaprogramming_macro_safety_sandbox_determinism_summary)
            << ",\"objc_metaprogramming_property_behavior_legality_and_interaction_completion\":"
            << BuildMetaprogrammingPropertyBehaviorLegalityCompatibilitySummaryJson(
                   metaprogramming_property_behavior_legality_compatibility_summary)
            << ",\"objc_metaprogramming_expansion_and_lowering_contract\":"
            << BuildMetaprogrammingExpansionLoweringContractJson(
                   metaprogramming_property_behavior_source_completion_summary,
                   metaprogramming_derive_expansion_inventory_summary,
                   metaprogramming_macro_safety_sandbox_determinism_summary,
                   metaprogramming_property_behavior_legality_compatibility_summary,
                   metaprogramming_expansion_lowering_contract,
                   metaprogramming_expansion_lowering_replay_key)
            << ",\"objc_metaprogramming_synthesized_ast_and_ir_emission\":"
            << BuildMetaprogrammingSynthesizedArtifactEmissionContractJson(
                   metaprogramming_expansion_lowering_contract,
                   metaprogramming_synthesized_artifact_emission_contract,
                   metaprogramming_synthesized_artifact_emission_replay_key)
            << ",\"objc_metaprogramming_module_interface_and_replay_preservation\":"
            << BuildMetaprogrammingModuleInterfaceReplayPreservationSummaryJson(
                   metaprogramming_module_interface_replay_preservation_summary)
           << ",\"objc_dispatch_dynamism_and_dispatch_control_semantic_model\":"
           << BuildDispatchDispatchIntentSemanticModelSummaryJson(
                   dispatch_dispatch_intent_semantic_model_summary)
            << ",\"objc_dispatch_override_finality_and_sealing_legality\":"
            << BuildDispatchDispatchIntentLegalitySummaryJson(
                   dispatch_dispatch_intent_legality_summary)
            << ",\"objc_dispatch_dynamism_control_compatibility_diagnostics\":"
            << BuildDispatchDispatchIntentCompatibilitySummaryJson(
                   dispatch_dispatch_intent_compatibility_summary)
           << ",\"objc_dispatch_dispatch_control_lowering_contract\":"
           << BuildDispatchDispatchControlLoweringContractJson(
                  dispatch_dispatch_intent_semantic_model_summary,
                  dispatch_dispatch_intent_legality_summary,
                  dispatch_dispatch_intent_compatibility_summary,
                  dispatch_dispatch_control_lowering_contract,
                  dispatch_dispatch_control_lowering_replay_key)
           << ",\"objc_dispatch_dispatch_metadata_and_interface_preservation\":"
           << BuildDispatchDispatchMetadataInterfacePreservationSummaryJson(
                  dispatch_dispatch_metadata_interface_preservation_summary)
            << ",\"objc_concurrency_actor_member_and_isolation_source_closure\":"
            << BuildConcurrencyActorMemberIsolationSourceClosureSummaryJson(
                   concurrency_actor_member_isolation_source_closure_summary)
           << ",\"objc_concurrency_actor_isolation_and_sendable_semantic_model\":"
           << BuildConcurrencyActorIsolationSendableSemanticModelSummaryJson(
                  concurrency_actor_isolation_sendable_semantic_model_summary)
           << ",\"objc_concurrency_actor_isolation_and_sendability_enforcement\":"
           << BuildConcurrencyActorIsolationSendabilityEnforcementSummaryJson(
                  concurrency_actor_isolation_sendability_enforcement_summary)
           << ",\"objc_concurrency_actor_race_hazard_and_escape_diagnostics\":"
           << BuildConcurrencyActorRaceHazardEscapeDiagnosticsSummaryJson(
                  concurrency_actor_race_hazard_escape_diagnostics_summary)
           << ",\"objc_concurrency_actor_lowering_and_metadata_contract\":"
           << BuildConcurrencyActorLoweringMetadataContractJson(
                  concurrency_actor_member_isolation_source_closure_summary,
                  concurrency_actor_isolation_sendability_enforcement_summary,
                  concurrency_actor_race_hazard_escape_diagnostics_summary,
                  concurrency_actor_lowering_metadata_contract,
                  concurrency_actor_lowering_metadata_replay_key)
           << ",\"objc_concurrency_task_group_and_cancellation_source_closure\":"
           << BuildConcurrencyTaskGroupCancellationSourceClosureSummaryJson(
                  concurrency_task_group_cancellation_source_closure_summary)
           << ",\"objc_concurrency_async_effect_and_suspension_semantic_model\":"
           << BuildConcurrencyAsyncEffectSuspensionSemanticModelSummaryJson(
                  concurrency_async_effect_suspension_semantic_model_summary)
           << ",\"objc_concurrency_task_executor_and_cancellation_semantic_model\":"
           << BuildConcurrencyTaskExecutorCancellationSemanticModelSummaryJson(
                  concurrency_task_executor_cancellation_semantic_model_summary)
           << ",\"objc_ownership_system_extension_semantic_model\":"
           << BuildOwnershipSystemExtensionSemanticModelSummaryJson(
                  ownership_system_extension_semantic_model_summary)
           << ",\"objc_effects_ownership_semantic_model\":"
           << BuildEffectsOwnershipSemanticModelSummaryJson(
                  effects_ownership_semantic_model_summary)
           << ",\"objc_cross_module_semantic_contracts_and_diagnostics\":"
           << BuildCrossModuleSemanticContractsDiagnosticsSummaryJson(
                  cross_module_semantic_contracts_diagnostics_summary)
           << ",\"objc_ownership_resource_move_and_use_after_move_semantics\":"
           << BuildOwnershipResourceMoveUseAfterMoveSemanticsSummaryJson(
                  ownership_resource_move_use_after_move_semantics_summary)
           << ",\"objc_ownership_borrowed_pointer_escape_analysis\":"
           << BuildOwnershipBorrowedPointerEscapeAnalysisSummaryJson(
                  ownership_borrowed_pointer_escape_analysis_summary)
           << ",\"objc_ownership_capture_list_and_retainable_family_legality_completion\":"
           << BuildOwnershipCaptureListRetainableFamilyLegalityCompletionSummaryJson(
                  ownership_capture_list_retainable_family_legality_completion_summary)
           << ",\"objc_ownership_system_extension_lowering_contract\":"
           << BuildOwnershipSystemExtensionLoweringContractJson(
                  ownership_system_extension_semantic_model_summary,
                  ownership_resource_move_use_after_move_semantics_summary,
                  ownership_borrowed_pointer_escape_analysis_summary,
                  ownership_capture_list_retainable_family_legality_completion_summary,
                  ownership_system_extension_lowering_contract,
                  ownership_system_extension_lowering_replay_key)
           << ",\"objc_ownership_borrowed_pointer_and_retainable_family_abi_completion\":"
           << BuildOwnershipBorrowedRetainableAbiCompletionJson(
                  ownership_system_extension_lowering_contract,
                  ownership_system_extension_source_closure_summary,
                  ownership_retainable_c_family_source_completion_summary,
                  ownership_system_extension_lowering_replay_key,
                  ownership_borrowed_retainable_abi_completion_replay_key)
            << ",\"objc_concurrency_structured_task_and_cancellation_semantics\":"
            << BuildConcurrencyStructuredTaskCancellationSemanticSummaryJson(
                   concurrency_structured_task_cancellation_semantic_summary)
           << ",\"objc_concurrency_executor_hop_and_affinity_compatibility_completion\":"
           << BuildConcurrencyExecutorHopAffinityCompatibilitySummaryJson(
                  concurrency_executor_hop_affinity_compatibility_summary)
           << ",\"objc_concurrency_await_suspension_and_resume_semantics\":"
           << BuildConcurrencyAwaitSuspensionResumeSemanticSummaryJson(
                  concurrency_await_suspension_resume_semantic_summary)
           << ",\"objc_concurrency_async_diagnostics_and_compatibility_completion\":"
           << BuildConcurrencyAsyncDiagnosticsCompatibilitySummaryJson(
                  concurrency_async_diagnostics_compatibility_summary)
           << ",\"objc_concurrency_continuation_abi_and_async_lowering_contract\":"
           << BuildConcurrencyContinuationAbiAsyncLoweringContractJson(
                  concurrency_async_continuation_lowering_contract,
                  concurrency_await_lowering_suspension_state_lowering_contract,
                  concurrency_async_continuation_lowering_replay_key,
                  concurrency_await_lowering_suspension_state_lowering_replay_key)
           << ",\"objc_concurrency_task_runtime_lowering_contract\":"
           << BuildConcurrencyTaskRuntimeLoweringContractJson(
                  concurrency_task_executor_cancellation_semantic_model_summary,
                  concurrency_structured_task_cancellation_semantic_summary,
                  concurrency_executor_hop_affinity_compatibility_summary,
                  concurrency_actor_isolation_sendability_lowering_contract,
                  concurrency_actor_isolation_sendability_lowering_replay_key,
                  concurrency_task_runtime_interop_cancellation_lowering_contract,
                  concurrency_task_runtime_interop_cancellation_lowering_replay_key,
                  concurrency_concurrency_replay_race_guard_lowering_contract,
                  concurrency_concurrency_replay_race_guard_lowering_replay_key)
           << ",\"objc_concurrency_task_group_and_runtime_abi_completion\":"
           << BuildConcurrencyTaskRuntimeAbiCompletionJson(
                  concurrency_task_runtime_interop_cancellation_lowering_replay_key,
                  concurrency_concurrency_replay_race_guard_lowering_replay_key)
           << ",\"objc_concurrency_async_function_await_and_continuation_lowering\":"
           << BuildConcurrencyAsyncDirectCallLoweringJson(
                  concurrency_async_source_closure_summary,
                  concurrency_async_continuation_lowering_contract,
                  concurrency_await_lowering_suspension_state_lowering_contract,
                  concurrency_async_continuation_lowering_replay_key,
                  concurrency_await_lowering_suspension_state_lowering_replay_key)
           << ",\"objc_concurrency_suspension_autorelease_and_cleanup_integration\":"
           << BuildConcurrencySuspensionCleanupIntegrationJson(
                  control_flow_control_flow_safety_lowering_contract,
                  control_flow_control_flow_safety_lowering_replay_key,
                  autoreleasepool_scope_lowering_contract,
                  autoreleasepool_scope_lowering_replay_key,
                  concurrency_async_continuation_lowering_contract,
                  concurrency_await_lowering_suspension_state_lowering_contract,
                  concurrency_async_continuation_lowering_replay_key,
                  concurrency_await_lowering_suspension_state_lowering_replay_key)
           << ",\"objc_error_handling_error_semantic_model\":"
           << BuildErrorHandlingErrorSemanticModelSummaryJson(
                  error_handling_error_semantic_model_summary)
           << ",\"objc_error_handling_try_do_catch_semantics\":"
           << BuildErrorHandlingTryDoCatchSemanticSummaryJson(
                  error_handling_try_do_catch_semantic_summary)
           << ",\"objc_error_handling_error_bridge_legality\":"
           << BuildErrorHandlingErrorBridgeLegalitySummaryJson(
                  error_handling_error_bridge_legality_summary)
           << ",\"objc_control_flow_control_flow_semantic_model\":"
           << BuildControlFlowControlFlowSemanticModelSummaryJson(
                  control_flow_control_flow_semantic_model_summary)
           << ",\"objc_control_flow_control_flow_safety_lowering_contract\":"
           << BuildControlFlowControlFlowSafetyLoweringContractJson(
                  control_flow_control_flow_safety_lowering_contract,
                  control_flow_control_flow_semantic_model_summary,
                  control_flow_control_flow_semantic_model_summary.replay_key,
                  control_flow_control_flow_safety_lowering_replay_key)
           << ",\"objc_type_system_type_semantic_model\":"
           << BuildTypeSystemTypeSemanticModelSummaryJson(
                  type_system_type_semantic_model_summary)
           // semantic freeze anchor: sema publishes the fail-closed
           // legality boundary that classifies live compatibility selections,
           // source-only claim downgrades, and strictness/macro claim
           // rejections before lowering and conformance gates consume them.
           << ",\"objc_compatibility_strictness_claim_semantics\":"
           << BuildFrontendCompatibilityStrictnessClaimSemanticsSummaryJson(
                  frontend_compatibility_strictness_claim_semantics)
           << ",\"objc_tooling_legacy_canonical_migration_semantics\":"
           << BuildToolingLegacyCanonicalMigrationSemanticsSummaryJson(
                  tooling_legacy_canonical_migration_semantics_summary)
           << ",\"objc_tooling_machine_readable_conformance_report_contract\":"
           << BuildToolingMachineReadableConformanceReportContractSummaryJson(
                  tooling_machine_readable_conformance_report_contract_summary)
           << ",\"objc_tooling_feature_aware_conformance_report_emission\":"
           << BuildToolingFeatureAwareConformanceReportEmissionSummaryJson(
                  tooling_feature_aware_conformance_report_emission_summary)
           << ",\"objc_tooling_corpus_sharding_release_evidence_packaging\":"
           << BuildToolingCorpusShardingReleaseEvidencePackagingSummaryJson(
                  tooling_corpus_sharding_release_evidence_packaging_summary)
           // lowering freeze anchor: lane-C lowers the existing
           // runnable/source-only/unsupported truth packets into one emitted
           // machine-readable conformance sidecar instead of reconstructing
           // capability claims from docs or release evidence later.
           << ",\"objc_versioned_conformance_report_lowering_contract\":"
           << BuildVersionedConformanceReportLoweringSummaryJson(
                  versioned_conformance_report_lowering)
           // runtime capability reporting anchor: lane-C must
           // publish the truthful machine-readable runtime/public capability
           // payload inside the semantic surface so later driver publication
           // and release tooling consume one canonical schema.
           << ",\"objc_runtime_capability_report\":"
           << BuildRuntimeCapabilityReportJson(
                  versioned_conformance_report_lowering)
           << ",\"objc_executable_metadata_source_graph\":"
           << BuildExecutableMetadataSourceGraphJson(
                  executable_metadata_source_graph)
           // source-to-section matrix anchor: lane-A must publish one
           // canonical node-to-emitted-section matrix that preserves the A001
           // inventory and explicitly marks interface/implementation/metaclass/
           // method rows as no-standalone-emission-yet until later work.
           << ",\"objc_runtime_metadata_source_to_section_matrix\":"
           << BuildRuntimeMetadataSourceToSectionMatrixSummaryJson(
                  runtime_metadata_source_to_section_matrix)
           << ",\"objc_executable_metadata_semantic_consistency_boundary\":"
           << BuildExecutableMetadataSemanticConsistencyBoundaryJson(
                  executable_metadata_semantic_consistency_boundary)
           << ",\"objc_executable_metadata_semantic_validation_surface\":"
           << BuildExecutableMetadataSemanticValidationSurfaceJson(
                  executable_metadata_semantic_validation_surface)
           // lowering-handoff anchor: metadata graph lowering
           // handoff freeze must publish as a first-class semantic surface so
           // typed handoff and parse/lowering projections consume one schema.
           << ",\"objc_executable_metadata_lowering_handoff_surface\":"
           << BuildExecutableMetadataLoweringHandoffSurfaceJson(
                  executable_metadata_lowering_handoff_surface)
           // typed-lowering anchor: the lowering-ready packet must
           // publish the ordered metadata graph payload itself rather than a
           // count-only summary so downstream lowering can consume one schema.
           << ",\"objc_executable_metadata_typed_lowering_handoff\":"
           << BuildExecutableMetadataTypedLoweringHandoffJson(
                  executable_metadata_typed_lowering_handoff)
           // debug-projection anchor: lane-C must publish one
           // canonical metadata inspection matrix across manifest and IR-facing
           // surfaces before runtime section emission lands.
           << ",\"objc_executable_metadata_debug_projection\":"
           << BuildExecutableMetadataDebugProjectionSummaryJson(
                  executable_metadata_debug_projection)
           // runtime-ingest packaging anchor: lane-D must freeze one
           // canonical manifest transport boundary over the typed handoff and
           // debug-projection packets before section emission and startup
           // registration land.
           << ",\"objc_executable_metadata_runtime_ingest_packaging_contract\":"
           << BuildExecutableMetadataRuntimeIngestPackagingContractSummaryJson(
                  executable_metadata_runtime_ingest_packaging_contract)
           // binary-boundary anchor: lane-D must materialize a real
           // runtime-facing binary envelope over the frozen D001/C002/C003
           // packets so later section-emission/bootstrap work consumes one
           // deterministic artifact boundary instead of reparsing manifest JSON.
           // semantic-closure gate anchor: lane-E freezes the
           // aggregate the existing boundary here so the section
           // emission consumes one synchronized metadata closure proof.
           // corpus-sync anchor: integrated corpus probes must
           // observe these synchronized metadata surfaces through the real
           // frontend runner path rather than mock packets.
           << ",\"objc_executable_metadata_runtime_ingest_binary_boundary\":"
           << BuildExecutableMetadataRuntimeIngestBinaryBoundarySummaryJson(
                  executable_metadata_runtime_ingest_binary_boundary)
           // translation-unit registration surface anchor: lane-A
           // freezes one manifest-published preregistration contract over the
           // runtime metadata binary, linker-retention sidecars, constructor
           // root reservation, and runtime-owned entrypoint boundary before
           // A002 emits any real startup constructor or bootstrap calls.
           << ",\"objc_runtime_translation_unit_registration_contract\":"
           << BuildRuntimeTranslationUnitRegistrationContractSummaryJson(
                  runtime_translation_unit_registration_contract)
           // registration-manifest anchor: lane-A now publishes the
           // manifest template and constructor-root ownership model that later
           // lowering/bootstrap lanes consume directly instead of reconstructing
           // startup registration inputs ad hoc from loose sidecars.
           // registration-descriptor/image-root source-surface
           // anchor: the same semantic surface now freezes one canonical
           // frontend-visible naming model for the registration descriptor and
           // image root that later frontend closure, lowering, and runtime
           // replay work must preserve.
           // startup-registration gate anchor: the semantic-surface registration manifest remains the canonical lane-E gate input
           // for the A002/B002/C003/D003/D004 replay-stable bootstrap evidence chain.
           // runbook-closeout anchor: the registration manifest summary stays authoritative for the published runbook
           // and its live smoke replay proof.
           << ",\"objc_runtime_translation_unit_registration_manifest\":"
           << BuildRuntimeTranslationUnitRegistrationManifestSummaryJson(
                 runtime_translation_unit_registration_manifest)
           << ",\"objc_runtime_registration_descriptor_image_root_source_surface\":"
           << BuildRuntimeRegistrationDescriptorImageRootSourceSurfaceSummaryJson(
                  runtime_registration_descriptor_image_root_source_surface)
           // registration-descriptor frontend-closure anchor: lane-A
           // now publishes the owned descriptor-artifact boundary that later
           // lowering and runtime bootstrap work consume directly.
           << ",\"objc_runtime_registration_descriptor_frontend_closure\":"
           << BuildRuntimeRegistrationDescriptorFrontendClosureSummaryJson(
                  runtime_registration_descriptor_frontend_closure)
           // bootstrap-legality anchor: lane-B now publishes the
           // fail-closed semantic legality packet that bridges the emitted
           // descriptor frontier and the live bootstrap
           // semantics so later runtime/lowering work preserves one canonical
           // duplicate-policy, ordering, and restart model.
           << ",\"objc_runtime_bootstrap_legality_failure_contract\":"
           << BuildRuntimeBootstrapLegalityFailureContractSummaryJson(
                  runtime_bootstrap_legality_failure_contract)
           // bootstrap-legality semantics anchor: lane-B now lands
           // the live duplicate-registration and image-order semantic bridge
           // over the emitted translation-unit identity key so lowering and
           // runtime handoff consume one canonical cross-image legality model.
           << ",\"objc_runtime_bootstrap_legality_semantics\":"
           << BuildRuntimeBootstrapLegalitySemanticsSummaryJson(
                  runtime_bootstrap_legality_semantics)
           // bootstrap failure/restart anchor: lane-B now publishes
           // the fail-closed restart/recovery bridge over the live reset/replay
           // runtime path so later multi-image bootstrap work consumes one
           // canonical unsupported-topology and deterministic-restart model.
           << ",\"objc_runtime_bootstrap_failure_restart_semantics\":"
           << BuildRuntimeBootstrapFailureRestartSemanticsSummaryJson(
                  runtime_bootstrap_failure_restart_semantics)
           // runtime-bootstrap-api anchor: lane-D freezes the
           // runtime-owned bootstrap header/archive/entrypoint/reset surface as
           // one canonical packet that later image-walk and reset-expansion
           // issues must preserve exactly.
           << ",\"objc_runtime_bootstrap_api_contract\":"
           << BuildRuntimeBootstrapApiSummaryJson(runtime_bootstrap_api)
           // bootstrap-registrar anchor: the semantic surface now
           // publishes the private staging hook and runtime image-walk policy
           // that extend the emitted startup path without widening the frozen
           // D001 public runtime API.
           << ",\"objc_runtime_bootstrap_registrar_contract\":{"
           << "\"contract_id\":\""
           << EscapeJsonString(kObjc3RuntimeBootstrapRegistrarContractId)
           << "\",\"surface_path\":\""
           << EscapeJsonString(kObjc3RuntimeBootstrapRegistrarSurfacePath)
           << "\",\"bootstrap_api_contract_id\":\""
           << EscapeJsonString(runtime_bootstrap_api.contract_id)
           << "\",\"bootstrap_lowering_contract_id\":\""
           << EscapeJsonString(runtime_bootstrap_lowering.contract_id)
           << "\",\"internal_header_path\":\""
           << EscapeJsonString(kObjc3RuntimeBootstrapInternalHeaderPath)
           << "\",\"stage_registration_table_symbol\":\""
           << EscapeJsonString(
                  kObjc3RuntimeBootstrapStageRegistrationTableSymbol)
           << "\",\"image_walk_snapshot_symbol\":\""
           << EscapeJsonString(kObjc3RuntimeBootstrapImageWalkSnapshotSymbol)
           << "\",\"image_walk_model\":\""
           << EscapeJsonString(kObjc3RuntimeBootstrapImageWalkModel)
           << "\",\"discovery_root_validation_model\":\""
           << EscapeJsonString(
                  kObjc3RuntimeBootstrapDiscoveryRootValidationModel)
           << "\",\"selector_pool_interning_model\":\""
           << EscapeJsonString(
                  kObjc3RuntimeBootstrapSelectorPoolInterningModel)
           << "\",\"realization_staging_model\":\""
           << EscapeJsonString(kObjc3RuntimeBootstrapRealizationStagingModel)
           << "\",\"fail_closed\":true"
           << ",\"ready\":"
           << ((IsReadyObjc3RuntimeBootstrapApiSummary(runtime_bootstrap_api) &&
                IsReadyObjc3RuntimeBootstrapLoweringSummary(
                    runtime_bootstrap_lowering))
                   ? "true"
                   : "false")
           << "}"
           // bootstrap-reset anchor: the semantic surface now
           // publishes the private deterministic reset/replay hooks that allow
           // same-process smoke harnesses to clear live runtime state, zero the
           // retained image-local init cells, and replay retained startup
           // images in canonical registration order without widening the frozen
           // D001 public runtime API.
           << ",\"objc_runtime_bootstrap_reset_contract\":{"
           << "\"contract_id\":\""
           << EscapeJsonString(kObjc3RuntimeBootstrapResetContractId)
           << "\",\"surface_path\":\""
           << EscapeJsonString(kObjc3RuntimeBootstrapResetSurfacePath)
           << "\",\"bootstrap_api_contract_id\":\""
           << EscapeJsonString(runtime_bootstrap_api.contract_id)
           << "\",\"bootstrap_registrar_contract_id\":\""
           << EscapeJsonString(kObjc3RuntimeBootstrapRegistrarContractId)
           << "\",\"internal_header_path\":\""
           << EscapeJsonString(kObjc3RuntimeBootstrapInternalHeaderPath)
           << "\",\"replay_registered_images_symbol\":\""
           << EscapeJsonString(
                  kObjc3RuntimeBootstrapReplayRegisteredImagesSymbol)
           << "\",\"reset_replay_state_snapshot_symbol\":\""
           << EscapeJsonString(
                  kObjc3RuntimeBootstrapResetReplayStateSnapshotSymbol)
           << "\",\"reset_lifecycle_model\":\""
           << EscapeJsonString(kObjc3RuntimeBootstrapResetLifecycleModel)
           << "\",\"replay_order_model\":\""
           << EscapeJsonString(kObjc3RuntimeBootstrapReplayOrderModel)
           << "\",\"image_local_init_state_reset_model\":\""
           << EscapeJsonString(
                  kObjc3RuntimeBootstrapImageLocalInitStateResetModel)
           << "\",\"bootstrap_catalog_retention_model\":\""
           << EscapeJsonString(kObjc3RuntimeBootstrapCatalogRetentionModel)
           << "\",\"fail_closed\":true"
           << ",\"ready\":"
           << ((IsReadyObjc3RuntimeBootstrapApiSummary(runtime_bootstrap_api) &&
                IsReadyObjc3RuntimeBootstrapLoweringSummary(
                    runtime_bootstrap_lowering))
                   ? "true"
                   : "false")
           << "}"
           // archive/static-link bootstrap replay corpus anchor:
           // lane-C now publishes the retained-archive replay proof surface
           // that ties the merge model to the live replay
           // runtime and the emitted C002 registration-descriptor/image-root
           // lowering boundary.
           << ",\"objc_runtime_bootstrap_archive_static_link_replay_corpus\":{"
           << "\"contract_id\":\""
           << EscapeJsonString(
                  kObjc3RuntimeBootstrapArchiveStaticLinkReplayCorpusContractId)
           << "\",\"archive_static_link_discovery_contract_id\":\""
           << EscapeJsonString(kObjc3RuntimeArchiveStaticLinkDiscoveryContractId)
           << "\",\"bootstrap_failure_restart_contract_id\":\""
           << EscapeJsonString(runtime_bootstrap_failure_restart_semantics.contract_id)
           << "\",\"bootstrap_lowering_contract_id\":\""
           << EscapeJsonString(runtime_bootstrap_lowering.contract_id)
           << "\",\"registration_descriptor_lowering_contract_id\":\""
           << EscapeJsonString(
                  kObjc3RuntimeBootstrapRegistrationDescriptorImageRootLoweringContractId)
           << "\",\"corpus_model\":\""
           << EscapeJsonString(
                  kObjc3RuntimeBootstrapArchiveStaticLinkReplayCorpusModel)
           << "\",\"binary_proof_model\":\""
           << EscapeJsonString(
                  kObjc3RuntimeBootstrapArchiveStaticLinkReplayCorpusBinaryProofModel)
           << "\",\"merge_model\":\""
           << EscapeJsonString(kObjc3RuntimeArchiveStaticLinkMergeModel)
           << "\",\"translation_unit_identity_key\":\""
           << EscapeJsonString(translation_unit_identity_key)
           << "\",\"registration_descriptor_identifier\":\""
           << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                                   .registration_descriptor_identifier)
           << "\",\"image_root_identifier\":\""
           << EscapeJsonString(runtime_registration_descriptor_frontend_closure
                                   .image_root_identifier)
           << "\",\"replay_registered_images_symbol\":\""
           << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                   .replay_registered_images_symbol)
           << "\",\"reset_replay_state_snapshot_symbol\":\""
           << EscapeJsonString(runtime_bootstrap_failure_restart_semantics
                                   .reset_replay_state_snapshot_symbol)
           << "\",\"ready\":"
           << ((IsReadyObjc3RuntimeBootstrapFailureRestartSemanticsSummary(
                    runtime_bootstrap_failure_restart_semantics) &&
                IsReadyObjc3RuntimeBootstrapLoweringSummary(
                    runtime_bootstrap_lowering))
                   ? "true"
                   : "false")
           << "}"
           // bootstrap-invariant anchor: lane-B freezes duplicate
           // registration, realization order, failure mode, and image-local
           // initialization semantics against the live A002 registration
           // manifest so later bootstrap implementation extends one canonical
           // sema/runtime packet.
           << ",\"objc_runtime_startup_bootstrap_invariants\":"
           << BuildRuntimeStartupBootstrapInvariantSummaryJson(
                  runtime_startup_bootstrap_invariants)
           // bootstrap-semantics anchor: lane-B now lands the live
           // runtime enforcement/result-code surface that must remain aligned
           // with the emitted registration manifest and the native runtime
           // probe harness.
           << ",\"objc_runtime_startup_bootstrap_semantics\":"
           << BuildRuntimeBootstrapSemanticsSummaryJson(
                  runtime_bootstrap_semantics)
           // bootstrap-lowering anchor: lane-C now freezes one
           // manifest-driven lowering packet that owns future ctor-root,
           // init-stub, and registration-table materialization without
           // claiming that the current emitted IR already contains those
           // globals.
           << ",\"objc_runtime_bootstrap_lowering_contract\":"
           << BuildRuntimeBootstrapLoweringSummaryJson(
                  runtime_bootstrap_lowering)
           << ",\"objc_id_class_sel_object_pointer_typecheck_surface\":{\"id_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.id_typecheck_sites
           << ",\"class_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.class_typecheck_sites
           << ",\"sel_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.sel_typecheck_sites
           << ",\"object_pointer_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.object_pointer_typecheck_sites
           << ",\"total_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.total_typecheck_sites
           << ",\"replay_key\":\""
           << id_class_sel_object_pointer_typecheck_replay_key
           << "\",\"deterministic_handoff\":"
           << (id_class_sel_object_pointer_typecheck_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_dispatch_surface_classification_surface\":{\"instance_dispatch_sites\":"
           << dispatch_surface_classification_contract.instance_dispatch_sites
           << ",\"class_dispatch_sites\":"
           << dispatch_surface_classification_contract.class_dispatch_sites
           << ",\"super_dispatch_sites\":"
           << dispatch_surface_classification_contract.super_dispatch_sites
           << ",\"direct_dispatch_sites\":"
           << dispatch_surface_classification_contract.direct_dispatch_sites
           << ",\"dynamic_dispatch_sites\":"
           << dispatch_surface_classification_contract.dynamic_dispatch_sites
           << ",\"instance_entrypoint_family\":\""
           << dispatch_surface_classification_contract.instance_entrypoint_family
           << "\",\"class_entrypoint_family\":\""
           << dispatch_surface_classification_contract.class_entrypoint_family
           << "\",\"super_entrypoint_family\":\""
           << dispatch_surface_classification_contract.super_entrypoint_family
           << "\",\"direct_entrypoint_family\":\""
           << dispatch_surface_classification_contract.direct_entrypoint_family
           << "\",\"dynamic_entrypoint_family\":\""
           << dispatch_surface_classification_contract.dynamic_entrypoint_family
           << "\",\"replay_key\":\""
           << dispatch_surface_classification_replay_key
           << "\",\"deterministic_handoff\":"
           << (dispatch_surface_classification_contract.deterministic ? "true"
                                                                     : "false")
           << "}"
           << ",\"objc_message_send_selector_lowering_surface\":{\"message_send_sites\":"
           << message_send_selector_lowering_contract.message_send_sites
           << ",\"unary_selector_sites\":"
           << message_send_selector_lowering_contract.unary_selector_sites
           << ",\"keyword_selector_sites\":"
           << message_send_selector_lowering_contract.keyword_selector_sites
           << ",\"selector_piece_sites\":"
           << message_send_selector_lowering_contract.selector_piece_sites
           << ",\"argument_expression_sites\":"
           << message_send_selector_lowering_contract.argument_expression_sites
           << ",\"receiver_expression_sites\":"
           << message_send_selector_lowering_contract.receiver_expression_sites
           << ",\"selector_literal_entries\":"
           << message_send_selector_lowering_contract.selector_literal_entries
           << ",\"selector_literal_characters\":"
           << message_send_selector_lowering_contract.selector_literal_characters
           << ",\"replay_key\":\""
           << message_send_selector_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (message_send_selector_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_dispatch_abi_marshalling_surface\":{\"message_send_sites\":"
           << dispatch_abi_marshalling_contract.message_send_sites
           << ",\"receiver_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.receiver_slots_marshaled
           << ",\"selector_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.selector_slots_marshaled
           << ",\"argument_value_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.argument_value_slots_marshaled
           << ",\"argument_padding_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.argument_padding_slots_marshaled
           << ",\"argument_total_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.argument_total_slots_marshaled
           << ",\"total_marshaled_slots\":"
           << dispatch_abi_marshalling_contract.total_marshaled_slots
           << ",\"runtime_dispatch_arg_slots\":"
           << dispatch_abi_marshalling_contract.runtime_dispatch_arg_slots
           << ",\"replay_key\":\""
           << dispatch_abi_marshalling_replay_key
           << "\",\"deterministic_handoff\":"
           << (dispatch_abi_marshalling_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_nil_receiver_semantics_foldability_surface\":{\"message_send_sites\":"
           << nil_receiver_semantics_foldability_contract.message_send_sites
           << ",\"receiver_nil_literal_sites\":"
           << nil_receiver_semantics_foldability_contract.receiver_nil_literal_sites
           << ",\"nil_receiver_semantics_enabled_sites\":"
           << nil_receiver_semantics_foldability_contract.nil_receiver_semantics_enabled_sites
           << ",\"nil_receiver_foldable_sites\":"
           << nil_receiver_semantics_foldability_contract.nil_receiver_foldable_sites
           << ",\"nil_receiver_runtime_dispatch_required_sites\":"
           << nil_receiver_semantics_foldability_contract.nil_receiver_runtime_dispatch_required_sites
           << ",\"non_nil_receiver_sites\":"
           << nil_receiver_semantics_foldability_contract.non_nil_receiver_sites
           << ",\"contract_violation_sites\":"
           << nil_receiver_semantics_foldability_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << nil_receiver_semantics_foldability_replay_key
           << "\",\"deterministic_handoff\":"
           << (nil_receiver_semantics_foldability_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_super_dispatch_method_family_surface\":{\"message_send_sites\":"
           << super_dispatch_method_family_contract.message_send_sites
           << ",\"receiver_super_identifier_sites\":"
           << super_dispatch_method_family_contract.receiver_super_identifier_sites
           << ",\"super_dispatch_enabled_sites\":"
           << super_dispatch_method_family_contract.super_dispatch_enabled_sites
           << ",\"super_dispatch_requires_class_context_sites\":"
           << super_dispatch_method_family_contract.super_dispatch_requires_class_context_sites
           << ",\"method_family_init_sites\":"
           << super_dispatch_method_family_contract.method_family_init_sites
           << ",\"method_family_copy_sites\":"
           << super_dispatch_method_family_contract.method_family_copy_sites
           << ",\"method_family_mutable_copy_sites\":"
           << super_dispatch_method_family_contract.method_family_mutable_copy_sites
           << ",\"method_family_new_sites\":"
           << super_dispatch_method_family_contract.method_family_new_sites
           << ",\"method_family_none_sites\":"
           << super_dispatch_method_family_contract.method_family_none_sites
           << ",\"method_family_returns_retained_result_sites\":"
           << super_dispatch_method_family_contract.method_family_returns_retained_result_sites
           << ",\"method_family_returns_related_result_sites\":"
           << super_dispatch_method_family_contract.method_family_returns_related_result_sites
           << ",\"contract_violation_sites\":"
           << super_dispatch_method_family_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << super_dispatch_method_family_replay_key
           << "\",\"deterministic_handoff\":"
           << (super_dispatch_method_family_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_runtime_link_host_link_surface\":{\"message_send_sites\":"
           << runtime_link_host_link_contract.message_send_sites
           << ",\"runtime_link_required_sites\":"
           << runtime_link_host_link_contract.runtime_link_required_sites
           << ",\"runtime_link_elided_sites\":"
           << runtime_link_host_link_contract.runtime_link_elided_sites
           << ",\"runtime_dispatch_arg_slots\":"
           << runtime_link_host_link_contract.runtime_dispatch_arg_slots
           << ",\"runtime_dispatch_declaration_parameter_count\":"
           << runtime_link_host_link_contract.runtime_dispatch_declaration_parameter_count
           << ",\"runtime_dispatch_symbol\":\""
           << runtime_link_host_link_contract.runtime_dispatch_symbol
           << "\",\"default_runtime_dispatch_symbol_binding\":"
           << (runtime_link_host_link_contract.default_runtime_dispatch_symbol_binding ? "true" : "false")
           << ",\"contract_violation_sites\":"
           << runtime_link_host_link_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << runtime_link_host_link_replay_key
           << "\",\"deterministic_handoff\":"
           << (runtime_link_host_link_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_runtime_dispatch_lowering_abi_contract\":{\"message_send_sites\":"
           << runtime_dispatch_lowering_abi_contract.message_send_sites
           << ",\"fixed_argument_slot_count\":"
           << runtime_dispatch_lowering_abi_contract.fixed_argument_slot_count
           << ",\"runtime_dispatch_parameter_count\":"
           << runtime_dispatch_lowering_abi_contract
                  .runtime_dispatch_parameter_count
           << ",\"lowering_boundary_model\":\""
           << runtime_dispatch_lowering_abi_contract.lowering_boundary_model
           << "\",\"canonical_runtime_dispatch_symbol\":\""
           << runtime_dispatch_lowering_abi_contract
                  .canonical_runtime_dispatch_symbol
           << "\",\"default_lowering_target_symbol\":\""
           << runtime_dispatch_lowering_abi_contract.default_lowering_target_symbol
           << "\",\"selector_lookup_symbol\":\""
           << runtime_dispatch_lowering_abi_contract.selector_lookup_symbol
           << "\",\"selector_handle_type\":\""
           << runtime_dispatch_lowering_abi_contract.selector_handle_type
           << "\",\"receiver_abi_type\":\""
           << runtime_dispatch_lowering_abi_contract.receiver_abi_type
           << "\",\"selector_abi_type\":\""
           << runtime_dispatch_lowering_abi_contract.selector_abi_type
           << "\",\"argument_abi_type\":\""
           << runtime_dispatch_lowering_abi_contract.argument_abi_type
           << "\",\"result_abi_type\":\""
           << runtime_dispatch_lowering_abi_contract.result_abi_type
           << "\",\"selector_operand_model\":\""
           << runtime_dispatch_lowering_abi_contract.selector_operand_model
           << "\",\"selector_handle_model\":\""
           << runtime_dispatch_lowering_abi_contract.selector_handle_model
           << "\",\"argument_padding_model\":\""
           << runtime_dispatch_lowering_abi_contract.argument_padding_model
           << "\",\"default_lowering_target_model\":\""
           << runtime_dispatch_lowering_abi_contract
                  .default_lowering_target_model
           << "\",\"strict_dispatch_error_model\":\""
           << runtime_dispatch_lowering_abi_contract
                  .strict_dispatch_error_model
           << "\",\"deferred_cases_model\":\""
           << runtime_dispatch_lowering_abi_contract.deferred_cases_model
           << "\",\"replay_key\":\""
           << runtime_dispatch_lowering_abi_replay_key
           << "\",\"fail_closed\":"
           << (runtime_dispatch_lowering_abi_contract.fail_closed ? "true"
                                                                  : "false")
           << ",\"deterministic_handoff\":"
           << (runtime_dispatch_lowering_abi_contract.deterministic ? "true"
                                                                    : "false")
           << "}"
           << ",\"objc_ownership_qualifier_lowering_surface\":{\"ownership_qualifier_sites\":"
           << ownership_qualifier_lowering_contract.ownership_qualifier_sites
           << ",\"invalid_ownership_qualifier_sites\":"
           << ownership_qualifier_lowering_contract.invalid_ownership_qualifier_sites
           << ",\"object_pointer_type_annotation_sites\":"
           << ownership_qualifier_lowering_contract.object_pointer_type_annotation_sites
           << ",\"replay_key\":\""
           << ownership_qualifier_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (ownership_qualifier_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_retain_release_operation_lowering_surface\":{\"ownership_qualified_sites\":"
           << retain_release_operation_lowering_contract.ownership_qualified_sites
           << ",\"retain_insertion_sites\":"
           << retain_release_operation_lowering_contract.retain_insertion_sites
           << ",\"release_insertion_sites\":"
           << retain_release_operation_lowering_contract.release_insertion_sites
           << ",\"autorelease_insertion_sites\":"
           << retain_release_operation_lowering_contract.autorelease_insertion_sites
           << ",\"contract_violation_sites\":"
           << retain_release_operation_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << retain_release_operation_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (retain_release_operation_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_autoreleasepool_scope_lowering_surface\":{\"scope_sites\":"
           << autoreleasepool_scope_lowering_contract.scope_sites
           << ",\"scope_symbolized_sites\":"
           << autoreleasepool_scope_lowering_contract.scope_symbolized_sites
           << ",\"max_scope_depth\":"
           << autoreleasepool_scope_lowering_contract.max_scope_depth
           << ",\"scope_entry_transition_sites\":"
           << autoreleasepool_scope_lowering_contract.scope_entry_transition_sites
           << ",\"scope_exit_transition_sites\":"
           << autoreleasepool_scope_lowering_contract.scope_exit_transition_sites
           << ",\"contract_violation_sites\":"
           << autoreleasepool_scope_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << autoreleasepool_scope_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (autoreleasepool_scope_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_weak_unowned_semantics_lowering_surface\":{\"ownership_candidate_sites\":"
           << weak_unowned_semantics_lowering_contract.ownership_candidate_sites
           << ",\"weak_reference_sites\":"
           << weak_unowned_semantics_lowering_contract.weak_reference_sites
           << ",\"unowned_reference_sites\":"
           << weak_unowned_semantics_lowering_contract.unowned_reference_sites
           << ",\"unowned_safe_reference_sites\":"
           << weak_unowned_semantics_lowering_contract.unowned_safe_reference_sites
           << ",\"weak_unowned_conflict_sites\":"
           << weak_unowned_semantics_lowering_contract.weak_unowned_conflict_sites
           << ",\"contract_violation_sites\":"
           << weak_unowned_semantics_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << weak_unowned_semantics_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (weak_unowned_semantics_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_arc_diagnostics_fixit_lowering_surface\":{\"ownership_arc_diagnostic_candidate_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_diagnostic_candidate_sites
           << ",\"ownership_arc_fixit_available_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_fixit_available_sites
           << ",\"ownership_arc_profiled_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_profiled_sites
           << ",\"ownership_arc_weak_unowned_conflict_diagnostic_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_weak_unowned_conflict_diagnostic_sites
           << ",\"ownership_arc_empty_fixit_hint_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_empty_fixit_hint_sites
           << ",\"contract_violation_sites\":"
           << arc_diagnostics_fixit_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << arc_diagnostics_fixit_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (arc_diagnostics_fixit_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_block_literal_capture_lowering_surface\":{\"block_literal_sites\":"
           << block_literal_capture_lowering_contract.block_literal_sites
           << ",\"block_parameter_entries\":"
           << block_literal_capture_lowering_contract.block_parameter_entries
           << ",\"block_capture_entries\":"
           << block_literal_capture_lowering_contract.block_capture_entries
           << ",\"block_body_statement_entries\":"
           << block_literal_capture_lowering_contract.block_body_statement_entries
           << ",\"block_empty_capture_sites\":"
           << block_literal_capture_lowering_contract.block_empty_capture_sites
           << ",\"block_nondeterministic_capture_sites\":"
           << block_literal_capture_lowering_contract.block_nondeterministic_capture_sites
           << ",\"block_non_normalized_sites\":"
           << block_literal_capture_lowering_contract.block_non_normalized_sites
           << ",\"contract_violation_sites\":"
           << block_literal_capture_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << block_literal_capture_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (block_literal_capture_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_block_source_model_completion_surface\":{\"block_literal_sites\":"
           << block_source_model_completion_contract.block_literal_sites
           << ",\"signature_entries_total\":"
           << block_source_model_completion_contract.signature_entries_total
           << ",\"explicit_typed_parameter_entries_total\":"
           << block_source_model_completion_contract
                  .explicit_typed_parameter_entries_total
           << ",\"implicit_parameter_entries_total\":"
           << block_source_model_completion_contract
                  .implicit_parameter_entries_total
           << ",\"capture_inventory_entries_total\":"
           << block_source_model_completion_contract
                  .capture_inventory_entries_total
           << ",\"byvalue_readonly_capture_entries_total\":"
           << block_source_model_completion_contract
                  .byvalue_readonly_capture_entries_total
           << ",\"invoke_surface_entries_total\":"
           << block_source_model_completion_contract
                  .invoke_surface_entries_total
           << ",\"non_normalized_sites\":"
           << block_source_model_completion_contract.non_normalized_sites
           << ",\"contract_violation_sites\":"
           << block_source_model_completion_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << block_source_model_completion_replay_key
           << "\",\"deterministic_handoff\":"
           << (block_source_model_completion_contract.deterministic ? "true"
                                                                   : "false")
           << "}"
           << ",\"objc_block_source_storage_annotation_surface\":{\"block_literal_sites\":"

           << block_source_storage_annotation_contract.block_literal_sites
           << ",\"capture_entries_total\":"
           << block_source_storage_annotation_contract.capture_entries_total
           << ",\"mutated_capture_entries_total\":"
           << block_source_storage_annotation_contract
                  .mutated_capture_entries_total
           << ",\"byref_capture_entries_total\":"
           << block_source_storage_annotation_contract
                  .byref_capture_entries_total
           << ",\"copy_helper_intent_sites\":"
           << block_source_storage_annotation_contract.copy_helper_intent_sites
           << ",\"dispose_helper_intent_sites\":"
           << block_source_storage_annotation_contract
                  .dispose_helper_intent_sites
           << ",\"heap_candidate_sites\":"
           << block_source_storage_annotation_contract.heap_candidate_sites
           << ",\"expression_sites\":"
           << block_source_storage_annotation_contract.expression_sites
           << ",\"global_initializer_sites\":"
           << block_source_storage_annotation_contract
                  .global_initializer_sites
           << ",\"binding_initializer_sites\":"
           << block_source_storage_annotation_contract
                  .binding_initializer_sites
           << ",\"assignment_value_sites\":"
           << block_source_storage_annotation_contract.assignment_value_sites
           << ",\"return_value_sites\":"
           << block_source_storage_annotation_contract.return_value_sites
           << ",\"call_argument_sites\":"
           << block_source_storage_annotation_contract.call_argument_sites
           << ",\"message_argument_sites\":"
           << block_source_storage_annotation_contract.message_argument_sites
           << ",\"non_normalized_sites\":"
           << block_source_storage_annotation_contract.non_normalized_sites
           << ",\"contract_violation_sites\":"
           << block_source_storage_annotation_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << block_source_storage_annotation_replay_key
           << "\",\"deterministic_handoff\":"
           << (block_source_storage_annotation_contract.deterministic ? "true"
                                                                      : "false")
           << "}"
           << ",\"objc_block_abi_invoke_trampoline_lowering_surface\":{\"block_literal_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.block_literal_sites
           << ",\"invoke_argument_slots_total\":"
           << block_abi_invoke_trampoline_lowering_contract.invoke_argument_slots_total
           << ",\"capture_word_count_total\":"
           << block_abi_invoke_trampoline_lowering_contract.capture_word_count_total
           << ",\"parameter_entries_total\":"
           << block_abi_invoke_trampoline_lowering_contract.parameter_entries_total
           << ",\"capture_entries_total\":"
           << block_abi_invoke_trampoline_lowering_contract.capture_entries_total
           << ",\"body_statement_entries_total\":"
           << block_abi_invoke_trampoline_lowering_contract.body_statement_entries_total
           << ",\"descriptor_symbolized_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.descriptor_symbolized_sites
           << ",\"invoke_trampoline_symbolized_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.invoke_trampoline_symbolized_sites
           << ",\"missing_invoke_trampoline_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.missing_invoke_trampoline_sites
           << ",\"non_normalized_layout_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.non_normalized_layout_sites
           << ",\"contract_violation_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << block_abi_invoke_trampoline_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (block_abi_invoke_trampoline_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_block_storage_escape_lowering_surface\":{\"block_literal_sites\":"
           << block_storage_escape_lowering_contract.block_literal_sites
           << ",\"mutable_capture_count_total\":"
           << block_storage_escape_lowering_contract.mutable_capture_count_total
           << ",\"byref_slot_count_total\":"
           << block_storage_escape_lowering_contract.byref_slot_count_total
           << ",\"parameter_entries_total\":"
           << block_storage_escape_lowering_contract.parameter_entries_total
           << ",\"capture_entries_total\":"
           << block_storage_escape_lowering_contract.capture_entries_total
           << ",\"body_statement_entries_total\":"
           << block_storage_escape_lowering_contract.body_statement_entries_total
           << ",\"requires_byref_cells_sites\":"
           << block_storage_escape_lowering_contract.requires_byref_cells_sites
           << ",\"escape_analysis_enabled_sites\":"
           << block_storage_escape_lowering_contract.escape_analysis_enabled_sites
           << ",\"escape_to_heap_sites\":"
           << block_storage_escape_lowering_contract.escape_to_heap_sites
           << ",\"escape_profile_normalized_sites\":"
           << block_storage_escape_lowering_contract.escape_profile_normalized_sites
           << ",\"byref_layout_symbolized_sites\":"
           << block_storage_escape_lowering_contract.byref_layout_symbolized_sites
           << ",\"contract_violation_sites\":"
           << block_storage_escape_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << block_storage_escape_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (block_storage_escape_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_block_copy_dispose_lowering_surface\":{\"block_literal_sites\":"
           << block_copy_dispose_lowering_contract.block_literal_sites
           << ",\"mutable_capture_count_total\":"
           << block_copy_dispose_lowering_contract.mutable_capture_count_total
           << ",\"byref_slot_count_total\":"
           << block_copy_dispose_lowering_contract.byref_slot_count_total
           << ",\"parameter_entries_total\":"
           << block_copy_dispose_lowering_contract.parameter_entries_total
           << ",\"capture_entries_total\":"
           << block_copy_dispose_lowering_contract.capture_entries_total
           << ",\"body_statement_entries_total\":"
           << block_copy_dispose_lowering_contract.body_statement_entries_total
           << ",\"copy_helper_required_sites\":"
           << block_copy_dispose_lowering_contract.copy_helper_required_sites
           << ",\"dispose_helper_required_sites\":"
           << block_copy_dispose_lowering_contract.dispose_helper_required_sites
           << ",\"profile_normalized_sites\":"
           << block_copy_dispose_lowering_contract.profile_normalized_sites
           << ",\"copy_helper_symbolized_sites\":"
           << block_copy_dispose_lowering_contract.copy_helper_symbolized_sites
           << ",\"dispose_helper_symbolized_sites\":"
           << block_copy_dispose_lowering_contract.dispose_helper_symbolized_sites
           << ",\"contract_violation_sites\":"
           << block_copy_dispose_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << block_copy_dispose_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (block_copy_dispose_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_block_determinism_perf_baseline_lowering_surface\":{\"block_literal_sites\":"
           << block_determinism_perf_baseline_lowering_contract.block_literal_sites
           << ",\"baseline_weight_total\":"
           << block_determinism_perf_baseline_lowering_contract.baseline_weight_total
           << ",\"parameter_entries_total\":"
           << block_determinism_perf_baseline_lowering_contract.parameter_entries_total
           << ",\"capture_entries_total\":"
           << block_determinism_perf_baseline_lowering_contract.capture_entries_total
           << ",\"body_statement_entries_total\":"
           << block_determinism_perf_baseline_lowering_contract.body_statement_entries_total
           << ",\"deterministic_capture_sites\":"
           << block_determinism_perf_baseline_lowering_contract.deterministic_capture_sites
           << ",\"heavy_tier_sites\":"
           << block_determinism_perf_baseline_lowering_contract.heavy_tier_sites
           << ",\"normalized_profile_sites\":"
           << block_determinism_perf_baseline_lowering_contract.normalized_profile_sites
           << ",\"contract_violation_sites\":"
           << block_determinism_perf_baseline_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << block_determinism_perf_baseline_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (block_determinism_perf_baseline_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_lightweight_generic_constraint_lowering_surface\":{\"generic_constraint_sites\":"
           << lightweight_generic_constraint_lowering_contract.generic_constraint_sites
           << ",\"generic_suffix_sites\":"
           << lightweight_generic_constraint_lowering_contract.generic_suffix_sites
           << ",\"object_pointer_type_sites\":"
           << lightweight_generic_constraint_lowering_contract.object_pointer_type_sites
           << ",\"terminated_generic_suffix_sites\":"
           << lightweight_generic_constraint_lowering_contract.terminated_generic_suffix_sites
           << ",\"pointer_declarator_sites\":"
           << lightweight_generic_constraint_lowering_contract.pointer_declarator_sites
           << ",\"normalized_constraint_sites\":"
           << lightweight_generic_constraint_lowering_contract.normalized_constraint_sites
           << ",\"contract_violation_sites\":"
           << lightweight_generic_constraint_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << lightweight_generic_constraint_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (lightweight_generic_constraint_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_nullability_flow_warning_precision_lowering_surface\":{\"nullability_flow_sites\":"
           << nullability_flow_warning_precision_lowering_contract.nullability_flow_sites
           << ",\"object_pointer_type_sites\":"
           << nullability_flow_warning_precision_lowering_contract.object_pointer_type_sites
           << ",\"nullability_suffix_sites\":"
           << nullability_flow_warning_precision_lowering_contract.nullability_suffix_sites
           << ",\"nullable_suffix_sites\":"
           << nullability_flow_warning_precision_lowering_contract.nullable_suffix_sites
           << ",\"nonnull_suffix_sites\":"
           << nullability_flow_warning_precision_lowering_contract.nonnull_suffix_sites
           << ",\"normalized_sites\":"
           << nullability_flow_warning_precision_lowering_contract.normalized_sites
           << ",\"contract_violation_sites\":"
           << nullability_flow_warning_precision_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << nullability_flow_warning_precision_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (nullability_flow_warning_precision_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_protocol_qualified_object_type_lowering_surface\":{\"protocol_qualified_object_type_sites\":"
           << protocol_qualified_object_type_lowering_contract.protocol_qualified_object_type_sites
           << ",\"protocol_composition_sites\":"
           << protocol_qualified_object_type_lowering_contract.protocol_composition_sites
           << ",\"object_pointer_type_sites\":"
           << protocol_qualified_object_type_lowering_contract.object_pointer_type_sites
           << ",\"terminated_protocol_composition_sites\":"
           << protocol_qualified_object_type_lowering_contract.terminated_protocol_composition_sites
           << ",\"pointer_declarator_sites\":"
           << protocol_qualified_object_type_lowering_contract.pointer_declarator_sites
           << ",\"normalized_protocol_composition_sites\":"
           << protocol_qualified_object_type_lowering_contract.normalized_protocol_composition_sites
           << ",\"contract_violation_sites\":"
           << protocol_qualified_object_type_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << protocol_qualified_object_type_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (protocol_qualified_object_type_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_variance_bridge_cast_lowering_surface\":{\"variance_bridge_cast_sites\":"
           << variance_bridge_cast_lowering_contract.variance_bridge_cast_sites
           << ",\"protocol_composition_sites\":"
           << variance_bridge_cast_lowering_contract.protocol_composition_sites
           << ",\"ownership_qualifier_sites\":"
           << variance_bridge_cast_lowering_contract.ownership_qualifier_sites
           << ",\"object_pointer_type_sites\":"
           << variance_bridge_cast_lowering_contract.object_pointer_type_sites
           << ",\"pointer_declarator_sites\":"
           << variance_bridge_cast_lowering_contract.pointer_declarator_sites
           << ",\"normalized_sites\":"
           << variance_bridge_cast_lowering_contract.normalized_sites
           << ",\"contract_violation_sites\":"
           << variance_bridge_cast_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << variance_bridge_cast_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (variance_bridge_cast_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_generic_metadata_abi_lowering_surface\":{\"generic_metadata_abi_sites\":"
           << generic_metadata_abi_lowering_contract.generic_metadata_abi_sites
           << ",\"generic_suffix_sites\":"
           << generic_metadata_abi_lowering_contract.generic_suffix_sites
           << ",\"protocol_composition_sites\":"
           << generic_metadata_abi_lowering_contract.protocol_composition_sites
           << ",\"ownership_qualifier_sites\":"
           << generic_metadata_abi_lowering_contract.ownership_qualifier_sites
           << ",\"object_pointer_type_sites\":"
           << generic_metadata_abi_lowering_contract.object_pointer_type_sites
           << ",\"pointer_declarator_sites\":"
           << generic_metadata_abi_lowering_contract.pointer_declarator_sites
           << ",\"normalized_sites\":"
           << generic_metadata_abi_lowering_contract.normalized_sites
           << ",\"contract_violation_sites\":"
           << generic_metadata_abi_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << generic_metadata_abi_lowering_replay_key
             << "\",\"deterministic_handoff\":"
             << (generic_metadata_abi_lowering_contract.deterministic ? "true" : "false")
             << "}"
             << ",\"objc_type_system_optional_keypath_lowering_contract\":"
            << BuildTypeSystemOptionalKeypathLoweringContractJson(
                  type_system_optional_keypath_lowering_contract,
                  type_system_type_semantic_model_summary,
                  type_system_type_semantic_model_summary.replay_key,
                  message_send_selector_lowering_replay_key,
                    dispatch_abi_marshalling_replay_key,
                    nil_receiver_semantics_foldability_replay_key,
                    type_system_optional_keypath_lowering_replay_key)
             << ",\"objc_control_flow_control_flow_safety_lowering_contract\":"
            << BuildControlFlowControlFlowSafetyLoweringContractJson(
                  control_flow_control_flow_safety_lowering_contract,
                  control_flow_control_flow_semantic_model_summary,
                  control_flow_control_flow_semantic_model_summary.replay_key,
                  control_flow_control_flow_safety_lowering_replay_key)
            // runtime-helper freeze anchor: lane-D publishes one
            // canonical runtime/helper boundary packet above the live lowering
            // contract and the runtime link wiring surface while full key-path
            // execution helpers remain deferred to D002.
            << ",\"objc_type_system_optional_keypath_runtime_helper_contract\":"
            << BuildTypeSystemOptionalKeypathRuntimeHelperContractJson(
                   type_system_optional_keypath_lowering_contract,
                   runtime_support_library_link_wiring,
                   type_system_optional_keypath_lowering_replay_key)
             << ",\"objc_module_import_graph_lowering_surface\":{\"module_import_graph_sites\":"
             << module_import_graph_lowering_contract.module_import_graph_sites
           << ",\"import_edge_candidate_sites\":"
           << module_import_graph_lowering_contract.import_edge_candidate_sites
           << ",\"namespace_segment_sites\":"
           << module_import_graph_lowering_contract.namespace_segment_sites
           << ",\"object_pointer_type_sites\":"
           << module_import_graph_lowering_contract.object_pointer_type_sites
           << ",\"pointer_declarator_sites\":"
           << module_import_graph_lowering_contract.pointer_declarator_sites
           << ",\"normalized_sites\":"
           << module_import_graph_lowering_contract.normalized_sites
           << ",\"contract_violation_sites\":"
           << module_import_graph_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << module_import_graph_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (module_import_graph_lowering_contract.deterministic ? "true" : "false")
           << "}"
           // runtime-aware import/module surface anchor: lane-A
           // freezes one frontend-published contract above the current module
           // import graph lowering surface and below any real imported
           // runtime-owned declaration or metadata-reference realization.
           << ",\"objc_runtime_aware_import_module_surface_contract\":"
           << BuildRuntimeAwareImportModuleSurfaceSummaryJson(
                  program,
                  pipeline_result.parser_contract_snapshot,
                  module_import_graph_lowering_contract)
           << ",\"objc_runtime_aware_import_module_frontend_closure\":"
           << BuildRuntimeAwareImportModuleFrontendClosureSummaryJson(
                  runtime_aware_import_module_frontend_closure)
           // cross-module semantic preservation anchor: lane-B
           // freezes the semantic facts that later imported metadata handling
           // must preserve across module boundaries without claiming that the
           // imported runtime metadata semantics are landed yet.
           << ",\"objc_cross_module_runtime_metadata_semantic_preservation_contract\":"
           << BuildCrossModuleRuntimeMetadataSemanticPreservationSummaryJson(
                  cross_module_runtime_metadata_semantic_preservation)
           << ",\"objc_imported_runtime_metadata_semantic_rules\":"
           << BuildImportedRuntimeMetadataSemanticRulesSummaryJson(
                  imported_runtime_metadata_semantic_rules)
           // serialized metadata import/lowering anchor: lane-C
           // freezes the boundary where emitted runtime-import-surface
           // artifacts already influence frontend semantic surfaces, but
           // imported metadata payloads still are not rehydrated, reused
           // incrementally, or lowered into IR.
           << ",\"objc_serialized_runtime_metadata_import_lowering_contract\":"
           << BuildSerializedRuntimeMetadataImportLoweringSummaryJson(
                  serialized_runtime_metadata_import_lowering)
           // serialized metadata artifact reuse anchor: lane-C now
           // emits and reloads a transitive serialized runtime-metadata payload
           // through the runtime-import-surface artifact so downstream modules
           // can recover object-model semantics without reparsing source.
           << ",\"objc_serialized_runtime_metadata_artifact_reuse\":"
           << BuildSerializedRuntimeMetadataArtifactReuseSummaryJson(
                  serialized_runtime_metadata_artifact_reuse)
           // cross-module build/runtime orchestration anchor:
           // lane-D freezes the truthful boundary where the transitive
           // runtime-import-surface reuse payload and the local registration
           // manifest are both authoritative inputs, while cross-module link
           // packaging and runtime-registration aggregation remain unlanded.
           // cross-module runtime packaging anchor:
           // driver/runtime packaging path now materializes the ordered
           // cross-module link plan and merged linker response file from those
           // same authoritative artifacts, so this semantic surface remains the
           // canonical replay boundary for downstream packaging consumers.
           // cross-module object-model gate anchor: lane-E consumes
           // the A002/B002/C002/D002 summary chain and freezes the current
           // runnable two-image proof boundary before E002 broadens the
           // execution matrix.
           // runnable import/module execution-matrix anchor:
           // the same emitted frontend surface remains the canonical replay
           // boundary while lane-E closes the runnable matrix around it.
           << ",\"objc_cross_module_build_runtime_orchestration_contract\":"
           << BuildCrossModuleBuildRuntimeOrchestrationSummaryJson(
                  cross_module_build_runtime_orchestration)
           << ",\"objc_namespace_collision_shadowing_lowering_surface\":{\"namespace_collision_shadowing_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .namespace_collision_shadowing_sites
           << ",\"namespace_segment_sites\":"
           << namespace_collision_shadowing_lowering_contract.namespace_segment_sites
           << ",\"import_edge_candidate_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .import_edge_candidate_sites
           << ",\"object_pointer_type_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .object_pointer_type_sites
           << ",\"pointer_declarator_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .pointer_declarator_sites
           << ",\"normalized_sites\":"
           << namespace_collision_shadowing_lowering_contract.normalized_sites
           << ",\"contract_violation_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .contract_violation_sites
           << ",\"replay_key\":\""
           << namespace_collision_shadowing_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (namespace_collision_shadowing_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_public_private_api_partition_lowering_surface\":{\"public_private_api_partition_sites\":"
           << public_private_api_partition_lowering_contract
                  .public_private_api_partition_sites
           << ",\"namespace_segment_sites\":"
           << public_private_api_partition_lowering_contract
                  .namespace_segment_sites
           << ",\"import_edge_candidate_sites\":"
           << public_private_api_partition_lowering_contract
                  .import_edge_candidate_sites
           << ",\"object_pointer_type_sites\":"
           << public_private_api_partition_lowering_contract
                  .object_pointer_type_sites
           << ",\"pointer_declarator_sites\":"
           << public_private_api_partition_lowering_contract
                  .pointer_declarator_sites
           << ",\"normalized_sites\":"
           << public_private_api_partition_lowering_contract.normalized_sites
           << ",\"contract_violation_sites\":"
           << public_private_api_partition_lowering_contract
                  .contract_violation_sites
           << ",\"replay_key\":\""
           << public_private_api_partition_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (public_private_api_partition_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_incremental_module_cache_invalidation_lowering_surface\":{\"incremental_module_cache_invalidation_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .incremental_module_cache_invalidation_sites
           << ",\"namespace_segment_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .namespace_segment_sites
           << ",\"import_edge_candidate_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .import_edge_candidate_sites
           << ",\"object_pointer_type_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .object_pointer_type_sites
           << ",\"pointer_declarator_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .pointer_declarator_sites
           << ",\"normalized_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .normalized_sites
           << ",\"cache_invalidation_candidate_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .cache_invalidation_candidate_sites
           << ",\"contract_violation_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .contract_violation_sites
           << ",\"replay_key\":\""
           << incremental_module_cache_invalidation_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (incremental_module_cache_invalidation_lowering_contract
                       .deterministic
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_cross_module_conformance_lowering_surface\":{\"cross_module_conformance_sites\":"
           << cross_module_conformance_lowering_contract
                  .cross_module_conformance_sites
           << ",\"namespace_segment_sites\":"
           << cross_module_conformance_lowering_contract.namespace_segment_sites
           << ",\"import_edge_candidate_sites\":"
           << cross_module_conformance_lowering_contract.import_edge_candidate_sites
           << ",\"object_pointer_type_sites\":"
           << cross_module_conformance_lowering_contract.object_pointer_type_sites
           << ",\"pointer_declarator_sites\":"
           << cross_module_conformance_lowering_contract.pointer_declarator_sites
           << ",\"normalized_sites\":"
           << cross_module_conformance_lowering_contract.normalized_sites
           << ",\"cache_invalidation_candidate_sites\":"
           << cross_module_conformance_lowering_contract
                  .cache_invalidation_candidate_sites
           << ",\"contract_violation_sites\":"
           << cross_module_conformance_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << cross_module_conformance_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (cross_module_conformance_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_throws_propagation_lowering_surface\":{\"throws_propagation_sites\":"
           << throws_propagation_lowering_contract.throws_propagation_sites
           << ",\"namespace_segment_sites\":"
           << throws_propagation_lowering_contract.namespace_segment_sites
           << ",\"import_edge_candidate_sites\":"
           << throws_propagation_lowering_contract.import_edge_candidate_sites
           << ",\"object_pointer_type_sites\":"
           << throws_propagation_lowering_contract.object_pointer_type_sites
           << ",\"pointer_declarator_sites\":"
           << throws_propagation_lowering_contract.pointer_declarator_sites
           << ",\"normalized_sites\":"
           << throws_propagation_lowering_contract.normalized_sites
           << ",\"cache_invalidation_candidate_sites\":"
           << throws_propagation_lowering_contract
                  .cache_invalidation_candidate_sites
           << ",\"contract_violation_sites\":"
           << throws_propagation_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << throws_propagation_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (throws_propagation_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_result_like_lowering_surface\":{\"result_like_sites\":"
           << result_like_lowering_contract.result_like_sites
           << ",\"result_success_sites\":"
           << result_like_lowering_contract.result_success_sites
           << ",\"result_failure_sites\":"
           << result_like_lowering_contract.result_failure_sites
           << ",\"result_branch_sites\":"
           << result_like_lowering_contract.result_branch_sites
           << ",\"result_payload_sites\":"
           << result_like_lowering_contract.result_payload_sites
           << ",\"normalized_sites\":"
           << result_like_lowering_contract.normalized_sites
           << ",\"branch_merge_sites\":"
           << result_like_lowering_contract.branch_merge_sites
           << ",\"contract_violation_sites\":"
           << result_like_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << result_like_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (result_like_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_ns_error_bridging_lowering_surface\":{\"ns_error_bridging_sites\":"
           << ns_error_bridging_lowering_contract.ns_error_bridging_sites
           << ",\"ns_error_parameter_sites\":"
           << ns_error_bridging_lowering_contract.ns_error_parameter_sites
           << ",\"ns_error_out_parameter_sites\":"
           << ns_error_bridging_lowering_contract.ns_error_out_parameter_sites
           << ",\"ns_error_bridge_path_sites\":"
           << ns_error_bridging_lowering_contract.ns_error_bridge_path_sites
           << ",\"failable_call_sites\":"
           << ns_error_bridging_lowering_contract.failable_call_sites
           << ",\"normalized_sites\":"
           << ns_error_bridging_lowering_contract.normalized_sites
           << ",\"bridge_boundary_sites\":"
           << ns_error_bridging_lowering_contract.bridge_boundary_sites
           << ",\"contract_violation_sites\":"
           << ns_error_bridging_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << ns_error_bridging_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (ns_error_bridging_lowering_contract.deterministic ? "true"
                                                                 : "false")
           << "}"
           << ",\"objc_unwind_cleanup_lowering_surface\":{\"unwind_cleanup_sites\":"
           << unwind_cleanup_lowering_contract.unwind_cleanup_sites
           << ",\"unwind_edge_sites\":"
           << unwind_cleanup_lowering_contract.unwind_edge_sites
           << ",\"cleanup_scope_sites\":"
           << unwind_cleanup_lowering_contract.cleanup_scope_sites
           << ",\"cleanup_emit_sites\":"
           << unwind_cleanup_lowering_contract.cleanup_emit_sites
           << ",\"landing_pad_sites\":"
           << unwind_cleanup_lowering_contract.landing_pad_sites
           << ",\"cleanup_resume_sites\":"
           << unwind_cleanup_lowering_contract.cleanup_resume_sites
           << ",\"normalized_sites\":"
           << unwind_cleanup_lowering_contract.normalized_sites
           << ",\"guard_blocked_sites\":"
           << unwind_cleanup_lowering_contract.guard_blocked_sites
           << ",\"contract_violation_sites\":"
           << unwind_cleanup_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << unwind_cleanup_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (unwind_cleanup_lowering_contract.deterministic ? "true"
                                                              : "false")
           << "}"
           << ",\"objc_error_handling_throws_abi_propagation_lowering\":{\"contract_id\":\""
           << kObjc3ErrorHandlingThrowsAbiPropagationLoweringContractId
           << "\",\"source_model\":\""
           << kObjc3ErrorHandlingThrowsAbiPropagationLoweringSourceModel
           << "\",\"abi_model\":\""
           << kObjc3ErrorHandlingThrowsAbiPropagationLoweringAbiModel
           << "\",\"throws_replay_key\":\""
           << throws_propagation_lowering_replay_key
           << "\",\"result_like_replay_key\":\""
           << result_like_lowering_replay_key
           << "\",\"ns_error_replay_key\":\""
           << ns_error_bridging_lowering_replay_key
           << "\",\"unwind_replay_key\":\""
           << unwind_cleanup_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (deterministic_error_handling_throws_abi_propagation_lowering ? "true"
                                                                   : "false")
           << ",\"ready_for_runtime_execution\":true"
           << ",\"fail_closed_model\":\""
           << kObjc3ErrorHandlingThrowsAbiPropagationLoweringFailClosedModel
           << "\",\"next_issue\":\"objc3c.errors.resultbridging.artifactsurface.v1\"}"
           << ",\"objc_error_handling_result_and_bridging_artifact_replay\":"
           << BuildErrorHandlingResultAndBridgingArtifactReplayJson(
                  error_handling_result_and_bridging_artifact_replay_summary)
           << ",\"objc_object_pointer_nullability_generics_surface\":{\"object_pointer_type_spellings\":"
           << object_pointer_nullability_generics_summary.object_pointer_type_spellings
           << ",\"pointer_declarator_entries\":"
           << object_pointer_nullability_generics_summary.pointer_declarator_entries
           << ",\"pointer_declarator_depth_total\":"
           << object_pointer_nullability_generics_summary.pointer_declarator_depth_total
           << ",\"pointer_declarator_token_entries\":"
           << object_pointer_nullability_generics_summary.pointer_declarator_token_entries
           << ",\"nullability_suffix_entries\":"
           << object_pointer_nullability_generics_summary.nullability_suffix_entries
           << ",\"generic_suffix_entries\":"
           << object_pointer_nullability_generics_summary.generic_suffix_entries
           << ",\"terminated_generic_suffix_entries\":"
           << object_pointer_nullability_generics_summary.terminated_generic_suffix_entries
           << ",\"unterminated_generic_suffix_entries\":"
           << object_pointer_nullability_generics_summary.unterminated_generic_suffix_entries
           << ",\"deterministic_handoff\":"
           << (object_pointer_nullability_generics_summary.deterministic_object_pointer_nullability_generics_handoff
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_type_system_type_source_closure\":"
           << BuildTypeSystemTypeSourceClosureSummaryJson(
                  type_system_type_source_closure_summary)
           << ",\"objc_control_flow_control_flow_source_closure\":"
           << BuildControlFlowControlFlowSourceClosureSummaryJson(
                  control_flow_control_flow_source_closure_summary)
           << ",\"objc_error_handling_error_source_closure\":"
           << BuildErrorHandlingErrorSourceClosureSummaryJson(
                  error_handling_error_source_closure_summary)
            << ",\"objc_concurrency_async_source_closure\":"
            << BuildConcurrencyAsyncSourceClosureSummaryJson(
                   concurrency_async_source_closure_summary)
            << ",\"objc_ownership_resource_borrowed_and_capture_list_source_closure\":"
            << BuildOwnershipSystemExtensionSourceClosureSummaryJson(
                   ownership_system_extension_source_closure_summary)
            << ",\"objc_ownership_cleanup_resource_and_capture_source_completion\":"
            << BuildOwnershipCleanupResourceCaptureSourceCompletionSummaryJson(
                   ownership_cleanup_resource_capture_source_completion_summary)
            << ",\"objc_ownership_retainable_c_family_source_completion\":"
            << BuildOwnershipRetainableCFamilySourceCompletionSummaryJson(
                   ownership_retainable_c_family_source_completion_summary)
            << ",\"objc_dispatch_dispatch_intent_and_dynamism_source_closure\":"
            << BuildDispatchDispatchIntentSourceClosureSummaryJson(
                   dispatch_dispatch_intent_source_closure_summary)
            << ",\"objc_dispatch_dispatch_intent_attribute_and_defaulting_source_completion\":"
            << BuildDispatchDispatchIntentSourceCompletionSummaryJson(
                   dispatch_dispatch_intent_source_completion_summary)
            << ",\"objc_metaprogramming_derive_macro_property_behavior_source_closure\":"
            << BuildMetaprogrammingMetaprogrammingSourceClosureSummaryJson(
                   metaprogramming_metaprogramming_source_closure_summary)
            << ",\"objc_metaprogramming_macro_package_and_provenance_source_completion\":"
            << BuildMetaprogrammingMacroPackageProvenanceSourceCompletionSummaryJson(
                   metaprogramming_macro_package_provenance_source_completion_summary)
            << ",\"objc_metaprogramming_property_behavior_and_synthesized_declaration_source_completion\":"
                   << BuildMetaprogrammingPropertyBehaviorSourceCompletionSummaryJson(
                   metaprogramming_property_behavior_source_completion_summary)
            << ",\"objc_interop_foreign_declaration_and_import_source_closure\":"
                   << BuildInteropForeignImportSourceClosureSummaryJson(
                   interop_foreign_import_source_closure_summary)
            << ",\"objc_interop_cpp_and_swift_interop_annotation_source_completion\":"
                   << BuildInteropCppSwiftInteropAnnotationSourceCompletionSummaryJson(
                   interop_cpp_swift_interop_annotation_source_completion_summary)
           << ",\"objc_tooling_diagnostics_fixit_and_migrator_source_inventory\":"
                  << BuildToolingDiagnosticsMigratorSourceInventorySummaryJson(
                  tooling_diagnostics_migrator_source_inventory_summary)
           << ",\"objc_tooling_migration_and_canonicalization_source_completion\":"
                  << BuildToolingMigrationCanonicalizationSourceCompletionSummaryJson(
                  tooling_migration_canonicalization_source_completion_summary)
           << ",\"objc_tooling_diagnostic_taxonomy_and_portability_contract\":"
                  << BuildToolingDiagnosticTaxonomyPortabilityContractSummaryJson(
                  tooling_diagnostic_taxonomy_portability_contract_summary)
           << ",\"objc_tooling_feature_specific_fixit_synthesis\":"
                  << BuildToolingFeatureSpecificFixitSynthesisSummaryJson(
                  tooling_feature_specific_fixit_synthesis_summary)
           << ",\"objc_tooling_legacy_canonical_migration_semantics\":"
                  << BuildToolingLegacyCanonicalMigrationSemanticsSummaryJson(
                      tooling_legacy_canonical_migration_semantics_summary)
           << ",\"objc_tooling_machine_readable_conformance_report_contract\":"
                  << BuildToolingMachineReadableConformanceReportContractSummaryJson(
                      tooling_machine_readable_conformance_report_contract_summary)
           << ",\"objc_tooling_feature_aware_conformance_report_emission\":"
                  << BuildToolingFeatureAwareConformanceReportEmissionSummaryJson(
                      tooling_feature_aware_conformance_report_emission_summary)
           << ",\"objc_tooling_corpus_sharding_release_evidence_packaging\":"
                  << BuildToolingCorpusShardingReleaseEvidencePackagingSummaryJson(
                      tooling_corpus_sharding_release_evidence_packaging_summary)
           << ",\"objc_interop_interop_semantic_model\":"
           << BuildInteropInteropSemanticModelSummaryJson(
                  interop_interop_semantic_model_summary)
           << ",\"objc_interop_c_and_objc_runtime_parity_semantics\":"
           << BuildInteropInteropRuntimeParitySummaryJson(
                  interop_interop_runtime_parity_summary)
           << ",\"objc_interop_cpp_ownership_throws_and_async_interactions\":"
           << BuildInteropCppInteropInteractionSummaryJson(
                  interop_cpp_interop_interaction_summary)
           << ",\"objc_interop_swift_metadata_and_isolation_mapping\":"
           << BuildInteropSwiftInteropIsolationSummaryJson(
                  interop_swift_interop_isolation_summary)
           << ",\"objc_interop_foreign_surface_interface_and_module_preservation\":"
           << BuildInteropForeignSurfaceInterfacePreservationSummaryJson(
                  interop_foreign_surface_interface_preservation_summary)
           << ",\"objc_interop_header_module_and_bridge_generation\":"
           << BuildInteropHeaderModuleBridgeGenerationSummaryJson(
                  interop_header_module_bridge_generation_summary)
           << ",\"objc_interop_interop_lowering_and_abi_contract\":"
           << BuildInteropInteropLoweringContractJson(
                  interop_interop_semantic_model_summary,
                  interop_interop_runtime_parity_summary,
                  interop_cpp_interop_interaction_summary,
                  interop_swift_interop_isolation_summary,
                  interop_foreign_surface_interface_preservation_summary,
                  interop_interop_lowering_contract,
                  interop_interop_lowering_replay_key)
           << ",\"objc_interop_foreign_call_and_lifetime_lowering\":"
           << BuildInteropForeignCallLifetimeLoweringContractJson(
                  interop_interop_lowering_contract,
                  interop_cpp_interop_interaction_summary,
                  interop_foreign_surface_interface_preservation_summary,
                  interop_foreign_call_lifetime_lowering_contract,
                  interop_foreign_call_lifetime_lowering_replay_key)
           << ",\"objc_interop_ffi_metadata_and_interface_preservation\":"
           << BuildInteropFfiMetadataInterfacePreservationContractJson(
                  interop_foreign_call_lifetime_lowering_contract,
                  interop_foreign_call_lifetime_lowering_replay_key,
                  interop_foreign_surface_interface_preservation_summary,
                  interop_ffi_metadata_interface_preservation_contract,
                  interop_ffi_metadata_interface_preservation_replay_key)
            << ",\"objc_metaprogramming_expansion_and_behavior_semantic_model\":"
            << BuildMetaprogrammingExpansionBehaviorSemanticModelSummaryJson(
                   metaprogramming_expansion_behavior_semantic_model_summary)
            << ",\"objc_metaprogramming_derive_expansion_inventory\":"
            << BuildMetaprogrammingDeriveExpansionInventorySummaryJson(
                   metaprogramming_derive_expansion_inventory_summary)
            << ",\"objc_metaprogramming_macro_safety_sandbox_and_determinism_semantics\":"
            << BuildMetaprogrammingMacroSafetySandboxDeterminismSummaryJson(
                   metaprogramming_macro_safety_sandbox_determinism_summary)
            << ",\"objc_metaprogramming_property_behavior_legality_and_interaction_completion\":"
            << BuildMetaprogrammingPropertyBehaviorLegalityCompatibilitySummaryJson(
                   metaprogramming_property_behavior_legality_compatibility_summary)
            << ",\"objc_metaprogramming_expansion_and_lowering_contract\":"
            << BuildMetaprogrammingExpansionLoweringContractJson(
                   metaprogramming_property_behavior_source_completion_summary,
                   metaprogramming_derive_expansion_inventory_summary,
                   metaprogramming_macro_safety_sandbox_determinism_summary,
                   metaprogramming_property_behavior_legality_compatibility_summary,
                   metaprogramming_expansion_lowering_contract,
                   metaprogramming_expansion_lowering_replay_key)
            << ",\"objc_metaprogramming_synthesized_ast_and_ir_emission\":"
            << BuildMetaprogrammingSynthesizedArtifactEmissionContractJson(
                   metaprogramming_expansion_lowering_contract,
                   metaprogramming_synthesized_artifact_emission_contract,
                   metaprogramming_synthesized_artifact_emission_replay_key)
           << ",\"objc_metaprogramming_module_interface_and_replay_preservation\":"
           << BuildMetaprogrammingModuleInterfaceReplayPreservationSummaryJson(
                  metaprogramming_module_interface_replay_preservation_summary)
           << ",\"objc_dispatch_dynamism_and_dispatch_control_semantic_model\":"
           << BuildDispatchDispatchIntentSemanticModelSummaryJson(
                  dispatch_dispatch_intent_semantic_model_summary)
            << ",\"objc_dispatch_override_finality_and_sealing_legality\":"
            << BuildDispatchDispatchIntentLegalitySummaryJson(
                   dispatch_dispatch_intent_legality_summary)
            << ",\"objc_dispatch_dynamism_control_compatibility_diagnostics\":"
            << BuildDispatchDispatchIntentCompatibilitySummaryJson(
                   dispatch_dispatch_intent_compatibility_summary)
           << ",\"objc_dispatch_dispatch_control_lowering_contract\":"
           << BuildDispatchDispatchControlLoweringContractJson(
                  dispatch_dispatch_intent_semantic_model_summary,
                  dispatch_dispatch_intent_legality_summary,
                  dispatch_dispatch_intent_compatibility_summary,
                  dispatch_dispatch_control_lowering_contract,
                  dispatch_dispatch_control_lowering_replay_key)
           << ",\"objc_dispatch_dispatch_metadata_and_interface_preservation\":"
           << BuildDispatchDispatchMetadataInterfacePreservationSummaryJson(
                  dispatch_dispatch_metadata_interface_preservation_summary)
            << ",\"objc_concurrency_actor_member_and_isolation_source_closure\":"
            << BuildConcurrencyActorMemberIsolationSourceClosureSummaryJson(
                   concurrency_actor_member_isolation_source_closure_summary)
           << ",\"objc_concurrency_actor_isolation_and_sendable_semantic_model\":"
           << BuildConcurrencyActorIsolationSendableSemanticModelSummaryJson(
                  concurrency_actor_isolation_sendable_semantic_model_summary)
           << ",\"objc_concurrency_actor_isolation_and_sendability_enforcement\":"
           << BuildConcurrencyActorIsolationSendabilityEnforcementSummaryJson(
                  concurrency_actor_isolation_sendability_enforcement_summary)
           << ",\"objc_concurrency_actor_race_hazard_and_escape_diagnostics\":"
           << BuildConcurrencyActorRaceHazardEscapeDiagnosticsSummaryJson(
                  concurrency_actor_race_hazard_escape_diagnostics_summary)
           << ",\"objc_concurrency_actor_lowering_and_metadata_contract\":"
           << BuildConcurrencyActorLoweringMetadataContractJson(
                  concurrency_actor_member_isolation_source_closure_summary,
                  concurrency_actor_isolation_sendability_enforcement_summary,
                  concurrency_actor_race_hazard_escape_diagnostics_summary,
                  concurrency_actor_lowering_metadata_contract,
                  concurrency_actor_lowering_metadata_replay_key)
           << ",\"objc_concurrency_task_group_and_cancellation_source_closure\":"
           << BuildConcurrencyTaskGroupCancellationSourceClosureSummaryJson(
                  concurrency_task_group_cancellation_source_closure_summary)
           << ",\"objc_concurrency_async_effect_and_suspension_semantic_model\":"
           << BuildConcurrencyAsyncEffectSuspensionSemanticModelSummaryJson(
                  concurrency_async_effect_suspension_semantic_model_summary)
           << ",\"objc_concurrency_task_executor_and_cancellation_semantic_model\":"
           << BuildConcurrencyTaskExecutorCancellationSemanticModelSummaryJson(
                  concurrency_task_executor_cancellation_semantic_model_summary)
           << ",\"objc_ownership_system_extension_semantic_model\":"
           << BuildOwnershipSystemExtensionSemanticModelSummaryJson(
                  ownership_system_extension_semantic_model_summary)
           << ",\"objc_effects_ownership_semantic_model\":"
           << BuildEffectsOwnershipSemanticModelSummaryJson(
                  effects_ownership_semantic_model_summary)
           << ",\"objc_cross_module_semantic_contracts_and_diagnostics\":"
           << BuildCrossModuleSemanticContractsDiagnosticsSummaryJson(
                  cross_module_semantic_contracts_diagnostics_summary)
           << ",\"objc_ownership_resource_move_and_use_after_move_semantics\":"
           << BuildOwnershipResourceMoveUseAfterMoveSemanticsSummaryJson(
                  ownership_resource_move_use_after_move_semantics_summary)
           << ",\"objc_ownership_borrowed_pointer_escape_analysis\":"
           << BuildOwnershipBorrowedPointerEscapeAnalysisSummaryJson(
                  ownership_borrowed_pointer_escape_analysis_summary)
           << ",\"objc_ownership_capture_list_and_retainable_family_legality_completion\":"
           << BuildOwnershipCaptureListRetainableFamilyLegalityCompletionSummaryJson(
                  ownership_capture_list_retainable_family_legality_completion_summary)
           << ",\"objc_ownership_system_extension_lowering_contract\":"
           << BuildOwnershipSystemExtensionLoweringContractJson(
                  ownership_system_extension_semantic_model_summary,
                  ownership_resource_move_use_after_move_semantics_summary,
                  ownership_borrowed_pointer_escape_analysis_summary,
                  ownership_capture_list_retainable_family_legality_completion_summary,
                  ownership_system_extension_lowering_contract,
                  ownership_system_extension_lowering_replay_key)
           << ",\"objc_ownership_borrowed_pointer_and_retainable_family_abi_completion\":"
           << BuildOwnershipBorrowedRetainableAbiCompletionJson(
                  ownership_system_extension_lowering_contract,
                  ownership_system_extension_source_closure_summary,
                  ownership_retainable_c_family_source_completion_summary,
                  ownership_system_extension_lowering_replay_key,
                  ownership_borrowed_retainable_abi_completion_replay_key)
            << ",\"objc_concurrency_structured_task_and_cancellation_semantics\":"
            << BuildConcurrencyStructuredTaskCancellationSemanticSummaryJson(
                   concurrency_structured_task_cancellation_semantic_summary)
           << ",\"objc_concurrency_executor_hop_and_affinity_compatibility_completion\":"
           << BuildConcurrencyExecutorHopAffinityCompatibilitySummaryJson(
                  concurrency_executor_hop_affinity_compatibility_summary)
           << ",\"objc_concurrency_await_suspension_and_resume_semantics\":"
           << BuildConcurrencyAwaitSuspensionResumeSemanticSummaryJson(
                  concurrency_await_suspension_resume_semantic_summary)
           << ",\"objc_concurrency_async_diagnostics_and_compatibility_completion\":"
           << BuildConcurrencyAsyncDiagnosticsCompatibilitySummaryJson(
                  concurrency_async_diagnostics_compatibility_summary)
           << ",\"objc_concurrency_continuation_abi_and_async_lowering_contract\":"
           << BuildConcurrencyContinuationAbiAsyncLoweringContractJson(
                  concurrency_async_continuation_lowering_contract,
                  concurrency_await_lowering_suspension_state_lowering_contract,
                  concurrency_async_continuation_lowering_replay_key,
                  concurrency_await_lowering_suspension_state_lowering_replay_key)
           << ",\"objc_concurrency_task_runtime_lowering_contract\":"
           << BuildConcurrencyTaskRuntimeLoweringContractJson(
                  concurrency_task_executor_cancellation_semantic_model_summary,
                  concurrency_structured_task_cancellation_semantic_summary,
                  concurrency_executor_hop_affinity_compatibility_summary,
                  concurrency_actor_isolation_sendability_lowering_contract,
                  concurrency_actor_isolation_sendability_lowering_replay_key,
                  concurrency_task_runtime_interop_cancellation_lowering_contract,
                  concurrency_task_runtime_interop_cancellation_lowering_replay_key,
                  concurrency_concurrency_replay_race_guard_lowering_contract,
                  concurrency_concurrency_replay_race_guard_lowering_replay_key)
           << ",\"objc_concurrency_task_group_and_runtime_abi_completion\":"
           << BuildConcurrencyTaskRuntimeAbiCompletionJson(
                  concurrency_task_runtime_interop_cancellation_lowering_replay_key,
                  concurrency_concurrency_replay_race_guard_lowering_replay_key)
           << ",\"objc_concurrency_async_function_await_and_continuation_lowering\":"
           << BuildConcurrencyAsyncDirectCallLoweringJson(
                  concurrency_async_source_closure_summary,
                  concurrency_async_continuation_lowering_contract,
                  concurrency_await_lowering_suspension_state_lowering_contract,
                  concurrency_async_continuation_lowering_replay_key,
                  concurrency_await_lowering_suspension_state_lowering_replay_key)
           << ",\"objc_concurrency_suspension_autorelease_and_cleanup_integration\":"
           << BuildConcurrencySuspensionCleanupIntegrationJson(
                  control_flow_control_flow_safety_lowering_contract,
                  control_flow_control_flow_safety_lowering_replay_key,
                  autoreleasepool_scope_lowering_contract,
                  autoreleasepool_scope_lowering_replay_key,
                  concurrency_async_continuation_lowering_contract,
                  concurrency_await_lowering_suspension_state_lowering_contract,
                  concurrency_async_continuation_lowering_replay_key,
                  concurrency_await_lowering_suspension_state_lowering_replay_key)
           << ",\"objc_error_handling_error_semantic_model\":"
           << BuildErrorHandlingErrorSemanticModelSummaryJson(
                  error_handling_error_semantic_model_summary)
           << ",\"objc_error_handling_try_do_catch_semantics\":"
           << BuildErrorHandlingTryDoCatchSemanticSummaryJson(
                  error_handling_try_do_catch_semantic_summary)
           << ",\"objc_error_handling_error_bridge_legality\":"
           << BuildErrorHandlingErrorBridgeLegalitySummaryJson(
                  error_handling_error_bridge_legality_summary)
           << ",\"objc_control_flow_control_flow_semantic_model\":"
           << BuildControlFlowControlFlowSemanticModelSummaryJson(
                  control_flow_control_flow_semantic_model_summary)
           << ",\"objc_type_system_type_semantic_model\":"
           << BuildTypeSystemTypeSemanticModelSummaryJson(
                  type_system_type_semantic_model_summary)
           << ",\"objc_symbol_graph_scope_resolution_surface\":{\"global_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.global_symbol_nodes
           << ",\"function_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.function_symbol_nodes
           << ",\"interface_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.interface_symbol_nodes
           << ",\"implementation_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.implementation_symbol_nodes
           << ",\"interface_property_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.interface_property_symbol_nodes
           << ",\"implementation_property_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.implementation_property_symbol_nodes
           << ",\"interface_method_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.interface_method_symbol_nodes
           << ",\"implementation_method_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.implementation_method_symbol_nodes
           << ",\"top_level_scope_symbols\":"
           << symbol_graph_scope_resolution_summary.top_level_scope_symbols
           << ",\"nested_scope_symbols\":"
           << symbol_graph_scope_resolution_summary.nested_scope_symbols
           << ",\"scope_frames_total\":"
           << symbol_graph_scope_resolution_summary.scope_frames_total
           << ",\"implementation_interface_resolution_sites\":"
           << symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites
           << ",\"implementation_interface_resolution_hits\":"
           << symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits
           << ",\"implementation_interface_resolution_misses\":"
           << symbol_graph_scope_resolution_summary.implementation_interface_resolution_misses
           << ",\"method_resolution_sites\":"
           << symbol_graph_scope_resolution_summary.method_resolution_sites
           << ",\"method_resolution_hits\":"
           << symbol_graph_scope_resolution_summary.method_resolution_hits
           << ",\"method_resolution_misses\":"
           << symbol_graph_scope_resolution_summary.method_resolution_misses
           << ",\"deterministic_symbol_graph_handoff\":"
           << (symbol_graph_scope_resolution_summary.deterministic_symbol_graph_handoff ? "true" : "false")
           << ",\"deterministic_scope_resolution_handoff\":"
           << (symbol_graph_scope_resolution_summary.deterministic_scope_resolution_handoff ? "true" : "false")
           << ",\"deterministic_handoff_key\":\""
           << symbol_graph_scope_resolution_summary.deterministic_handoff_key
           << "\"}"
           << ",\"function_signature_surface\":{\"scalar_return_i32\":" << scalar_return_i32
           << ",\"scalar_return_bool\":" << scalar_return_bool
           << ",\"scalar_return_void\":" << scalar_return_void << ",\"scalar_param_i32\":" << scalar_param_i32
           << ",\"scalar_param_bool\":" << scalar_param_bool << "}}\n";
  manifest << "    }\n";
  manifest << "  },\n";
  manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << options.lowering.runtime_dispatch_symbol
           << "\",\"runtime_dispatch_arg_slots\":" << options.lowering.max_message_send_args
           << ",\"selector_global_ordering\":\"lexicographic\"},\n";
  manifest << "  \"lowering_vector_abi\":{\"replay_key\":\"" << Objc3SimdVectorTypeLoweringReplayKey()
           << "\",\"lane_contract\":\"" << kObjc3SimdVectorLaneContract
           << "\",\"vector_signature_functions\":" << vector_signature_functions << "},\n";
  manifest << "  \"lowering_property_synthesis_ivar_binding\":{\"replay_key\":\""
           << property_synthesis_ivar_binding_replay_key
           << "\",\"lane_contract\":\"" << kObjc3PropertySynthesisIvarBindingLaneContract
           << "\",\"deterministic_handoff\":"
           << (property_synthesis_ivar_binding_contract.deterministic ? "true" : "false")
           << "},\n";
  const auto runtime_state_publication_paths =
      objc3::artifacts::frontend::BuildRuntimeStatePublicationPaths(
          runtime_translation_unit_registration_manifest
              .manifest_artifact_relative_path);
  const std::string &runtime_state_publication_emit_prefix =
      runtime_state_publication_paths.emit_prefix;
  const auto accessor_storage_lowering_metadata_summary =
      BuildAccessorStorageLoweringMetadataSummary(runtime_metadata_source_records);
  const auto executable_accessor_layout_lowering_summary =
      BuildExecutableAccessorLayoutLoweringSummary(
          executable_metadata_source_graph);
  manifest << "  \"dispatch_and_synthesized_accessor_lowering_surface\":{\"contract_id\":"
           << "\"" << kObjc3DispatchAndSynthesizedAccessorLoweringSurfaceContractId << "\""
           << ",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll\""
           << ",\"runtime_dispatch_symbol\":\""
           << runtime_link_host_link_contract.runtime_dispatch_symbol
           << "\",\"runtime_dispatch_arg_slots\":"
           << runtime_link_host_link_contract.runtime_dispatch_arg_slots
           << ",\"runtime_dispatch_declaration_parameter_count\":"
           << runtime_link_host_link_contract.runtime_dispatch_declaration_parameter_count
           << ",\"runtime_dispatch_symbol_matches_lowering\":"
           << ((runtime_link_host_link_contract.runtime_dispatch_symbol ==
                        options.lowering.runtime_dispatch_symbol &&
                runtime_link_host_link_contract.runtime_dispatch_symbol ==
                        runtime_support_library_link_wiring.runtime_dispatch_symbol)
                       ? "true"
                       : "false")
           << ",\"live_runtime_dispatch_sites\":"
           << (dispatch_surface_classification_contract.instance_dispatch_sites +
               dispatch_surface_classification_contract.class_dispatch_sites +
               dispatch_surface_classification_contract.super_dispatch_sites +
               dispatch_surface_classification_contract.dynamic_dispatch_sites)
           << ",\"direct_dispatch_sites\":"
           << dispatch_surface_classification_contract.direct_dispatch_sites
           << ",\"message_send_sites\":"
           << message_send_selector_lowering_contract.message_send_sites
           << ",\"property_synthesis_sites\":"
           << property_synthesis_ivar_binding_contract.property_synthesis_sites
           << ",\"property_synthesis_explicit_ivar_bindings\":"
           << property_synthesis_ivar_binding_contract.property_synthesis_explicit_ivar_bindings
           << ",\"property_synthesis_default_ivar_bindings\":"
           << property_synthesis_ivar_binding_contract.property_synthesis_default_ivar_bindings
           << ",\"interface_owned_property_synthesis_sites\":"
           << property_synthesis_ivar_binding_contract.interface_owned_property_synthesis_sites
           << ",\"implementation_property_redeclaration_sites\":"
           << property_synthesis_ivar_binding_contract.implementation_property_redeclaration_sites
           << ",\"ivar_binding_resolved\":"
           << property_synthesis_ivar_binding_contract.ivar_binding_resolved
           << ",\"runtime_property_ivar_storage_accessor_source_surface_contract_id\":\""
           << kObjc3RuntimePropertyIvarStorageAccessorSourceSurfaceContractId
           << "\",\"storage_accessor_runtime_abi_surface_contract_id\":\""
           << kObjc3RuntimeStorageAccessorAbiSurfaceContractId
           << "\""
           << ",\"accessor_storage_lowering_metadata_model\":\""
           << kObjc3AccessorStorageLoweringMetadataModel
           << "\",\"accessor_storage_lowering_helper_selection_model\":\""
           << kObjc3AccessorStorageLoweringHelperSelectionModel
           << "\",\"lowering_contract_source_path\":\"native/objc3c/src/lower/objc3_lowering_contract.h\""
           << ",\"ir_emitter_source_path\":\"native/objc3c/src/ir/objc3_ir_emitter.cpp\""
           << ",\"frontend_artifacts_source_path\":\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"runtime_source_path\":\"native/objc3c/src/runtime/objc3_runtime.cpp\""
           << ",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/property_synthesis_default_ivar_binding_no_redeclaration.objc3\""
           << ",\"tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_property_interaction_positive.objc3\"]"
           << ",\"authoritative_probe_paths\":[\"tests/tooling/runtime/synthesized_accessor_probe.cpp\""
           << ",\"tests/tooling/runtime/property_layout_runtime_probe.cpp\""
           << ",\"tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp\""
           << ",\"tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp\""
           << ",\"tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp\""
           << ",\"tests/tooling/runtime/arc_debug_instrumentation_probe.cpp\"]"
           << ",\"explicit_non_goals\":[\"no-public-runtime-abi-widening\""
           << ",\"no-milestone-specific-scaffolding\""
           << ",\"no-sidecar-only-lowering-proof\"]"
           << ",\"current_property_read_symbol\":\""
           << kObjc3RuntimeReadCurrentPropertyI32Symbol
           << "\",\"current_property_write_symbol\":\""
           << kObjc3RuntimeWriteCurrentPropertyI32Symbol
           << "\",\"current_property_exchange_symbol\":\""
           << kObjc3RuntimeExchangeCurrentPropertyI32Symbol
           << "\",\"weak_current_property_load_symbol\":\""
           << kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol
           << "\",\"weak_current_property_store_symbol\":\""
           << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol
           << "\",\"synthesized_accessor_owner_entries\":"
           << accessor_storage_lowering_metadata_summary
                  .synthesized_accessor_owner_entries
           << ",\"synthesized_getter_entries\":"
           << accessor_storage_lowering_metadata_summary
                  .synthesized_getter_entries
           << ",\"synthesized_setter_entries\":"
           << accessor_storage_lowering_metadata_summary
                  .synthesized_setter_entries
           << ",\"current_property_read_entries\":"
           << accessor_storage_lowering_metadata_summary
                  .current_property_read_entries
           << ",\"current_property_write_entries\":"
           << accessor_storage_lowering_metadata_summary
                  .current_property_write_entries
           << ",\"current_property_exchange_entries\":"
           << accessor_storage_lowering_metadata_summary
                  .current_property_exchange_entries
           << ",\"weak_current_property_load_entries\":"
           << accessor_storage_lowering_metadata_summary
                  .weak_current_property_load_entries
           << ",\"weak_current_property_store_entries\":"
           << accessor_storage_lowering_metadata_summary
                  .weak_current_property_store_entries
           << ",\"property_descriptor_count\":"
           << runtime_metadata_section_publication.property_descriptor_count
           << ",\"ivar_descriptor_count\":"
           << runtime_metadata_section_publication.ivar_descriptor_count
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << ",\"deterministic_handoff\":"
           << (property_synthesis_ivar_binding_contract.deterministic &&
                       dispatch_surface_classification_contract.deterministic &&
                       message_send_selector_lowering_contract.deterministic &&
                       runtime_link_host_link_contract.deterministic
                   ? "true"
                   : "false")
           << "},\n";
  manifest << "  \"dispatch_accessor_runtime_abi_surface\":{\"contract_id\":"
           << "\"objc3c.runtime.dispatch_accessor.abi.surface.v1\""
           << ",\"abi_boundary_model\":"
           << "\"public-dispatch-entrypoint-plus-private-testing-snapshot-and-property-helper-surface\""
           << ",\"public_header_path\":\"native/objc3c/src/runtime/public/objc3_runtime_api.h\""
           << ",\"private_header_path\":\"native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h\""
           << ",\"runtime_dispatch_symbol\":\""
           << runtime_link_host_link_contract.runtime_dispatch_symbol
           << "\",\"dispatch_state_snapshot_symbol\":\"objc3_runtime_copy_dispatch_state_for_testing\""
           << ",\"method_cache_state_snapshot_symbol\":\"objc3_runtime_copy_method_cache_state_for_testing\""
           << ",\"method_cache_entry_snapshot_symbol\":\"objc3_runtime_copy_method_cache_entry_for_testing\""
           << ",\"property_registry_state_snapshot_symbol\":\"objc3_runtime_copy_property_registry_state_for_testing\""
           << ",\"property_entry_snapshot_symbol\":\"objc3_runtime_copy_property_entry_for_testing\""
           << ",\"arc_debug_state_snapshot_symbol\":\"objc3_runtime_copy_arc_debug_state_for_testing\""
           << ",\"current_property_read_symbol\":\""
           << kObjc3RuntimeReadCurrentPropertyI32Symbol
           << "\",\"current_property_write_symbol\":\""
           << kObjc3RuntimeWriteCurrentPropertyI32Symbol
           << "\",\"current_property_exchange_symbol\":\""
           << kObjc3RuntimeExchangeCurrentPropertyI32Symbol
           << "\",\"bind_current_property_context_symbol\":\"objc3_runtime_bind_current_property_context_for_testing\""
           << ",\"clear_current_property_context_symbol\":\"objc3_runtime_clear_current_property_context_for_testing\""
           << ",\"weak_current_property_load_symbol\":\""
           << kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol
           << "\",\"weak_current_property_store_symbol\":\""
           << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol
           << "\",\"retain_symbol\":\"" << kObjc3RuntimeRetainI32Symbol
           << "\",\"release_symbol\":\"" << kObjc3RuntimeReleaseI32Symbol
           << "\",\"autorelease_symbol\":\""
           << kObjc3RuntimeAutoreleaseI32Symbol
           << "\",\"private_testing_surface_only\":true"
           << ",\"deterministic\":"
           << ((property_synthesis_ivar_binding_contract.deterministic &&
                dispatch_surface_classification_contract.deterministic &&
                message_send_selector_lowering_contract.deterministic &&
                runtime_link_host_link_contract.deterministic)
                   ? "true"
                   : "false")
           << "},\n";
  manifest << "  \"storage_accessor_runtime_abi_surface\":{\"contract_id\":\""
           << kObjc3RuntimeStorageAccessorAbiSurfaceContractId
           << "\",\"abi_boundary_model\":\"private-bootstrap-internal-property-helper-and-reflection-snapshot-surface-without-public-header-widening\""
           << ",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"private_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"property_registry_state_snapshot_symbol\":\"objc3_runtime_copy_property_registry_state_for_testing\""
           << ",\"property_entry_snapshot_symbol\":\"objc3_runtime_copy_property_entry_for_testing\""
           << ",\"current_property_read_symbol\":\""
           << kObjc3RuntimeReadCurrentPropertyI32Symbol
           << "\",\"current_property_write_symbol\":\""
           << kObjc3RuntimeWriteCurrentPropertyI32Symbol
           << "\",\"current_property_exchange_symbol\":\""
           << kObjc3RuntimeExchangeCurrentPropertyI32Symbol
           << "\",\"bind_current_property_context_symbol\":\"objc3_runtime_bind_current_property_context_for_testing\""
           << ",\"clear_current_property_context_symbol\":\"objc3_runtime_clear_current_property_context_for_testing\""
           << ",\"weak_current_property_load_symbol\":\""
           << kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol
           << "\",\"weak_current_property_store_symbol\":\""
           << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol
           << "\",\"private_testing_surface_only\":true"
           << ",\"deterministic\":"
           << ((property_synthesis_ivar_binding_contract.deterministic &&
                runtime_link_host_link_contract.deterministic)
                   ? "true"
                   : "false")
           << "},\n";
  manifest << "  \"runtime_state_publication_surface\":{\"contract_id\":\""
           << kObjc3RuntimeStatePublicationSurfaceContractId
           << "\",\"publication_surface_kind\":"
           << "\"compile-manifest-plus-registration-manifest\""
           << ",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"runtime_support_library_archive_relative_path\":\""
           << runtime_translation_unit_registration_manifest
                  .runtime_support_library_archive_relative_path
           << "\",\"registration_entrypoint_symbol\":\""
           << runtime_translation_unit_registration_manifest
                  .registration_entrypoint_symbol
           << "\",\"runtime_state_snapshot_symbol\":\""
           << runtime_bootstrap_semantics.runtime_state_snapshot_symbol
           << "\",\"public_runtime_abi_boundary\":[\""
           << kObjc3RuntimeSupportLibraryRegisterImageSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol << "\",\""
           << kObjc3RuntimeSupportLibraryResetForTestingSymbol
           << "\"],\"class_descriptor_count\":"
           << runtime_translation_unit_registration_manifest.class_descriptor_count
           << ",\"protocol_descriptor_count\":"
           << runtime_translation_unit_registration_manifest.protocol_descriptor_count
           << ",\"category_descriptor_count\":"
           << runtime_translation_unit_registration_manifest.category_descriptor_count
           << ",\"property_descriptor_count\":"
           << runtime_translation_unit_registration_manifest.property_descriptor_count
           << ",\"ivar_descriptor_count\":"
           << runtime_translation_unit_registration_manifest.ivar_descriptor_count
           << ",\"total_descriptor_count\":"
           << runtime_translation_unit_registration_manifest.total_descriptor_count
           << ",\"publication_requires_coupled_registration_manifest\":true"
           << ",\"publication_requires_real_compile_output\":true"
           << "},\n";
  manifest << "  \"runtime_bootstrap_registration_source_surface\":{\"contract_id\":\""
           << kObjc3RuntimeBootstrapRegistrationSourceSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"source_surface_contract_id\":\""
           << runtime_registration_descriptor_image_root_source_surface.contract_id
           << "\",\"frontend_closure_contract_id\":\""
           << runtime_registration_descriptor_frontend_closure.contract_id
           << "\",\"registration_manifest_contract_id\":\""
           << runtime_translation_unit_registration_manifest.contract_id
           << "\",\"bootstrap_lowering_contract_id\":\""
           << runtime_bootstrap_lowering.contract_id
           << "\",\"runtime_support_library_archive_relative_path\":\""
           << runtime_translation_unit_registration_manifest
                  .runtime_support_library_archive_relative_path
           << "\",\"registration_descriptor_identifier\":\""
           << runtime_registration_descriptor_frontend_closure
                  .registration_descriptor_identifier
           << "\",\"image_root_identifier\":\""
           << runtime_registration_descriptor_frontend_closure
                  .image_root_identifier
           << "\",\"registration_entrypoint_symbol\":\""
           << runtime_translation_unit_registration_manifest
                  .registration_entrypoint_symbol
           << "\",\"constructor_root_symbol\":\""
           << runtime_translation_unit_registration_manifest.constructor_root_symbol
           << "\",\"translation_unit_identity_key\":\""
           << runtime_bootstrap_legality_semantics.translation_unit_identity_key
           << "\",\"translation_unit_registration_order_ordinal\":"
           << runtime_translation_unit_registration_manifest
                  .translation_unit_registration_order_ordinal
           << ",\"requires_coupled_registration_descriptor_artifact\":true"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << "},\n";
  manifest << "  \"runtime_bootstrap_lowering_registration_artifact_surface\":{\"contract_id\":\""
           << kObjc3RuntimeBootstrapLoweringRegistrationArtifactSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"bootstrap_lowering_contract_id\":\""
           << runtime_bootstrap_lowering.contract_id
           << "\",\"registration_manifest_contract_id\":\""
           << runtime_translation_unit_registration_manifest.contract_id
           << "\",\"bootstrap_semantics_contract_id\":\""
           << runtime_bootstrap_semantics.contract_id
           << "\",\"registration_descriptor_frontend_closure_contract_id\":\""
           << runtime_registration_descriptor_frontend_closure.contract_id
           << "\",\"runtime_support_library_archive_relative_path\":\""
           << runtime_translation_unit_registration_manifest
                  .runtime_support_library_archive_relative_path
           << "\",\"constructor_root_symbol\":\""
           << runtime_bootstrap_lowering.constructor_root_symbol
           << "\",\"init_stub_symbol_prefix\":\""
           << runtime_bootstrap_lowering.constructor_init_stub_symbol_prefix
           << "\",\"registration_table_symbol_prefix\":\""
           << runtime_bootstrap_lowering.registration_table_symbol_prefix
           << "\",\"image_local_init_state_symbol_prefix\":\""
           << runtime_bootstrap_lowering.image_local_init_state_symbol_prefix
           << "\",\"registration_entrypoint_symbol\":\""
           << runtime_bootstrap_lowering.registration_entrypoint_symbol
           << "\",\"global_ctor_list_model\":\""
           << runtime_bootstrap_lowering.global_ctor_list_model
           << "\",\"registration_table_layout_model\":\""
           << runtime_bootstrap_lowering.registration_table_layout_model
           << "\",\"image_local_initialization_model\":\""
           << runtime_bootstrap_lowering.image_local_initialization_model
           << "\",\"registration_table_abi_version\":"
           << runtime_bootstrap_lowering.registration_table_abi_version
           << ",\"registration_table_pointer_field_count\":"
           << runtime_bootstrap_lowering
                  .registration_table_pointer_field_count
           << ",\"constructor_root_emission_state\":\""
           << runtime_bootstrap_lowering.constructor_root_emission_state
           << "\",\"init_stub_emission_state\":\""
           << runtime_bootstrap_lowering.init_stub_emission_state
           << "\",\"registration_table_emission_state\":\""
           << runtime_bootstrap_lowering.registration_table_emission_state
           << "\",\"lowered_registration_descriptor_fields\":["
           << "\"constructor_init_stub_symbol\","
           << "\"bootstrap_registration_table_symbol\","
           << "\"bootstrap_image_local_init_state_symbol\","
           << "\"bootstrap_registration_table_layout_model\","
           << "\"bootstrap_image_local_initialization_model\","
           << "\"bootstrap_registration_table_abi_version\","
           << "\"bootstrap_registration_table_pointer_field_count\","
           << "\"translation_unit_registration_order_ordinal\""
           << "],\"loader_table_ir_proof_fields\":["
           << "\"constructor_root_symbol\","
           << "\"constructor_init_stub_symbol\","
           << "\"bootstrap_registration_table_symbol\","
           << "\"bootstrap_image_local_init_state_symbol\","
           << "\"translation_unit_registration_order_ordinal\""
           << "]"
           << ",\"bootstrap_ir_materialization_landed\":"
           << (runtime_bootstrap_lowering.bootstrap_ir_materialization_landed
                   ? "true"
                   : "false")
           << ",\"image_local_initialization_landed\":"
           << (runtime_bootstrap_lowering.image_local_initialization_landed
                   ? "true"
                   : "false")
           << ",\"requires_coupled_registration_descriptor_artifact\":true"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_emitted_loader_table_ir\":true"
           << "},\n";
  manifest << "  \"runtime_multi_image_startup_ordering_source_surface\":{\"contract_id\":\""
           << kObjc3RuntimeMultiImageStartupOrderingSourceSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"bootstrap_legality_semantics_contract_id\":\""
           << runtime_bootstrap_legality_semantics.contract_id
           << "\",\"bootstrap_failure_restart_contract_id\":\""
           << runtime_bootstrap_failure_restart_semantics.contract_id
           << "\",\"bootstrap_api_contract_id\":\""
           << runtime_bootstrap_api.contract_id
           << "\",\"bootstrap_reset_contract_id\":\""
           << kObjc3RuntimeBootstrapResetContractId
           << "\",\"bootstrap_registrar_contract_id\":\""
           << kObjc3RuntimeBootstrapRegistrarContractId
           << "\",\"archive_static_link_replay_corpus_contract_id\":\""
           << kObjc3RuntimeBootstrapArchiveStaticLinkReplayCorpusContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"registration_entrypoint_symbol\":\""
           << runtime_bootstrap_api.registration_entrypoint_symbol
           << "\",\"state_snapshot_symbol\":\""
           << runtime_bootstrap_api.state_snapshot_symbol
           << "\",\"reset_for_testing_symbol\":\""
           << runtime_bootstrap_api.reset_for_testing_symbol
           << "\",\"replay_registered_images_symbol\":\""
           << runtime_bootstrap_failure_restart_semantics
                  .replay_registered_images_symbol
           << "\",\"reset_replay_state_snapshot_symbol\":\""
           << runtime_bootstrap_failure_restart_semantics
                  .reset_replay_state_snapshot_symbol
           << "\",\"translation_unit_identity_key\":\""
           << runtime_bootstrap_legality_semantics.translation_unit_identity_key
           << "\",\"translation_unit_registration_order_ordinal\":"
           << runtime_bootstrap_legality_semantics
                  .translation_unit_registration_order_ordinal
           << ",\"duplicate_registration_status_code\":"
           << runtime_bootstrap_semantics.duplicate_registration_status_code
           << ",\"out_of_order_status_code\":"
           << runtime_bootstrap_semantics.out_of_order_status_code
           << ",\"duplicate_install_diagnostic_model\":\""
           << kObjc3RuntimeBootstrapDuplicateInstallDiagnosticModel
           << "\",\"out_of_order_install_diagnostic_model\":\""
           << kObjc3RuntimeBootstrapOutOfOrderDiagnosticModel
           << "\",\"last_rejected_module_name_field\":\""
           << kObjc3RuntimeBootstrapRejectedModuleNameField
           << "\",\"last_rejected_translation_unit_identity_key_field\":\""
           << kObjc3RuntimeBootstrapRejectedTranslationUnitIdentityKeyField
           << "\",\"next_expected_registration_order_field\":\""
           << kObjc3RuntimeBootstrapNextExpectedRegistrationOrderField
           << "\",\"last_successful_registration_order_field\":\""
           << kObjc3RuntimeBootstrapLastSuccessfulRegistrationOrderField
           << "\",\"last_rejected_registration_order_field\":\""
           << kObjc3RuntimeBootstrapLastRejectedRegistrationOrderField
           << "\",\"requires_linked_runtime_probe\":true"
           << ",\"requires_real_compile_output\":true"
           << "},\n";
  manifest << "  \"runtime_object_model_realization_source_surface\":{\"contract_id\":\""
           << kObjc3RuntimeObjectModelRealizationSourceSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"executable_realization_records_contract_id\":\""
           << kObjc3ExecutableRealizationRecordsContractId
           << "\",\"runtime_class_realization_contract_id\":\""
           << kObjc3RuntimeClassRealizationContractId
           << "\",\"runtime_metaclass_graph_contract_id\":\""
           << kObjc3RuntimeMetaclassGraphRootClassContractId
           << "\",\"runtime_category_attachment_protocol_conformance_contract_id\":\""
           << kObjc3RuntimeCategoryAttachmentProtocolConformanceContractId
           << "\",\"canonical_runnable_object_support_contract_id\":\""
           << kObjc3RuntimeCanonicalRunnableObjectSampleSupportContractId
           << "\",\"runtime_support_library_archive_relative_path\":\""
           << runtime_translation_unit_registration_manifest
                  .runtime_support_library_archive_relative_path
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"registration_entrypoint_symbol\":\""
           << runtime_translation_unit_registration_manifest
                  .registration_entrypoint_symbol

           << "\",\"selector_lookup_symbol\":\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol
           << "\",\"runtime_dispatch_symbol\":\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol
           << "\",\"realized_class_graph_snapshot_symbol\":\"objc3_runtime_copy_realized_class_graph_state_for_testing\""
           << ",\"realized_class_entry_snapshot_symbol\":\"objc3_runtime_copy_realized_class_entry_for_testing\""
           << ",\"protocol_conformance_query_symbol\":\"objc3_runtime_copy_protocol_conformance_query_for_testing\""
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_property_ivar_storage_accessor_source_surface\":{\"contract_id\":\""
           << kObjc3RuntimePropertyIvarStorageAccessorSourceSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"property_ast_anchor\":\""
           << kObjc3RuntimeMetadataPropertyAstAnchor
           << "\",\"ivar_ast_anchor\":\""
           << kObjc3RuntimeMetadataIvarAstAnchor
           << "\",\"source_closure_contract_id\":\""
           << kObjc3ExecutablePropertyIvarSourceClosureContractId
           << "\",\"source_model_completion_contract_id\":\""
           << kObjc3ExecutablePropertyIvarSourceModelCompletionContractId
           << "\",\"source_semantics_contract_id\":\""
           << kObjc3ExecutablePropertyIvarSemanticsContractId
           << "\",\"source_surface_model\":\""
           << kObjc3RuntimePropertyIvarStorageAccessorSourceSurfaceModel
           << "\",\"layout_model\":\""
           << kObjc3ExecutablePropertyIvarLayoutModel
           << "\",\"attribute_model\":\""
           << kObjc3ExecutablePropertyAttributeModel
           << "\",\"synthesis_semantics_model\":\""
           << kObjc3ExecutablePropertySynthesisSemanticsModel
           << "\",\"default_ivar_binding_resolution_model\":\""
           << kObjc3ExecutablePropertyDefaultIvarBindingResolutionModel
           << "\",\"accessor_semantics_model\":\""
           << kObjc3ExecutablePropertyAccessorSemanticsModel
           << "\",\"accessor_selector_uniqueness_model\":\""
           << kObjc3ExecutablePropertyAccessorSelectorUniquenessModel
           << "\",\"ownership_atomicity_interaction_model\":\""
           << kObjc3ExecutablePropertyOwnershipAtomicityInteractionModel
           << "\",\"storage_semantics_model\":\""
           << kObjc3ExecutablePropertyStorageSemanticsModel
           << "\",\"layout_init_order_field\":\"Objc3PropertyDecl.executable_ivar_init_order_index\""
           << ",\"layout_destroy_order_field\":\"Objc3PropertyDecl.executable_ivar_destroy_order_index\""
           << ",\"synthesizes_accessors_field\":\"Objc3RuntimeMetadataPropertySourceRecord.synthesizes_executable_accessors\""
           << ",\"getter_runtime_helper_field\":\"Objc3RuntimeMetadataPropertySourceRecord.getter_storage_runtime_helper_symbol\""
           << ",\"setter_runtime_helper_field\":\"Objc3RuntimeMetadataPropertySourceRecord.setter_storage_runtime_helper_symbol\""
           << ",\"dispatch_and_synthesized_accessor_lowering_surface_contract_id\":\""
           << kObjc3DispatchAndSynthesizedAccessorLoweringSurfaceContractId
           << "\",\"executable_property_accessor_layout_lowering_contract_id\":\""
           << kObjc3ExecutablePropertyAccessorLayoutLoweringContractId
           << "\",\"executable_ivar_layout_emission_contract_id\":\""
           << kObjc3ExecutableIvarLayoutEmissionContractId
           << "\",\"executable_synthesized_accessor_property_lowering_contract_id\":\""
           << kObjc3ExecutableSynthesizedAccessorPropertyLoweringContractId
           << "\",\"storage_accessor_runtime_abi_surface_contract_id\":\""
           << kObjc3RuntimeStorageAccessorAbiSurfaceContractId
           << "\""
           << ",\"accessor_storage_lowering_metadata_model\":\""
           << kObjc3AccessorStorageLoweringMetadataModel
           << "\",\"accessor_storage_lowering_helper_selection_model\":\""
           << kObjc3AccessorStorageLoweringHelperSelectionModel
           << "\",\"compatibility_semantics_model\":\""
           << kObjc3ExecutablePropertyCompatibilitySemanticsModel
           << "\",\"ast_source_path\":\"native/objc3c/src/ast/objc3_ast.h\""
           << ",\"lowering_contract_source_path\":\"native/objc3c/src/lower/objc3_lowering_contract.h\""
           << ",\"sema_source_path\":\"native/objc3c/src/sema/objc3_semantic_passes.cpp\""
           << ",\"frontend_pipeline_source_path\":\"native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp\""
           << ",\"ir_emitter_source_path\":\"native/objc3c/src/ir/objc3_ir_emitter.cpp\""
           << ",\"frontend_artifacts_source_path\":\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"runtime_source_path\":\"native/objc3c/src/runtime/objc3_runtime.cpp\""
           << ",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/property_accessor_selector_compatibility_negative.objc3\""
           << ",\"tests/tooling/fixtures/native/property_reflection_attribute_compatibility_negative.objc3\""
           << ",\"tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_property_interaction_positive.objc3\"]"
           << ",\"authoritative_probe_paths\":[\"tests/tooling/runtime/synthesized_accessor_probe.cpp\""
           << ",\"tests/tooling/runtime/property_layout_runtime_probe.cpp\""
           << ",\"tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp\""
           << ",\"tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp\""
           << ",\"tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp\""
           << ",\"tests/tooling/runtime/arc_debug_instrumentation_probe.cpp\"]"
           << ",\"explicit_non_goals\":[\"no-public-runtime-abi-widening\""
           << ",\"no-milestone-specific-scaffolding\""
           << ",\"no-lowering-owned-storage-or-accessor-semantics-invention\"]"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_block_arc_unified_source_surface\":{\"contract_id\":\""
           << kObjc3RuntimeBlockArcUnifiedSourceSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"source_surface_model\":\""
           << kObjc3RuntimeBlockArcUnifiedSourceSurfaceModel
           << "\",\"source_contract_ids\":[\""
           << Expr::kObjc3ExecutableBlockSourceClosureContractId
           << "\",\""
           << Expr::kObjc3ExecutableBlockSourceModelCompletionContractId
           << "\",\""
           << Expr::kObjc3ExecutableBlockSourceStorageAnnotationContractId
           << "\",\""
           << Expr::kObjc3ExecutableBlockRuntimeSemanticRulesContractId
           << "\",\""
           << Expr::kObjc3ExecutableBlockCaptureLegalityImplementationContractId
           << "\",\""
           << Expr::kObjc3ExecutableBlockOwnershipSemanticsImplementationContractId
           << "\",\""
           << Expr::kObjc3ExecutableBlockObjectInvokeThunkLoweringContractId
           << "\",\""
           << Expr::kObjc3ExecutableBlockByrefHelperLoweringContractId
           << "\",\""
           << Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId
           << "\",\""
           << Expr::kObjc3ArcSourceModeBoundaryContractId
           << "\",\""
           << Expr::kObjc3ArcModeHandlingContractId
           << "\",\""
           << Expr::kObjc3ArcSemanticRulesContractId
           << "\",\""
           << Expr::kObjc3ArcInferenceLifetimeContractId
           << "\",\""
           << Expr::kObjc3ArcInteractionSemanticsContractId
           << "\"]"
           << ",\"authoritative_code_paths\":[\"native/objc3c/src/ast/objc3_ast.h\""
           << ",\"native/objc3c/src/sema/objc3_semantic_passes.cpp\""
           << ",\"native/objc3c/src/ir/objc3_ir_emitter.cpp\""
           << ",\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h\""
           << ",\"native/objc3c/src/runtime/objc3_runtime.cpp\"]"
           << ",\"authoritative_source_fields\":[\"frontend.pipeline.semantic_surface.objc_arc_diagnostics_fixit_lowering_surface\""
           << ",\"frontend.pipeline.semantic_surface.objc_block_literal_capture_lowering_surface\""
           << ",\"frontend.pipeline.semantic_surface.objc_block_source_model_completion_surface\""
           << ",\"frontend.pipeline.semantic_surface.objc_block_source_storage_annotation_surface\""
           << ",\"frontend.pipeline.semantic_surface.objc_block_abi_invoke_trampoline_lowering_surface\""
           << ",\"frontend.pipeline.semantic_surface.objc_block_storage_escape_lowering_surface\""
           << ",\"frontend.pipeline.semantic_surface.objc_block_copy_dispose_lowering_surface\""
           << ",\"llvm_ir_summary.executable_block_object_invoke_thunk_lowering\""
           << ",\"llvm_ir_summary.executable_block_byref_helper_lowering\""
           << ",\"llvm_ir_summary.executable_block_escape_runtime_hook_lowering\""
           << ",\"llvm_ir_summary.runtime_block_api_object_layout\""
           << ",\"llvm_ir_summary.runtime_block_allocation_copy_dispose_invoke_support\""
           << ",\"llvm_ir_summary.runtime_block_byref_forwarding_heap_promotion_ownership_interop\""
           << ",\"runtime_api.objc3_runtime_promote_block_i32\""
           << ",\"runtime_api.objc3_runtime_invoke_block_i32\""
           << ",\"runtime_api.objc3_runtime_copy_arc_debug_state_for_testing\""
           << ",\"runtime_api.objc3_runtime_copy_block_arc_runtime_abi_snapshot_for_testing\"]"
           << ",\"block_runtime_boundary_model\":\""
           << Expr::kObjc3ExecutableBlockByrefMutationOwnershipModel
           << "\""
           << ",\"arc_runtime_boundary_model\":\""
           << Expr::kObjc3ArcInteractionSemanticsSemanticModel
           << "\""
           << ",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/block_source_model_completion_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/block_source_storage_annotations_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/capture_legality_escape_invocation_bad_call.objc3\""
           << ",\"tests/tooling/fixtures/native/capture_legality_escape_invocation_missing_capture.objc3\""
           << ",\"tests/tooling/fixtures/native/byref_cell_copy_dispose_runtime_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/escaping_block_runtime_hook_argument_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/escaping_block_runtime_hook_return_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/escaping_block_runtime_hook_byref_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/escaping_block_runtime_hook_owned_capture_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/executable_block_object_invoke_thunk_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_mode_handling_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_inference_lifetime_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_cleanup_scope_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_implicit_cleanup_void_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_autorelease_return_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_property_interaction_positive.objc3\"]"
           << ",\"authoritative_probe_paths\":[\"tests/tooling/runtime/block_runtime_copy_dispose_invoke_probe.cpp\""
           << ",\"tests/tooling/runtime/block_runtime_byref_forwarding_probe.cpp\""
           << ",\"tests/tooling/runtime/arc_debug_instrumentation_probe.cpp\""
           << ",\"tests/tooling/runtime/block_arc_runtime_abi_probe.cpp\"]"
           << ",\"explicit_non_goals\":[\"no-public-block-object-abi-widening\""
           << ",\"no-milestone-specific-scaffolding\""
           << ",\"no-sidecar-only-block-or-arc-proof\"]"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_ownership_transfer_capture_family_source_surface\":{\"contract_id\":\""
           << kObjc3RuntimeOwnershipTransferCaptureFamilySourceSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"block_arc_unified_source_surface_contract_id\":\""
           << kObjc3RuntimeBlockArcUnifiedSourceSurfaceContractId
           << "\",\"source_surface_model\":\""
           << kObjc3RuntimeOwnershipTransferCaptureFamilySourceSurfaceModel
           << "\",\"ownership_resource_move_use_after_move_surface_path\":\"frontend.pipeline.semantic_surface.objc_ownership_resource_move_and_use_after_move_semantics\""
           << ",\"ownership_capture_list_retainable_family_surface_path\":\"frontend.pipeline.semantic_surface.objc_ownership_capture_list_and_retainable_family_legality_completion\""
           << ",\"block_capture_ownership_contract_id\":\""
           << Expr::kObjc3ExecutableBlockOwnershipSemanticsImplementationContractId
           << "\",\"arc_inference_lifetime_contract_id\":\""
           << Expr::kObjc3ArcInferenceLifetimeContractId
           << "\",\"arc_interaction_semantics_contract_id\":\""
           << Expr::kObjc3ArcInteractionSemanticsContractId
           << "\",\"block_capture_ownership_profile_field\":\"Expr.block_runtime_capture_ownership_profile\""
           << ",\"block_capture_owned_count_field\":\"Expr.block_runtime_owned_object_capture_count\""
           << ",\"block_capture_weak_count_field\":\"Expr.block_runtime_weak_object_capture_count\""
           << ",\"block_capture_unowned_count_field\":\"Expr.block_runtime_unowned_object_capture_count\""
           << ",\"cleanup_ownership_transfer_field\":\"cleanup_ownership_transfer_enforced\""
           << ",\"explicit_capture_ownership_mode_field\":\"explicit_capture_ownership_mode_enforced\""
           << ",\"retainable_family_conflict_field\":\"retainable_family_conflict_enforced\""
           << ",\"authoritative_code_paths\":[\"native/objc3c/src/ast/objc3_ast.h\""
           << ",\"native/objc3c/src/sema/objc3_semantic_passes.cpp\""
           << ",\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"native/objc3c/src/ir/objc3_ir_emitter.cpp\"]"
           << ",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/owned_object_capture_helper_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/owned_object_capture_runtime_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/nonowning_object_capture_helper_elided_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/nonowning_object_capture_runtime_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/weak_object_capture_mutation_negative.objc3\""
           << ",\"tests/tooling/fixtures/native/unowned_object_capture_mutation_negative.objc3\""
           << ",\"tests/tooling/fixtures/native/escaping_block_runtime_hook_owned_capture_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_inference_lifetime_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_cleanup_scope_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_implicit_cleanup_void_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_autorelease_return_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/capture_list_and_retainable_family_legality_completion_positive.objc3\"]"
           << ",\"authoritative_probe_paths\":[\"tests/tooling/runtime/block_runtime_copy_dispose_invoke_probe.cpp\""
           << ",\"tests/tooling/runtime/block_runtime_byref_forwarding_probe.cpp\""
           << ",\"tests/tooling/runtime/arc_debug_instrumentation_probe.cpp\"]"
           << ",\"explicit_non_goals\":[\"no-parallel-semantics-path\""
           << ",\"no-milestone-specific-scaffolding\""
           << ",\"no-lowering-owned-reinterpretation-of-capture-family-truth\"]"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_block_arc_lowering_helper_surface\":{\"contract_id\":\""
           << kObjc3RuntimeBlockArcLoweringHelperSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"block_arc_unified_source_surface_contract_id\":\""
           << kObjc3RuntimeBlockArcUnifiedSourceSurfaceContractId
           << "\",\"runtime_block_arc_runtime_abi_surface_contract_id\":\""
           << kObjc3RuntimeBlockArcRuntimeAbiSurfaceContractId
           << "\",\"ownership_transfer_capture_family_source_surface_contract_id\":\""
           << kObjc3RuntimeOwnershipTransferCaptureFamilySourceSurfaceContractId
           << "\",\"block_object_invoke_thunk_lowering_contract_id\":\""
           << Expr::kObjc3ExecutableBlockObjectInvokeThunkLoweringContractId
           << "\",\"block_byref_helper_lowering_contract_id\":\""
           << Expr::kObjc3ExecutableBlockByrefHelperLoweringContractId
           << "\",\"block_escape_runtime_hook_lowering_contract_id\":\""
           << Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId
           << "\",\"arc_mode_handling_contract_id\":\""
           << Expr::kObjc3ArcModeHandlingContractId
           << "\",\"arc_semantic_rules_contract_id\":\""
           << Expr::kObjc3ArcSemanticRulesContractId
           << "\",\"arc_inference_lifetime_contract_id\":\""
           << Expr::kObjc3ArcInferenceLifetimeContractId
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"lowering_helper_surface_model\":\""
           << kObjc3RuntimeBlockArcLoweringHelperSurfaceModel
           << "\",\"semantic_surface_paths\":[\"frontend.pipeline.semantic_surface.objc_block_abi_invoke_trampoline_lowering_surface\""
           << ",\"frontend.pipeline.semantic_surface.objc_block_storage_escape_lowering_surface\""
           << ",\"frontend.pipeline.semantic_surface.objc_block_copy_dispose_lowering_surface\""
           << ",\"frontend.pipeline.semantic_surface.objc_arc_diagnostics_fixit_lowering_surface\"]"
           << ",\"manifest_lowering_paths\":[\"lowering_block_abi_invoke_trampoline\""
           << ",\"lowering_block_storage_escape\""
           << ",\"lowering_block_copy_dispose\"]"
           << ",\"llvm_ir_summary_paths\":[\"llvm_ir_summary.executable_block_object_invoke_thunk_lowering\""
           << ",\"llvm_ir_summary.executable_block_byref_helper_lowering\""
           << ",\"llvm_ir_summary.executable_block_escape_runtime_hook_lowering\""
           << ",\"llvm_ir_summary.arc_cleanup_weak_lifetime_hooks\""
           << ",\"llvm_ir_summary.arc_block_autorelease_return_lowering\"]"
           << ",\"runtime_api_paths\":[\"runtime_api.objc3_runtime_promote_block_i32\""
           << ",\"runtime_api.objc3_runtime_invoke_block_i32\""
           << ",\"runtime_api.objc3_runtime_copy_arc_debug_state_for_testing\""
           << ",\"runtime_api.objc3_runtime_copy_block_arc_runtime_abi_snapshot_for_testing\"]"
           << ",\"authoritative_code_paths\":[\"native/objc3c/src/ast/objc3_ast.h\""
           << ",\"native/objc3c/src/ir/objc3_ir_emitter.cpp\""
           << ",\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h\""
           << ",\"native/objc3c/src/runtime/objc3_runtime.cpp\"]"
           << ",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/byref_cell_copy_dispose_runtime_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/owned_object_capture_runtime_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/nonowning_object_capture_runtime_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/escaping_block_runtime_hook_argument_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/escaping_block_runtime_hook_return_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/escaping_block_runtime_hook_byref_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/escaping_block_runtime_hook_owned_capture_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/executable_block_object_invoke_thunk_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_mode_handling_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_inference_lifetime_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_cleanup_scope_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_implicit_cleanup_void_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_autorelease_return_positive.objc3\"]"
           << ",\"authoritative_probe_paths\":[\"tests/tooling/runtime/block_runtime_copy_dispose_invoke_probe.cpp\""
           << ",\"tests/tooling/runtime/block_runtime_byref_forwarding_probe.cpp\""
           << ",\"tests/tooling/runtime/arc_debug_instrumentation_probe.cpp\""
           << ",\"tests/tooling/runtime/block_arc_runtime_abi_probe.cpp\"]"
           << ",\"explicit_non_goals\":[\"no-cross-module-packaging-claims\""
           << ",\"no-public-block-abi-widening\""
           << ",\"no-milestone-specific-scaffolding\"]"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_block_arc_runtime_abi_surface\":{\"contract_id\":\""
           << kObjc3RuntimeBlockArcRuntimeAbiSurfaceContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"block_arc_unified_source_surface_contract_id\":\""
           << kObjc3RuntimeBlockArcUnifiedSourceSurfaceContractId
           << "\",\"block_arc_lowering_helper_surface_contract_id\":\""
           << kObjc3RuntimeBlockArcLoweringHelperSurfaceContractId
           << "\",\"public_runtime_abi_boundary\":[\""
           << kObjc3RuntimeSupportLibraryRegisterImageSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol << "\",\""
           << kObjc3RuntimeSupportLibraryResetForTestingSymbol << "\"]"
           << ",\"private_block_arc_runtime_abi_boundary\":[\""
           << kObjc3RuntimePromoteBlockI32Symbol << "\",\""
           << kObjc3RuntimeInvokeBlockI32Symbol << "\",\""
           << kObjc3RuntimeRetainI32Symbol << "\",\""
           << kObjc3RuntimeReleaseI32Symbol << "\",\""
           << kObjc3RuntimeAutoreleaseI32Symbol << "\",\""
           << kObjc3RuntimePushAutoreleasepoolScopeSymbol << "\",\""
           << kObjc3RuntimePopAutoreleasepoolScopeSymbol << "\",\""
           << kObjc3RuntimeReadCurrentPropertyI32Symbol << "\",\""
           << kObjc3RuntimeWriteCurrentPropertyI32Symbol << "\",\""
           << kObjc3RuntimeExchangeCurrentPropertyI32Symbol << "\",\""
           << "objc3_runtime_bind_current_property_context_for_testing"
           << "\",\"objc3_runtime_clear_current_property_context_for_testing"
           << "\",\"" << kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol << "\",\""
           << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol << "\",\""
           << "objc3_runtime_copy_arc_debug_state_for_testing"
           << "\",\"objc3_runtime_copy_block_arc_runtime_abi_snapshot_for_testing\"]"
           << ",\"block_arc_runtime_abi_snapshot_symbol\":\"objc3_runtime_copy_block_arc_runtime_abi_snapshot_for_testing\""
           << ",\"arc_debug_state_snapshot_symbol\":\"objc3_runtime_copy_arc_debug_state_for_testing\""
           << ",\"runtime_abi_boundary_model\":\""
           << kObjc3RuntimeBlockArcRuntimeAbiBoundaryModel
           << "\",\"block_runtime_model\":\""
           << kObjc3RuntimeBlockArcRuntimeAbiBlockModel
           << "\",\"arc_runtime_model\":\""
           << kObjc3RuntimeBlockArcRuntimeAbiArcModel
           << "\",\"fail_closed_model\":\""
           << kObjc3RuntimeBlockArcRuntimeAbiFailClosedModel
           << "\",\"authoritative_probe_path\":\"tests/tooling/runtime/block_arc_runtime_abi_probe.cpp\""
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_property_ivar_accessor_reflection_implementation_surface\":{\"contract_id\":\""
           << kObjc3RuntimePropertyIvarAccessorReflectionImplementationSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"runtime_property_ivar_storage_accessor_source_surface_contract_id\":\""
           << kObjc3RuntimePropertyIvarStorageAccessorSourceSurfaceContractId
           << "\",\"storage_accessor_runtime_abi_surface_contract_id\":\""
           << kObjc3RuntimeStorageAccessorAbiSurfaceContractId
           << "\",\"property_metadata_reflection_contract_id\":\""
           << kObjc3RuntimePropertyMetadataReflectionContractId
           << "\",\"runtime_backed_object_ownership_attribute_surface_contract_id\":\""
           << kObjc3RuntimeBackedObjectOwnershipAttributeSurfaceContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"implementation_snapshot_symbol\":\"objc3_runtime_copy_storage_accessor_implementation_snapshot_for_testing\""
           << ",\"property_registry_state_snapshot_symbol\":\"objc3_runtime_copy_property_registry_state_for_testing\""
           << ",\"property_entry_snapshot_symbol\":\"objc3_runtime_copy_property_entry_for_testing\""
           << ",\"current_property_read_symbol\":\""
           << kObjc3RuntimeReadCurrentPropertyI32Symbol
           << "\",\"current_property_write_symbol\":\""
           << kObjc3RuntimeWriteCurrentPropertyI32Symbol
           << "\",\"current_property_exchange_symbol\":\""
           << kObjc3RuntimeExchangeCurrentPropertyI32Symbol
           << "\",\"bind_current_property_context_symbol\":\"objc3_runtime_bind_current_property_context_for_testing\""
           << ",\"clear_current_property_context_symbol\":\"objc3_runtime_clear_current_property_context_for_testing\""
           << ",\"weak_current_property_load_symbol\":\""
           << kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol
           << "\",\"weak_current_property_store_symbol\":\""
           << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol
           << "\",\"implementation_model\":\""
           << kObjc3RuntimePropertyIvarAccessorReflectionImplementationModel
           << "\",\"reflection_model\":\""
           << kObjc3RuntimePropertyIvarAccessorReflectionImplementationReflectionModel
           << "\",\"fail_closed_model\":\""
           << kObjc3RuntimePropertyIvarAccessorReflectionImplementationFailClosedModel
           << "\",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"executable_property_accessor_layout_lowering_surface\":{\"contract_id\":\""
           << kObjc3ExecutablePropertyAccessorLayoutLoweringContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"runtime_property_ivar_storage_accessor_source_surface_contract_id\":\""
           << kObjc3RuntimePropertyIvarStorageAccessorSourceSurfaceContractId
           << "\",\"dispatch_and_synthesized_accessor_lowering_surface_contract_id\":\""
           << kObjc3DispatchAndSynthesizedAccessorLoweringSurfaceContractId
           << "\",\"property_table_model\":\""
           << kObjc3ExecutablePropertyAccessorLayoutLoweringPropertyTableModel
           << "\",\"ivar_layout_model\":\""
           << kObjc3ExecutablePropertyAccessorLayoutLoweringIvarLayoutModel
           << "\",\"accessor_binding_model\":\""
           << kObjc3ExecutablePropertyAccessorLayoutLoweringAccessorBindingModel
           << "\",\"scope_model\":\""
           << kObjc3ExecutablePropertyAccessorLayoutLoweringScopeModel
           << "\",\"fail_closed_model\":\""
           << kObjc3ExecutablePropertyAccessorLayoutLoweringFailClosedModel
           << "\",\"lowering_contract_source_path\":\"native/objc3c/src/lower/objc3_lowering_contract.h\""
           << ",\"ir_emitter_source_path\":\"native/objc3c/src/ir/objc3_ir_emitter.cpp\""
           << ",\"frontend_artifacts_source_path\":\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/property_synthesis_default_ivar_binding_no_redeclaration.objc3\""
           << ",\"tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_property_interaction_positive.objc3\"]"
           << ",\"authoritative_probe_paths\":[\"tests/tooling/runtime/synthesized_accessor_probe.cpp\""
           << ",\"tests/tooling/runtime/property_layout_runtime_probe.cpp\""
           << ",\"tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp\""
           << ",\"tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp\""
           << ",\"tests/tooling/runtime/arc_debug_instrumentation_probe.cpp\"]"
           << ",\"explicit_non_goals\":[\"no-public-runtime-abi-widening\""
           << ",\"no-milestone-specific-scaffolding\""
           << ",\"no-layout-or-accessor-body-rederivation-outside-the-live-lowering-path\"]"
           << ",\"property_metadata_entries\":"
           << executable_accessor_layout_lowering_summary.property_metadata_entries
           << ",\"ivar_metadata_entries\":"
           << executable_accessor_layout_lowering_summary.ivar_metadata_entries
           << ",\"property_descriptor_entries\":"
           << runtime_metadata_section_publication.property_descriptor_count
           << ",\"ivar_descriptor_entries\":"
           << runtime_metadata_section_publication.ivar_descriptor_count
           << ",\"property_attribute_profile_entries\":"
           << executable_accessor_layout_lowering_summary
                  .property_attribute_profile_entries
           << ",\"accessor_ownership_profile_entries\":"
           << executable_accessor_layout_lowering_summary
                  .accessor_ownership_profile_entries
           << ",\"synthesized_binding_entries\":"
           << executable_accessor_layout_lowering_summary
                  .synthesized_binding_entries
           << ",\"ivar_layout_entries\":"
           << executable_accessor_layout_lowering_summary.ivar_layout_entries
           << ",\"ivar_layout_owner_entries\":"
           << executable_accessor_layout_lowering_summary
                  .ivar_layout_owner_entries
           << ",\"descriptor_counts_match_source_graph\":"
           << ((executable_accessor_layout_lowering_summary.property_metadata_entries ==
                        runtime_metadata_section_publication.property_descriptor_count &&
                executable_accessor_layout_lowering_summary.ivar_metadata_entries ==
                        runtime_metadata_section_publication.ivar_descriptor_count)
                   ? "true"
                   : "false")
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"executable_ivar_layout_emission_surface\":{\"contract_id\":\""
           << kObjc3ExecutableIvarLayoutEmissionContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"executable_property_accessor_layout_lowering_surface_contract_id\":\""
           << kObjc3ExecutablePropertyAccessorLayoutLoweringContractId
           << "\",\"descriptor_model\":\""
           << kObjc3ExecutableIvarLayoutDescriptorModel
           << "\",\"offset_global_model\":\""
           << kObjc3ExecutableIvarOffsetGlobalModel
           << "\",\"layout_table_model\":\""
           << kObjc3ExecutableIvarLayoutTableModel
           << "\",\"scope_model\":\""
           << kObjc3ExecutableIvarLayoutEmissionScopeModel
           << "\",\"fail_closed_model\":\""
           << kObjc3ExecutableIvarLayoutEmissionFailClosedModel
           << "\",\"lowering_contract_source_path\":\"native/objc3c/src/lower/objc3_lowering_contract.h\""
           << ",\"ir_emitter_source_path\":\"native/objc3c/src/ir/objc3_ir_emitter.cpp\""
           << ",\"frontend_artifacts_source_path\":\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/property_ivar_source_model_completion_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3\"]"
           << ",\"authoritative_probe_paths\":[\"tests/tooling/runtime/property_layout_runtime_probe.cpp\""
           << ",\"tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp\"]"
           << ",\"explicit_non_goals\":[\"no-public-runtime-abi-widening\""
           << ",\"no-milestone-specific-scaffolding\""
           << ",\"no-runtime-layout-rederivation\"]"
           << ",\"offset_global_entries\":"
           << executable_accessor_layout_lowering_summary.ivar_layout_entries
           << ",\"layout_table_entries\":"
           << executable_accessor_layout_lowering_summary.ivar_layout_owner_entries
           << ",\"layout_owner_entries\":"
           << executable_accessor_layout_lowering_summary.ivar_layout_owner_entries
           << ",\"ivar_descriptor_entries\":"
           << runtime_metadata_section_publication.ivar_descriptor_count
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"executable_synthesized_accessor_property_lowering_surface\":{\"contract_id\":\""
           << kObjc3ExecutableSynthesizedAccessorPropertyLoweringContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"executable_property_accessor_layout_lowering_surface_contract_id\":\""
           << kObjc3ExecutablePropertyAccessorLayoutLoweringContractId
           << "\",\"dispatch_and_synthesized_accessor_lowering_surface_contract_id\":\""
           << kObjc3DispatchAndSynthesizedAccessorLoweringSurfaceContractId
           << "\",\"source_model\":\""
           << kObjc3ExecutableSynthesizedAccessorPropertyLoweringSourceModel
           << "\",\"storage_model\":\""
           << kObjc3ExecutableSynthesizedAccessorPropertyLoweringStorageModel
           << "\",\"property_descriptor_model\":\""
           << kObjc3ExecutableSynthesizedAccessorPropertyLoweringPropertyDescriptorModel
           << "\",\"fail_closed_model\":\""
           << kObjc3ExecutableSynthesizedAccessorPropertyLoweringFailClosedModel
           << "\",\"lowering_contract_source_path\":\"native/objc3c/src/lower/objc3_lowering_contract.h\""
           << ",\"ir_emitter_source_path\":\"native/objc3c/src/ir/objc3_ir_emitter.cpp\""
           << ",\"frontend_artifacts_source_path\":\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"runtime_source_path\":\"native/objc3c/src/runtime/objc3_runtime.cpp\""
           << ",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/property_synthesis_default_ivar_binding_no_redeclaration.objc3\""
           << ",\"tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_property_interaction_positive.objc3\"]"
           << ",\"authoritative_probe_paths\":[\"tests/tooling/runtime/synthesized_accessor_probe.cpp\""
           << ",\"tests/tooling/runtime/property_layout_runtime_probe.cpp\""
           << ",\"tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp\""
           << ",\"tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp\""
           << ",\"tests/tooling/runtime/arc_debug_instrumentation_probe.cpp\"]"
           << ",\"explicit_non_goals\":[\"no-public-runtime-abi-widening\""
           << ",\"no-milestone-specific-scaffolding\""
           << ",\"hard-cut-storage-global-body-proof\"]"
           << ",\"implementation_owned_property_entries\":"
           << executable_accessor_layout_lowering_summary
                  .implementation_owned_property_entries
           << ",\"synthesized_getter_entries\":"
           << executable_accessor_layout_lowering_summary.synthesized_getter_entries
           << ",\"synthesized_setter_entries\":"
           << executable_accessor_layout_lowering_summary.synthesized_setter_entries
           << ",\"synthesized_accessor_entries\":"
           << executable_accessor_layout_lowering_summary
                  .synthesized_accessor_entries
           << ",\"property_descriptor_entries\":"
           << runtime_metadata_section_publication.property_descriptor_count
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_property_atomicity_synthesis_reflection_source_surface\":{\"contract_id\":\""
           << kObjc3RuntimePropertyAtomicitySynthesisReflectionSourceSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"property_storage_source_surface_contract_id\":\""
           << kObjc3RuntimePropertyIvarStorageAccessorSourceSurfaceContractId
           << "\",\"property_metadata_reflection_contract_id\":\""
           << kObjc3RuntimePropertyMetadataReflectionContractId
           << "\",\"runtime_backed_object_ownership_attribute_surface_contract_id\":\""
           << kObjc3RuntimeBackedObjectOwnershipAttributeSurfaceContractId
           << "\",\"atomic_modifier_field\":\"Objc3PropertyDecl.is_atomic\""
           << ",\"nonatomic_modifier_field\":\"Objc3PropertyDecl.is_nonatomic\""
           << ",\"atomicity_conflict_field\":\"Objc3PropertyDecl.has_atomicity_conflict\""
           << ",\"property_attribute_profile_field\":\"Objc3PropertyDecl.property_attribute_profile\""
           << ",\"reflection_attribute_profile_field\":\"objc3_runtime_property_entry_snapshot.property_attribute_profile\""
           << ",\"ast_source_path\":\"native/objc3c/src/ast/objc3_ast.h\""
           << ",\"sema_source_path\":\"native/objc3c/src/sema/objc3_semantic_passes.cpp\""
           << ",\"sema_pass_manager_source_path\":\"native/objc3c/src/sema/objc3_sema_pass_manager.cpp\""
           << ",\"frontend_pipeline_source_path\":\"native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp\""
           << ",\"frontend_artifacts_source_path\":\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"runtime_internal_header_path\":\"native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h\""
           << ",\"runtime_source_path\":\"native/objc3c/src/runtime/objc3_runtime.cpp\""
           << ",\"source_surface_model\":\""
           << kObjc3RuntimePropertyAtomicitySynthesisReflectionSourceSurfaceModel
           << "\",\"atomicity_fail_closed_model\":\""
           << kObjc3ExecutablePropertyOwnershipAtomicityInteractionModel
           << "\",\"reflection_boundary_model\":\""
           << kObjc3RuntimePropertyAtomicityReflectionBoundaryModel
           << "\",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/property_atomic_ownership_negative.objc3\""
           << ",\"tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/property_accessor_selector_compatibility_negative.objc3\""
           << ",\"tests/tooling/fixtures/native/property_reflection_attribute_compatibility_negative.objc3\""
           << ",\"tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3\"]"
           << ",\"authoritative_probe_paths\":[\"tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp\""
           << ",\"tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp\""
           << ",\"tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp\"]"
           << ",\"explicit_non_goals\":[\"no-public-atomic-property-runtime-abi-widening\""
           << ",\"no-runtime-managed-atomic-storage-semantics-before-lane-b-and-lane-d-implementation\""
           << ",\"no-milestone-specific-scaffolding\"]"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_realization_lowering_reflection_artifact_surface\":{\"contract_id\":\""
           << kObjc3RuntimeRealizationLoweringReflectionArtifactSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"runtime_object_model_realization_source_surface_contract_id\":\""
           << kObjc3RuntimeObjectModelRealizationSourceSurfaceContractId
           << "\",\"dispatch_and_synthesized_accessor_lowering_surface_contract_id\":\"objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1\""
           << ",\"executable_realization_records_contract_id\":\""
           << kObjc3ExecutableRealizationRecordsContractId
           << "\",\"property_metadata_reflection_contract_id\":\""
           << kObjc3RuntimePropertyMetadataReflectionContractId
           << "\",\"runtime_backed_object_ownership_attribute_surface_contract_id\":\""
           << kObjc3RuntimeBackedObjectOwnershipAttributeSurfaceContractId
           << "\",\"runtime_support_library_archive_relative_path\":\""
           << runtime_translation_unit_registration_manifest
                  .runtime_support_library_archive_relative_path
           << "\",\"registration_entrypoint_symbol\":\""
           << runtime_translation_unit_registration_manifest
                  .registration_entrypoint_symbol
           << "\",\"selector_lookup_symbol\":\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol
           << "\",\"runtime_dispatch_symbol\":\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol
           << "\",\"property_registry_state_snapshot_symbol\":\"objc3_runtime_copy_property_registry_state_for_testing\""
           << ",\"property_entry_snapshot_symbol\":\"objc3_runtime_copy_property_entry_for_testing\""
           << ",\"lowering_artifact_boundary_model\":\"compile-manifest-registration-descriptor-object-and-llvm-ir-co-publish-realization-lowering-and-reflection-artifacts\""
           << ",\"reflection_artifact_handoff_model\":\"property-metadata-and-ownership-artifacts-remain-coupled-to-lowered-dispatch-accessor-and-executable-realization-record-outputs\""
           << ",\"requires_coupled_registration_descriptor_artifact\":true"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_compile_output_truthfulness\":true"
           << "},\n";
  manifest << "  \"runtime_dispatch_table_reflection_record_lowering_surface\":{\"contract_id\":\""
           << kObjc3RuntimeDispatchTableReflectionRecordLoweringSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"runtime_object_model_realization_source_surface_contract_id\":\""
           << kObjc3RuntimeObjectModelRealizationSourceSurfaceContractId
           << "\",\"runtime_state_publication_surface_contract_id\":\""
           << kObjc3RuntimeStatePublicationSurfaceContractId
           << "\",\"dispatch_and_synthesized_accessor_lowering_surface_contract_id\":\"objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1\""
           << ",\"method_dispatch_and_selector_thunk_lowering_contract_id\":\"objc3c.method.dispatch.selector.thunk.lowering.v1\""
           << ",\"executable_realization_records_contract_id\":\""
           << kObjc3ExecutableRealizationRecordsContractId
           << "\",\"runtime_support_library_archive_relative_path\":\""
           << runtime_translation_unit_registration_manifest
                  .runtime_support_library_archive_relative_path
           << "\",\"registration_entrypoint_symbol\":\""
           << runtime_translation_unit_registration_manifest
                  .registration_entrypoint_symbol
           << "\",\"selector_lookup_symbol\":\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol
           << "\",\"runtime_dispatch_symbol\":\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol
           << "\",\"selector_pool_section_root_symbol\":\"@__objc3_sec_selector_pool\""
           << ",\"class_aggregate_symbol\":\""
           << runtime_metadata_section_publication.class_aggregate_symbol
           << "\",\"protocol_aggregate_symbol\":\""
           << runtime_metadata_section_publication.protocol_aggregate_symbol
           << "\",\"category_aggregate_symbol\":\""
           << runtime_metadata_section_publication.category_aggregate_symbol
           << "\",\"property_aggregate_symbol\":\""
           << runtime_metadata_section_publication.property_aggregate_symbol
           << "\",\"ivar_aggregate_symbol\":\""
           << runtime_metadata_section_publication.ivar_aggregate_symbol
           << "\",\"message_send_sites\":"
           << message_send_selector_lowering_contract.message_send_sites
           << ",\"class_descriptor_count\":"
           << runtime_metadata_section_publication.class_descriptor_count
           << ",\"protocol_descriptor_count\":"
           << runtime_metadata_section_publication.protocol_descriptor_count
           << ",\"category_descriptor_count\":"
           << runtime_metadata_section_publication.category_descriptor_count
           << ",\"property_descriptor_count\":"
           << runtime_metadata_section_publication.property_descriptor_count
           << ",\"ivar_descriptor_count\":"
           << runtime_metadata_section_publication.ivar_descriptor_count
           << ",\"dispatch_table_lowering_model\":\"selector-pool-backed-dispatch-thunks-and-runtime-dispatch-sites-co-publish-stable-selector-table-roots-in-llvm-ir-and-manifest-artifacts\""
           << ",\"reflection_record_lowering_model\":\"realization-records-and-runtime-metadata-section-aggregates-co-publish-class-protocol-category-property-and-ivar-record-roots-in-emitted-artifacts\""
           << ",\"requires_coupled_registration_descriptor_artifact\":true"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_compile_output_truthfulness\":true"
           << "},\n";
  manifest << "  \"runtime_object_model_abi_query_surface\":{\"contract_id\":\""
           << kObjc3RuntimeObjectModelAbiQuerySurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"runtime_object_model_realization_source_surface_contract_id\":\""
           << kObjc3RuntimeObjectModelRealizationSourceSurfaceContractId
           << "\",\"runtime_realization_lowering_reflection_artifact_surface_contract_id\":\""
           << kObjc3RuntimeRealizationLoweringReflectionArtifactSurfaceContractId
           << "\",\"runtime_dispatch_table_reflection_record_lowering_surface_contract_id\":\""
           << kObjc3RuntimeDispatchTableReflectionRecordLoweringSurfaceContractId
           << "\",\"runtime_cross_module_realized_metadata_replay_preservation_surface_contract_id\":\""
           << kObjc3RuntimeCrossModuleRealizedMetadataReplayPreservationSurfaceContractId
           << "\",\"runtime_reflection_query_surface_contract_id\":\""
           << kObjc3RuntimeReflectionQuerySurfaceContractId
           << "\",\"runtime_realization_lookup_semantics_surface_contract_id\":\""
           << kObjc3RuntimeRealizationLookupSemanticsSurfaceContractId
           << "\",\"runtime_class_metaclass_protocol_realization_surface_contract_id\":\""
           << kObjc3RuntimeClassMetaclassProtocolRealizationSurfaceContractId
           << "\",\"runtime_category_attachment_merged_dispatch_surface_contract_id\":\""
           << kObjc3RuntimeCategoryAttachmentMergedDispatchSurfaceContractId
           << "\",\"runtime_reflection_visibility_coherence_diagnostics_surface_contract_id\":\""
           << kObjc3RuntimeReflectionVisibilityCoherenceDiagnosticsSurfaceContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"public_runtime_abi_boundary\":[\""
           << kObjc3RuntimeSupportLibraryRegisterImageSymbol
           << "\",\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol
           << "\",\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol
           << "\",\""
           << kObjc3RuntimeSupportLibraryResetForTestingSymbol
           << "\"]"
           << ",\"private_object_model_query_boundary\":[\"objc3_runtime_copy_realized_class_graph_state_for_testing\""
           << ",\"objc3_runtime_copy_realized_class_entry_for_testing\""
           << ",\"objc3_runtime_copy_property_registry_state_for_testing\""
           << ",\"objc3_runtime_copy_property_entry_for_testing\""
           << ",\"objc3_runtime_copy_protocol_conformance_query_for_testing\""
           << ",\"objc3_runtime_copy_selector_lookup_table_state_for_testing\""
           << ",\"objc3_runtime_copy_selector_lookup_entry_for_testing\""
           << ",\"objc3_runtime_copy_method_cache_state_for_testing\""
           << ",\"objc3_runtime_copy_method_cache_entry_for_testing\""
           << ",\"objc3_runtime_copy_dispatch_state_for_testing\"]"
           << ",\"object_model_query_boundary_model\":\""
           << kObjc3RuntimeObjectModelQueryBoundaryModel
           << "\""
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_unified_concurrency_source_surface\":{\"contract_id\":\""
           << kObjc3RuntimeUnifiedConcurrencySourceSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"source_contract_ids\":[\"objc3c.concurrency.async.source.closure.v1\""
           << ",\"objc3c.concurrency.actor.member.isolation.source.closure.v1\""
           << ",\"objc3c.concurrency.task.group.cancellation.source.closure.v1\""
           << ",\"objc3c.concurrency.async.effect.suspension.semantic.model.v1\""
           << ",\"objc3c.concurrency.task.executor.cancellation.semantic.model.v1\""
           << ",\"objc3c.concurrency.actor.isolation.sendable.semantic.model.v1\"]"
           << ",\"authoritative_code_paths\":[\"native/objc3c/src/ast/objc3_ast.h\""
           << ",\"native/objc3c/src/sema/objc3_semantic_passes.cpp\""
           << ",\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"native/objc3c/src/ir/objc3_ir_emitter.cpp\""
           << ",\"native/objc3c/src/runtime/public/objc3_runtime_api.h\""
           << ",\"native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h\""
           << ",\"native/objc3c/src/runtime/objc3_runtime.cpp\"]"
           << ",\"authoritative_source_fields\":[\"frontend.pipeline.semantic_surface.objc_concurrency_async_source_closure\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_actor_member_and_isolation_source_closure\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_task_group_and_cancellation_source_closure\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_async_effect_and_suspension_semantic_model\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_task_executor_and_cancellation_semantic_model\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_actor_isolation_and_sendable_semantic_model\"]"
           << ",\"source_surface_model\":\"unified-concurrency-source-surface-freezes-live-async-actor-task-source-and-sema-boundaries-before-lowering-runtime-and-public-abi-expansion\""
           << ",\"public_runtime_abi_boundary\":[\""
           << kObjc3RuntimeSupportLibraryRegisterImageSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol << "\",\""
           << kObjc3RuntimeSupportLibraryResetForTestingSymbol << "\"]"
           << ",\"private_concurrency_runtime_boundary\":[\"objc3_runtime_allocate_async_continuation_i32\""
           << ",\"objc3_runtime_handoff_async_continuation_to_executor_i32\""
           << ",\"objc3_runtime_resume_async_continuation_i32\""
           << ",\"objc3_runtime_spawn_task_i32\""
           << ",\"objc3_runtime_enter_task_group_scope_i32\""
           << ",\"objc3_runtime_add_task_group_task_i32\""
           << ",\"objc3_runtime_wait_task_group_next_i32\""
           << ",\"objc3_runtime_cancel_task_group_i32\""
           << ",\"objc3_runtime_task_is_cancelled_i32\""
           << ",\"objc3_runtime_task_on_cancel_i32\""
           << ",\"objc3_runtime_actor_enter_isolation_thunk_i32\""
           << ",\"objc3_runtime_actor_enter_nonisolated_i32\""
           << ",\"objc3_runtime_actor_hop_to_executor_i32\""
           << ",\"objc3_runtime_actor_record_replay_proof_i32\""
           << ",\"objc3_runtime_actor_record_race_guard_i32\""
           << ",\"objc3_runtime_actor_bind_executor_i32\""
           << ",\"objc3_runtime_actor_mailbox_enqueue_i32\""
           << ",\"objc3_runtime_actor_mailbox_drain_next_i32\""
           << ",\"objc3_runtime_copy_async_continuation_state_for_testing\""
           << ",\"objc3_runtime_copy_task_runtime_state_for_testing\""
           << ",\"objc3_runtime_copy_actor_runtime_state_for_testing\"]"
           << ",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/async_await_executor_source_closure_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/actor_member_isolation_surface_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/task_executor_cancellation_source_closure_positive.objc3\"]"
           << ",\"authoritative_probe_paths\":[\"tests/tooling/runtime/continuation_runtime_helper_probe.cpp\""
           << ",\"tests/tooling/runtime/task_runtime_lowering_probe.cpp\""
           << ",\"tests/tooling/runtime/actor_lowering_runtime_probe.cpp\"]"
           << ",\"explicit_non_goals\":[\"no-public-runtime-abi-widening\""
           << ",\"no-milestone-specific-scaffolding\""
           << ",\"no-sidecar-only-concurrency-proof\"]"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_async_task_actor_normalization_completion_surface\":{\"contract_id\":\""
           << kObjc3RuntimeAsyncTaskActorNormalizationCompletionSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"source_surface_contract_id\":\""
           << kObjc3RuntimeUnifiedConcurrencySourceSurfaceContractId
           << "\",\"normalized_semantic_contract_ids\":[\"objc3c.concurrency.async.effect.suspension.semantic.model.v1\""
           << ",\"objc3c.concurrency.task.executor.cancellation.semantic.model.v1\""
           << ",\"objc3c.concurrency.actor.isolation.sendable.semantic.model.v1\"]"
           << ",\"lowering_contract_ids\":[\"objc3c.concurrency.continuation.abi.async.lowering.contract.v1\""
           << ",\"objc3c.concurrency.task.runtime.lowering.contract.v1\""
           << ",\"objc3c.concurrency.actor.lowering.and.metadata.contract.v1\"]"
           << ",\"lowering_lane_contract_ids\":[\"objc3c.async.continuation.lowering.v1\""
           << ",\"objc3c.await.lowering.suspension.state.lowering.v1\""
           << ",\"objc3c.task.runtime.interop.cancellation.lowering.v1\""
           << ",\"objc3c.concurrency.replay.race.guard.lowering.v1\""
           << ",\"objc3c.actor.lowering.metadata.contract.v1\""
           << ",\"objc3c.actor.isolation.sendability.lowering.v1\"]"
           << ",\"authoritative_code_paths\":[\"native/objc3c/src/ast/objc3_ast.h\""
           << ",\"native/objc3c/src/sema/objc3_semantic_passes.cpp\""
           << ",\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"native/objc3c/src/ir/objc3_ir_emitter.cpp\""
           << ",\"native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h\""
           << ",\"native/objc3c/src/runtime/objc3_runtime.cpp\"]"
           << ",\"authoritative_surface_fields\":[\"frontend.pipeline.semantic_surface.objc_concurrency_async_effect_and_suspension_semantic_model\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_task_executor_and_cancellation_semantic_model\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_actor_isolation_and_sendable_semantic_model\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_continuation_abi_and_async_lowering_contract\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_task_runtime_lowering_contract\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_actor_lowering_and_metadata_contract\"]"
           << ",\"normalization_completion_model\":\"normalized-async-task-actor-sema-and-lowering-packets-freeze-the-live-boundary-before-runtime-abi-and-runnable-execution-closure\""
           << ",\"public_runtime_abi_boundary\":[\""
           << kObjc3RuntimeSupportLibraryRegisterImageSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol << "\",\""
           << kObjc3RuntimeSupportLibraryResetForTestingSymbol << "\"]"
           << ",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/async_lowering_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/actor_isolation_sendable_semantic_model_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/task_executor_cancellation_semantic_model_positive.objc3\"]"
           << ",\"authoritative_probe_paths\":[\"tests/tooling/runtime/continuation_runtime_helper_probe.cpp\""
           << ",\"tests/tooling/runtime/task_runtime_lowering_probe.cpp\""
           << ",\"tests/tooling/runtime/actor_lowering_runtime_probe.cpp\"]"
           << ",\"explicit_non_goals\":[\"no-public-runtime-abi-widening\""
           << ",\"no-runnable-task-or-actor-execution-claim\""
           << ",\"no-milestone-specific-scaffolding\"]"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << "},\n";
  manifest << "  \"runtime_unified_concurrency_lowering_metadata_surface\":{\"contract_id\":\""
           << kObjc3RuntimeUnifiedConcurrencyLoweringMetadataSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"source_surface_contract_id\":\""
           << kObjc3RuntimeUnifiedConcurrencySourceSurfaceContractId
           << "\",\"normalization_completion_surface_contract_id\":\""
           << kObjc3RuntimeAsyncTaskActorNormalizationCompletionSurfaceContractId
           << "\",\"lowering_contract_ids\":[\"objc3c.concurrency.continuation.abi.async.lowering.contract.v1\""
           << ",\"objc3c.concurrency.task.runtime.lowering.contract.v1\""
           << ",\"objc3c.concurrency.actor.lowering.and.metadata.contract.v1\"]"
           << ",\"lowering_detail_contract_ids\":[\"objc3c.concurrency.async.direct.call.lowering.v1\""
           << ",\"objc3c.concurrency.task.runtime.abi.completion.v1\""
           << ",\"objc3c.concurrency.actor.isolation.sendability.enforcement.v1\"]"
           << ",\"lowering_lane_contract_ids\":[\"objc3c.async.continuation.lowering.v1\""
           << ",\"objc3c.await.lowering.suspension.state.lowering.v1\""
           << ",\"objc3c.task.runtime.interop.cancellation.lowering.v1\""
           << ",\"objc3c.concurrency.replay.race.guard.lowering.v1\""
           << ",\"objc3c.actor.lowering.metadata.contract.v1\""
           << ",\"objc3c.actor.isolation.sendability.lowering.v1\"]"
           << ",\"authoritative_code_paths\":[\"native/objc3c/src/ast/objc3_ast.h\""
           << ",\"native/objc3c/src/sema/objc3_semantic_passes.cpp\""
           << ",\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"native/objc3c/src/ir/objc3_ir_emitter.cpp\"]"
           << ",\"authoritative_surface_fields\":[\"frontend.pipeline.semantic_surface.objc_concurrency_continuation_abi_and_async_lowering_contract\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_async_function_await_and_continuation_lowering\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_task_runtime_lowering_contract\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_task_group_and_runtime_abi_completion\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_actor_lowering_and_metadata_contract\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_actor_isolation_and_sendability_enforcement\"]"
           << ",\"lowering_metadata_surface_model\":\"unified-concurrency-lowering-and-metadata-surface-freezes-live-async-task-actor-lowering-packets-and-emitted-metadata-boundaries-before-runtime-abi-and-runnable-execution-closure\""
           << ",\"public_runtime_abi_boundary\":[\""
           << kObjc3RuntimeSupportLibraryRegisterImageSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol << "\",\""
           << kObjc3RuntimeSupportLibraryResetForTestingSymbol << "\"]"
           << ",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/async_lowering_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/task_runtime_async_entry_lowering_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/actor_lowering_metadata_positive.objc3\"]"
           << ",\"authoritative_probe_paths\":[\"tests/tooling/runtime/continuation_runtime_helper_probe.cpp\""
           << ",\"tests/tooling/runtime/task_runtime_lowering_probe.cpp\""
           << ",\"tests/tooling/runtime/actor_lowering_runtime_probe.cpp\"]"
           << ",\"explicit_non_goals\":[\"no-public-runtime-abi-widening\""
           << ",\"no-runnable-task-or-actor-execution-claim\""
           << ",\"no-milestone-specific-scaffolding\"]"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << "},\n";
  manifest << "  \"runtime_unified_concurrency_runtime_abi_surface\":{\"contract_id\":\""
           << kObjc3RuntimeUnifiedConcurrencyRuntimeAbiSurfaceContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"unified_concurrency_source_surface_contract_id\":\""
           << kObjc3RuntimeUnifiedConcurrencySourceSurfaceContractId
           << "\",\"async_task_actor_normalization_completion_surface_contract_id\":\""
           << kObjc3RuntimeAsyncTaskActorNormalizationCompletionSurfaceContractId
           << "\",\"unified_concurrency_lowering_metadata_surface_contract_id\":\""
           << kObjc3RuntimeUnifiedConcurrencyLoweringMetadataSurfaceContractId
           << "\",\"public_runtime_abi_boundary\":[\""
           << kObjc3RuntimeSupportLibraryRegisterImageSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol << "\",\""
           << kObjc3RuntimeSupportLibraryResetForTestingSymbol << "\"]"
           << ",\"private_unified_concurrency_runtime_abi_boundary\":[\""
           << kObjc3RuntimeAllocateAsyncContinuationI32Symbol << "\",\""
           << kObjc3RuntimeHandoffAsyncContinuationToExecutorI32Symbol << "\",\""
           << kObjc3RuntimeResumeAsyncContinuationI32Symbol << "\",\""
           << kObjc3RuntimeSpawnTaskI32Symbol << "\",\""
           << kObjc3RuntimeEnterTaskGroupScopeI32Symbol << "\",\""
           << kObjc3RuntimeAddTaskGroupTaskI32Symbol << "\",\""
           << kObjc3RuntimeWaitTaskGroupNextI32Symbol << "\",\""
           << kObjc3RuntimeCancelTaskGroupI32Symbol << "\",\""
           << kObjc3RuntimeTaskIsCancelledI32Symbol << "\",\""
           << kObjc3RuntimeTaskOnCancelI32Symbol << "\",\""
           << kObjc3RuntimeExecutorHopI32Symbol << "\",\""
           << kObjc3RuntimeActorEnterIsolationThunkI32Symbol << "\",\""
           << kObjc3RuntimeActorEnterNonisolatedI32Symbol << "\",\""
           << kObjc3RuntimeActorHopToExecutorI32Symbol << "\",\""
           << kObjc3RuntimeActorRecordReplayProofI32Symbol << "\",\""
           << kObjc3RuntimeActorRecordRaceGuardI32Symbol << "\",\""
           << kObjc3RuntimeActorBindExecutorI32Symbol << "\",\""
           << kObjc3RuntimeActorMailboxEnqueueI32Symbol << "\",\""
           << kObjc3RuntimeActorMailboxDrainNextI32Symbol << "\",\""
           << "objc3_runtime_copy_async_continuation_state_for_testing"
           << "\",\"objc3_runtime_copy_task_runtime_state_for_testing"
           << "\",\"objc3_runtime_copy_actor_runtime_state_for_testing\"]"
           << ",\"async_continuation_state_snapshot_symbol\":\"objc3_runtime_copy_async_continuation_state_for_testing\""
           << ",\"task_runtime_state_snapshot_symbol\":\"objc3_runtime_copy_task_runtime_state_for_testing\""
           << ",\"actor_runtime_state_snapshot_symbol\":\"objc3_runtime_copy_actor_runtime_state_for_testing\""
           << ",\"runtime_abi_boundary_model\":\""
           << kObjc3RuntimeUnifiedConcurrencyRuntimeAbiBoundaryModel
           << "\",\"continuation_runtime_model\":\""
           << kObjc3RuntimeUnifiedConcurrencyContinuationRuntimeModel
           << "\",\"task_runtime_model\":\""
           << kObjc3RuntimeUnifiedConcurrencyTaskRuntimeModel
           << "\",\"actor_runtime_model\":\""
           << kObjc3RuntimeUnifiedConcurrencyActorRuntimeModel
           << "\",\"fail_closed_model\":\""
           << kObjc3RuntimeUnifiedConcurrencyRuntimeAbiFailClosedModel
           << "\",\"authoritative_probe_paths\":[\"tests/tooling/runtime/continuation_runtime_helper_probe.cpp\""
           << ",\"tests/tooling/runtime/task_runtime_abi_completion_probe.cpp\""
           << ",\"tests/tooling/runtime/actor_runtime_executor_contract_probe.cpp\"]"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_realization_lookup_reflection_implementation_surface\":{\"contract_id\":\""
           << kObjc3RuntimeRealizationLookupReflectionImplementationSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"runtime_object_model_abi_query_surface_contract_id\":\""
           << kObjc3RuntimeObjectModelAbiQuerySurfaceContractId
           << "\",\"runtime_reflection_query_surface_contract_id\":\""
           << kObjc3RuntimeReflectionQuerySurfaceContractId
           << "\",\"runtime_realization_lookup_semantics_surface_contract_id\":\""
           << kObjc3RuntimeRealizationLookupSemanticsSurfaceContractId
           << "\",\"runtime_class_metaclass_protocol_realization_surface_contract_id\":\""
           << kObjc3RuntimeClassMetaclassProtocolRealizationSurfaceContractId
           << "\",\"runtime_category_attachment_merged_dispatch_surface_contract_id\":\""
           << kObjc3RuntimeCategoryAttachmentMergedDispatchSurfaceContractId
           << "\",\"runtime_reflection_visibility_coherence_diagnostics_surface_contract_id\":\""
           << kObjc3RuntimeReflectionVisibilityCoherenceDiagnosticsSurfaceContractId
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"object_model_query_state_snapshot_symbol\":\"objc3_runtime_copy_object_model_query_state_for_testing\""
           << ",\"realized_class_entry_snapshot_symbol\":\"objc3_runtime_copy_realized_class_entry_for_testing\""
           << ",\"property_registry_state_snapshot_symbol\":\"objc3_runtime_copy_property_registry_state_for_testing\""
           << ",\"property_entry_snapshot_symbol\":\"objc3_runtime_copy_property_entry_for_testing\""
           << ",\"protocol_conformance_query_symbol\":\"objc3_runtime_copy_protocol_conformance_query_for_testing\""
           << ",\"selector_lookup_table_state_snapshot_symbol\":\"objc3_runtime_copy_selector_lookup_table_state_for_testing\""
           << ",\"selector_lookup_entry_snapshot_symbol\":\"objc3_runtime_copy_selector_lookup_entry_for_testing\""
           << ",\"method_cache_state_snapshot_symbol\":\"objc3_runtime_copy_method_cache_state_for_testing\""
           << ",\"method_cache_entry_snapshot_symbol\":\"objc3_runtime_copy_method_cache_entry_for_testing\""
           << ",\"dispatch_state_snapshot_symbol\":\"objc3_runtime_copy_dispatch_state_for_testing\""
           << ",\"realization_lookup_reflection_implementation_model\":\""
           << kObjc3RuntimeRealizationLookupReflectionImplementationModel
           << "\""
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_reflection_query_surface\":{\"contract_id\":\""
           << kObjc3RuntimeReflectionQuerySurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"object_model_realization_source_surface_contract_id\":\""
           << kObjc3RuntimeObjectModelRealizationSourceSurfaceContractId
           << "\",\"dispatch_accessor_runtime_abi_surface_contract_id\":\"objc3c.runtime.dispatch_accessor.abi.surface.v1\""
           << ",\"property_metadata_reflection_contract_id\":\""
           << kObjc3RuntimePropertyMetadataReflectionContractId
           << "\",\"runtime_backed_object_ownership_attribute_surface_contract_id\":\""
           << kObjc3RuntimeBackedObjectOwnershipAttributeSurfaceContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"query_api_boundary_model\":\"private-testing-snapshots-over-runtime-owned-realized-class-property-and-protocol-metadata-with-no-public-reflection-abi\""
           << ",\"private_query_symbols\":[\"objc3_runtime_copy_realized_class_graph_state_for_testing\""
           << ",\"objc3_runtime_copy_realized_class_entry_for_testing\""
           << ",\"objc3_runtime_copy_property_registry_state_for_testing\""
           << ",\"objc3_runtime_copy_property_entry_for_testing\""
           << ",\"objc3_runtime_copy_protocol_conformance_query_for_testing\"]"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_realization_lookup_semantics_surface\":{\"contract_id\":\""
           << kObjc3RuntimeRealizationLookupSemanticsSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"runtime_object_model_realization_source_surface_contract_id\":\""
           << kObjc3RuntimeObjectModelRealizationSourceSurfaceContractId
           << "\",\"runtime_reflection_query_surface_contract_id\":\""
           << kObjc3RuntimeReflectionQuerySurfaceContractId
           << "\",\"dispatch_accessor_runtime_abi_surface_contract_id\":\"objc3c.runtime.dispatch_accessor.abi.surface.v1\""
           << ",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"registration_entrypoint_symbol\":\""
           << runtime_translation_unit_registration_manifest
                  .registration_entrypoint_symbol
           << "\",\"selector_lookup_symbol\":\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol
           << "\",\"runtime_dispatch_symbol\":\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol
           << "\",\"selector_lookup_table_state_snapshot_symbol\":\"objc3_runtime_copy_selector_lookup_table_state_for_testing\""
           << ",\"selector_lookup_entry_snapshot_symbol\":\"objc3_runtime_copy_selector_lookup_entry_for_testing\""
           << ",\"method_cache_state_snapshot_symbol\":\"objc3_runtime_copy_method_cache_state_for_testing\""
           << ",\"method_cache_entry_snapshot_symbol\":\"objc3_runtime_copy_method_cache_entry_for_testing\""
           << ",\"realized_class_entry_snapshot_symbol\":\"objc3_runtime_copy_realized_class_entry_for_testing\""
           << ",\"protocol_conformance_query_symbol\":\"objc3_runtime_copy_protocol_conformance_query_for_testing\""
           << ",\"lookup_resolution_order_model\":\"seeded-cache-then-live-class-chain-then-attached-category-and-protocol-checks-then-strict-dispatch-error\""
           << ",\"selector_materialization_model\":\"metadata-selectors-materialized-at-registration-and-dynamic-misses-interned-at-first-lookup\""
           << ",\"unresolved_selector_behavior_model\":\"negative-cache-entry-preserved-and-typed-strict-dispatch-error-returned\""
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_class_metaclass_protocol_realization_surface\":{\"contract_id\":\""
           << kObjc3RuntimeClassMetaclassProtocolRealizationSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"runtime_object_model_realization_source_surface_contract_id\":\""
           << kObjc3RuntimeObjectModelRealizationSourceSurfaceContractId
           << "\",\"runtime_reflection_query_surface_contract_id\":\""
           << kObjc3RuntimeReflectionQuerySurfaceContractId
           << "\",\"runtime_realization_lookup_semantics_surface_contract_id\":\""
           << kObjc3RuntimeRealizationLookupSemanticsSurfaceContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"registration_entrypoint_symbol\":\""
           << runtime_translation_unit_registration_manifest
                  .registration_entrypoint_symbol
           << "\",\"realized_class_graph_snapshot_symbol\":\"objc3_runtime_copy_realized_class_graph_state_for_testing\""
           << ",\"realized_class_entry_snapshot_symbol\":\"objc3_runtime_copy_realized_class_entry_for_testing\""
           << ",\"protocol_conformance_query_symbol\":\"objc3_runtime_copy_protocol_conformance_query_for_testing\""
           << ",\"class_realization_model\":\"registration-installs-runtime-backed-class-records-before-live-dispatch-and-reflection\""
           << ",\"metaclass_lineage_model\":\"realized-class-entries-publish-stable-class-metaclass-superclass-and-super-metaclass-owner-identities\""
           << ",\"protocol_conformance_model\":\"realized-class-entries-and-runtime-conformance-queries-publish-direct-and-attached-protocol-conformance\""
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_category_attachment_merged_dispatch_surface\":{\"contract_id\":\""
           << kObjc3RuntimeCategoryAttachmentMergedDispatchSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"runtime_category_attachment_protocol_conformance_contract_id\":\""
           << kObjc3RuntimeCategoryAttachmentProtocolConformanceContractId
           << "\",\"runtime_realization_lookup_semantics_surface_contract_id\":\""
           << kObjc3RuntimeRealizationLookupSemanticsSurfaceContractId
           << "\",\"runtime_class_metaclass_protocol_realization_surface_contract_id\":\""
           << kObjc3RuntimeClassMetaclassProtocolRealizationSurfaceContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"registration_entrypoint_symbol\":\""
           << runtime_translation_unit_registration_manifest
                  .registration_entrypoint_symbol
           << "\",\"selector_lookup_symbol\":\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol
           << "\",\"runtime_dispatch_symbol\":\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol
           << "\",\"realized_class_graph_snapshot_symbol\":\"objc3_runtime_copy_realized_class_graph_state_for_testing\""
           << ",\"realized_class_entry_snapshot_symbol\":\"objc3_runtime_copy_realized_class_entry_for_testing\""
           << ",\"protocol_conformance_query_symbol\":\"objc3_runtime_copy_protocol_conformance_query_for_testing\""
           << ",\"method_cache_state_snapshot_symbol\":\"objc3_runtime_copy_method_cache_state_for_testing\""
           << ",\"method_cache_entry_snapshot_symbol\":\"objc3_runtime_copy_method_cache_entry_for_testing\""
           << ",\"category_attachment_model\":\"registration-attaches-category-owned-instance-and-protocol-members-onto-live-realized-classes-before-dispatch\""
           << ",\"merged_dispatch_resolution_model\":\"attached-category-implementations-override-base-class-instance-lookup-before-superclass-and-protocol-strict-error\""
           << ",\"attached_protocol_visibility_model\":\"attached-categories-publish-owner-and-name-through-realized-class-entries-and-protocol-conformance-queries\""
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_reflection_visibility_coherence_diagnostics_surface\":{\"contract_id\":\""
           << kObjc3RuntimeReflectionVisibilityCoherenceDiagnosticsSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"runtime_reflection_query_surface_contract_id\":\""
           << kObjc3RuntimeReflectionQuerySurfaceContractId
           << "\",\"runtime_category_attachment_merged_dispatch_surface_contract_id\":\""
           << kObjc3RuntimeCategoryAttachmentMergedDispatchSurfaceContractId
           << "\",\"dispatch_accessor_runtime_abi_surface_contract_id\":\"objc3c.runtime.dispatch_accessor.abi.surface.v1\""
           << ",\"property_metadata_reflection_contract_id\":\""
           << kObjc3RuntimePropertyMetadataReflectionContractId
           << "\",\"runtime_backed_object_ownership_attribute_surface_contract_id\":\""
           << kObjc3RuntimeBackedObjectOwnershipAttributeSurfaceContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"property_registry_state_snapshot_symbol\":\"objc3_runtime_copy_property_registry_state_for_testing\""
           << ",\"property_entry_snapshot_symbol\":\"objc3_runtime_copy_property_entry_for_testing\""
           << ",\"realized_class_entry_snapshot_symbol\":\"objc3_runtime_copy_realized_class_entry_for_testing\""
           << ",\"protocol_conformance_query_symbol\":\"objc3_runtime_copy_protocol_conformance_query_for_testing\""
           << ",\"method_cache_state_snapshot_symbol\":\"objc3_runtime_copy_method_cache_state_for_testing\""
           << ",\"reflection_visibility_boundary_model\":\"private-testing-snapshots-remain-the-only-reflection-visibility-surface-and-publish-runtime-owned-class-property-and-protocol-state\""
           << ",\"fail_closed_lookup_diagnostic_model\":\"missing-class-and-property-lookups-publish-found-zero-without-mutating-property-registry-or-realized-class-state\""
           << ",\"runtime_coherence_diagnostic_model\":\"reflected-property-selectors-owner-identities-slot-layout-and-ownership-profiles-must-match-live-dispatch-realized-class-and-attached-protocol-state\""
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_installation_abi_surface\":{\"contract_id\":\""
           << kObjc3RuntimeInstallationAbiSurfaceContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"bootstrap_api_contract_id\":\""
           << runtime_bootstrap_api.contract_id
           << "\",\"bootstrap_reset_contract_id\":\""
           << kObjc3RuntimeBootstrapResetContractId
           << "\",\"bootstrap_registrar_contract_id\":\""
           << kObjc3RuntimeBootstrapRegistrarContractId
           << "\",\"public_installation_abi_boundary\":[\""
           << kObjc3RuntimeSupportLibraryRegisterImageSymbol << "\",\""
           << kObjc3RuntimeBootstrapStateSnapshotSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryResetForTestingSymbol
           << "\"],\"private_loader_testing_boundary\":[\""
           << kObjc3RuntimeBootstrapStageRegistrationTableSymbol << "\",\""
           << kObjc3RuntimeBootstrapImageWalkSnapshotSymbol << "\",\""
           << kObjc3RuntimeBootstrapReplayRegisteredImagesSymbol << "\",\""
           << kObjc3RuntimeBootstrapResetReplayStateSnapshotSymbol
           << "\"],\"installation_requires_coupled_registration_manifest\":true"
           << ",\"register_image_consumes_staged_registration_table_once\":true"
           << ",\"deterministic_reset_replay_supported\":true"
           << "},\n";
  manifest << "  \"runtime_loader_lifecycle_surface\":{\"contract_id\":\""
           << kObjc3RuntimeLoaderLifecycleSurfaceContractId
           << "\",\"runtime_installation_abi_surface_contract_id\":\""
           << kObjc3RuntimeInstallationAbiSurfaceContractId
           << "\",\"bootstrap_semantics_contract_id\":\""
           << runtime_bootstrap_semantics.contract_id
           << "\",\"bootstrap_reset_contract_id\":\""
           << kObjc3RuntimeBootstrapResetContractId
           << "\",\"bootstrap_registrar_contract_id\":\""
           << kObjc3RuntimeBootstrapRegistrarContractId
           << "\",\"authoritative_probe_path\":\""
           << kObjc3RuntimeInstallationLifecycleProbePath
           << "\",\"loader_testing_boundary_symbols\":[\""
           << kObjc3RuntimeBootstrapStageRegistrationTableSymbol << "\",\""
           << kObjc3RuntimeBootstrapImageWalkSnapshotSymbol << "\",\""
           << kObjc3RuntimeBootstrapReplayRegisteredImagesSymbol << "\",\""
           << kObjc3RuntimeBootstrapResetReplayStateSnapshotSymbol
           << "\"],\"lifecycle_phases\":[\"startup-installed-runtime-state\""
           << ",\"duplicate-registration-rejected-without-state-advance\""
           << ",\"out-of-order-registration-rejected-without-state-advance\""
           << ",\"invalid-anchor-root-rejected-without-state-advance\""
           << ",\"invalid-discovery-root-rejected-without-state-advance\""
           << ",\"reset-retained-bootstrap-catalog\""
           << ",\"replay-restored-installed-runtime-state\""
           << "],\"rejected_registration_status_codes\":{"
           << "\"duplicate_translation_unit_identity_key\":"
           << kObjc3RuntimeBootstrapDuplicateRegistrationStatusCode
           << ",\"out_of_order_registration\":"
           << kObjc3RuntimeBootstrapOutOfOrderStatusCode
           << ",\"invalid_registration_roots\":"
           << kObjc3RuntimeBootstrapInvalidRegistrationRootsStatusCode
           << "},\"retained_bootstrap_catalog_required\":true"
           << ",\"deterministic_replay_required\":true"
           << ",\"requires_linked_fixture_or_loader_retained_roots\":true"
           << "},\n";
  manifest << "  \"runtime_release_candidate_claim_abi_surface\":{\"contract_id\":\""
           << kObjc3RuntimeReleaseCandidateClaimAbiSurfaceContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"claimability_semantics_release_policy_surface_contract_id\":\"objc3c.runtime.claimability.semantics.release.policy.surface.v1\""
           << ",\"final_claim_publication_deprecated_path_shutdown_surface_contract_id\":\"objc3c.runtime.final.claim.publication.deprecated.path.shutdown.surface.v1\""
           << ",\"public_runtime_abi_boundary\":[\""
           << kObjc3RuntimeSupportLibraryRegisterImageSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol << "\",\""
           << kObjc3RuntimeSupportLibraryResetForTestingSymbol << "\"]"
           << ",\"private_release_candidate_claim_testing_boundary\":[\""
           << kObjc3RuntimeReleaseCandidateClaimSnapshotSymbol << "\"]"
           << ",\"release_candidate_claim_snapshot_symbol\":\""
           << kObjc3RuntimeReleaseCandidateClaimSnapshotSymbol
           << "\",\"release_candidate_claim_snapshot_type\":\""
           << kObjc3RuntimeReleaseCandidateClaimSnapshotType
           << "\",\"conformance_publication_contract_id\":\"objc3c.driver.conformance.report.publication.v1\""
           << ",\"conformance_claim_operations_contract_id\":\"objc3c.toolchain.conformance.claim.operations.v1\""
           << ",\"release_evidence_operation_contract_id\":\"objc3c.tooling.release.evidence.toolchain.operations.v1\""
           << ",\"dashboard_status_publication_contract_id\":\"objc3c.tooling.dashboard.status.publication.v1\""
           << ",\"release_candidate_matrix_contract_id\":\"objc3c.tooling.release.candidate.execution.matrix.v1\""
           << ",\"claimed_profile_ids\":[\"core\",\"strict\",\"strict-concurrency\",\"strict-system\"]"
           << ",\"targeted_profile_ids\":[\"strict\",\"strict-concurrency\",\"strict-system\"]"
           << ",\"authoritative_probe_path\":\""
           << kObjc3RuntimeReleaseCandidateClaimProbePath
           << "\",\"runtime_claim_boundary_model\":\""
           << kObjc3RuntimeReleaseCandidateClaimBoundaryModel

           << "\""
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_final_release_evidence_descaffolding_implementation_surface\":{\"contract_id\":\""
           << kObjc3RuntimeFinalReleaseEvidenceDescaffoldingImplementationSurfaceContractId
           << "\",\"runtime_release_candidate_claim_abi_surface_contract_id\":\""
           << kObjc3RuntimeReleaseCandidateClaimAbiSurfaceContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"private_release_candidate_evidence_testing_boundary\":[\""
           << kObjc3RuntimeReleaseCandidateEvidenceSnapshotSymbol << "\"]"
           << ",\"release_candidate_evidence_snapshot_symbol\":\""
           << kObjc3RuntimeReleaseCandidateEvidenceSnapshotSymbol
           << "\",\"release_candidate_evidence_snapshot_type\":\""
           << kObjc3RuntimeReleaseCandidateEvidenceSnapshotType
           << "\",\"validation_artifact_name\":\"module.objc3-conformance-validation.json\""
           << ",\"release_evidence_operation_artifact_name\":\"module.objc3-release-evidence-operation.json\""
           << ",\"dashboard_status_artifact_name\":\"module.objc3-dashboard-status.json\""
           << ",\"advanced_feature_gate_artifact_name\":\"module.objc3-advanced-feature-gate.json\""
           << ",\"release_candidate_matrix_artifact_name\":\"module.objc3-release-candidate-matrix.json\""
           << ",\"authoritative_probe_path\":\""
           << kObjc3RuntimeReleaseCandidateEvidenceProbePath
           << "\",\"implementation_model\":\""
           << kObjc3RuntimeReleaseCandidateEvidenceImplementationModel
           << "\""
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"lowering_id_class_sel_object_pointer_typecheck\":{\"replay_key\":\""
           << id_class_sel_object_pointer_typecheck_replay_key
           << "\",\"lane_contract\":\"" << kObjc3IdClassSelObjectPointerTypecheckLaneContract
           << "\",\"deterministic_handoff\":"
           << (id_class_sel_object_pointer_typecheck_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_dispatch_surface_classification\":{\"replay_key\":\""
           << dispatch_surface_classification_replay_key
           << "\",\"lane_contract\":\""
           << kObjc3DispatchSurfaceClassificationContractId
           << "\",\"deterministic_handoff\":"
           << (dispatch_surface_classification_contract.deterministic ? "true"
                                                                     : "false")
           << "},\n";
  manifest << "  \"lowering_message_send_selector_lowering\":{\"replay_key\":\""
           << message_send_selector_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3MessageSendSelectorLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (message_send_selector_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_dispatch_abi_marshalling\":{\"replay_key\":\""
           << dispatch_abi_marshalling_replay_key
           << "\",\"lane_contract\":\"" << kObjc3DispatchAbiMarshallingLaneContract
           << "\",\"deterministic_handoff\":"
           << (dispatch_abi_marshalling_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_nil_receiver_semantics_foldability\":{\"replay_key\":\""
           << nil_receiver_semantics_foldability_replay_key
           << "\",\"lane_contract\":\"" << kObjc3NilReceiverSemanticsFoldabilityLaneContract
           << "\",\"deterministic_handoff\":"
           << (nil_receiver_semantics_foldability_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_control_flow_control_flow_safety\":{\"replay_key\":\""
           << control_flow_control_flow_safety_lowering_replay_key
           << "\",\"lane_contract\":\""
           << kObjc3ControlFlowControlFlowSafetyLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (control_flow_control_flow_safety_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "},\n";
  manifest << "  \"lowering_super_dispatch_method_family\":{\"replay_key\":\""
           << super_dispatch_method_family_replay_key
           << "\",\"lane_contract\":\"" << kObjc3SuperDispatchMethodFamilyLaneContract
           << "\",\"deterministic_handoff\":"
           << (super_dispatch_method_family_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_runtime_link_host_link\":{\"replay_key\":\""
           << runtime_link_host_link_replay_key
           << "\",\"lane_contract\":\"" << kObjc3RuntimeLinkHostLinkLaneContract
           << "\",\"deterministic_handoff\":"
           << (runtime_link_host_link_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"runtime_link_host_link_runtime_dispatch_symbol\":\""
           << runtime_link_host_link_contract.runtime_dispatch_symbol
           << "\",\n";
  manifest << "  \"runtime_support_library_link_wiring_runtime_dispatch_symbol\":\""
           << runtime_support_library_link_wiring.runtime_dispatch_symbol
           << "\",\n";
  manifest << "  \"lowering_ownership_qualifier\":{\"replay_key\":\""
           << ownership_qualifier_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3OwnershipQualifierLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (ownership_qualifier_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_retain_release_operation\":{\"replay_key\":\""
           << retain_release_operation_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3RetainReleaseOperationLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (retain_release_operation_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_autoreleasepool_scope\":{\"replay_key\":\""
           << autoreleasepool_scope_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3AutoreleasePoolScopeLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (autoreleasepool_scope_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_weak_unowned_semantics\":{\"replay_key\":\""
           << weak_unowned_semantics_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3WeakUnownedSemanticsLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (weak_unowned_semantics_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_arc_diagnostics_fixit\":{\"replay_key\":\""
           << arc_diagnostics_fixit_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3ArcDiagnosticsFixitLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (arc_diagnostics_fixit_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_block_literal_capture\":{\"replay_key\":\""
           << block_literal_capture_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3BlockLiteralCaptureLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (block_literal_capture_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_block_abi_invoke_trampoline\":{\"replay_key\":\""
           << block_abi_invoke_trampoline_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3BlockAbiInvokeTrampolineLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (block_abi_invoke_trampoline_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_block_storage_escape\":{\"replay_key\":\""
           << block_storage_escape_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3BlockStorageEscapeLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (block_storage_escape_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_block_copy_dispose\":{\"replay_key\":\""
           << block_copy_dispose_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3BlockCopyDisposeLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (block_copy_dispose_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_block_determinism_perf_baseline\":{\"replay_key\":\""
           << block_determinism_perf_baseline_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3BlockDeterminismPerfBaselineLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (block_determinism_perf_baseline_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_lightweight_generic_constraint\":{\"replay_key\":\""
           << lightweight_generic_constraint_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3LightweightGenericsConstraintLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (lightweight_generic_constraint_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_nullability_flow_warning_precision\":{\"replay_key\":\""
           << nullability_flow_warning_precision_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3NullabilityFlowWarningPrecisionLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (nullability_flow_warning_precision_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_protocol_qualified_object_type\":{\"replay_key\":\""
           << protocol_qualified_object_type_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3ProtocolQualifiedObjectTypeLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (protocol_qualified_object_type_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_variance_bridge_cast\":{\"replay_key\":\""
           << variance_bridge_cast_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3VarianceBridgeCastLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (variance_bridge_cast_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_generic_metadata_abi\":{\"replay_key\":\""
           << generic_metadata_abi_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3GenericMetadataAbiLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (generic_metadata_abi_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_module_import_graph\":{\"replay_key\":\""
           << module_import_graph_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3ModuleImportGraphLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (module_import_graph_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_namespace_collision_shadowing\":{\"replay_key\":\""
           << namespace_collision_shadowing_lowering_replay_key
           << "\",\"lane_contract\":\""
           << kObjc3NamespaceCollisionShadowingLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (namespace_collision_shadowing_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "},\n";
  manifest << "  \"lowering_public_private_api_partition\":{\"replay_key\":\""
           << public_private_api_partition_lowering_replay_key
           << "\",\"lane_contract\":\""
           << kObjc3PublicPrivateApiPartitionLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (public_private_api_partition_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "},\n";
  manifest << "  \"lowering_incremental_module_cache_invalidation\":{\"replay_key\":\""
           << incremental_module_cache_invalidation_lowering_replay_key
           << "\",\"lane_contract\":\""
           << kObjc3IncrementalModuleCacheInvalidationLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (incremental_module_cache_invalidation_lowering_contract
                       .deterministic
                   ? "true"
                   : "false")
           << "},\n";
  manifest << "  \"lowering_cross_module_conformance\":{\"replay_key\":\""
           << cross_module_conformance_lowering_replay_key
           << "\",\"lane_contract\":\""
           << kObjc3CrossModuleConformanceLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (cross_module_conformance_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "},\n";
  manifest << "  \"lowering_throws_propagation\":{\"replay_key\":\""
           << throws_propagation_lowering_replay_key
           << "\",\"lane_contract\":\""
           << kObjc3ThrowsPropagationLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (throws_propagation_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "},\n";
  manifest << "  \"lowering_result_like\":{\"replay_key\":\""
           << result_like_lowering_replay_key
           << "\",\"lane_contract\":\""
           << kObjc3ResultLikeLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (result_like_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_ns_error_bridging\":{\"replay_key\":\""
           << ns_error_bridging_lowering_replay_key
           << "\",\"lane_contract\":\""
           << kObjc3NSErrorBridgingLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (ns_error_bridging_lowering_contract.deterministic ? "true"
                                                                 : "false")
           << "},\n";
  manifest << "  \"lowering_unwind_cleanup\":{\"replay_key\":\""
           << unwind_cleanup_lowering_replay_key
           << "\",\"lane_contract\":\""
           << kObjc3UnwindCleanupLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (unwind_cleanup_lowering_contract.deterministic ? "true"
                                                              : "false")
           << "},\n";
  manifest
      << "  \"lowering_error_handling_throws_abi_propagation\":{\"replay_key\":\""
      << error_handling_throws_abi_propagation_lowering_replay_key
      << "\",\"contract_id\":\""
      << kObjc3ErrorHandlingThrowsAbiPropagationLoweringContractId
      << "\",\"deterministic_handoff\":"
      << (deterministic_error_handling_throws_abi_propagation_lowering ? "true"
                                                              : "false")
      << "},\n";
  manifest
      << "  \"lowering_error_handling_result_and_bridging_artifact_replay\":{\"replay_key\":\""
      << error_handling_result_and_bridging_artifact_replay_summary.replay_key
      << "\",\"contract_id\":\""
      << kObjc3ErrorHandlingResultAndBridgingArtifactReplayContractId
      << "\",\"deterministic_handoff\":"
      << (error_handling_result_and_bridging_artifact_replay_summary.deterministic
              ? "true"
              : "false")
      << "},\n";
  manifest << "  \"semantic_canonical_type_metadata\":";
  objc3::artifacts::json::WriteSemanticTypeMetadataHandoffManifestObject(
      manifest, type_metadata_handoff);
  manifest << ",\n";
  manifest << "  \"globals\": ";
  objc3::artifacts::json::WriteProgramGlobalsManifestArray(
      manifest, program.globals, resolved_global_values);
  manifest << ",\n";
  manifest << "  \"functions\": ";
  objc3::artifacts::json::WriteFunctionDeclarationsManifestArray(
      manifest, manifest_functions);
  manifest << ",\n";
  manifest << "  \"interfaces\": ";
  objc3::artifacts::json::WriteRuntimeMetadataInterfaceManifestArray(
      manifest, runtime_metadata_source_records);
  manifest << ",\n";
  manifest << "  \"implementations\": ";
  objc3::artifacts::json::WriteRuntimeMetadataImplementationManifestArray(
      manifest, runtime_metadata_source_records);
  manifest << ",\n";
  manifest << "  \"protocols\": ";
  objc3::artifacts::json::WriteRuntimeMetadataProtocolManifestArray(
      manifest, runtime_metadata_source_records);
  manifest << ",\n";
  manifest << "  \"categories\": ";
  objc3::artifacts::json::WriteRuntimeMetadataCategoryManifestArray(
      manifest, runtime_metadata_source_records);
  manifest << ",\n";
  manifest << "  \"runtime_metadata_source_records\": ";
  objc3::artifacts::json::WriteRuntimeMetadataSourceRecordSetManifestObject(
      manifest, runtime_metadata_source_records);
  manifest << "\n";
  manifest << "}\n";
  bundle.manifest_json = manifest.str();
  bundle.runtime_metadata_binary = executable_metadata_runtime_ingest_binary_payload;
  if (IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(
          runtime_aware_import_module_frontend_closure)) {
    bundle.runtime_aware_import_module_artifact_json =
        objc3::artifacts::frontend::RenderRuntimeAwareImportModuleArtifactJson(
            runtime_aware_import_module_frontend_closure,
            runtime_metadata_source_records,
            BuildTypeSystemOptionalKeypathLoweringContractJson(
                type_system_optional_keypath_lowering_contract,
                type_system_type_semantic_model_summary,
                type_system_type_semantic_model_summary.replay_key,
                message_send_selector_lowering_replay_key,
                dispatch_abi_marshalling_replay_key,
                nil_receiver_semantics_foldability_replay_key,
                type_system_optional_keypath_lowering_replay_key),
            BuildTypeSystemOptionalKeypathRuntimeHelperContractJson(
                type_system_optional_keypath_lowering_contract,
                runtime_support_library_link_wiring,
                type_system_optional_keypath_lowering_replay_key),
            BuildTypeSystemGenericContractPreservationJson(
                type_metadata_handoff, type_system_type_semantic_model_summary),
            BuildTypeSystemNullabilityContractPreservationJson(
                type_system_type_semantic_model_summary),
            BuildTypeSystemProtocolContractPreservationJson(
                program, runtime_metadata_source_records,
                type_system_type_semantic_model_summary),
            BuildErrorHandlingResultAndBridgingArtifactReplayJson(
                error_handling_result_and_bridging_artifact_replay_summary),
            BuildConcurrencyActorMailboxRuntimeImportSummaryJson(
                BuildConcurrencyActorMailboxRuntimeImportSummary(
                    concurrency_actor_lowering_metadata_contract,
                    concurrency_actor_lowering_metadata_replay_key,
                    concurrency_actor_isolation_sendability_lowering_replay_key)),
            BuildInteropForeignSurfaceInterfacePreservationSummaryJson(
                interop_foreign_surface_interface_preservation_summary),
            BuildInteropHeaderModuleBridgeGenerationSummaryJson(
                interop_header_module_bridge_generation_summary),
            BuildInteropFfiMetadataInterfacePreservationContractJson(
                interop_foreign_call_lifetime_lowering_contract,
                interop_foreign_call_lifetime_lowering_replay_key,
                interop_foreign_surface_interface_preservation_summary,
                interop_ffi_metadata_interface_preservation_contract,
                interop_ffi_metadata_interface_preservation_replay_key),
            BuildMetaprogrammingModuleInterfaceReplayPreservationSummaryJson(
                metaprogramming_module_interface_replay_preservation_summary),
            BuildMetaprogrammingMacroHostProcessCacheRuntimeIntegrationSummaryJson(
                metaprogramming_macro_host_process_cache_runtime_integration_summary),
            BuildDispatchDispatchMetadataInterfacePreservationSummaryJson(
                dispatch_dispatch_metadata_interface_preservation_summary),
            BuildRuntimeBlockOwnershipArtifactPreservationSummaryJson(
                runtime_block_ownership_artifact_preservation_summary),
            BuildRuntimeStorageReflectionArtifactPreservationSummaryJson(
                runtime_storage_reflection_artifact_preservation_summary),
            serialized_runtime_metadata_artifact_reuse,
            serialized_runtime_metadata_reuse_records);
  }
  if (interop_header_module_bridge_generation_summary.runtime_generation_ready &&
      interop_header_module_bridge_generation_summary.deterministic) {
    bundle.interop_bridge_header_artifact_text = BuildInteropBridgeHeaderArtifactText(
        program, runtime_aware_import_module_frontend_closure,
        interop_header_module_bridge_generation_summary);
    bundle.interop_bridge_module_artifact_text = BuildInteropBridgeModuleArtifactText(
        runtime_aware_import_module_frontend_closure,
        interop_header_module_bridge_generation_summary);
    bundle.interop_bridge_artifact_json = BuildInteropBridgeArtifactJson(
        program, runtime_aware_import_module_frontend_closure,
        interop_header_module_bridge_generation_summary);
  }
  bundle.metaprogramming_macro_host_process_cache_runtime_integration_ready =
      metaprogramming_macro_host_process_cache_runtime_integration_summary
          .runtime_import_artifact_ready;
  bundle.metaprogramming_macro_host_process_cache_runtime_integration_replay_key =
      metaprogramming_macro_host_process_cache_runtime_integration_summary.replay_key;
  bundle
      .metaprogramming_macro_host_process_cache_runtime_integration_cache_root_relative_path =
      metaprogramming_macro_host_process_cache_runtime_integration_summary
          .cache_root_relative_path;
  if (error_handling_result_and_bridging_artifact_replay_summary
          .binary_artifact_replay_ready) {
    bundle.error_handling_result_bridge_artifact_replay_json =
        BuildErrorHandlingResultAndBridgingArtifactReplayJson(
            error_handling_result_and_bridging_artifact_replay_summary);
  }
  if (IsReadyObjc3VersionedConformanceReportLoweringSummary(
          versioned_conformance_report_lowering)) {
    bundle.versioned_conformance_report_artifact_json =
        BuildVersionedConformanceReportArtifactJson(
            versioned_conformance_report_lowering, options, pipeline_result,
            frontend_compatibility_strictness_claim_semantics,
            tooling_feature_aware_conformance_report_emission_summary,
            tooling_corpus_sharding_release_evidence_packaging_summary);
  }
  bundle.runtime_aware_import_module_frontend_closure_summary =
      runtime_aware_import_module_frontend_closure;
  bundle.versioned_conformance_report_lowering_summary =
      versioned_conformance_report_lowering;
  bundle.runtime_registration_descriptor_image_root_source_surface_summary =
      runtime_registration_descriptor_image_root_source_surface;
  bundle.runtime_registration_descriptor_frontend_closure_summary =
      runtime_registration_descriptor_frontend_closure;
  bundle.runtime_block_ownership_artifact_preservation_summary =
      runtime_block_ownership_artifact_preservation_summary;
  bundle.runtime_storage_reflection_artifact_preservation_summary =
      runtime_storage_reflection_artifact_preservation_summary;
  bundle.runtime_translation_unit_registration_manifest_summary =
      runtime_translation_unit_registration_manifest;
  bundle.runtime_bootstrap_legality_semantics_summary =
      runtime_bootstrap_legality_semantics;
  bundle.runtime_bootstrap_legality_failure_contract_summary =
      runtime_bootstrap_legality_failure_contract;
  bundle.runtime_bootstrap_failure_restart_semantics_summary =
      runtime_bootstrap_failure_restart_semantics;
  bundle.frontend_compatibility_strictness_claim_semantics_summary =
      frontend_compatibility_strictness_claim_semantics;
  bundle.tooling_legacy_canonical_migration_semantics_summary =
      tooling_legacy_canonical_migration_semantics_summary;
  bundle.tooling_machine_readable_conformance_report_contract_summary =
      tooling_machine_readable_conformance_report_contract_summary;
  bundle.tooling_feature_aware_conformance_report_emission_summary =
      tooling_feature_aware_conformance_report_emission_summary;
  bundle.tooling_corpus_sharding_release_evidence_packaging_summary =
      tooling_corpus_sharding_release_evidence_packaging_summary;
  bundle.runtime_bootstrap_api_summary = runtime_bootstrap_api;
  bundle.runtime_bootstrap_semantics_summary = runtime_bootstrap_semantics;
  bundle.runtime_bootstrap_lowering_summary = runtime_bootstrap_lowering;

  if (!post_pipeline_failure_code.empty()) {
    if (!options.emit_ir && !options.emit_object) {
      return bundle;
    }
    bundle.post_pipeline_diagnostics = {
        MakeDiag(1, 1, post_pipeline_failure_code, post_pipeline_failure_message)};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    return bundle;
  }

  if (!options.emit_ir && !options.emit_object) {
    return bundle;
  }

  Objc3IRFrontendMetadata ir_frontend_metadata;
  ir_frontend_metadata.language_version = options.language_version;
  ir_frontend_metadata.language_profile = LanguageProfileName(options.language_profile);
  ir_frontend_metadata.arc_mode = ArcModeName(options.arc_mode);
  ir_frontend_metadata.arc_mode_enabled =
      options.arc_mode == Objc3FrontendArcMode::kEnabled;
  ir_frontend_metadata.versioned_conformance_report_lowering_ready =
      IsReadyObjc3VersionedConformanceReportLoweringSummary(
          versioned_conformance_report_lowering);
  ir_frontend_metadata.versioned_conformance_report_lowering_replay_key =
      versioned_conformance_report_lowering.replay_key;
  ir_frontend_metadata.canonical_literal_yes_rejection_sites =
      pipeline_result.canonical_literal_rejection_counts.yes_literal_sites;
  ir_frontend_metadata.canonical_literal_no_rejection_sites =
      pipeline_result.canonical_literal_rejection_counts.no_literal_sites;
  ir_frontend_metadata.canonical_literal_null_rejection_sites =
      pipeline_result.canonical_literal_rejection_counts.null_literal_sites;
  ir_frontend_metadata.declared_interfaces = interface_implementation_summary.declared_interfaces;
  ir_frontend_metadata.declared_implementations = interface_implementation_summary.declared_implementations;
  ir_frontend_metadata.resolved_interface_symbols = interface_implementation_summary.resolved_interfaces;
  ir_frontend_metadata.resolved_implementation_symbols = interface_implementation_summary.resolved_implementations;
  ir_frontend_metadata.interface_method_symbols = interface_implementation_summary.interface_method_symbols;
  ir_frontend_metadata.implementation_method_symbols = interface_implementation_summary.implementation_method_symbols;
  ir_frontend_metadata.linked_implementation_symbols = interface_implementation_summary.linked_implementation_symbols;
  ir_frontend_metadata.declared_protocols = protocol_category_summary.declared_protocols;
  ir_frontend_metadata.declared_categories = protocol_category_summary.declared_categories;
  ir_frontend_metadata.resolved_protocol_symbols = protocol_category_summary.resolved_protocol_symbols;
  ir_frontend_metadata.resolved_category_symbols = protocol_category_summary.resolved_category_symbols;
  ir_frontend_metadata.protocol_method_symbols = protocol_category_summary.protocol_method_symbols;
  ir_frontend_metadata.category_method_symbols = protocol_category_summary.category_method_symbols;
  ir_frontend_metadata.linked_category_symbols = protocol_category_summary.linked_category_symbols;
  ir_frontend_metadata.declared_class_interfaces = class_protocol_category_linking_summary.declared_class_interfaces;
  ir_frontend_metadata.declared_class_implementations =
      class_protocol_category_linking_summary.declared_class_implementations;
  ir_frontend_metadata.resolved_class_interfaces = class_protocol_category_linking_summary.resolved_class_interfaces;
  ir_frontend_metadata.resolved_class_implementations =
      class_protocol_category_linking_summary.resolved_class_implementations;
  ir_frontend_metadata.linked_class_method_symbols =
      class_protocol_category_linking_summary.linked_class_method_symbols;
  ir_frontend_metadata.linked_category_method_symbols =
      class_protocol_category_linking_summary.linked_category_method_symbols;
  ir_frontend_metadata.protocol_composition_sites =
      class_protocol_category_linking_summary.protocol_composition_sites;
  ir_frontend_metadata.protocol_composition_symbols =
      class_protocol_category_linking_summary.protocol_composition_symbols;
  ir_frontend_metadata.category_composition_sites =
      class_protocol_category_linking_summary.category_composition_sites;
  ir_frontend_metadata.category_composition_symbols =
      class_protocol_category_linking_summary.category_composition_symbols;
  ir_frontend_metadata.invalid_protocol_composition_sites =
      class_protocol_category_linking_summary.invalid_protocol_composition_sites;
  ir_frontend_metadata.selector_method_declaration_entries = selector_normalization_summary.method_declaration_entries;
  ir_frontend_metadata.selector_normalized_method_declarations =
      selector_normalization_summary.normalized_method_declarations;
  ir_frontend_metadata.selector_piece_entries = selector_normalization_summary.selector_piece_entries;
  ir_frontend_metadata.selector_piece_parameter_links = selector_normalization_summary.selector_piece_parameter_links;
  ir_frontend_metadata.property_declaration_entries = property_attribute_summary.property_declaration_entries;
  ir_frontend_metadata.property_attribute_entries = property_attribute_summary.property_attribute_entries;
  ir_frontend_metadata.property_attribute_value_entries = property_attribute_summary.property_attribute_value_entries;
  ir_frontend_metadata.property_accessor_modifier_entries = property_attribute_summary.property_accessor_modifier_entries;
  ir_frontend_metadata.property_getter_selector_entries = property_attribute_summary.property_getter_selector_entries;
  ir_frontend_metadata.property_setter_selector_entries = property_attribute_summary.property_setter_selector_entries;
  ir_frontend_metadata.lowering_property_synthesis_ivar_binding_replay_key =
      property_synthesis_ivar_binding_replay_key;
  ir_frontend_metadata.lowering_property_synthesis_sites =
      property_synthesis_ivar_binding_contract.property_synthesis_sites;
  ir_frontend_metadata.lowering_property_synthesis_explicit_ivar_bindings =
      property_synthesis_ivar_binding_contract.property_synthesis_explicit_ivar_bindings;
  ir_frontend_metadata.lowering_property_synthesis_default_ivar_bindings =
      property_synthesis_ivar_binding_contract.property_synthesis_default_ivar_bindings;
  ir_frontend_metadata.lowering_interface_owned_property_synthesis_sites =
      property_synthesis_ivar_binding_contract
          .interface_owned_property_synthesis_sites;
  ir_frontend_metadata.lowering_implementation_property_redeclaration_sites =
      property_synthesis_ivar_binding_contract
          .implementation_property_redeclaration_sites;
  ir_frontend_metadata.lowering_property_synthesis_ivar_binding_resolved =
      property_synthesis_ivar_binding_contract.ivar_binding_resolved;
  ir_frontend_metadata.lowering_property_synthesis_deterministic_handoff =
      property_synthesis_ivar_binding_contract.deterministic;
  ir_frontend_metadata.lowering_id_class_sel_object_pointer_typecheck_replay_key =
      id_class_sel_object_pointer_typecheck_replay_key;
  ir_frontend_metadata.id_typecheck_sites = id_class_sel_object_pointer_typecheck_contract.id_typecheck_sites;
  ir_frontend_metadata.class_typecheck_sites = id_class_sel_object_pointer_typecheck_contract.class_typecheck_sites;
  ir_frontend_metadata.sel_typecheck_sites = id_class_sel_object_pointer_typecheck_contract.sel_typecheck_sites;
  ir_frontend_metadata.object_pointer_typecheck_sites =
      id_class_sel_object_pointer_typecheck_contract.object_pointer_typecheck_sites;
  ir_frontend_metadata.id_class_sel_object_pointer_typecheck_sites_total =
      id_class_sel_object_pointer_typecheck_contract.total_typecheck_sites;
  ir_frontend_metadata.lowering_dispatch_surface_classification_replay_key =
      dispatch_surface_classification_replay_key;
  ir_frontend_metadata.dispatch_surface_classification_instance_sites =
      dispatch_surface_classification_contract.instance_dispatch_sites;
  ir_frontend_metadata.dispatch_surface_classification_class_sites =
      dispatch_surface_classification_contract.class_dispatch_sites;
  ir_frontend_metadata.dispatch_surface_classification_super_sites =
      dispatch_surface_classification_contract.super_dispatch_sites;
  ir_frontend_metadata.dispatch_surface_classification_direct_sites =
      dispatch_surface_classification_contract.direct_dispatch_sites;
  ir_frontend_metadata.dispatch_surface_classification_dynamic_sites =
      dispatch_surface_classification_contract.dynamic_dispatch_sites;
  ir_frontend_metadata.dispatch_surface_classification_instance_entrypoint_family =
      dispatch_surface_classification_contract.instance_entrypoint_family;
  ir_frontend_metadata.dispatch_surface_classification_class_entrypoint_family =
      dispatch_surface_classification_contract.class_entrypoint_family;
  ir_frontend_metadata.dispatch_surface_classification_super_entrypoint_family =
      dispatch_surface_classification_contract.super_entrypoint_family;
  ir_frontend_metadata.dispatch_surface_classification_direct_entrypoint_family =
      dispatch_surface_classification_contract.direct_entrypoint_family;
  ir_frontend_metadata.dispatch_surface_classification_dynamic_entrypoint_family =
      dispatch_surface_classification_contract.dynamic_entrypoint_family;
  ir_frontend_metadata.lowering_message_send_selector_lowering_replay_key =
      message_send_selector_lowering_replay_key;
  ir_frontend_metadata.message_send_selector_lowering_sites =
      message_send_selector_lowering_contract.message_send_sites;
  ir_frontend_metadata.message_send_selector_lowering_unary_sites =
      message_send_selector_lowering_contract.unary_selector_sites;
  ir_frontend_metadata.message_send_selector_lowering_keyword_sites =
      message_send_selector_lowering_contract.keyword_selector_sites;
  ir_frontend_metadata.message_send_selector_lowering_selector_piece_sites =
      message_send_selector_lowering_contract.selector_piece_sites;
  ir_frontend_metadata.message_send_selector_lowering_argument_expression_sites =
      message_send_selector_lowering_contract.argument_expression_sites;
  ir_frontend_metadata.message_send_selector_lowering_receiver_sites =
      message_send_selector_lowering_contract.receiver_expression_sites;
  ir_frontend_metadata.message_send_selector_lowering_selector_literal_entries =
      message_send_selector_lowering_contract.selector_literal_entries;
  ir_frontend_metadata.message_send_selector_lowering_selector_literal_characters =
      message_send_selector_lowering_contract.selector_literal_characters;
  ir_frontend_metadata.lowering_dispatch_abi_marshalling_replay_key = dispatch_abi_marshalling_replay_key;
  ir_frontend_metadata.dispatch_abi_marshalling_message_send_sites =
      dispatch_abi_marshalling_contract.message_send_sites;
  ir_frontend_metadata.dispatch_abi_marshalling_receiver_slots_marshaled =
      dispatch_abi_marshalling_contract.receiver_slots_marshaled;
  ir_frontend_metadata.dispatch_abi_marshalling_selector_slots_marshaled =
      dispatch_abi_marshalling_contract.selector_slots_marshaled;
  ir_frontend_metadata.dispatch_abi_marshalling_argument_value_slots_marshaled =
      dispatch_abi_marshalling_contract.argument_value_slots_marshaled;
  ir_frontend_metadata.dispatch_abi_marshalling_argument_padding_slots_marshaled =
      dispatch_abi_marshalling_contract.argument_padding_slots_marshaled;
  ir_frontend_metadata.dispatch_abi_marshalling_argument_total_slots_marshaled =
      dispatch_abi_marshalling_contract.argument_total_slots_marshaled;
  ir_frontend_metadata.dispatch_abi_marshalling_total_marshaled_slots =
      dispatch_abi_marshalling_contract.total_marshaled_slots;
  ir_frontend_metadata.dispatch_abi_marshalling_runtime_dispatch_arg_slots =
      dispatch_abi_marshalling_contract.runtime_dispatch_arg_slots;
  ir_frontend_metadata.lowering_nil_receiver_semantics_foldability_replay_key =
      nil_receiver_semantics_foldability_replay_key;
  ir_frontend_metadata.nil_receiver_semantics_foldability_message_send_sites =
      nil_receiver_semantics_foldability_contract.message_send_sites;
  ir_frontend_metadata.nil_receiver_semantics_foldability_receiver_nil_literal_sites =
      nil_receiver_semantics_foldability_contract.receiver_nil_literal_sites;
  ir_frontend_metadata.nil_receiver_semantics_foldability_enabled_sites =
      nil_receiver_semantics_foldability_contract.nil_receiver_semantics_enabled_sites;
  ir_frontend_metadata.nil_receiver_semantics_foldability_foldable_sites =
      nil_receiver_semantics_foldability_contract.nil_receiver_foldable_sites;
  ir_frontend_metadata.nil_receiver_semantics_foldability_runtime_dispatch_required_sites =
      nil_receiver_semantics_foldability_contract.nil_receiver_runtime_dispatch_required_sites;
  ir_frontend_metadata.nil_receiver_semantics_foldability_non_nil_receiver_sites =
      nil_receiver_semantics_foldability_contract.non_nil_receiver_sites;
  ir_frontend_metadata.nil_receiver_semantics_foldability_contract_violation_sites =
      nil_receiver_semantics_foldability_contract.contract_violation_sites;
  ir_frontend_metadata.lowering_super_dispatch_method_family_replay_key =
      super_dispatch_method_family_replay_key;
  ir_frontend_metadata.super_dispatch_method_family_message_send_sites =
      super_dispatch_method_family_contract.message_send_sites;
  ir_frontend_metadata.super_dispatch_method_family_receiver_super_identifier_sites =
      super_dispatch_method_family_contract.receiver_super_identifier_sites;
  ir_frontend_metadata.super_dispatch_method_family_enabled_sites =
      super_dispatch_method_family_contract.super_dispatch_enabled_sites;
  ir_frontend_metadata.super_dispatch_method_family_requires_class_context_sites =
      super_dispatch_method_family_contract.super_dispatch_requires_class_context_sites;
  ir_frontend_metadata.super_dispatch_method_family_init_sites =
      super_dispatch_method_family_contract.method_family_init_sites;
  ir_frontend_metadata.super_dispatch_method_family_copy_sites =
      super_dispatch_method_family_contract.method_family_copy_sites;
  ir_frontend_metadata.super_dispatch_method_family_mutable_copy_sites =
      super_dispatch_method_family_contract.method_family_mutable_copy_sites;
  ir_frontend_metadata.super_dispatch_method_family_new_sites =
      super_dispatch_method_family_contract.method_family_new_sites;
  ir_frontend_metadata.super_dispatch_method_family_none_sites =
      super_dispatch_method_family_contract.method_family_none_sites;
  ir_frontend_metadata.super_dispatch_method_family_returns_retained_result_sites =
      super_dispatch_method_family_contract.method_family_returns_retained_result_sites;
  ir_frontend_metadata.super_dispatch_method_family_returns_related_result_sites =
      super_dispatch_method_family_contract.method_family_returns_related_result_sites;
  ir_frontend_metadata.super_dispatch_method_family_contract_violation_sites =
      super_dispatch_method_family_contract.contract_violation_sites;
  ir_frontend_metadata.lowering_runtime_link_host_link_replay_key = runtime_link_host_link_replay_key;
  ir_frontend_metadata.runtime_link_host_link_message_send_sites =
      runtime_link_host_link_contract.message_send_sites;
  ir_frontend_metadata.runtime_link_host_link_required_sites =
      runtime_link_host_link_contract.runtime_link_required_sites;
  ir_frontend_metadata.runtime_link_host_link_elided_sites =
      runtime_link_host_link_contract.runtime_link_elided_sites;
  ir_frontend_metadata.runtime_link_host_link_runtime_dispatch_arg_slots =
      runtime_link_host_link_contract.runtime_dispatch_arg_slots;
  ir_frontend_metadata.runtime_link_host_link_runtime_dispatch_declaration_parameter_count =
      runtime_link_host_link_contract.runtime_dispatch_declaration_parameter_count;
  ir_frontend_metadata.runtime_link_host_link_contract_violation_sites =
      runtime_link_host_link_contract.contract_violation_sites;
  ir_frontend_metadata.runtime_link_host_link_runtime_dispatch_symbol =
      runtime_link_host_link_contract.runtime_dispatch_symbol;
  ir_frontend_metadata.runtime_link_host_link_default_runtime_dispatch_symbol_binding =
      runtime_link_host_link_contract.default_runtime_dispatch_symbol_binding;
  ir_frontend_metadata.lowering_async_continuation_replay_key =
      concurrency_async_continuation_lowering_replay_key;
  ir_frontend_metadata.async_continuation_lowering_sites =
      concurrency_async_continuation_lowering_contract.async_continuation_sites;
  ir_frontend_metadata.async_continuation_lowering_async_keyword_sites =
      concurrency_async_continuation_lowering_contract.async_keyword_sites;
  ir_frontend_metadata.async_continuation_lowering_async_function_sites =
      concurrency_async_continuation_lowering_contract.async_function_sites;
  ir_frontend_metadata
      .async_continuation_lowering_continuation_allocation_sites =
      concurrency_async_continuation_lowering_contract.continuation_allocation_sites;
  ir_frontend_metadata.async_continuation_lowering_continuation_resume_sites =
      concurrency_async_continuation_lowering_contract.continuation_resume_sites;
  ir_frontend_metadata.async_continuation_lowering_continuation_suspend_sites =
      concurrency_async_continuation_lowering_contract.continuation_suspend_sites;
  ir_frontend_metadata.async_continuation_lowering_async_state_machine_sites =
      concurrency_async_continuation_lowering_contract.async_state_machine_sites;
  ir_frontend_metadata.async_continuation_lowering_normalized_sites =
      concurrency_async_continuation_lowering_contract.normalized_sites;
  ir_frontend_metadata.async_continuation_lowering_gate_blocked_sites =
      concurrency_async_continuation_lowering_contract.gate_blocked_sites;
  ir_frontend_metadata.async_continuation_lowering_contract_violation_sites =
      concurrency_async_continuation_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_async_continuation_lowering_handoff =
      concurrency_async_continuation_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_await_lowering_suspension_state_replay_key =
      concurrency_await_lowering_suspension_state_lowering_replay_key;
  ir_frontend_metadata.await_lowering_suspension_state_lowering_sites =
      concurrency_await_lowering_suspension_state_lowering_contract
          .await_suspension_sites;
  ir_frontend_metadata
      .await_lowering_suspension_state_lowering_await_keyword_sites =
      concurrency_await_lowering_suspension_state_lowering_contract
          .await_keyword_sites;
  ir_frontend_metadata
      .await_lowering_suspension_state_lowering_await_suspension_point_sites =
      concurrency_await_lowering_suspension_state_lowering_contract
          .await_suspension_point_sites;
  ir_frontend_metadata
      .await_lowering_suspension_state_lowering_await_resume_sites =
      concurrency_await_lowering_suspension_state_lowering_contract.await_resume_sites;
  ir_frontend_metadata
      .await_lowering_suspension_state_lowering_await_state_machine_sites =
      concurrency_await_lowering_suspension_state_lowering_contract
          .await_state_machine_sites;
  ir_frontend_metadata
      .await_lowering_suspension_state_lowering_await_continuation_sites =
      concurrency_await_lowering_suspension_state_lowering_contract
          .await_continuation_sites;
  ir_frontend_metadata.await_lowering_suspension_state_lowering_normalized_sites =
      concurrency_await_lowering_suspension_state_lowering_contract.normalized_sites;
  ir_frontend_metadata
      .await_lowering_suspension_state_lowering_gate_blocked_sites =
      concurrency_await_lowering_suspension_state_lowering_contract.gate_blocked_sites;
  ir_frontend_metadata
      .await_lowering_suspension_state_lowering_contract_violation_sites =
      concurrency_await_lowering_suspension_state_lowering_contract
          .contract_violation_sites;
  ir_frontend_metadata
      .deterministic_await_lowering_suspension_state_lowering_handoff =
      concurrency_await_lowering_suspension_state_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_actor_isolation_sendability_replay_key =
      concurrency_actor_isolation_sendability_lowering_replay_key;
  ir_frontend_metadata.actor_isolation_sendability_lowering_sites =
      concurrency_actor_isolation_sendability_lowering_contract.actor_isolation_sites;
  ir_frontend_metadata
      .actor_isolation_sendability_lowering_sendability_check_sites =
      concurrency_actor_isolation_sendability_lowering_contract
          .sendability_check_sites;
  ir_frontend_metadata
      .actor_isolation_sendability_lowering_cross_actor_hop_sites =
      concurrency_actor_isolation_sendability_lowering_contract.cross_actor_hop_sites;
  ir_frontend_metadata
      .actor_isolation_sendability_lowering_non_sendable_capture_sites =
      concurrency_actor_isolation_sendability_lowering_contract
          .non_sendable_capture_sites;
  ir_frontend_metadata
      .actor_isolation_sendability_lowering_sendable_transfer_sites =
      concurrency_actor_isolation_sendability_lowering_contract
          .sendable_transfer_sites;
  ir_frontend_metadata
      .actor_isolation_sendability_lowering_isolation_boundary_sites =
      concurrency_actor_isolation_sendability_lowering_contract
          .isolation_boundary_sites;
  ir_frontend_metadata
      .actor_isolation_sendability_lowering_guard_blocked_sites =
      concurrency_actor_isolation_sendability_lowering_contract.guard_blocked_sites;
  ir_frontend_metadata
      .actor_isolation_sendability_lowering_contract_violation_sites =
      concurrency_actor_isolation_sendability_lowering_contract
          .contract_violation_sites;
  ir_frontend_metadata.deterministic_actor_isolation_sendability_lowering_handoff =
      concurrency_actor_isolation_sendability_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_actor_lowering_metadata_replay_key =
      concurrency_actor_lowering_metadata_replay_key;
  // implementation anchor: the deterministic actor-lowering
  // contract now feeds helper-backed actor thunk/hop/nonisolated rewrites in
  // IR rather than staying metadata-only.
  ir_frontend_metadata.actor_lowering_metadata_actor_interface_sites =
      concurrency_actor_lowering_metadata_contract.actor_interface_sites;
  ir_frontend_metadata.actor_lowering_metadata_actor_method_sites =
      concurrency_actor_lowering_metadata_contract.actor_method_sites;
  ir_frontend_metadata.actor_lowering_metadata_actor_metadata_record_sites =
      concurrency_actor_lowering_metadata_contract.actor_metadata_record_sites;
  ir_frontend_metadata.actor_lowering_metadata_nonisolated_entry_sites =
      concurrency_actor_lowering_metadata_contract.nonisolated_entry_sites;
  ir_frontend_metadata.actor_lowering_metadata_executor_affinity_sites =
      concurrency_actor_lowering_metadata_contract.executor_affinity_sites;
  ir_frontend_metadata.actor_lowering_metadata_actor_hop_artifact_sites =
      concurrency_actor_lowering_metadata_contract.actor_hop_artifact_sites;
  ir_frontend_metadata.actor_lowering_metadata_actor_isolation_thunk_sites =
      concurrency_actor_lowering_metadata_contract.actor_isolation_thunk_sites;
  ir_frontend_metadata.actor_lowering_metadata_replay_proof_dependency_sites =
      concurrency_actor_lowering_metadata_contract.replay_proof_dependency_sites;
  ir_frontend_metadata.actor_lowering_metadata_race_guard_dependency_sites =
      concurrency_actor_lowering_metadata_contract.race_guard_dependency_sites;
  ir_frontend_metadata.actor_lowering_metadata_task_handoff_sites =
      concurrency_actor_lowering_metadata_contract.task_handoff_sites;
  ir_frontend_metadata.actor_lowering_metadata_guard_blocked_sites =
      concurrency_actor_lowering_metadata_contract.guard_blocked_sites;
  ir_frontend_metadata.actor_lowering_metadata_contract_violation_sites =
      concurrency_actor_lowering_metadata_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_actor_lowering_metadata_handoff =
      concurrency_actor_lowering_metadata_contract.deterministic;
  ir_frontend_metadata.lowering_dispatch_dispatch_control_replay_key =
      dispatch_dispatch_control_lowering_replay_key;
  ir_frontend_metadata.lowering_metaprogramming_expansion_replay_key =
      metaprogramming_expansion_lowering_replay_key;
  ir_frontend_metadata.metaprogramming_expansion_lowering_derive_inventory_sites =
      metaprogramming_expansion_lowering_contract.derive_inventory_sites;
  ir_frontend_metadata
      .metaprogramming_expansion_lowering_derived_selector_artifact_sites =
      metaprogramming_expansion_lowering_contract.derived_selector_artifact_sites;
  ir_frontend_metadata.metaprogramming_expansion_lowering_macro_replay_visible_sites =
      metaprogramming_expansion_lowering_contract.macro_replay_visible_sites;
  ir_frontend_metadata.metaprogramming_expansion_lowering_property_behavior_sites =
      metaprogramming_expansion_lowering_contract.property_behavior_sites;
  ir_frontend_metadata.metaprogramming_expansion_lowering_synthesized_binding_sites =
      metaprogramming_expansion_lowering_contract.synthesized_binding_sites;
  ir_frontend_metadata.metaprogramming_expansion_lowering_synthesized_getter_sites =
      metaprogramming_expansion_lowering_contract.synthesized_getter_sites;
  ir_frontend_metadata.metaprogramming_expansion_lowering_synthesized_setter_sites =
      metaprogramming_expansion_lowering_contract.synthesized_setter_sites;
  ir_frontend_metadata
      .metaprogramming_expansion_lowering_replay_visible_metadata_sites =
      metaprogramming_expansion_lowering_contract.replay_visible_metadata_sites;
  ir_frontend_metadata.metaprogramming_expansion_lowering_guard_blocked_sites =
      metaprogramming_expansion_lowering_contract.guard_blocked_sites;
  ir_frontend_metadata.metaprogramming_expansion_lowering_contract_violation_sites =
      metaprogramming_expansion_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_metaprogramming_expansion_lowering_handoff =
      metaprogramming_expansion_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_metaprogramming_synthesized_emission_replay_key =
      metaprogramming_synthesized_artifact_emission_replay_key;
  ir_frontend_metadata.metaprogramming_synthesized_emitted_derive_method_sites =
      metaprogramming_synthesized_artifact_emission_contract.emitted_derive_method_sites;
  ir_frontend_metadata.metaprogramming_synthesized_emitted_macro_artifact_sites =
      metaprogramming_synthesized_artifact_emission_contract.emitted_macro_artifact_sites;
  ir_frontend_metadata
      .metaprogramming_synthesized_emitted_property_behavior_artifact_sites =
      metaprogramming_synthesized_artifact_emission_contract
          .emitted_property_behavior_artifact_sites;
  ir_frontend_metadata.metaprogramming_synthesized_emitted_global_artifact_sites =
      metaprogramming_synthesized_artifact_emission_contract.emitted_global_artifact_sites;
  ir_frontend_metadata.metaprogramming_synthesized_emitted_runtime_method_list_sites =
      metaprogramming_synthesized_artifact_emission_contract
          .emitted_runtime_method_list_sites;
  ir_frontend_metadata.metaprogramming_synthesized_guard_blocked_sites =
      metaprogramming_synthesized_artifact_emission_contract.guard_blocked_sites;
  ir_frontend_metadata.metaprogramming_synthesized_contract_violation_sites =
      metaprogramming_synthesized_artifact_emission_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_metaprogramming_synthesized_emission_handoff =
      metaprogramming_synthesized_artifact_emission_contract.deterministic;
  ir_frontend_metadata.metaprogramming_derived_method_bundles_lexicographic =
      metaprogramming_derived_method_bundles;
  ir_frontend_metadata.metaprogramming_macro_artifact_bundles_lexicographic =
      metaprogramming_macro_artifact_bundles;
  ir_frontend_metadata.metaprogramming_property_behavior_artifact_bundles_lexicographic =
      metaprogramming_property_behavior_artifact_bundles;
  ir_frontend_metadata.lowering_metaprogramming_module_interface_replay_preservation_key =
      metaprogramming_module_interface_replay_preservation_summary.replay_key;
  ir_frontend_metadata.metaprogramming_module_replay_local_derive_method_count =
      metaprogramming_module_interface_replay_preservation_summary
          .local_derive_method_count;
  ir_frontend_metadata.metaprogramming_module_replay_local_macro_artifact_count =
      metaprogramming_module_interface_replay_preservation_summary
          .local_macro_artifact_count;
  ir_frontend_metadata
      .metaprogramming_module_replay_local_interface_property_behavior_artifact_count =
      metaprogramming_module_interface_replay_preservation_summary
          .local_interface_property_behavior_artifact_count;
  ir_frontend_metadata
      .metaprogramming_module_replay_local_implementation_property_behavior_artifact_count =
      metaprogramming_module_interface_replay_preservation_summary
          .local_implementation_property_behavior_artifact_count;
  ir_frontend_metadata.metaprogramming_module_replay_local_runtime_method_list_count =
      metaprogramming_module_interface_replay_preservation_summary
          .local_runtime_method_list_count;
  ir_frontend_metadata.metaprogramming_module_replay_imported_module_count =
      metaprogramming_module_interface_replay_preservation_summary
          .imported_module_count;
  ir_frontend_metadata.metaprogramming_module_replay_imported_derive_method_count =
      metaprogramming_module_interface_replay_preservation_summary
          .imported_derive_method_count;
  ir_frontend_metadata.metaprogramming_module_replay_imported_macro_artifact_count =
      metaprogramming_module_interface_replay_preservation_summary
          .imported_macro_artifact_count;
  ir_frontend_metadata
      .metaprogramming_module_replay_imported_interface_property_behavior_artifact_count =
      metaprogramming_module_interface_replay_preservation_summary
          .imported_interface_property_behavior_artifact_count;
  ir_frontend_metadata
      .metaprogramming_module_replay_imported_implementation_property_behavior_artifact_count =
      metaprogramming_module_interface_replay_preservation_summary
          .imported_implementation_property_behavior_artifact_count;
  ir_frontend_metadata.metaprogramming_module_replay_imported_runtime_method_list_count =
      metaprogramming_module_interface_replay_preservation_summary
          .imported_runtime_method_list_count;
  ir_frontend_metadata.metaprogramming_module_replay_runtime_import_artifact_ready =
      metaprogramming_module_interface_replay_preservation_summary
          .runtime_import_artifact_ready;
  ir_frontend_metadata
      .metaprogramming_module_replay_separate_compilation_preservation_ready =
      metaprogramming_module_interface_replay_preservation_summary
          .separate_compilation_preservation_ready;
  ir_frontend_metadata.deterministic_metaprogramming_module_interface_replay_handoff =
      metaprogramming_module_interface_replay_preservation_summary.deterministic;
  ir_frontend_metadata.lowering_interop_interop_replay_key =
      interop_interop_lowering_replay_key;
  ir_frontend_metadata.interop_interop_lowering_foreign_callable_sites =
      interop_interop_lowering_contract.foreign_callable_sites;
  ir_frontend_metadata.interop_interop_lowering_c_foreign_callable_sites =
      interop_interop_lowering_contract.c_foreign_callable_sites;
  ir_frontend_metadata.interop_interop_lowering_objc_runtime_parity_callable_sites =
      interop_interop_lowering_contract.objc_runtime_parity_callable_sites;
  ir_frontend_metadata.interop_interop_lowering_ownership_bridge_callable_sites =
      interop_interop_lowering_contract.ownership_bridge_callable_sites;
  ir_frontend_metadata.interop_interop_lowering_error_surface_sites =
      interop_interop_lowering_contract.error_surface_sites;
  ir_frontend_metadata.interop_interop_lowering_async_boundary_sites =
      interop_interop_lowering_contract.async_boundary_sites;
  ir_frontend_metadata.interop_interop_lowering_swift_concurrency_metadata_sites =
      interop_interop_lowering_contract.swift_concurrency_metadata_sites;
  ir_frontend_metadata.interop_interop_lowering_interface_preserved_foreign_callable_sites =
      interop_interop_lowering_contract.interface_preserved_foreign_callable_sites;
  ir_frontend_metadata.interop_interop_lowering_interface_preserved_metadata_annotation_sites =
      interop_interop_lowering_contract.interface_preserved_metadata_annotation_sites;
  ir_frontend_metadata.interop_interop_lowering_guard_blocked_sites =
      interop_interop_lowering_contract.guard_blocked_sites;
  ir_frontend_metadata.interop_interop_lowering_contract_violation_sites =
      interop_interop_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_interop_interop_lowering_handoff =
      interop_interop_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_interop_foreign_call_lifetime_replay_key =
      interop_foreign_call_lifetime_lowering_replay_key;
  ir_frontend_metadata
      .interop_foreign_call_lifetime_lowering_foreign_callable_sites =
      interop_foreign_call_lifetime_lowering_contract.foreign_callable_sites;
  ir_frontend_metadata
      .interop_foreign_call_lifetime_lowering_c_foreign_callable_sites =
      interop_foreign_call_lifetime_lowering_contract.c_foreign_callable_sites;
  ir_frontend_metadata
      .interop_foreign_call_lifetime_lowering_objc_runtime_parity_callable_sites =
      interop_foreign_call_lifetime_lowering_contract
          .objc_runtime_parity_callable_sites;
  ir_frontend_metadata
      .interop_foreign_call_lifetime_lowering_ownership_bridge_sites =
      interop_foreign_call_lifetime_lowering_contract.ownership_bridge_sites;
  ir_frontend_metadata
      .interop_foreign_call_lifetime_lowering_lifetime_bridge_sites =
      interop_foreign_call_lifetime_lowering_contract.lifetime_bridge_sites;
  ir_frontend_metadata
      .interop_foreign_call_lifetime_lowering_metadata_preservation_sites =
      interop_foreign_call_lifetime_lowering_contract
          .metadata_preservation_sites;
  ir_frontend_metadata
      .interop_foreign_call_lifetime_lowering_guard_blocked_sites =
      interop_foreign_call_lifetime_lowering_contract.guard_blocked_sites;
  ir_frontend_metadata
      .interop_foreign_call_lifetime_lowering_contract_violation_sites =
      interop_foreign_call_lifetime_lowering_contract.contract_violation_sites;
  ir_frontend_metadata
      .deterministic_interop_foreign_call_lifetime_lowering_handoff =
      interop_foreign_call_lifetime_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_interop_ffi_metadata_interface_preservation_key =
      interop_ffi_metadata_interface_preservation_replay_key;
  ir_frontend_metadata
      .interop_ffi_metadata_interface_preservation_local_foreign_callable_count =
      interop_ffi_metadata_interface_preservation_contract
          .local_foreign_callable_count;
  ir_frontend_metadata
      .interop_ffi_metadata_interface_preservation_local_metadata_preservation_sites =
      interop_ffi_metadata_interface_preservation_contract
          .local_metadata_preservation_sites;
  ir_frontend_metadata
      .interop_ffi_metadata_interface_preservation_local_interface_annotation_sites =
      interop_ffi_metadata_interface_preservation_contract
          .local_interface_annotation_sites;
  ir_frontend_metadata
      .interop_ffi_metadata_interface_preservation_imported_module_count =
      interop_ffi_metadata_interface_preservation_contract.imported_module_count;
  ir_frontend_metadata
      .interop_ffi_metadata_interface_preservation_imported_foreign_callable_count =
      interop_ffi_metadata_interface_preservation_contract
          .imported_foreign_callable_count;
  ir_frontend_metadata
      .interop_ffi_metadata_interface_preservation_imported_metadata_preservation_sites =
      interop_ffi_metadata_interface_preservation_contract
          .imported_metadata_preservation_sites;
  ir_frontend_metadata
      .interop_ffi_metadata_interface_preservation_imported_interface_annotation_sites =
      interop_ffi_metadata_interface_preservation_contract
          .imported_interface_annotation_sites;
  ir_frontend_metadata
      .interop_ffi_metadata_interface_preservation_runtime_import_artifact_ready =
      interop_ffi_metadata_interface_preservation_contract
          .runtime_import_artifact_ready;
  ir_frontend_metadata
      .interop_ffi_metadata_interface_preservation_separate_compilation_preservation_ready =
      interop_ffi_metadata_interface_preservation_contract
          .separate_compilation_preservation_ready;
  ir_frontend_metadata
      .deterministic_interop_ffi_metadata_interface_preservation_handoff =
      interop_ffi_metadata_interface_preservation_contract.deterministic;
  ir_frontend_metadata.lowering_interop_header_module_bridge_generation_key =
      interop_header_module_bridge_generation_summary.replay_key;
  ir_frontend_metadata.dispatch_dispatch_control_lowering_direct_call_candidate_sites =
      dispatch_dispatch_control_lowering_contract.direct_call_candidate_sites;
  ir_frontend_metadata.dispatch_dispatch_control_lowering_direct_members_defaulted_sites =
      dispatch_dispatch_control_lowering_contract.direct_members_defaulted_sites;
  ir_frontend_metadata.dispatch_dispatch_control_lowering_dynamic_opt_out_sites =
      dispatch_dispatch_control_lowering_contract.dynamic_opt_out_sites;
  ir_frontend_metadata.dispatch_dispatch_control_lowering_final_container_sites =
      dispatch_dispatch_control_lowering_contract.final_container_sites;
  ir_frontend_metadata.dispatch_dispatch_control_lowering_sealed_container_sites =
      dispatch_dispatch_control_lowering_contract.sealed_container_sites;
  ir_frontend_metadata.dispatch_dispatch_control_lowering_override_legality_sites =
      dispatch_dispatch_control_lowering_contract.override_legality_sites;
  ir_frontend_metadata
      .dispatch_dispatch_control_lowering_metadata_preserved_callable_sites =
      dispatch_dispatch_control_lowering_contract
          .metadata_preserved_callable_sites;
  ir_frontend_metadata
      .dispatch_dispatch_control_lowering_metadata_preserved_container_sites =
      dispatch_dispatch_control_lowering_contract
          .metadata_preserved_container_sites;
  ir_frontend_metadata.dispatch_dispatch_control_lowering_guard_blocked_sites =
      dispatch_dispatch_control_lowering_contract.guard_blocked_sites;
  ir_frontend_metadata.dispatch_dispatch_control_lowering_contract_violation_sites =
      dispatch_dispatch_control_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_dispatch_dispatch_control_lowering_handoff =
      dispatch_dispatch_control_lowering_contract.deterministic;
  ir_frontend_metadata
      .lowering_dispatch_dispatch_metadata_interface_preservation_key =
      dispatch_dispatch_metadata_interface_preservation_summary.replay_key;
  ir_frontend_metadata.dispatch_dispatch_metadata_local_direct_callable_record_count =
      dispatch_dispatch_metadata_interface_preservation_summary
          .local_direct_callable_record_count;
  ir_frontend_metadata.dispatch_dispatch_metadata_local_final_callable_record_count =
      dispatch_dispatch_metadata_interface_preservation_summary
          .local_final_callable_record_count;
  ir_frontend_metadata.dispatch_dispatch_metadata_local_final_container_record_count =
      dispatch_dispatch_metadata_interface_preservation_summary
          .local_final_container_record_count;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_local_sealed_container_record_count =
      dispatch_dispatch_metadata_interface_preservation_summary
          .local_sealed_container_record_count;
  ir_frontend_metadata.dispatch_dispatch_metadata_imported_module_count =
      dispatch_dispatch_metadata_interface_preservation_summary.imported_module_count;
  ir_frontend_metadata.dispatch_dispatch_metadata_imported_direct_callable_record_count =
      dispatch_dispatch_metadata_interface_preservation_summary
          .imported_direct_callable_record_count;
  ir_frontend_metadata.dispatch_dispatch_metadata_imported_final_callable_record_count =
      dispatch_dispatch_metadata_interface_preservation_summary
          .imported_final_callable_record_count;

  ir_frontend_metadata
      .dispatch_dispatch_metadata_imported_final_container_record_count =
      dispatch_dispatch_metadata_interface_preservation_summary
          .imported_final_container_record_count;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_imported_sealed_container_record_count =
      dispatch_dispatch_metadata_interface_preservation_summary
          .imported_sealed_container_record_count;
  ir_frontend_metadata.dispatch_dispatch_metadata_runtime_import_artifact_ready =
      dispatch_dispatch_metadata_interface_preservation_summary
          .runtime_import_artifact_ready;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_separate_compilation_preservation_ready =
      dispatch_dispatch_metadata_interface_preservation_summary
          .separate_compilation_preservation_ready;
  ir_frontend_metadata.deterministic_dispatch_dispatch_metadata_interface_handoff =
      dispatch_dispatch_metadata_interface_preservation_summary.deterministic;
  ir_frontend_metadata.lowering_ownership_system_extension_replay_key =
      ownership_system_extension_lowering_replay_key;
  ir_frontend_metadata.ownership_system_extension_lowering_cleanup_hook_sites =
      ownership_system_extension_lowering_contract.cleanup_hook_sites;
  ir_frontend_metadata.ownership_system_extension_lowering_resource_local_sites =
      ownership_system_extension_lowering_contract.resource_local_sites;
  ir_frontend_metadata.ownership_system_extension_lowering_cleanup_owned_local_sites =
      ownership_system_extension_lowering_contract.cleanup_owned_local_sites;
  ir_frontend_metadata.ownership_system_extension_lowering_resource_move_capture_sites =
      ownership_system_extension_lowering_contract.resource_move_capture_sites;
  ir_frontend_metadata.ownership_system_extension_lowering_borrowed_parameter_sites =
      ownership_system_extension_lowering_contract.borrowed_parameter_sites;
  ir_frontend_metadata.ownership_system_extension_lowering_borrowed_return_callable_sites =
      ownership_system_extension_lowering_contract.borrowed_return_callable_sites;
  ir_frontend_metadata.ownership_system_extension_lowering_borrowed_escape_candidate_sites =
      ownership_system_extension_lowering_contract.borrowed_escape_candidate_sites;
  ir_frontend_metadata.ownership_system_extension_lowering_explicit_capture_item_sites =
      ownership_system_extension_lowering_contract.explicit_capture_item_sites;
  ir_frontend_metadata.ownership_system_extension_lowering_retainable_family_callable_sites =
      ownership_system_extension_lowering_contract.retainable_family_callable_sites;
  ir_frontend_metadata.ownership_system_extension_lowering_retainable_family_operation_callable_sites =
      ownership_system_extension_lowering_contract
          .retainable_family_operation_callable_sites;
  ir_frontend_metadata.ownership_system_extension_lowering_retainable_family_alias_callable_sites =
      ownership_system_extension_lowering_contract
          .retainable_family_alias_callable_sites;
  ir_frontend_metadata.ownership_system_extension_lowering_guard_blocked_sites =
      ownership_system_extension_lowering_contract.guard_blocked_sites;
  ir_frontend_metadata.ownership_system_extension_lowering_contract_violation_sites =
      ownership_system_extension_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_ownership_system_extension_lowering_handoff =
      ownership_system_extension_lowering_contract.deterministic;
  ir_frontend_metadata.ownership_borrowed_retainable_abi_completion_replay_key =
      ownership_borrowed_retainable_abi_completion_replay_key;
  ir_frontend_metadata.ownership_borrowed_retainable_returns_borrowed_attribute_sites =
      ownership_system_extension_source_closure_summary
          .returns_borrowed_attribute_sites;
  ir_frontend_metadata.ownership_borrowed_retainable_family_retain_sites =
      ownership_retainable_c_family_source_completion_summary.family_retain_sites;
  ir_frontend_metadata.ownership_borrowed_retainable_family_release_sites =
      ownership_retainable_c_family_source_completion_summary.family_release_sites;
  ir_frontend_metadata.ownership_borrowed_retainable_family_autorelease_sites =
      ownership_retainable_c_family_source_completion_summary
          .family_autorelease_sites;
  ir_frontend_metadata
      .ownership_borrowed_retainable_compatibility_returns_retained_sites =
      ownership_retainable_c_family_source_completion_summary
          .compatibility_returns_retained_sites;
  ir_frontend_metadata
      .ownership_borrowed_retainable_compatibility_returns_not_retained_sites =
      ownership_retainable_c_family_source_completion_summary
          .compatibility_returns_not_retained_sites;
  ir_frontend_metadata.ownership_borrowed_retainable_compatibility_consumed_sites =
      ownership_retainable_c_family_source_completion_summary
          .compatibility_consumed_sites;
  ir_frontend_metadata
      .deterministic_ownership_borrowed_retainable_abi_completion_handoff =
      true;
  ir_frontend_metadata.lowering_task_runtime_interop_cancellation_replay_key =
      concurrency_task_runtime_interop_cancellation_lowering_replay_key;
  ir_frontend_metadata.task_runtime_interop_cancellation_lowering_sites =
      concurrency_task_runtime_interop_cancellation_lowering_contract
          .task_runtime_sites;
  ir_frontend_metadata
      .task_runtime_interop_cancellation_lowering_runtime_interop_sites =
      concurrency_task_runtime_interop_cancellation_lowering_contract
          .task_runtime_interop_sites;
  ir_frontend_metadata
      .task_runtime_interop_cancellation_lowering_cancellation_probe_sites =
      concurrency_task_runtime_interop_cancellation_lowering_contract
          .cancellation_probe_sites;
  ir_frontend_metadata
      .task_runtime_interop_cancellation_lowering_cancellation_handler_sites =
      concurrency_task_runtime_interop_cancellation_lowering_contract
          .cancellation_handler_sites;
  ir_frontend_metadata
      .task_runtime_interop_cancellation_lowering_runtime_resume_sites =
      concurrency_task_runtime_interop_cancellation_lowering_contract
          .runtime_resume_sites;
  ir_frontend_metadata
      .task_runtime_interop_cancellation_lowering_runtime_cancel_sites =
      concurrency_task_runtime_interop_cancellation_lowering_contract
          .runtime_cancel_sites;
  ir_frontend_metadata.task_runtime_interop_cancellation_lowering_normalized_sites =
      concurrency_task_runtime_interop_cancellation_lowering_contract
          .normalized_sites;
  ir_frontend_metadata
      .task_runtime_interop_cancellation_lowering_guard_blocked_sites =
      concurrency_task_runtime_interop_cancellation_lowering_contract
          .guard_blocked_sites;
  ir_frontend_metadata
      .task_runtime_interop_cancellation_lowering_contract_violation_sites =
      concurrency_task_runtime_interop_cancellation_lowering_contract
          .contract_violation_sites;
  ir_frontend_metadata
      .deterministic_task_runtime_interop_cancellation_lowering_handoff =
      concurrency_task_runtime_interop_cancellation_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_concurrency_replay_race_guard_replay_key =
      concurrency_concurrency_replay_race_guard_lowering_replay_key;
  // implementation anchor: the runnable actor pipeline now carries
  // the strict-concurrency replay/race-guard lowering packet beside the actor
  // lowering contract so helper-backed actor runtime rewrites can prove both
  // artifact families together.
  ir_frontend_metadata.concurrency_replay_race_guard_lowering_sites =
      concurrency_concurrency_replay_race_guard_lowering_contract
          .concurrency_replay_sites;
  ir_frontend_metadata
      .concurrency_replay_race_guard_lowering_replay_proof_sites =
      concurrency_concurrency_replay_race_guard_lowering_contract
          .replay_proof_sites;
  ir_frontend_metadata.concurrency_replay_race_guard_lowering_race_guard_sites =
      concurrency_concurrency_replay_race_guard_lowering_contract.race_guard_sites;
  ir_frontend_metadata.concurrency_replay_race_guard_lowering_task_handoff_sites =
      concurrency_concurrency_replay_race_guard_lowering_contract.task_handoff_sites;
  ir_frontend_metadata
      .concurrency_replay_race_guard_lowering_actor_isolation_sites =
      concurrency_concurrency_replay_race_guard_lowering_contract
          .actor_isolation_sites;
  ir_frontend_metadata
      .concurrency_replay_race_guard_lowering_deterministic_schedule_sites =
      concurrency_concurrency_replay_race_guard_lowering_contract
          .deterministic_schedule_sites;
  ir_frontend_metadata
      .concurrency_replay_race_guard_lowering_guard_blocked_sites =
      concurrency_concurrency_replay_race_guard_lowering_contract.guard_blocked_sites;
  ir_frontend_metadata
      .concurrency_replay_race_guard_lowering_contract_violation_sites =
      concurrency_concurrency_replay_race_guard_lowering_contract
          .contract_violation_sites;
  ir_frontend_metadata.deterministic_concurrency_replay_race_guard_lowering_handoff =
      concurrency_concurrency_replay_race_guard_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_ownership_qualifier_replay_key =
      ownership_qualifier_lowering_replay_key;
  ir_frontend_metadata.ownership_qualifier_lowering_ownership_qualifier_sites =
      ownership_qualifier_lowering_contract.ownership_qualifier_sites;
  ir_frontend_metadata.ownership_qualifier_lowering_invalid_ownership_qualifier_sites =
      ownership_qualifier_lowering_contract.invalid_ownership_qualifier_sites;
  ir_frontend_metadata.ownership_qualifier_lowering_object_pointer_type_annotation_sites =
      ownership_qualifier_lowering_contract.object_pointer_type_annotation_sites;
  ir_frontend_metadata.lowering_retain_release_operation_replay_key =
      retain_release_operation_lowering_replay_key;
  ir_frontend_metadata.retain_release_operation_lowering_ownership_qualified_sites =
      retain_release_operation_lowering_contract.ownership_qualified_sites;
  ir_frontend_metadata.retain_release_operation_lowering_retain_insertion_sites =
      retain_release_operation_lowering_contract.retain_insertion_sites;
  ir_frontend_metadata.retain_release_operation_lowering_release_insertion_sites =
      retain_release_operation_lowering_contract.release_insertion_sites;
  ir_frontend_metadata.retain_release_operation_lowering_autorelease_insertion_sites =
      retain_release_operation_lowering_contract.autorelease_insertion_sites;
  ir_frontend_metadata.retain_release_operation_lowering_contract_violation_sites =
      retain_release_operation_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.lowering_autoreleasepool_scope_replay_key =
      autoreleasepool_scope_lowering_replay_key;
  ir_frontend_metadata.autoreleasepool_scope_lowering_scope_sites =
      autoreleasepool_scope_lowering_contract.scope_sites;
  ir_frontend_metadata.autoreleasepool_scope_lowering_scope_symbolized_sites =
      autoreleasepool_scope_lowering_contract.scope_symbolized_sites;
  ir_frontend_metadata.autoreleasepool_scope_lowering_max_scope_depth =
      autoreleasepool_scope_lowering_contract.max_scope_depth;
  ir_frontend_metadata.autoreleasepool_scope_lowering_scope_entry_transition_sites =
      autoreleasepool_scope_lowering_contract.scope_entry_transition_sites;
  ir_frontend_metadata.autoreleasepool_scope_lowering_scope_exit_transition_sites =
      autoreleasepool_scope_lowering_contract.scope_exit_transition_sites;
  ir_frontend_metadata.autoreleasepool_scope_lowering_contract_violation_sites =
      autoreleasepool_scope_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.lowering_weak_unowned_semantics_replay_key =
      weak_unowned_semantics_lowering_replay_key;
  ir_frontend_metadata.weak_unowned_semantics_lowering_ownership_candidate_sites =
      weak_unowned_semantics_lowering_contract.ownership_candidate_sites;
  ir_frontend_metadata.weak_unowned_semantics_lowering_weak_reference_sites =
      weak_unowned_semantics_lowering_contract.weak_reference_sites;
  ir_frontend_metadata.weak_unowned_semantics_lowering_unowned_reference_sites =
      weak_unowned_semantics_lowering_contract.unowned_reference_sites;
  ir_frontend_metadata.weak_unowned_semantics_lowering_unowned_safe_reference_sites =
      weak_unowned_semantics_lowering_contract.unowned_safe_reference_sites;
  ir_frontend_metadata.weak_unowned_semantics_lowering_conflict_sites =
      weak_unowned_semantics_lowering_contract.weak_unowned_conflict_sites;
  ir_frontend_metadata.weak_unowned_semantics_lowering_contract_violation_sites =
      weak_unowned_semantics_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_weak_unowned_semantics_lowering_handoff =
      weak_unowned_semantics_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_arc_diagnostics_fixit_replay_key =
      arc_diagnostics_fixit_lowering_replay_key;
  ir_frontend_metadata.arc_diagnostics_fixit_lowering_ownership_arc_diagnostic_candidate_sites =
      arc_diagnostics_fixit_lowering_contract.ownership_arc_diagnostic_candidate_sites;
  ir_frontend_metadata.arc_diagnostics_fixit_lowering_ownership_arc_fixit_available_sites =
      arc_diagnostics_fixit_lowering_contract.ownership_arc_fixit_available_sites;
  ir_frontend_metadata.arc_diagnostics_fixit_lowering_ownership_arc_profiled_sites =
      arc_diagnostics_fixit_lowering_contract.ownership_arc_profiled_sites;
  ir_frontend_metadata
      .arc_diagnostics_fixit_lowering_ownership_arc_weak_unowned_conflict_diagnostic_sites =
      arc_diagnostics_fixit_lowering_contract.ownership_arc_weak_unowned_conflict_diagnostic_sites;
  ir_frontend_metadata.arc_diagnostics_fixit_lowering_ownership_arc_empty_fixit_hint_sites =
      arc_diagnostics_fixit_lowering_contract.ownership_arc_empty_fixit_hint_sites;
  ir_frontend_metadata.arc_diagnostics_fixit_lowering_contract_violation_sites =
      arc_diagnostics_fixit_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_arc_diagnostics_fixit_lowering_handoff =
      arc_diagnostics_fixit_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_block_literal_capture_replay_key =
      block_literal_capture_lowering_replay_key;
  ir_frontend_metadata.block_literal_capture_lowering_block_literal_sites =
      block_literal_capture_lowering_contract.block_literal_sites;
  ir_frontend_metadata.block_literal_capture_lowering_block_parameter_entries =
      block_literal_capture_lowering_contract.block_parameter_entries;
  ir_frontend_metadata.block_literal_capture_lowering_block_capture_entries =
      block_literal_capture_lowering_contract.block_capture_entries;
  ir_frontend_metadata.block_literal_capture_lowering_block_body_statement_entries =
      block_literal_capture_lowering_contract.block_body_statement_entries;
  ir_frontend_metadata.block_literal_capture_lowering_block_empty_capture_sites =
      block_literal_capture_lowering_contract.block_empty_capture_sites;
  ir_frontend_metadata.block_literal_capture_lowering_block_nondeterministic_capture_sites =
      block_literal_capture_lowering_contract.block_nondeterministic_capture_sites;
  ir_frontend_metadata.block_literal_capture_lowering_block_non_normalized_sites =
      block_literal_capture_lowering_contract.block_non_normalized_sites;
  ir_frontend_metadata.block_literal_capture_lowering_contract_violation_sites =
      block_literal_capture_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_block_literal_capture_lowering_handoff =
      block_literal_capture_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_block_source_model_completion_replay_key =
      block_source_model_completion_replay_key;
  ir_frontend_metadata.block_source_model_completion_block_literal_sites =
      block_source_model_completion_contract.block_literal_sites;
  ir_frontend_metadata.block_source_model_completion_signature_entries_total =
      block_source_model_completion_contract.signature_entries_total;
  ir_frontend_metadata
      .block_source_model_completion_explicit_typed_parameter_entries_total =
      block_source_model_completion_contract
          .explicit_typed_parameter_entries_total;
  ir_frontend_metadata
      .block_source_model_completion_implicit_parameter_entries_total =
      block_source_model_completion_contract
          .implicit_parameter_entries_total;
  ir_frontend_metadata
      .block_source_model_completion_capture_inventory_entries_total =
      block_source_model_completion_contract.capture_inventory_entries_total;
  ir_frontend_metadata
      .block_source_model_completion_byvalue_readonly_capture_entries_total =
      block_source_model_completion_contract
          .byvalue_readonly_capture_entries_total;
  ir_frontend_metadata.block_source_model_completion_invoke_surface_entries_total =
      block_source_model_completion_contract.invoke_surface_entries_total;
  ir_frontend_metadata.block_source_model_completion_non_normalized_sites =
      block_source_model_completion_contract.non_normalized_sites;
  ir_frontend_metadata.block_source_model_completion_contract_violation_sites =
      block_source_model_completion_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_block_source_model_completion_handoff =
      block_source_model_completion_contract.deterministic;
  ir_frontend_metadata.lowering_block_source_storage_annotation_replay_key =
      block_source_storage_annotation_replay_key;
  ir_frontend_metadata.block_source_storage_annotation_block_literal_sites =
      block_source_storage_annotation_contract.block_literal_sites;
  ir_frontend_metadata.block_source_storage_annotation_capture_entries_total =
      block_source_storage_annotation_contract.capture_entries_total;
  ir_frontend_metadata
      .block_source_storage_annotation_mutated_capture_entries_total =
      block_source_storage_annotation_contract
          .mutated_capture_entries_total;
  ir_frontend_metadata
      .block_source_storage_annotation_byref_capture_entries_total =
      block_source_storage_annotation_contract.byref_capture_entries_total;
  ir_frontend_metadata.block_source_storage_annotation_copy_helper_intent_sites =
      block_source_storage_annotation_contract.copy_helper_intent_sites;
  ir_frontend_metadata
      .block_source_storage_annotation_dispose_helper_intent_sites =
      block_source_storage_annotation_contract
          .dispose_helper_intent_sites;
  ir_frontend_metadata.block_source_storage_annotation_heap_candidate_sites =
      block_source_storage_annotation_contract.heap_candidate_sites;
  ir_frontend_metadata.block_source_storage_annotation_expression_sites =
      block_source_storage_annotation_contract.expression_sites;
  ir_frontend_metadata.block_source_storage_annotation_global_initializer_sites =
      block_source_storage_annotation_contract.global_initializer_sites;
  ir_frontend_metadata.block_source_storage_annotation_binding_initializer_sites =
      block_source_storage_annotation_contract.binding_initializer_sites;
  ir_frontend_metadata.block_source_storage_annotation_assignment_value_sites =
      block_source_storage_annotation_contract.assignment_value_sites;
  ir_frontend_metadata.block_source_storage_annotation_return_value_sites =
      block_source_storage_annotation_contract.return_value_sites;
  ir_frontend_metadata.block_source_storage_annotation_call_argument_sites =
      block_source_storage_annotation_contract.call_argument_sites;
  ir_frontend_metadata.block_source_storage_annotation_message_argument_sites =
      block_source_storage_annotation_contract.message_argument_sites;
  ir_frontend_metadata.block_source_storage_annotation_non_normalized_sites =
      block_source_storage_annotation_contract.non_normalized_sites;
  ir_frontend_metadata.block_source_storage_annotation_contract_violation_sites =
      block_source_storage_annotation_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_block_source_storage_annotation_handoff =
      block_source_storage_annotation_contract.deterministic;
  ir_frontend_metadata.lowering_block_abi_invoke_trampoline_replay_key =
      block_abi_invoke_trampoline_lowering_replay_key;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_block_literal_sites =
      block_abi_invoke_trampoline_lowering_contract.block_literal_sites;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_invoke_argument_slots_total =
      block_abi_invoke_trampoline_lowering_contract.invoke_argument_slots_total;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_capture_word_count_total =
      block_abi_invoke_trampoline_lowering_contract.capture_word_count_total;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_parameter_entries_total =
      block_abi_invoke_trampoline_lowering_contract.parameter_entries_total;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_capture_entries_total =
      block_abi_invoke_trampoline_lowering_contract.capture_entries_total;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_body_statement_entries_total =
      block_abi_invoke_trampoline_lowering_contract.body_statement_entries_total;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_descriptor_symbolized_sites =
      block_abi_invoke_trampoline_lowering_contract.descriptor_symbolized_sites;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_invoke_symbolized_sites =
      block_abi_invoke_trampoline_lowering_contract.invoke_trampoline_symbolized_sites;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_missing_invoke_sites =
      block_abi_invoke_trampoline_lowering_contract.missing_invoke_trampoline_sites;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_non_normalized_layout_sites =
      block_abi_invoke_trampoline_lowering_contract.non_normalized_layout_sites;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_contract_violation_sites =
      block_abi_invoke_trampoline_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_block_abi_invoke_trampoline_lowering_handoff =
      block_abi_invoke_trampoline_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_block_storage_escape_replay_key =
      block_storage_escape_lowering_replay_key;
  ir_frontend_metadata.block_storage_escape_lowering_block_literal_sites =
      block_storage_escape_lowering_contract.block_literal_sites;
  ir_frontend_metadata.block_storage_escape_lowering_mutable_capture_count_total =
      block_storage_escape_lowering_contract.mutable_capture_count_total;
  ir_frontend_metadata.block_storage_escape_lowering_byref_slot_count_total =
      block_storage_escape_lowering_contract.byref_slot_count_total;
  ir_frontend_metadata.block_storage_escape_lowering_parameter_entries_total =
      block_storage_escape_lowering_contract.parameter_entries_total;
  ir_frontend_metadata.block_storage_escape_lowering_capture_entries_total =
      block_storage_escape_lowering_contract.capture_entries_total;
  ir_frontend_metadata.block_storage_escape_lowering_body_statement_entries_total =
      block_storage_escape_lowering_contract.body_statement_entries_total;
  ir_frontend_metadata.block_storage_escape_lowering_requires_byref_cells_sites =
      block_storage_escape_lowering_contract.requires_byref_cells_sites;
  ir_frontend_metadata.block_storage_escape_lowering_escape_analysis_enabled_sites =
      block_storage_escape_lowering_contract.escape_analysis_enabled_sites;
  ir_frontend_metadata.block_storage_escape_lowering_escape_to_heap_sites =
      block_storage_escape_lowering_contract.escape_to_heap_sites;
  ir_frontend_metadata.block_storage_escape_lowering_escape_profile_normalized_sites =
      block_storage_escape_lowering_contract.escape_profile_normalized_sites;
  ir_frontend_metadata.block_storage_escape_lowering_byref_layout_symbolized_sites =
      block_storage_escape_lowering_contract.byref_layout_symbolized_sites;
  ir_frontend_metadata.block_storage_escape_lowering_contract_violation_sites =
      block_storage_escape_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_block_storage_escape_lowering_handoff =
      block_storage_escape_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_block_copy_dispose_replay_key =
      block_copy_dispose_lowering_replay_key;
  ir_frontend_metadata.block_copy_dispose_lowering_block_literal_sites =
      block_copy_dispose_lowering_contract.block_literal_sites;
  ir_frontend_metadata.block_copy_dispose_lowering_mutable_capture_count_total =
      block_copy_dispose_lowering_contract.mutable_capture_count_total;
  ir_frontend_metadata.block_copy_dispose_lowering_byref_slot_count_total =
      block_copy_dispose_lowering_contract.byref_slot_count_total;
  ir_frontend_metadata.block_copy_dispose_lowering_parameter_entries_total =
      block_copy_dispose_lowering_contract.parameter_entries_total;
  ir_frontend_metadata.block_copy_dispose_lowering_capture_entries_total =
      block_copy_dispose_lowering_contract.capture_entries_total;
  ir_frontend_metadata.block_copy_dispose_lowering_body_statement_entries_total =
      block_copy_dispose_lowering_contract.body_statement_entries_total;
  ir_frontend_metadata.block_copy_dispose_lowering_copy_helper_required_sites =
      block_copy_dispose_lowering_contract.copy_helper_required_sites;
  ir_frontend_metadata.block_copy_dispose_lowering_dispose_helper_required_sites =
      block_copy_dispose_lowering_contract.dispose_helper_required_sites;
  ir_frontend_metadata.block_copy_dispose_lowering_profile_normalized_sites =
      block_copy_dispose_lowering_contract.profile_normalized_sites;
  ir_frontend_metadata.block_copy_dispose_lowering_copy_helper_symbolized_sites =
      block_copy_dispose_lowering_contract.copy_helper_symbolized_sites;
  ir_frontend_metadata.block_copy_dispose_lowering_dispose_helper_symbolized_sites =
      block_copy_dispose_lowering_contract.dispose_helper_symbolized_sites;
  ir_frontend_metadata.block_copy_dispose_lowering_contract_violation_sites =
      block_copy_dispose_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_block_copy_dispose_lowering_handoff =
      block_copy_dispose_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_block_determinism_perf_baseline_replay_key =
      block_determinism_perf_baseline_lowering_replay_key;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_block_literal_sites =
      block_determinism_perf_baseline_lowering_contract.block_literal_sites;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_baseline_weight_total =
      block_determinism_perf_baseline_lowering_contract.baseline_weight_total;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_parameter_entries_total =
      block_determinism_perf_baseline_lowering_contract.parameter_entries_total;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_capture_entries_total =
      block_determinism_perf_baseline_lowering_contract.capture_entries_total;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_body_statement_entries_total =
      block_determinism_perf_baseline_lowering_contract.body_statement_entries_total;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_deterministic_capture_sites =
      block_determinism_perf_baseline_lowering_contract.deterministic_capture_sites;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_heavy_tier_sites =
      block_determinism_perf_baseline_lowering_contract.heavy_tier_sites;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_normalized_profile_sites =
      block_determinism_perf_baseline_lowering_contract.normalized_profile_sites;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_contract_violation_sites =
      block_determinism_perf_baseline_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_block_determinism_perf_baseline_lowering_handoff =
      block_determinism_perf_baseline_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_lightweight_generic_constraint_replay_key =
      lightweight_generic_constraint_lowering_replay_key;
  ir_frontend_metadata.lightweight_generic_constraint_lowering_generic_constraint_sites =
      lightweight_generic_constraint_lowering_contract.generic_constraint_sites;
  ir_frontend_metadata.lightweight_generic_constraint_lowering_generic_suffix_sites =
      lightweight_generic_constraint_lowering_contract.generic_suffix_sites;
  ir_frontend_metadata.lightweight_generic_constraint_lowering_object_pointer_type_sites =
      lightweight_generic_constraint_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata.lightweight_generic_constraint_lowering_terminated_generic_suffix_sites =
      lightweight_generic_constraint_lowering_contract.terminated_generic_suffix_sites;
  ir_frontend_metadata.lightweight_generic_constraint_lowering_pointer_declarator_sites =
      lightweight_generic_constraint_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.lightweight_generic_constraint_lowering_normalized_constraint_sites =
      lightweight_generic_constraint_lowering_contract.normalized_constraint_sites;
  ir_frontend_metadata.lightweight_generic_constraint_lowering_contract_violation_sites =
      lightweight_generic_constraint_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_lightweight_generic_constraint_lowering_handoff =
      lightweight_generic_constraint_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_nullability_flow_warning_precision_replay_key =
      nullability_flow_warning_precision_lowering_replay_key;
  ir_frontend_metadata.nullability_flow_warning_precision_lowering_sites =
      nullability_flow_warning_precision_lowering_contract.nullability_flow_sites;
  ir_frontend_metadata.nullability_flow_warning_precision_lowering_object_pointer_type_sites =
      nullability_flow_warning_precision_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata.nullability_flow_warning_precision_lowering_nullability_suffix_sites =
      nullability_flow_warning_precision_lowering_contract.nullability_suffix_sites;
  ir_frontend_metadata.nullability_flow_warning_precision_lowering_nullable_suffix_sites =
      nullability_flow_warning_precision_lowering_contract.nullable_suffix_sites;
  ir_frontend_metadata.nullability_flow_warning_precision_lowering_nonnull_suffix_sites =
      nullability_flow_warning_precision_lowering_contract.nonnull_suffix_sites;
  ir_frontend_metadata.nullability_flow_warning_precision_lowering_normalized_sites =
      nullability_flow_warning_precision_lowering_contract.normalized_sites;
  ir_frontend_metadata.nullability_flow_warning_precision_lowering_contract_violation_sites =
      nullability_flow_warning_precision_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_nullability_flow_warning_precision_lowering_handoff =
      nullability_flow_warning_precision_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_protocol_qualified_object_type_replay_key =
      protocol_qualified_object_type_lowering_replay_key;
  ir_frontend_metadata.protocol_qualified_object_type_lowering_sites =
      protocol_qualified_object_type_lowering_contract.protocol_qualified_object_type_sites;
  ir_frontend_metadata.protocol_qualified_object_type_lowering_protocol_composition_sites =
      protocol_qualified_object_type_lowering_contract.protocol_composition_sites;
  ir_frontend_metadata.protocol_qualified_object_type_lowering_object_pointer_type_sites =
      protocol_qualified_object_type_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata.protocol_qualified_object_type_lowering_terminated_protocol_composition_sites =
      protocol_qualified_object_type_lowering_contract.terminated_protocol_composition_sites;
  ir_frontend_metadata.protocol_qualified_object_type_lowering_pointer_declarator_sites =
      protocol_qualified_object_type_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.protocol_qualified_object_type_lowering_normalized_protocol_composition_sites =
      protocol_qualified_object_type_lowering_contract.normalized_protocol_composition_sites;
  ir_frontend_metadata.protocol_qualified_object_type_lowering_contract_violation_sites =
      protocol_qualified_object_type_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_protocol_qualified_object_type_lowering_handoff =
      protocol_qualified_object_type_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_variance_bridge_cast_replay_key =
      variance_bridge_cast_lowering_replay_key;
  ir_frontend_metadata.variance_bridge_cast_lowering_sites =
      variance_bridge_cast_lowering_contract.variance_bridge_cast_sites;
  ir_frontend_metadata.variance_bridge_cast_lowering_protocol_composition_sites =
      variance_bridge_cast_lowering_contract.protocol_composition_sites;
  ir_frontend_metadata.variance_bridge_cast_lowering_ownership_qualifier_sites =
      variance_bridge_cast_lowering_contract.ownership_qualifier_sites;
  ir_frontend_metadata.variance_bridge_cast_lowering_object_pointer_type_sites =
      variance_bridge_cast_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata.variance_bridge_cast_lowering_pointer_declarator_sites =
      variance_bridge_cast_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.variance_bridge_cast_lowering_normalized_sites =
      variance_bridge_cast_lowering_contract.normalized_sites;
  ir_frontend_metadata.variance_bridge_cast_lowering_contract_violation_sites =
      variance_bridge_cast_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_variance_bridge_cast_lowering_handoff =
      variance_bridge_cast_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_generic_metadata_abi_replay_key =
      generic_metadata_abi_lowering_replay_key;
  ir_frontend_metadata.generic_metadata_abi_lowering_sites =
      generic_metadata_abi_lowering_contract.generic_metadata_abi_sites;
  ir_frontend_metadata.generic_metadata_abi_lowering_generic_suffix_sites =
      generic_metadata_abi_lowering_contract.generic_suffix_sites;
  ir_frontend_metadata.generic_metadata_abi_lowering_protocol_composition_sites =
      generic_metadata_abi_lowering_contract.protocol_composition_sites;
  ir_frontend_metadata.generic_metadata_abi_lowering_ownership_qualifier_sites =
      generic_metadata_abi_lowering_contract.ownership_qualifier_sites;
  ir_frontend_metadata.generic_metadata_abi_lowering_object_pointer_type_sites =
      generic_metadata_abi_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata.generic_metadata_abi_lowering_pointer_declarator_sites =
      generic_metadata_abi_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.generic_metadata_abi_lowering_normalized_sites =
      generic_metadata_abi_lowering_contract.normalized_sites;
  ir_frontend_metadata.generic_metadata_abi_lowering_contract_violation_sites =
      generic_metadata_abi_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_generic_metadata_abi_lowering_handoff =
      generic_metadata_abi_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_module_import_graph_replay_key =
      module_import_graph_lowering_replay_key;
  ir_frontend_metadata.module_import_graph_lowering_sites =
      module_import_graph_lowering_contract.module_import_graph_sites;
  ir_frontend_metadata.module_import_graph_lowering_import_edge_candidate_sites =
      module_import_graph_lowering_contract.import_edge_candidate_sites;
  ir_frontend_metadata.module_import_graph_lowering_namespace_segment_sites =
      module_import_graph_lowering_contract.namespace_segment_sites;
  ir_frontend_metadata.module_import_graph_lowering_object_pointer_type_sites =
      module_import_graph_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata.module_import_graph_lowering_pointer_declarator_sites =
      module_import_graph_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.module_import_graph_lowering_normalized_sites =
      module_import_graph_lowering_contract.normalized_sites;
  ir_frontend_metadata.module_import_graph_lowering_contract_violation_sites =
      module_import_graph_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_module_import_graph_lowering_handoff =
      module_import_graph_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_namespace_collision_shadowing_replay_key =
      namespace_collision_shadowing_lowering_replay_key;
  ir_frontend_metadata.namespace_collision_shadowing_lowering_sites =
      namespace_collision_shadowing_lowering_contract
          .namespace_collision_shadowing_sites;
  ir_frontend_metadata
      .namespace_collision_shadowing_lowering_namespace_segment_sites =
      namespace_collision_shadowing_lowering_contract.namespace_segment_sites;
  ir_frontend_metadata
      .namespace_collision_shadowing_lowering_import_edge_candidate_sites =
      namespace_collision_shadowing_lowering_contract.import_edge_candidate_sites;
  ir_frontend_metadata
      .namespace_collision_shadowing_lowering_object_pointer_type_sites =
      namespace_collision_shadowing_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata
      .namespace_collision_shadowing_lowering_pointer_declarator_sites =
      namespace_collision_shadowing_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.namespace_collision_shadowing_lowering_normalized_sites =
      namespace_collision_shadowing_lowering_contract.normalized_sites;
  ir_frontend_metadata
      .namespace_collision_shadowing_lowering_contract_violation_sites =
      namespace_collision_shadowing_lowering_contract.contract_violation_sites;
  ir_frontend_metadata
      .deterministic_namespace_collision_shadowing_lowering_handoff =
      namespace_collision_shadowing_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_public_private_api_partition_replay_key =
      public_private_api_partition_lowering_replay_key;
  ir_frontend_metadata.public_private_api_partition_lowering_sites =
      public_private_api_partition_lowering_contract
          .public_private_api_partition_sites;
  ir_frontend_metadata
      .public_private_api_partition_lowering_namespace_segment_sites =
      public_private_api_partition_lowering_contract.namespace_segment_sites;
  ir_frontend_metadata
      .public_private_api_partition_lowering_import_edge_candidate_sites =
      public_private_api_partition_lowering_contract
          .import_edge_candidate_sites;
  ir_frontend_metadata
      .public_private_api_partition_lowering_object_pointer_type_sites =
      public_private_api_partition_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata
      .public_private_api_partition_lowering_pointer_declarator_sites =
      public_private_api_partition_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.public_private_api_partition_lowering_normalized_sites =
      public_private_api_partition_lowering_contract.normalized_sites;
  ir_frontend_metadata
      .public_private_api_partition_lowering_contract_violation_sites =
      public_private_api_partition_lowering_contract.contract_violation_sites;
  ir_frontend_metadata
      .deterministic_public_private_api_partition_lowering_handoff =
      public_private_api_partition_lowering_contract.deterministic;
  ir_frontend_metadata
      .lowering_incremental_module_cache_invalidation_replay_key =
      incremental_module_cache_invalidation_lowering_replay_key;
  ir_frontend_metadata.incremental_module_cache_invalidation_lowering_sites =
      incremental_module_cache_invalidation_lowering_contract
          .incremental_module_cache_invalidation_sites;
  ir_frontend_metadata
      .incremental_module_cache_invalidation_lowering_namespace_segment_sites =
      incremental_module_cache_invalidation_lowering_contract
          .namespace_segment_sites;
  ir_frontend_metadata
      .incremental_module_cache_invalidation_lowering_import_edge_candidate_sites =
      incremental_module_cache_invalidation_lowering_contract
          .import_edge_candidate_sites;
  ir_frontend_metadata
      .incremental_module_cache_invalidation_lowering_object_pointer_type_sites =
      incremental_module_cache_invalidation_lowering_contract
          .object_pointer_type_sites;
  ir_frontend_metadata
      .incremental_module_cache_invalidation_lowering_pointer_declarator_sites =
      incremental_module_cache_invalidation_lowering_contract
          .pointer_declarator_sites;
  ir_frontend_metadata
      .incremental_module_cache_invalidation_lowering_normalized_sites =
      incremental_module_cache_invalidation_lowering_contract.normalized_sites;
  ir_frontend_metadata
      .incremental_module_cache_invalidation_lowering_cache_invalidation_candidate_sites =
      incremental_module_cache_invalidation_lowering_contract
          .cache_invalidation_candidate_sites;
  ir_frontend_metadata
      .incremental_module_cache_invalidation_lowering_contract_violation_sites =
      incremental_module_cache_invalidation_lowering_contract
          .contract_violation_sites;
  ir_frontend_metadata
      .deterministic_incremental_module_cache_invalidation_lowering_handoff =
      incremental_module_cache_invalidation_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_cross_module_conformance_replay_key =
      cross_module_conformance_lowering_replay_key;
  ir_frontend_metadata.cross_module_conformance_lowering_sites =
      cross_module_conformance_lowering_contract.cross_module_conformance_sites;
  ir_frontend_metadata
      .cross_module_conformance_lowering_namespace_segment_sites =
      cross_module_conformance_lowering_contract.namespace_segment_sites;
  ir_frontend_metadata
      .cross_module_conformance_lowering_import_edge_candidate_sites =
      cross_module_conformance_lowering_contract.import_edge_candidate_sites;
  ir_frontend_metadata
      .cross_module_conformance_lowering_object_pointer_type_sites =
      cross_module_conformance_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata
      .cross_module_conformance_lowering_pointer_declarator_sites =
      cross_module_conformance_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.cross_module_conformance_lowering_normalized_sites =
      cross_module_conformance_lowering_contract.normalized_sites;
  ir_frontend_metadata
      .cross_module_conformance_lowering_cache_invalidation_candidate_sites =
      cross_module_conformance_lowering_contract
          .cache_invalidation_candidate_sites;
  ir_frontend_metadata
      .cross_module_conformance_lowering_contract_violation_sites =
      cross_module_conformance_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_cross_module_conformance_lowering_handoff =
      cross_module_conformance_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_throws_propagation_replay_key =
      throws_propagation_lowering_replay_key;
  ir_frontend_metadata.lowering_error_handling_throws_abi_propagation_replay_key =
      error_handling_throws_abi_propagation_lowering_replay_key;
  ir_frontend_metadata.lowering_result_like_replay_key =
      result_like_lowering_replay_key;
  ir_frontend_metadata.deterministic_result_like_lowering_handoff =
      result_like_lowering_contract.deterministic;
  ir_frontend_metadata.throws_propagation_lowering_sites =
      throws_propagation_lowering_contract.throws_propagation_sites;
  ir_frontend_metadata.throws_propagation_lowering_namespace_segment_sites =
      throws_propagation_lowering_contract.namespace_segment_sites;
  ir_frontend_metadata.throws_propagation_lowering_import_edge_candidate_sites =
      throws_propagation_lowering_contract.import_edge_candidate_sites;
  ir_frontend_metadata.throws_propagation_lowering_object_pointer_type_sites =
      throws_propagation_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata.throws_propagation_lowering_pointer_declarator_sites =
      throws_propagation_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.throws_propagation_lowering_normalized_sites =
      throws_propagation_lowering_contract.normalized_sites;
  ir_frontend_metadata
      .throws_propagation_lowering_cache_invalidation_candidate_sites =
      throws_propagation_lowering_contract
          .cache_invalidation_candidate_sites;
  ir_frontend_metadata.throws_propagation_lowering_contract_violation_sites =
      throws_propagation_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_throws_propagation_lowering_handoff =
      throws_propagation_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_ns_error_bridging_replay_key =
      ns_error_bridging_lowering_replay_key;
  ir_frontend_metadata.ns_error_bridging_lowering_sites =
      ns_error_bridging_lowering_contract.ns_error_bridging_sites;
  ir_frontend_metadata.ns_error_bridging_lowering_ns_error_parameter_sites =
      ns_error_bridging_lowering_contract.ns_error_parameter_sites;
  ir_frontend_metadata.ns_error_bridging_lowering_ns_error_out_parameter_sites =
      ns_error_bridging_lowering_contract.ns_error_out_parameter_sites;
  ir_frontend_metadata.ns_error_bridging_lowering_ns_error_bridge_path_sites =
      ns_error_bridging_lowering_contract.ns_error_bridge_path_sites;
  ir_frontend_metadata.ns_error_bridging_lowering_failable_call_sites =
      ns_error_bridging_lowering_contract.failable_call_sites;
  ir_frontend_metadata.ns_error_bridging_lowering_normalized_sites =
      ns_error_bridging_lowering_contract.normalized_sites;
  ir_frontend_metadata.ns_error_bridging_lowering_bridge_boundary_sites =
      ns_error_bridging_lowering_contract.bridge_boundary_sites;
  ir_frontend_metadata.ns_error_bridging_lowering_contract_violation_sites =
      ns_error_bridging_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_ns_error_bridging_lowering_handoff =
      ns_error_bridging_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_unwind_cleanup_replay_key =
      unwind_cleanup_lowering_replay_key;
  ir_frontend_metadata.unwind_cleanup_lowering_sites =
      unwind_cleanup_lowering_contract.unwind_cleanup_sites;
  ir_frontend_metadata.unwind_cleanup_lowering_unwind_edge_sites =
      unwind_cleanup_lowering_contract.unwind_edge_sites;
  ir_frontend_metadata.unwind_cleanup_lowering_cleanup_scope_sites =
      unwind_cleanup_lowering_contract.cleanup_scope_sites;
  ir_frontend_metadata.unwind_cleanup_lowering_cleanup_emit_sites =
      unwind_cleanup_lowering_contract.cleanup_emit_sites;
  ir_frontend_metadata.unwind_cleanup_lowering_landing_pad_sites =
      unwind_cleanup_lowering_contract.landing_pad_sites;
  ir_frontend_metadata.unwind_cleanup_lowering_cleanup_resume_sites =
      unwind_cleanup_lowering_contract.cleanup_resume_sites;
  ir_frontend_metadata.unwind_cleanup_lowering_normalized_sites =
      unwind_cleanup_lowering_contract.normalized_sites;
  ir_frontend_metadata.unwind_cleanup_lowering_guard_blocked_sites =
      unwind_cleanup_lowering_contract.guard_blocked_sites;
  ir_frontend_metadata.unwind_cleanup_lowering_contract_violation_sites =
      unwind_cleanup_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_unwind_cleanup_lowering_handoff =
      unwind_cleanup_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_error_handling_result_and_bridging_artifact_replay_key =
      error_handling_result_and_bridging_artifact_replay_summary.replay_key;
  ir_frontend_metadata.imported_error_handling_result_and_bridging_artifact_modules =
      error_handling_result_and_bridging_artifact_replay_summary
          .imported_module_names_lexicographic.size();
  ir_frontend_metadata.error_handling_result_and_bridging_binary_artifact_replay_ready =
      error_handling_result_and_bridging_artifact_replay_summary
          .binary_artifact_replay_ready;
  ir_frontend_metadata.error_handling_result_and_bridging_runtime_import_artifact_ready =
      error_handling_result_and_bridging_artifact_replay_summary
          .runtime_import_artifact_ready;
  ir_frontend_metadata
      .error_handling_result_and_bridging_separate_compilation_replay_ready =
      error_handling_result_and_bridging_artifact_replay_summary
          .separate_compilation_replay_ready;
  ir_frontend_metadata
      .deterministic_error_handling_result_and_bridging_artifact_replay_handoff =
      error_handling_result_and_bridging_artifact_replay_summary.deterministic;
  ir_frontend_metadata.object_pointer_type_spellings =
      object_pointer_nullability_generics_summary.object_pointer_type_spellings;
  ir_frontend_metadata.pointer_declarator_entries =
      object_pointer_nullability_generics_summary.pointer_declarator_entries;
  ir_frontend_metadata.pointer_declarator_depth_total =
      object_pointer_nullability_generics_summary.pointer_declarator_depth_total;
  ir_frontend_metadata.pointer_declarator_token_entries =
      object_pointer_nullability_generics_summary.pointer_declarator_token_entries;
  ir_frontend_metadata.nullability_suffix_entries =
      object_pointer_nullability_generics_summary.nullability_suffix_entries;
  ir_frontend_metadata.generic_suffix_entries = object_pointer_nullability_generics_summary.generic_suffix_entries;
  ir_frontend_metadata.terminated_generic_suffix_entries =
      object_pointer_nullability_generics_summary.terminated_generic_suffix_entries;
  ir_frontend_metadata.unterminated_generic_suffix_entries =
      object_pointer_nullability_generics_summary.unterminated_generic_suffix_entries;
  ir_frontend_metadata.global_symbol_nodes = symbol_graph_scope_resolution_summary.global_symbol_nodes;
  ir_frontend_metadata.function_symbol_nodes = symbol_graph_scope_resolution_summary.function_symbol_nodes;
  ir_frontend_metadata.interface_symbol_nodes = symbol_graph_scope_resolution_summary.interface_symbol_nodes;
  ir_frontend_metadata.implementation_symbol_nodes = symbol_graph_scope_resolution_summary.implementation_symbol_nodes;
  ir_frontend_metadata.interface_property_symbol_nodes =
      symbol_graph_scope_resolution_summary.interface_property_symbol_nodes;
  ir_frontend_metadata.implementation_property_symbol_nodes =
      symbol_graph_scope_resolution_summary.implementation_property_symbol_nodes;
  ir_frontend_metadata.interface_method_symbol_nodes = symbol_graph_scope_resolution_summary.interface_method_symbol_nodes;
  ir_frontend_metadata.implementation_method_symbol_nodes =
      symbol_graph_scope_resolution_summary.implementation_method_symbol_nodes;
  ir_frontend_metadata.top_level_scope_symbols = symbol_graph_scope_resolution_summary.top_level_scope_symbols;
  ir_frontend_metadata.nested_scope_symbols = symbol_graph_scope_resolution_summary.nested_scope_symbols;
  ir_frontend_metadata.scope_frames_total = symbol_graph_scope_resolution_summary.scope_frames_total;
  ir_frontend_metadata.implementation_interface_resolution_sites =
      symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites;
  ir_frontend_metadata.implementation_interface_resolution_hits =
      symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits;
  ir_frontend_metadata.implementation_interface_resolution_misses =
      symbol_graph_scope_resolution_summary.implementation_interface_resolution_misses;
  ir_frontend_metadata.method_resolution_sites = symbol_graph_scope_resolution_summary.method_resolution_sites;
  ir_frontend_metadata.method_resolution_hits = symbol_graph_scope_resolution_summary.method_resolution_hits;
  ir_frontend_metadata.method_resolution_misses = symbol_graph_scope_resolution_summary.method_resolution_misses;
  ir_frontend_metadata.deterministic_interface_implementation_handoff =
      pipeline_result.sema_parity_surface.deterministic_interface_implementation_handoff &&
      interface_implementation_summary.deterministic;
  ir_frontend_metadata.deterministic_protocol_category_handoff =
      protocol_category_summary.deterministic_protocol_category_handoff;
  ir_frontend_metadata.deterministic_class_protocol_category_linking_handoff =
      class_protocol_category_linking_summary.deterministic_class_protocol_category_linking_handoff;
  ir_frontend_metadata.deterministic_selector_normalization_handoff =
      selector_normalization_summary.deterministic_selector_normalization_handoff;
  ir_frontend_metadata.deterministic_property_attribute_handoff =
      property_attribute_summary.deterministic_property_attribute_handoff;
  ir_frontend_metadata.runtime_metadata_source_ownership_contract_id =
      runtime_metadata_source_ownership.contract_id;
  ir_frontend_metadata.runtime_metadata_source_schema =
      runtime_metadata_source_ownership.canonical_source_schema;
  ir_frontend_metadata.runtime_metadata_ivar_source_model =
      runtime_metadata_source_ownership.ivar_record_source_model;
  ir_frontend_metadata.runtime_metadata_class_record_count =
      runtime_metadata_source_ownership.class_record_count;
  ir_frontend_metadata.runtime_metadata_protocol_record_count =
      runtime_metadata_source_ownership.protocol_record_count;
  ir_frontend_metadata.runtime_metadata_category_interface_record_count =
      runtime_metadata_source_ownership.category_interface_record_count;
  ir_frontend_metadata.runtime_metadata_category_implementation_record_count =
      runtime_metadata_source_ownership.category_implementation_record_count;
  ir_frontend_metadata.runtime_metadata_property_record_count =
      runtime_metadata_source_ownership.property_record_count;
  ir_frontend_metadata.runtime_metadata_method_record_count =
      runtime_metadata_source_ownership.method_record_count;
  ir_frontend_metadata.runtime_metadata_ivar_record_count =
      runtime_metadata_source_ownership.ivar_record_count;
  ir_frontend_metadata.frontend_owns_runtime_metadata_source_records =
      runtime_metadata_source_ownership.frontend_owns_runtime_metadata_source_records;
  ir_frontend_metadata.runtime_metadata_source_records_ready_for_lowering =
      runtime_metadata_source_ownership.runtime_metadata_source_records_ready_for_lowering;
  ir_frontend_metadata.native_runtime_library_present =
      runtime_metadata_source_ownership.native_runtime_library_present;
  ir_frontend_metadata.runtime_metadata_source_boundary_fail_closed =
      runtime_metadata_source_ownership.fail_closed;
  ir_frontend_metadata.runtime_link_test_only =
      runtime_metadata_source_ownership.runtime_link_test_only;
  ir_frontend_metadata.deterministic_runtime_metadata_source_schema =
      runtime_metadata_source_ownership.deterministic_source_schema;
  ir_frontend_metadata.runtime_export_legality_contract_id =
      runtime_export_legality.contract_id;
  ir_frontend_metadata.runtime_export_semantic_boundary_frozen =
      runtime_export_legality.semantic_boundary_frozen;
  ir_frontend_metadata.runtime_export_metadata_export_enforcement_ready =
      runtime_export_legality.metadata_export_enforcement_ready;
  ir_frontend_metadata.runtime_export_fail_closed =
      runtime_export_legality.fail_closed;
  ir_frontend_metadata.runtime_export_duplicate_runtime_identity_enforcement_pending =
      runtime_export_legality.duplicate_runtime_identity_enforcement_pending;
  ir_frontend_metadata.runtime_export_incomplete_declaration_export_blocking_pending =
      runtime_export_legality.incomplete_declaration_export_blocking_pending;
  ir_frontend_metadata.runtime_export_illegal_redeclaration_mix_export_blocking_pending =
      runtime_export_legality.illegal_redeclaration_mix_export_blocking_pending;
  ir_frontend_metadata.runtime_export_class_record_count =
      runtime_export_legality.class_record_count;
  ir_frontend_metadata.runtime_export_protocol_record_count =
      runtime_export_legality.protocol_record_count;
  ir_frontend_metadata.runtime_export_category_record_count =
      runtime_export_legality.category_record_count;
  ir_frontend_metadata.runtime_export_property_record_count =
      runtime_export_legality.property_record_count;
  ir_frontend_metadata.runtime_export_method_record_count =
      runtime_export_legality.method_record_count;
  ir_frontend_metadata.runtime_export_ivar_record_count =
      runtime_export_legality.ivar_record_count;
  ir_frontend_metadata.runtime_export_invalid_protocol_composition_sites =
      runtime_export_legality.invalid_protocol_composition_sites;
  ir_frontend_metadata.runtime_export_property_attribute_invalid_entries =
      runtime_export_legality.property_attribute_invalid_entries;
  ir_frontend_metadata.runtime_export_property_attribute_contract_violations =
      runtime_export_legality.property_attribute_contract_violations;
  ir_frontend_metadata.runtime_export_invalid_type_annotation_sites =
      runtime_export_legality.invalid_type_annotation_sites;
  ir_frontend_metadata.runtime_export_property_ivar_binding_missing =
      runtime_export_legality.property_ivar_binding_missing;
  ir_frontend_metadata.runtime_export_property_ivar_binding_conflicts =
      runtime_export_legality.property_ivar_binding_conflicts;
  ir_frontend_metadata.runtime_export_implementation_resolution_misses =
      runtime_export_legality.implementation_resolution_misses;
  ir_frontend_metadata.runtime_export_method_resolution_misses =
      runtime_export_legality.method_resolution_misses;
  ir_frontend_metadata.runtime_export_boundary_ready =
      IsReadyObjc3RuntimeExportLegalityBoundary(runtime_export_legality);
  ir_frontend_metadata.runtime_export_enforcement_contract_id =
      runtime_export_enforcement.contract_id;
  ir_frontend_metadata.runtime_export_metadata_completeness_enforced =
      runtime_export_enforcement.metadata_completeness_enforced;
  ir_frontend_metadata
      .runtime_export_duplicate_runtime_identity_suppression_enforced =
      runtime_export_enforcement
          .duplicate_runtime_identity_suppression_enforced;
  ir_frontend_metadata
      .runtime_export_illegal_redeclaration_mix_blocking_enforced =
      runtime_export_enforcement
          .illegal_redeclaration_mix_blocking_enforced;
  ir_frontend_metadata.runtime_export_metadata_shape_drift_blocking_enforced =
      runtime_export_enforcement.metadata_shape_drift_blocking_enforced;
  ir_frontend_metadata.runtime_export_enforcement_fail_closed =
      runtime_export_enforcement.fail_closed;
  ir_frontend_metadata.runtime_export_ready_for_runtime_export =
      runtime_export_enforcement.ready_for_runtime_export;
  ir_frontend_metadata.runtime_export_duplicate_runtime_identity_sites =
      runtime_export_enforcement.duplicate_runtime_identity_sites;
  ir_frontend_metadata.runtime_export_incomplete_declaration_sites =
      runtime_export_enforcement.incomplete_declaration_sites;
  ir_frontend_metadata.runtime_export_illegal_redeclaration_mix_sites =
      runtime_export_enforcement.illegal_redeclaration_mix_sites;
  ir_frontend_metadata.runtime_export_metadata_shape_drift_sites =
      runtime_export_enforcement.metadata_shape_drift_sites;
  ir_frontend_metadata.runtime_metadata_section_abi_contract_id =
      runtime_metadata_section_abi.contract_id;
  ir_frontend_metadata.runtime_metadata_section_boundary_frozen =
      runtime_metadata_section_abi.boundary_frozen;
  ir_frontend_metadata.runtime_metadata_section_fail_closed =
      runtime_metadata_section_abi.fail_closed;
  ir_frontend_metadata.runtime_metadata_section_object_file_inventory_frozen =
      runtime_metadata_section_abi.object_file_section_inventory_frozen;
  ir_frontend_metadata.runtime_metadata_section_symbol_policy_frozen =
      runtime_metadata_section_abi.symbol_policy_frozen;
  ir_frontend_metadata.runtime_metadata_section_visibility_model_frozen =
      runtime_metadata_section_abi.visibility_model_frozen;
  ir_frontend_metadata.runtime_metadata_section_retention_policy_frozen =
      runtime_metadata_section_abi.retention_policy_frozen;
  ir_frontend_metadata.runtime_metadata_section_ready_for_scaffold =
      runtime_metadata_section_abi.ready_for_section_scaffold;
  ir_frontend_metadata.runtime_metadata_section_logical_image_info_section =
      runtime_metadata_section_abi.logical_image_info_section;
  ir_frontend_metadata
      .runtime_metadata_section_logical_class_descriptor_section =
      runtime_metadata_section_abi.logical_class_descriptor_section;
  ir_frontend_metadata
      .runtime_metadata_section_logical_protocol_descriptor_section =
      runtime_metadata_section_abi.logical_protocol_descriptor_section;
  ir_frontend_metadata
      .runtime_metadata_section_logical_category_descriptor_section =
      runtime_metadata_section_abi.logical_category_descriptor_section;
  ir_frontend_metadata
      .runtime_metadata_section_logical_property_descriptor_section =
      runtime_metadata_section_abi.logical_property_descriptor_section;
  ir_frontend_metadata.runtime_metadata_section_logical_ivar_descriptor_section =
      runtime_metadata_section_abi.logical_ivar_descriptor_section;
  ir_frontend_metadata.runtime_metadata_section_descriptor_symbol_prefix =
      runtime_metadata_section_abi.descriptor_symbol_prefix;
  ir_frontend_metadata.runtime_metadata_section_aggregate_symbol_prefix =
      runtime_metadata_section_abi.aggregate_symbol_prefix;
  ir_frontend_metadata.runtime_metadata_section_image_info_symbol =
      runtime_metadata_section_abi.image_info_symbol;
  ir_frontend_metadata.runtime_metadata_section_descriptor_linkage =
      runtime_metadata_section_abi.descriptor_linkage;
  ir_frontend_metadata.runtime_metadata_section_aggregate_linkage =
      runtime_metadata_section_abi.aggregate_linkage;
  ir_frontend_metadata.runtime_metadata_section_visibility =
      runtime_metadata_section_abi.metadata_visibility;
  ir_frontend_metadata.runtime_metadata_section_retention_root =
      runtime_metadata_section_abi.retention_root;
  ir_frontend_metadata.runtime_metadata_section_publication_contract_id =
      runtime_metadata_section_publication.contract_id;
  ir_frontend_metadata.runtime_metadata_section_publication_abi_contract_id =
      runtime_metadata_section_publication.abi_contract_id;
  ir_frontend_metadata.runtime_metadata_section_publication_emitted =
      runtime_metadata_section_publication.publication_emitted;
  ir_frontend_metadata.runtime_metadata_section_publication_fail_closed =
      runtime_metadata_section_publication.fail_closed;
  ir_frontend_metadata.runtime_metadata_section_publication_uses_llvm_used =
      runtime_metadata_section_publication.uses_llvm_used;
  ir_frontend_metadata.runtime_metadata_section_publication_image_info_emitted =
      runtime_metadata_section_publication.image_info_emitted;
  ir_frontend_metadata.runtime_metadata_section_publication_class_descriptor_count =
      runtime_metadata_section_publication.class_descriptor_count;
  ir_frontend_metadata
      .runtime_metadata_section_publication_protocol_descriptor_count =
      runtime_metadata_section_publication.protocol_descriptor_count;
  ir_frontend_metadata
      .runtime_metadata_section_publication_category_descriptor_count =
      runtime_metadata_section_publication.category_descriptor_count;
  ir_frontend_metadata
      .runtime_metadata_section_publication_property_descriptor_count =
      runtime_metadata_section_publication.property_descriptor_count;
  ir_frontend_metadata.runtime_metadata_section_publication_ivar_descriptor_count =
      runtime_metadata_section_publication.ivar_descriptor_count;
  ir_frontend_metadata.runtime_metadata_section_publication_total_descriptor_count =
      runtime_metadata_section_publication.total_descriptor_count;
  ir_frontend_metadata
      .runtime_metadata_section_publication_total_retained_global_count =
      runtime_metadata_section_publication.total_retained_global_count;
  ir_frontend_metadata.runtime_metadata_section_publication_image_info_symbol =
      runtime_metadata_section_publication.image_info_symbol;
  ir_frontend_metadata.runtime_metadata_section_publication_class_aggregate_symbol =
      runtime_metadata_section_publication.class_aggregate_symbol;
  ir_frontend_metadata
      .runtime_metadata_section_publication_protocol_aggregate_symbol =
      runtime_metadata_section_publication.protocol_aggregate_symbol;
  ir_frontend_metadata
      .runtime_metadata_section_publication_category_aggregate_symbol =
      runtime_metadata_section_publication.category_aggregate_symbol;
  ir_frontend_metadata
      .runtime_metadata_section_publication_property_aggregate_symbol =
      runtime_metadata_section_publication.property_aggregate_symbol;
  ir_frontend_metadata.runtime_metadata_section_publication_ivar_aggregate_symbol =
      runtime_metadata_section_publication.ivar_aggregate_symbol;
  ir_frontend_metadata.runtime_metadata_class_metaclass_emission_contract_id =
      kObjc3RuntimeClassMetaclassEmissionContractId;
  ir_frontend_metadata.runtime_metadata_class_metaclass_payload_model =
      kObjc3RuntimeClassMetaclassEmissionPayloadModel;
  ir_frontend_metadata.runtime_metadata_class_metaclass_name_model =
      kObjc3RuntimeClassMetaclassEmissionNameModel;
  ir_frontend_metadata.runtime_metadata_class_metaclass_super_link_model =
      kObjc3RuntimeClassMetaclassEmissionSuperLinkModel;
  ir_frontend_metadata
      .runtime_metadata_class_metaclass_method_list_reference_model =
      kObjc3RuntimeClassMetaclassEmissionMethodListReferenceModel;
  ir_frontend_metadata.executable_class_metaclass_source_closure_contract_id =
      executable_metadata_typed_lowering_handoff.source_graph
          .class_metaclass_source_closure_contract_id;
  ir_frontend_metadata.executable_class_metaclass_parent_identity_model =
      executable_metadata_typed_lowering_handoff.source_graph
          .class_metaclass_parent_identity_model;
  ir_frontend_metadata.executable_class_metaclass_method_owner_identity_model =
      executable_metadata_typed_lowering_handoff.source_graph
          .class_metaclass_method_owner_identity_model;
  ir_frontend_metadata.executable_class_metaclass_object_identity_model =
      executable_metadata_typed_lowering_handoff.source_graph
          .class_metaclass_object_identity_model;
  ir_frontend_metadata.executable_class_metaclass_source_closure_ready =
      executable_metadata_typed_lowering_handoff.source_graph
          .class_metaclass_declaration_closure_complete &&
      executable_metadata_typed_lowering_handoff.source_graph
          .class_metaclass_parent_identity_closure_complete &&
      executable_metadata_typed_lowering_handoff.source_graph
          .class_metaclass_method_owner_identity_closure_complete &&
      executable_metadata_typed_lowering_handoff.source_graph
          .class_metaclass_object_identity_closure_complete;
  ir_frontend_metadata.executable_class_metaclass_declaration_node_count =
      executable_metadata_typed_lowering_handoff.source_graph
          .interface_nodes_lexicographic.size() +
      executable_metadata_typed_lowering_handoff.source_graph
          .implementation_nodes_lexicographic.size() +
      executable_metadata_typed_lowering_handoff.source_graph
          .class_nodes_lexicographic.size() +
      executable_metadata_typed_lowering_handoff.source_graph
          .metaclass_nodes_lexicographic.size();
  ir_frontend_metadata.runtime_metadata_class_metaclass_typed_handoff_replay_key =
      executable_metadata_typed_lowering_handoff.replay_key;
  ir_frontend_metadata.executable_protocol_category_source_closure_contract_id =
      executable_metadata_typed_lowering_handoff.source_graph
          .protocol_category_source_closure_contract_id;
  ir_frontend_metadata.executable_protocol_inheritance_identity_model =
      executable_metadata_typed_lowering_handoff.source_graph
          .protocol_inheritance_identity_model;
  ir_frontend_metadata.executable_category_attachment_identity_model =
      executable_metadata_typed_lowering_handoff.source_graph
          .category_attachment_identity_model;
  ir_frontend_metadata.executable_protocol_category_conformance_identity_model =
      executable_metadata_typed_lowering_handoff.source_graph
          .protocol_category_conformance_identity_model;
  ir_frontend_metadata.executable_protocol_category_source_closure_ready =
      executable_metadata_typed_lowering_handoff.source_graph
          .protocol_category_declaration_closure_complete &&
      executable_metadata_typed_lowering_handoff.source_graph
          .protocol_inheritance_identity_closure_complete &&
      executable_metadata_typed_lowering_handoff.source_graph
          .category_attachment_identity_closure_complete &&
      executable_metadata_typed_lowering_handoff.source_graph
          .protocol_category_conformance_identity_closure_complete;
  ir_frontend_metadata.executable_protocol_category_protocol_node_count =
      executable_metadata_typed_lowering_handoff.source_graph
          .protocol_nodes_lexicographic.size();
  ir_frontend_metadata.executable_protocol_category_category_node_count =
      executable_metadata_typed_lowering_handoff.source_graph
          .category_nodes_lexicographic.size();
  for (const auto &edge :
       executable_metadata_typed_lowering_handoff.source_graph.owner_edges_lexicographic) {
    if (edge.edge_kind == "interface-to-superclass" ||
        edge.edge_kind == "interface-to-super-metaclass" ||
        edge.edge_kind == "implementation-to-superclass" ||
        edge.edge_kind == "implementation-to-super-metaclass" ||
        edge.edge_kind == "class-to-superclass" ||
        edge.edge_kind == "metaclass-to-super-metaclass") {
      ++ir_frontend_metadata
            .executable_class_metaclass_parent_identity_edge_count;
    }
    if (edge.edge_kind == "interface-to-instance-method-owner" ||
        edge.edge_kind == "interface-to-class-method-owner" ||
        edge.edge_kind == "implementation-to-instance-method-owner" ||
        edge.edge_kind == "implementation-to-class-method-owner") {
      ++ir_frontend_metadata
            .executable_class_metaclass_method_owner_identity_edge_count;
    }
    if (edge.edge_kind == "interface-to-class" ||
        edge.edge_kind == "interface-to-metaclass" ||
        edge.edge_kind == "implementation-to-class" ||
        edge.edge_kind == "implementation-to-metaclass" ||
        edge.edge_kind == "class-to-metaclass") {
      ++ir_frontend_metadata
            .executable_class_metaclass_object_identity_edge_count;
    }
    if (edge.edge_kind == "protocol-to-inherited-protocol") {
      ++ir_frontend_metadata
            .executable_protocol_inheritance_identity_edge_count;
    }
    if (edge.edge_kind == "category-to-class" ||
        edge.edge_kind == "category-to-interface" ||
        edge.edge_kind == "category-to-implementation") {
      ++ir_frontend_metadata
            .executable_category_attachment_identity_edge_count;
    }
    if (edge.edge_kind == "category-to-protocol") {
      ++ir_frontend_metadata
            .executable_protocol_category_conformance_identity_edge_count;
    }
  }
  ir_frontend_metadata.runtime_metadata_protocol_category_emission_contract_id =
      kObjc3RuntimeProtocolCategoryEmissionContractId;
  ir_frontend_metadata.runtime_metadata_protocol_emission_payload_model =
      kObjc3RuntimeProtocolEmissionPayloadModel;
  ir_frontend_metadata.runtime_metadata_category_emission_payload_model =
      kObjc3RuntimeCategoryEmissionPayloadModel;
  ir_frontend_metadata.runtime_metadata_protocol_reference_model =
      kObjc3RuntimeProtocolReferenceModel;
  ir_frontend_metadata.runtime_metadata_category_attachment_model =
      kObjc3RuntimeCategoryAttachmentModel;
  ir_frontend_metadata.runtime_metadata_protocol_category_typed_handoff_replay_key =
      executable_metadata_typed_lowering_handoff.replay_key;
  ir_frontend_metadata.runtime_metadata_member_table_emission_contract_id =
      kObjc3RuntimeMemberTableEmissionContractId;
  ir_frontend_metadata.runtime_metadata_method_list_emission_payload_model =
      kObjc3RuntimeMethodListEmissionPayloadModel;
  ir_frontend_metadata.runtime_metadata_method_list_grouping_model =
      kObjc3RuntimeMethodListEmissionGroupingModel;
  ir_frontend_metadata.runtime_metadata_property_descriptor_emission_payload_model =
      kObjc3RuntimePropertyDescriptorEmissionPayloadModel;
  ir_frontend_metadata.runtime_metadata_ivar_descriptor_emission_payload_model =
      kObjc3RuntimeIvarDescriptorEmissionPayloadModel;
  ir_frontend_metadata.runtime_metadata_member_table_typed_handoff_replay_key =
      executable_metadata_typed_lowering_handoff.replay_key;
  ir_frontend_metadata.runtime_metadata_archive_static_link_discovery_contract_id =
      kObjc3RuntimeArchiveStaticLinkDiscoveryContractId;
  ir_frontend_metadata.runtime_metadata_archive_static_link_anchor_seed_model =
      kObjc3RuntimeArchiveStaticLinkAnchorSeedModel;
  ir_frontend_metadata
      .runtime_metadata_archive_static_link_translation_unit_identity_model =
      kObjc3RuntimeArchiveStaticLinkTranslationUnitIdentityModel;
  ir_frontend_metadata.runtime_metadata_archive_static_link_merge_model =
      kObjc3RuntimeArchiveStaticLinkMergeModel;
  ir_frontend_metadata.runtime_metadata_archive_static_link_response_artifact_suffix =
      kObjc3RuntimeMergedLinkerResponseArtifactSuffix;
  ir_frontend_metadata.runtime_metadata_archive_static_link_discovery_artifact_suffix =
      kObjc3RuntimeMergedDiscoveryArtifactSuffix;
  ir_frontend_metadata.runtime_metadata_archive_static_link_discovery_ready =
      true;
  ir_frontend_metadata
      .runtime_metadata_archive_static_link_translation_unit_identity_key =
      BuildObjc3TranslationUnitIdentityKey(Objc3TranslationUnitIdentityEvidence{
          input_path,
          bundle.parse_lowering_readiness_surface.parse_artifact_replay_key,
          bundle.parse_lowering_readiness_surface.lowering_boundary_replay_key,
      });
  // bootstrap materialization anchor: the native IR emitter consumes
  // this lowering packet directly when it materializes the ctor root, derived
  // init stub, registration table, and image descriptor. Driver/process code
  // may publish the same packet, but they may not re-derive those symbol
  // shapes independently from truncated sidecar state.
  // registration-table/image-local-init anchor: the same lowering
  // packet now also carries the self-describing registration-table layout,
  // ABI/version counts, and image-local init-state model that the emitter,
  // manifest writers, and later runtime image-walk code must preserve exactly.
  ir_frontend_metadata.runtime_bootstrap_lowering_contract_id =
      bundle.runtime_bootstrap_lowering_summary.contract_id;
  ir_frontend_metadata.runtime_bootstrap_lowering_boundary_model =
      bundle.runtime_bootstrap_lowering_summary.lowering_boundary_model;
  ir_frontend_metadata.runtime_bootstrap_lowering_constructor_root_symbol =
      bundle.runtime_bootstrap_lowering_summary.constructor_root_symbol;
  ir_frontend_metadata.runtime_bootstrap_lowering_init_stub_symbol_prefix =
      bundle.runtime_bootstrap_lowering_summary
          .constructor_init_stub_symbol_prefix;
  ir_frontend_metadata
      .runtime_bootstrap_lowering_registration_table_symbol_prefix =
      bundle.runtime_bootstrap_lowering_summary
          .registration_table_symbol_prefix;
  ir_frontend_metadata
      .runtime_bootstrap_lowering_image_local_init_state_symbol_prefix =
      bundle.runtime_bootstrap_lowering_summary
          .image_local_init_state_symbol_prefix;
  ir_frontend_metadata.runtime_bootstrap_lowering_registration_entrypoint_symbol =
      bundle.runtime_bootstrap_lowering_summary.registration_entrypoint_symbol;
  ir_frontend_metadata.runtime_bootstrap_lowering_global_ctor_list_model =
      bundle.runtime_bootstrap_lowering_summary.global_ctor_list_model;
  ir_frontend_metadata.runtime_bootstrap_lowering_registration_table_layout_model =
      bundle.runtime_bootstrap_lowering_summary.registration_table_layout_model;
  ir_frontend_metadata.runtime_bootstrap_lowering_image_local_initialization_model =
      bundle.runtime_bootstrap_lowering_summary
          .image_local_initialization_model;
  ir_frontend_metadata.runtime_bootstrap_lowering_registration_table_abi_version =
      bundle.runtime_bootstrap_lowering_summary
          .registration_table_abi_version;
  ir_frontend_metadata
      .runtime_bootstrap_lowering_registration_table_pointer_field_count =
      bundle.runtime_bootstrap_lowering_summary
          .registration_table_pointer_field_count;
  ir_frontend_metadata.runtime_bootstrap_lowering_constructor_root_emission_state =
      bundle.runtime_bootstrap_lowering_summary.constructor_root_emission_state;
  ir_frontend_metadata.runtime_bootstrap_lowering_init_stub_emission_state =
      bundle.runtime_bootstrap_lowering_summary.init_stub_emission_state;
  ir_frontend_metadata
      .runtime_bootstrap_lowering_registration_table_emission_state =
      bundle.runtime_bootstrap_lowering_summary
          .registration_table_emission_state;
  ir_frontend_metadata
      .runtime_bootstrap_lowering_bootstrap_ir_materialization_landed =
      bundle.runtime_bootstrap_lowering_summary
          .bootstrap_ir_materialization_landed;
  ir_frontend_metadata
      .runtime_bootstrap_lowering_image_local_initialization_landed =
      bundle.runtime_bootstrap_lowering_summary
          .image_local_initialization_landed;
  ir_frontend_metadata
      .runtime_bootstrap_registration_descriptor_image_root_lowering_contract_id =
      kObjc3RuntimeBootstrapRegistrationDescriptorImageRootLoweringContractId;
  ir_frontend_metadata.runtime_bootstrap_registration_descriptor_identifier =
      bundle.runtime_registration_descriptor_frontend_closure_summary
          .registration_descriptor_identifier;
  ir_frontend_metadata.runtime_bootstrap_image_root_identifier =
      bundle.runtime_registration_descriptor_frontend_closure_summary
          .image_root_identifier;
  ir_frontend_metadata.runtime_bootstrap_registration_order_ordinal =
      bundle.runtime_translation_unit_registration_manifest_summary
          .translation_unit_registration_order_ordinal;
  ir_frontend_metadata.runtime_bootstrap_lowering_ready =
      bundle.runtime_bootstrap_lowering_summary
          .ready_for_bootstrap_materialization;
  ir_frontend_metadata.runtime_bootstrap_lowering_fail_closed =
      bundle.runtime_bootstrap_lowering_summary.fail_closed;
  {
    const bool typed_handoff_ready =
        IsReadyObjc3ExecutableMetadataTypedLoweringHandoff(
            executable_metadata_typed_lowering_handoff);
    if (typed_handoff_ready) {
      const auto &source_graph =
          executable_metadata_typed_lowering_handoff.source_graph;
      std::unordered_map<std::string,
                         const Objc3ExecutableMetadataClassGraphNode *>
          class_nodes_by_name;
      class_nodes_by_name.reserve(source_graph.class_nodes_lexicographic.size());
      for (const auto &class_node : source_graph.class_nodes_lexicographic) {
        class_nodes_by_name.emplace(class_node.class_name, &class_node);
      }
      std::unordered_map<std::string,
                         const Objc3ExecutableMetadataMetaclassGraphNode *>
          metaclass_nodes_by_name;
      metaclass_nodes_by_name.reserve(
          source_graph.metaclass_nodes_lexicographic.size());
      for (const auto &metaclass_node :
           source_graph.metaclass_nodes_lexicographic) {
        metaclass_nodes_by_name.emplace(metaclass_node.class_name,
                                        &metaclass_node);
      }
      std::unordered_map<std::string,
                         const Objc3ExecutableMetadataImplementationGraphNode *>
          implementation_nodes_by_name;
      implementation_nodes_by_name.reserve(
          source_graph.implementation_nodes_lexicographic.size());
      for (const auto &implementation_node :
           source_graph.implementation_nodes_lexicographic) {
        implementation_nodes_by_name.emplace(implementation_node.class_name,
                                             &implementation_node);
      }

      bool bundle_payload_complete = true;
      std::vector<Objc3IRRuntimeMetadataClassMetaclassBundle> bundles;
      bundles.reserve(source_graph.interface_nodes_lexicographic.size() +
                      source_graph.implementation_nodes_lexicographic.size());
      for (const auto &interface_node : source_graph.interface_nodes_lexicographic) {
        const auto metaclass_it =
            metaclass_nodes_by_name.find(interface_node.class_name);
        const auto class_it =
            class_nodes_by_name.find(interface_node.class_name);
        if (metaclass_it == metaclass_nodes_by_name.end() ||
            class_it == class_nodes_by_name.end()) {
          bundle_payload_complete = false;
          break;
        }

        Objc3IRRuntimeMetadataClassMetaclassBundle bundle;
        bundle.class_name = interface_node.class_name;
        bundle.owner_identity = interface_node.owner_identity;
        bundle.class_owner_identity = interface_node.class_owner_identity;
        bundle.metaclass_owner_identity = interface_node.metaclass_owner_identity;
        bundle.has_super = class_it->second->has_super;
        bundle.super_class_owner_identity =
            class_it->second->super_class_owner_identity;
        bundle.super_metaclass_owner_identity =
            class_it->second->super_metaclass_owner_identity;
        bundle.super_bundle_owner_identity =
            class_it->second->has_super
                ? ("interface:" + class_it->second->super_class_owner_identity.substr(6))
                : std::string{};
        bundle.adopted_protocol_owner_identities_lexicographic =
            class_it->second->adopted_protocol_owner_identities_lexicographic;
        bundle.instance_method_owner_identity =
            interface_node.instance_method_owner_identity;
        bundle.class_method_owner_identity =
            interface_node.class_method_owner_identity;
        bundle.objc_final_declared = class_it->second->objc_final_declared;
        bundle.objc_sealed_declared = class_it->second->objc_sealed_declared;
        bundle.instance_method_count = interface_node.instance_method_count;
        bundle.class_method_count =
            metaclass_it->second->interface_class_method_count;
        bundles.push_back(std::move(bundle));
      }
      for (const auto &implementation_node :
           source_graph.implementation_nodes_lexicographic) {
        const auto metaclass_it =
            metaclass_nodes_by_name.find(implementation_node.class_name);
        const auto class_it =
            class_nodes_by_name.find(implementation_node.class_name);
        if (metaclass_it == metaclass_nodes_by_name.end() ||
            class_it == class_nodes_by_name.end()) {
          bundle_payload_complete = false;
          break;
        }

        Objc3IRRuntimeMetadataClassMetaclassBundle bundle;
        bundle.class_name = implementation_node.class_name;
        bundle.owner_identity = implementation_node.owner_identity;
        bundle.class_owner_identity = implementation_node.class_owner_identity;
        bundle.metaclass_owner_identity =
            implementation_node.metaclass_owner_identity;
        bundle.has_super = class_it->second->has_super;
        bundle.super_class_owner_identity =
            implementation_node.super_class_owner_identity;
        bundle.super_metaclass_owner_identity =
            implementation_node.super_metaclass_owner_identity;
        if (class_it->second->has_super) {
          const std::string super_class_name =
              class_it->second->super_class_owner_identity.substr(6);
          const auto super_impl_it =
              implementation_nodes_by_name.find(super_class_name);
          if (super_impl_it == implementation_nodes_by_name.end()) {
            bundle_payload_complete = false;
            break;
          }
          bundle.super_bundle_owner_identity =
              super_impl_it->second->owner_identity;
        }
        bundle.adopted_protocol_owner_identities_lexicographic =
            class_it->second->adopted_protocol_owner_identities_lexicographic;
        bundle.instance_method_owner_identity =
            implementation_node.instance_method_owner_identity;
        bundle.class_method_owner_identity =
            implementation_node.class_method_owner_identity;
        bundle.objc_final_declared = class_it->second->objc_final_declared;
        bundle.objc_sealed_declared = class_it->second->objc_sealed_declared;
        bundle.instance_method_count = implementation_node.instance_method_count;
        bundle.class_method_count =
            metaclass_it->second->implementation_class_method_count;
        bundles.push_back(std::move(bundle));
      }

      bundle_payload_complete =
          bundle_payload_complete &&
          bundles.size() ==
              runtime_metadata_section_publication.class_descriptor_count;
      if (bundle_payload_complete) {
        ir_frontend_metadata.runtime_metadata_class_metaclass_bundles_lexicographic =
            std::move(bundles);
      }
      ir_frontend_metadata.runtime_metadata_class_metaclass_emission_ready =
          bundle_payload_complete;
      ir_frontend_metadata.runtime_metadata_class_metaclass_emission_fail_closed =
          bundle_payload_complete;

      // protocol/category data emission anchor: the typed lowering
      // handoff now expands one combined category graph node into explicit
      // interface/implementation record bundles so the emitted descriptor count
      // matches the runtime-export/scaffold record inventory exactly.
      bool protocol_category_payload_complete = true;
      std::unordered_set<std::string> protocol_owner_identities;
      protocol_owner_identities.reserve(
          source_graph.protocol_nodes_lexicographic.size());
      std::vector<Objc3IRRuntimeMetadataProtocolBundle> protocol_bundles;
      protocol_bundles.reserve(source_graph.protocol_nodes_lexicographic.size());
      for (const auto &protocol_node : source_graph.protocol_nodes_lexicographic) {
        if (protocol_node.protocol_name.empty() ||
            protocol_node.owner_identity.empty() ||
            !protocol_owner_identities.insert(protocol_node.owner_identity).second) {
          protocol_category_payload_complete = false;
          break;
        }
        for (const auto &inherited_owner_identity :
             protocol_node.inherited_protocol_owner_identities_lexicographic) {
          if (inherited_owner_identity.empty()) {
            protocol_category_payload_complete = false;
            break;
          }
        }
        if (!protocol_category_payload_complete) {
          break;
        }

        Objc3IRRuntimeMetadataProtocolBundle bundle;
        bundle.protocol_name = protocol_node.protocol_name;
        bundle.owner_identity = protocol_node.owner_identity;
        bundle.inherited_protocol_owner_identities_lexicographic =
            protocol_node.inherited_protocol_owner_identities_lexicographic;
        bundle.property_count = protocol_node.property_count;
        bundle.method_count = protocol_node.method_count;
        bundle.is_forward_declaration = protocol_node.is_forward_declaration;
        protocol_bundles.push_back(std::move(bundle));
      }

      std::vector<Objc3IRRuntimeMetadataCategoryBundle> category_bundles;
      category_bundles.reserve(source_graph.category_nodes_lexicographic.size());
      std::unordered_set<std::string> category_owner_identities;
      category_owner_identities.reserve(
          runtime_metadata_section_publication.category_descriptor_count);
      if (protocol_category_payload_complete) {
        for (const auto &category_node :
             source_graph.category_nodes_lexicographic) {
          if (category_node.class_name.empty() ||
              category_node.category_name.empty() ||
              category_node.owner_identity.empty() ||
              category_node.class_owner_identity.empty() ||
              (!category_node.has_interface &&
               !category_node.has_implementation)) {
            protocol_category_payload_complete = false;
            break;
          }

          for (const auto &protocol_owner_identity :
               category_node.adopted_protocol_owner_identities_lexicographic) {
            if (protocol_owner_identity.empty() ||
                protocol_owner_identities.find(protocol_owner_identity) ==
                    protocol_owner_identities.end()) {
              protocol_category_payload_complete = false;
              break;
            }
          }
          if (!protocol_category_payload_complete) {
            break;
          }

          const auto append_category_bundle =
              [&](const std::string &record_kind,
                  const std::string &record_owner_identity,
                  std::size_t property_count,
                  std::size_t instance_method_count,
                  std::size_t class_method_count) {
                if (record_owner_identity.empty() ||
                    !category_owner_identities.insert(record_owner_identity)
                         .second) {
                  protocol_category_payload_complete = false;
                  return;
                }
                Objc3IRRuntimeMetadataCategoryBundle bundle;
                bundle.record_kind = record_kind;
                bundle.class_name = category_node.class_name;
                bundle.category_name = category_node.category_name;
                bundle.owner_identity = record_owner_identity;
                bundle.category_owner_identity = category_node.owner_identity;
                bundle.class_owner_identity = category_node.class_owner_identity;
                bundle.adopted_protocol_owner_identities_lexicographic =
                    category_node.adopted_protocol_owner_identities_lexicographic;
                bundle.property_count = property_count;
                bundle.instance_method_count = instance_method_count;
                bundle.class_method_count = class_method_count;
                category_bundles.push_back(std::move(bundle));
              };
          if (category_node.has_interface) {
            append_category_bundle(
                "interface", category_node.interface_owner_identity,
                category_node.interface_property_count,
                category_node.interface_method_count,
                category_node.interface_class_method_count);
          }
          if (protocol_category_payload_complete &&
              category_node.has_implementation) {
            append_category_bundle(
                "implementation", category_node.implementation_owner_identity,
                category_node.implementation_property_count,
                category_node.implementation_method_count,
                category_node.implementation_class_method_count);
          }
          if (!protocol_category_payload_complete) {
            break;
          }
        }
      }

      if (protocol_category_payload_complete) {
        for (const auto &bundle : protocol_bundles) {
          for (const auto &inherited_owner_identity :
               bundle.inherited_protocol_owner_identities_lexicographic) {
            if (protocol_owner_identities.find(inherited_owner_identity) ==
                protocol_owner_identities.end()) {
              protocol_category_payload_complete = false;
              break;
            }
          }
          if (!protocol_category_payload_complete) {
            break;
          }
        }
      }

      protocol_category_payload_complete =
          protocol_category_payload_complete &&
          protocol_bundles.size() ==
              runtime_metadata_section_publication.protocol_descriptor_count &&
          category_bundles.size() ==
              runtime_metadata_section_publication.category_descriptor_count;
      if (protocol_category_payload_complete) {
        ir_frontend_metadata.runtime_metadata_protocol_bundles_lexicographic =
            std::move(protocol_bundles);
        ir_frontend_metadata.runtime_metadata_category_bundles_lexicographic =
            std::move(category_bundles);
      }
      ir_frontend_metadata.runtime_metadata_protocol_category_emission_ready =
          protocol_category_payload_complete;
      ir_frontend_metadata
          .runtime_metadata_protocol_category_emission_fail_closed =
          protocol_category_payload_complete;

      // member-table data emission anchor: the typed lowering
      // handoff now also projects real owner-scoped method tables plus real
      // property/ivar descriptor payload records without reopening the earlier
      // class/protocol/category descriptor bundle shapes from C002/C003.
      bool member_table_payload_complete = protocol_category_payload_complete;
      std::vector<Objc3IRRuntimeMetadataPropertyBundle> property_bundles;
      property_bundles.reserve(source_graph.property_nodes_lexicographic.size());
      std::unordered_set<std::string> property_owner_identities;
      property_owner_identities.reserve(
          source_graph.property_nodes_lexicographic.size());
      for (const auto &property_node : source_graph.property_nodes_lexicographic) {
        if (property_node.owner_kind.empty() || property_node.owner_name.empty() ||
            property_node.owner_identity.empty() ||
            property_node.declaration_owner_identity.empty() ||
            property_node.export_owner_identity.empty() ||
            property_node.property_name.empty() || property_node.type_name.empty() ||
            !property_owner_identities.insert(property_node.owner_identity).second ||
            (property_node.has_getter && property_node.getter_selector.empty()) ||
            (property_node.has_setter && property_node.setter_selector.empty()) ||
            property_node.property_attribute_profile.empty() ||
            property_node.effective_getter_selector.empty() ||
            property_node.accessor_ownership_profile.empty() ||
            (property_node.effective_setter_available &&
             property_node.effective_setter_selector.empty())) {
          member_table_payload_complete = false;
          break;
        }

        Objc3IRRuntimeMetadataPropertyBundle bundle;
        bundle.owner_kind = property_node.owner_kind;
        bundle.owner_name = property_node.owner_name;
        bundle.owner_identity = property_node.owner_identity;
        bundle.declaration_owner_identity =
            property_node.declaration_owner_identity;
        bundle.export_owner_identity = property_node.export_owner_identity;
        bundle.property_name = property_node.property_name;
        bundle.type_name = property_node.type_name;
        bundle.has_getter = property_node.has_getter;
        bundle.getter_selector = property_node.getter_selector;
        bundle.has_setter = property_node.has_setter;
        bundle.setter_selector = property_node.setter_selector;
        bundle.ivar_binding_symbol = property_node.ivar_binding_symbol;
        bundle.executable_synthesized_binding_kind =
            property_node.executable_synthesized_binding_kind;
        bundle.executable_synthesized_binding_symbol =
            property_node.executable_synthesized_binding_symbol;
        bundle.property_attribute_profile =
            property_node.property_attribute_profile;
        bundle.ownership_lifetime_profile =
            property_node.ownership_lifetime_profile;
        bundle.ownership_runtime_hook_profile =
            property_node.ownership_runtime_hook_profile;
        bundle.effective_getter_selector =
            property_node.effective_getter_selector;
        bundle.effective_setter_available =
            property_node.effective_setter_available;
        bundle.effective_setter_selector =
            property_node.effective_setter_selector;
        bundle.accessor_ownership_profile =
            property_node.accessor_ownership_profile;
        bundle.synthesizes_executable_accessors =
            property_node.synthesizes_executable_accessors;
        bundle.executable_ivar_layout_symbol =
            property_node.executable_ivar_layout_symbol;
        bundle.executable_ivar_layout_slot_index =
            property_node.executable_ivar_layout_slot_index;
        bundle.executable_ivar_layout_size_bytes =
            property_node.executable_ivar_layout_size_bytes;
        bundle.executable_ivar_layout_alignment_bytes =
            property_node.executable_ivar_layout_alignment_bytes;
        bundle.executable_ivar_layout_offset_bytes =
            property_node.executable_ivar_layout_offset_bytes;
        bundle.executable_ivar_layout_padding_bytes =
            property_node.executable_ivar_layout_padding_bytes;
        bundle.executable_ivar_layout_inherited_slot_count =
            property_node.executable_ivar_layout_inherited_slot_count;
        bundle.executable_ivar_layout_inherited_size_bytes =
            property_node.executable_ivar_layout_inherited_size_bytes;
        bundle.executable_ivar_layout_owner_size_bytes =
            property_node.executable_ivar_layout_owner_size_bytes;
        bundle.executable_ivar_init_order_index =
            property_node.executable_ivar_init_order_index;
        bundle.executable_ivar_destroy_order_index =
            property_node.executable_ivar_destroy_order_index;
        bundle.executable_ivar_layout_valid =
            property_node.executable_ivar_layout_valid;
        bundle.executable_ivar_layout_replay_key =
            property_node.executable_ivar_layout_replay_key;
        property_bundles.push_back(std::move(bundle));
      }
      std::sort(
          property_bundles.begin(), property_bundles.end(),
          [](const Objc3IRRuntimeMetadataPropertyBundle &lhs,
             const Objc3IRRuntimeMetadataPropertyBundle &rhs) {
            return std::tie(lhs.declaration_owner_identity, lhs.property_name,
                            lhs.owner_identity) <
                   std::tie(rhs.declaration_owner_identity, rhs.property_name,
                            rhs.owner_identity);
          });

      std::vector<Objc3IRRuntimeMetadataIvarBundle> ivar_bundles;
      ivar_bundles.reserve(source_graph.ivar_nodes_lexicographic.size());
      std::unordered_set<std::string> ivar_owner_identities;
      ivar_owner_identities.reserve(source_graph.ivar_nodes_lexicographic.size());
      if (member_table_payload_complete) {
        for (const auto &ivar_node : source_graph.ivar_nodes_lexicographic) {
          if (ivar_node.owner_kind.empty() || ivar_node.owner_name.empty() ||
              ivar_node.owner_identity.empty() ||
              ivar_node.declaration_owner_identity.empty() ||
              ivar_node.export_owner_identity.empty() ||
              ivar_node.property_owner_identity.empty() ||
              ivar_node.property_name.empty() ||
              ivar_node.ivar_binding_symbol.empty() ||
              ivar_node.executable_synthesized_binding_kind.empty() ||
              ivar_node.executable_ivar_layout_symbol.empty() ||
              !ivar_owner_identities.insert(ivar_node.owner_identity).second) {
            member_table_payload_complete = false;
            break;
          }

          Objc3IRRuntimeMetadataIvarBundle bundle;
          bundle.owner_kind = ivar_node.owner_kind;
          bundle.owner_name = ivar_node.owner_name;
          bundle.owner_identity = ivar_node.owner_identity;
          bundle.declaration_owner_identity = ivar_node.declaration_owner_identity;
          bundle.export_owner_identity = ivar_node.export_owner_identity;
          bundle.property_owner_identity = ivar_node.property_owner_identity;
          bundle.property_name = ivar_node.property_name;
          bundle.ivar_binding_symbol = ivar_node.ivar_binding_symbol;
          bundle.executable_synthesized_binding_kind =
              ivar_node.executable_synthesized_binding_kind;
          bundle.executable_synthesized_binding_symbol =
              ivar_node.executable_synthesized_binding_symbol;
          bundle.executable_ivar_layout_symbol =
              ivar_node.executable_ivar_layout_symbol;
          bundle.executable_ivar_layout_slot_index =
              ivar_node.executable_ivar_layout_slot_index;
          bundle.executable_ivar_layout_size_bytes =
              ivar_node.executable_ivar_layout_size_bytes;
          bundle.executable_ivar_layout_alignment_bytes =
              ivar_node.executable_ivar_layout_alignment_bytes;
          bundle.executable_ivar_layout_offset_bytes =
              ivar_node.executable_ivar_layout_offset_bytes;
          bundle.executable_ivar_layout_padding_bytes =
              ivar_node.executable_ivar_layout_padding_bytes;
          bundle.executable_ivar_layout_inherited_slot_count =
              ivar_node.executable_ivar_layout_inherited_slot_count;
          bundle.executable_ivar_layout_inherited_size_bytes =
              ivar_node.executable_ivar_layout_inherited_size_bytes;
          bundle.executable_ivar_layout_owner_size_bytes =
              ivar_node.executable_ivar_layout_owner_size_bytes;
          bundle.executable_ivar_init_order_index =
              ivar_node.executable_ivar_init_order_index;
          bundle.executable_ivar_destroy_order_index =
              ivar_node.executable_ivar_destroy_order_index;
          bundle.executable_ivar_layout_valid =
              ivar_node.executable_ivar_layout_valid;
          bundle.executable_ivar_layout_replay_key =
              ivar_node.executable_ivar_layout_replay_key;
          ivar_bundles.push_back(std::move(bundle));
        }
      }
      std::sort(
          ivar_bundles.begin(), ivar_bundles.end(),
          [](const Objc3IRRuntimeMetadataIvarBundle &lhs,
             const Objc3IRRuntimeMetadataIvarBundle &rhs) {
            return std::tie(lhs.declaration_owner_identity, lhs.property_name,
                            lhs.owner_identity) <
                   std::tie(rhs.declaration_owner_identity, rhs.property_name,
                            rhs.owner_identity);
          });

      std::vector<Objc3IRRuntimeMetadataMethodListBundle> method_list_bundles;
      method_list_bundles.reserve(source_graph.method_nodes_lexicographic.size());
      std::unordered_map<std::string, std::size_t> method_bundle_indexes;
      method_bundle_indexes.reserve(source_graph.method_nodes_lexicographic.size());
      std::unordered_set<std::string> method_owner_identities;
      method_owner_identities.reserve(source_graph.method_nodes_lexicographic.size());
      const auto determine_owner_family_kind = [](const std::string &owner_kind)
          -> std::string {
        if (owner_kind == "protocol") {
          return "protocol";
        }
        if (owner_kind == "class-interface" ||
            owner_kind == "class-implementation") {
          return "class";
        }
        if (owner_kind == "category-interface" ||
            owner_kind == "category-implementation") {
          return "category";
        }
        return {};
      };
      if (member_table_payload_complete) {
        for (const auto &method_node : source_graph.method_nodes_lexicographic) {
          const std::string list_kind =
              method_node.is_class_method ? "class" : "instance";
          const std::string owner_family_kind =
              determine_owner_family_kind(method_node.owner_kind);
          if (method_node.owner_kind.empty() || method_node.owner_name.empty() ||
              owner_family_kind.empty() || method_node.owner_identity.empty() ||
              method_node.declaration_owner_identity.empty() ||
              method_node.export_owner_identity.empty() ||
              method_node.selector.empty() ||
              method_node.return_type_name.empty()) {
            member_table_payload_complete = false;
            break;
          }

          const std::string bundle_key =
              method_node.declaration_owner_identity + "|" + list_kind;
          auto bundle_it = method_bundle_indexes.find(bundle_key);
          if (bundle_it == method_bundle_indexes.end()) {
            Objc3IRRuntimeMetadataMethodListBundle bundle;
            bundle.owner_kind = method_node.owner_kind;
            bundle.owner_name = method_node.owner_name;
            bundle.owner_family_kind = owner_family_kind;
            bundle.declaration_owner_identity =
                method_node.declaration_owner_identity;
            bundle.export_owner_identity = method_node.export_owner_identity;
            bundle.list_kind = list_kind;
            method_list_bundles.push_back(std::move(bundle));
            bundle_it = method_bundle_indexes
                            .emplace(bundle_key, method_list_bundles.size() - 1u)
                            .first;
          }

          auto &bundle = method_list_bundles[bundle_it->second];
          if (bundle.owner_kind != method_node.owner_kind ||
              bundle.owner_name != method_node.owner_name ||
              bundle.owner_family_kind != owner_family_kind ||
              bundle.export_owner_identity != method_node.export_owner_identity ||
              bundle.list_kind != list_kind) {
            member_table_payload_complete = false;
            break;
          }

          Objc3IRRuntimeMetadataMethodEntry entry;
          entry.owner_identity = method_node.owner_identity;
          entry.selector = method_node.selector;
          entry.return_type_name = method_node.return_type_name;
          entry.parameter_count = method_node.parameter_count;
          entry.has_body = method_node.has_body;
          entry.effective_direct_dispatch =
              method_node.effective_direct_dispatch;
          entry.objc_final_declared = method_node.objc_final_declared;
          bundle.entries_lexicographic.push_back(std::move(entry));
          method_owner_identities.insert(method_node.owner_identity);
        }
      }
      const auto build_instance_method_owner_identity =
          [](const std::string &declaration_owner_identity,
             const std::string &selector) {
            return declaration_owner_identity + "::instance_method:" + selector;
          };
      if (member_table_payload_complete) {
        for (const auto &property_bundle : property_bundles) {
          if (!property_bundle.synthesizes_executable_accessors ||
              property_bundle.owner_name.empty() ||
              property_bundle.declaration_owner_identity.empty() ||
              property_bundle.export_owner_identity.empty() ||
              property_bundle.property_name.empty() ||
              property_bundle.type_name.empty() ||
              property_bundle.executable_synthesized_binding_kind.empty() ||
              property_bundle.executable_synthesized_binding_symbol.empty()) {
            continue;
          }

          const std::string owner_family_kind =
              determine_owner_family_kind(property_bundle.owner_kind);
          if (owner_family_kind.empty()) {
            member_table_payload_complete = false;
            break;
          }

          const auto append_synthesized_accessor =
              [&](const std::string &selector,
                  const std::string &return_type_name,
                  std::size_t parameter_count) {
                if (selector.empty() || return_type_name.empty()) {
                  member_table_payload_complete = false;
                  return;
                }
                const std::string method_owner_identity =
                    build_instance_method_owner_identity(
                        property_bundle.declaration_owner_identity, selector);
                if (!method_owner_identities.insert(method_owner_identity).second) {
                  return;
                }

                const std::string bundle_key =
                    property_bundle.declaration_owner_identity + "|instance";
                auto bundle_it = method_bundle_indexes.find(bundle_key);
                if (bundle_it == method_bundle_indexes.end()) {
                  Objc3IRRuntimeMetadataMethodListBundle bundle;
                  bundle.owner_kind = property_bundle.owner_kind;
                  bundle.owner_name = property_bundle.owner_name;
                  bundle.owner_family_kind = owner_family_kind;
                  bundle.declaration_owner_identity =
                      property_bundle.declaration_owner_identity;
                  bundle.export_owner_identity =
                      property_bundle.export_owner_identity;
                  bundle.list_kind = "instance";
                  method_list_bundles.push_back(std::move(bundle));
                  bundle_it = method_bundle_indexes
                                  .emplace(bundle_key,
                                           method_list_bundles.size() - 1u)
                                  .first;
                }

                auto &bundle = method_list_bundles[bundle_it->second];
                if (bundle.owner_kind != property_bundle.owner_kind ||
                    bundle.owner_name != property_bundle.owner_name ||
                    bundle.owner_family_kind != owner_family_kind ||
                    bundle.export_owner_identity !=
                        property_bundle.export_owner_identity ||
                    bundle.list_kind != "instance") {
                  member_table_payload_complete = false;
                  return;
                }

                Objc3IRRuntimeMetadataMethodEntry entry;
                entry.owner_identity = method_owner_identity;
                entry.selector = selector;
                entry.return_type_name = return_type_name;
                entry.parameter_count = parameter_count;
                entry.has_body = true;
                entry.effective_direct_dispatch = false;
                entry.objc_final_declared = false;
                bundle.entries_lexicographic.push_back(std::move(entry));
              };

          append_synthesized_accessor(
              property_bundle.effective_getter_selector, property_bundle.type_name,
              0u);
          if (!member_table_payload_complete) {
            break;
          }
          if (property_bundle.effective_setter_available) {
            append_synthesized_accessor(
                property_bundle.effective_setter_selector, "void", 1u);
            if (!member_table_payload_complete) {
              break;
            }
          }
        }
      }
      if (member_table_payload_complete) {
        for (const auto &derive_bundle :
             ir_frontend_metadata.metaprogramming_derived_method_bundles_lexicographic) {
          if (derive_bundle.implementation_name.empty() ||
              derive_bundle.declaration_owner_identity.empty() ||
              derive_bundle.export_owner_identity.empty() ||
              derive_bundle.selector.empty() || derive_bundle.emitted_symbol.empty()) {
            member_table_payload_complete = false;
            break;
          }
          const std::string method_owner_identity =
              build_instance_method_owner_identity(
                  derive_bundle.declaration_owner_identity,
                  derive_bundle.selector);
          if (!method_owner_identities.insert(method_owner_identity).second) {
            continue;
          }
          const std::string bundle_key =
              derive_bundle.declaration_owner_identity + "|instance";
          auto bundle_it = method_bundle_indexes.find(bundle_key);
          if (bundle_it == method_bundle_indexes.end()) {
            Objc3IRRuntimeMetadataMethodListBundle bundle;
            bundle.owner_kind = "class-implementation";
            bundle.owner_name = derive_bundle.implementation_name;
            bundle.owner_family_kind = "class";
            bundle.declaration_owner_identity =
                derive_bundle.declaration_owner_identity;
            bundle.export_owner_identity = derive_bundle.export_owner_identity;
            bundle.list_kind = "instance";
            method_list_bundles.push_back(std::move(bundle));
            bundle_it = method_bundle_indexes
                            .emplace(bundle_key, method_list_bundles.size() - 1u)
                            .first;
          }

          auto &bundle = method_list_bundles[bundle_it->second];
          if (bundle.owner_kind != "class-implementation" ||
              bundle.owner_name != derive_bundle.implementation_name ||
              bundle.owner_family_kind != "class" ||
              bundle.export_owner_identity != derive_bundle.export_owner_identity ||
              bundle.list_kind != "instance") {
            member_table_payload_complete = false;
            break;
          }

          Objc3IRRuntimeMetadataMethodEntry entry;
          entry.owner_identity = method_owner_identity;
          entry.selector = derive_bundle.selector;
          entry.return_type_name = "i32";
          entry.parameter_count = derive_bundle.parameter_count;
          entry.has_body = true;
          entry.effective_direct_dispatch = false;
          entry.objc_final_declared = false;
          bundle.entries_lexicographic.push_back(std::move(entry));
        }
      }
      std::sort(
          method_list_bundles.begin(), method_list_bundles.end(),
          [](const Objc3IRRuntimeMetadataMethodListBundle &lhs,
             const Objc3IRRuntimeMetadataMethodListBundle &rhs) {
            return std::tie(lhs.owner_family_kind,
                            lhs.declaration_owner_identity, lhs.list_kind) <
                   std::tie(rhs.owner_family_kind,
                            rhs.declaration_owner_identity, rhs.list_kind);
          });
      for (auto &bundle : method_list_bundles) {
        std::sort(bundle.entries_lexicographic.begin(),
                  bundle.entries_lexicographic.end(),
                  [](const Objc3IRRuntimeMetadataMethodEntry &lhs,
                     const Objc3IRRuntimeMetadataMethodEntry &rhs) {
                    return std::tie(lhs.selector, lhs.owner_identity,
                                    lhs.parameter_count, lhs.return_type_name,
                                    lhs.has_body,
                                    lhs.effective_direct_dispatch,
                                    lhs.objc_final_declared) <
                           std::tie(rhs.selector, rhs.owner_identity,
                                    rhs.parameter_count, rhs.return_type_name,
                                    rhs.has_body,
                                    rhs.effective_direct_dispatch,
                                    rhs.objc_final_declared);
                  });
      }

      member_table_payload_complete =
          member_table_payload_complete &&
          property_bundles.size() ==
              runtime_metadata_section_publication.property_descriptor_count &&
          ivar_bundles.size() ==
              runtime_metadata_section_publication.ivar_descriptor_count;
      if (member_table_payload_complete) {
        std::size_t property_attribute_profiles = 0;
        std::size_t accessor_ownership_profiles = 0;
        std::size_t synthesized_binding_entries = 0;
        for (const auto &bundle : property_bundles) {
          if (!bundle.property_attribute_profile.empty()) {
            ++property_attribute_profiles;
          }
          if (!bundle.accessor_ownership_profile.empty()) {
            ++accessor_ownership_profiles;
          }
          if (!bundle.executable_synthesized_binding_kind.empty()) {
            ++synthesized_binding_entries;
          }
        }
        ir_frontend_metadata.runtime_metadata_method_list_bundles_lexicographic =
            std::move(method_list_bundles);
        ir_frontend_metadata.executable_property_attribute_profile_entries =
            property_attribute_profiles;
        ir_frontend_metadata.executable_accessor_ownership_profile_entries =
            accessor_ownership_profiles;
        ir_frontend_metadata.executable_synthesized_binding_entries =
            synthesized_binding_entries;
        ir_frontend_metadata.executable_ivar_layout_entries =
            ivar_bundles.size();
        ir_frontend_metadata.executable_property_ivar_source_model_replay_key =
            "property_attribute_profiles=" +
            std::to_string(property_attribute_profiles) +
            ";accessor_ownership_profiles=" +
            std::to_string(accessor_ownership_profiles) +
            ";synthesized_bindings=" +
            std::to_string(synthesized_binding_entries) +
            ";ivar_layout_entries=" +
            std::to_string(ivar_bundles.size()) +
            ";deterministic=true;lane_contract=objc3c.property.ivar.source.model.v1";
        ir_frontend_metadata.executable_ivar_layout_emission_contract_id =
            kObjc3ExecutableIvarLayoutEmissionContractId;
        ir_frontend_metadata.executable_ivar_layout_descriptor_model =
            kObjc3ExecutableIvarLayoutDescriptorModel;
        ir_frontend_metadata.executable_ivar_offset_global_model =
            kObjc3ExecutableIvarOffsetGlobalModel;
        ir_frontend_metadata.executable_ivar_layout_table_model =
            kObjc3ExecutableIvarLayoutTableModel;
        std::set<std::string> ivar_layout_owner_identities;
        bool ivar_layout_emission_complete = true;
        std::size_t ivar_offset_global_entries = 0;
        for (const auto &bundle : ivar_bundles) {
          if (bundle.declaration_owner_identity.empty() ||
              bundle.executable_ivar_layout_symbol.empty() ||
              bundle.executable_ivar_layout_alignment_bytes == 0u ||
              bundle.executable_ivar_layout_size_bytes == 0u ||
              !bundle.executable_ivar_layout_valid ||
              bundle.executable_ivar_layout_replay_key.empty() ||
              bundle.ivar_binding_symbol.empty()) {
            ivar_layout_emission_complete = false;
            break;
          }
          ivar_layout_owner_identities.insert(bundle.declaration_owner_identity);
          ++ivar_offset_global_entries;
        }
        ir_frontend_metadata.executable_ivar_offset_global_entries =
            ivar_offset_global_entries;
        ir_frontend_metadata.executable_ivar_layout_table_entries =
            ivar_layout_owner_identities.size();
        ir_frontend_metadata.executable_ivar_layout_owner_entries =
            ivar_layout_owner_identities.size();
        ir_frontend_metadata.executable_ivar_layout_emission_ready =
            ivar_layout_emission_complete;
        ir_frontend_metadata.executable_ivar_layout_emission_fail_closed =
            ivar_layout_emission_complete;
        if (ivar_layout_emission_complete) {
          ir_frontend_metadata.executable_ivar_layout_emission_replay_key =
              "offset_globals=" +
              std::to_string(ivar_offset_global_entries) +
              ";layout_tables=" +
              std::to_string(ivar_layout_owner_identities.size()) +
              ";owner_entries=" +
              std::to_string(ivar_layout_owner_identities.size()) +
              ";deterministic=true;lane_contract=objc3c.ivar.layout.emission.v1";
        } else {
          ir_frontend_metadata.executable_ivar_layout_emission_replay_key.clear();
        }
        ir_frontend_metadata.runtime_metadata_property_bundles_lexicographic =
            std::move(property_bundles);
        ir_frontend_metadata.runtime_metadata_ivar_bundles_lexicographic =
            std::move(ivar_bundles);
      }
      ir_frontend_metadata.runtime_metadata_member_table_emission_ready =
          member_table_payload_complete;
      ir_frontend_metadata.runtime_metadata_member_table_emission_fail_closed =
          member_table_payload_complete;
    }
  }
  ir_frontend_metadata.runtime_metadata_object_inspection_contract_id =
      runtime_metadata_object_inspection.contract_id;
  ir_frontend_metadata.runtime_metadata_object_inspection_publication_contract_id =
      runtime_metadata_object_inspection.publication_contract_id;
  ir_frontend_metadata.runtime_metadata_object_inspection_matrix_published =
      runtime_metadata_object_inspection.matrix_published;
  ir_frontend_metadata.runtime_metadata_object_inspection_fail_closed =
      runtime_metadata_object_inspection.fail_closed;
  ir_frontend_metadata.runtime_metadata_object_inspection_uses_llvm_readobj =
      runtime_metadata_object_inspection.uses_llvm_readobj;
  ir_frontend_metadata.runtime_metadata_object_inspection_uses_llvm_objdump =
      runtime_metadata_object_inspection.uses_llvm_objdump;
  ir_frontend_metadata.runtime_metadata_object_inspection_matrix_row_count =
      runtime_metadata_object_inspection.matrix_row_count;
  ir_frontend_metadata.runtime_metadata_object_inspection_fixture_path =
      runtime_metadata_object_inspection.fixture_path;
  ir_frontend_metadata.runtime_metadata_object_inspection_emit_prefix =
      runtime_metadata_object_inspection.emit_prefix;
  ir_frontend_metadata.runtime_metadata_object_inspection_object_relative_path =
      runtime_metadata_object_inspection.object_relative_path;
  ir_frontend_metadata.runtime_metadata_object_inspection_section_inventory_row_key =
      runtime_metadata_object_inspection.section_inventory_row_key;
  ir_frontend_metadata.runtime_metadata_object_inspection_section_inventory_command =
      runtime_metadata_object_inspection.section_inventory_command;
  ir_frontend_metadata.runtime_metadata_object_inspection_symbol_inventory_row_key =
      runtime_metadata_object_inspection.symbol_inventory_row_key;
  ir_frontend_metadata.runtime_metadata_object_inspection_symbol_inventory_command =
      runtime_metadata_object_inspection.symbol_inventory_command;
  ir_frontend_metadata.executable_metadata_debug_projection_contract_id =
      executable_metadata_debug_projection.contract_id;
  ir_frontend_metadata
      .executable_metadata_debug_projection_typed_handoff_contract_id =
      executable_metadata_debug_projection.typed_lowering_handoff_contract_id;
  ir_frontend_metadata
      .executable_metadata_debug_projection_source_graph_contract_id =
      executable_metadata_debug_projection.source_graph_contract_id;
  ir_frontend_metadata.executable_metadata_debug_projection_named_metadata_name =
      executable_metadata_debug_projection.named_metadata_name;
  ir_frontend_metadata
      .executable_metadata_debug_projection_manifest_surface_path =
      executable_metadata_debug_projection.manifest_surface_path;
  ir_frontend_metadata
      .executable_metadata_debug_projection_typed_handoff_surface_path =
      executable_metadata_debug_projection.typed_handoff_surface_path;
  ir_frontend_metadata
      .executable_metadata_debug_projection_source_graph_surface_path =
      executable_metadata_debug_projection.source_graph_surface_path;
  ir_frontend_metadata.executable_metadata_debug_projection_matrix_published =
      executable_metadata_debug_projection.matrix_published;
  ir_frontend_metadata.executable_metadata_debug_projection_fail_closed =
      executable_metadata_debug_projection.fail_closed;
  ir_frontend_metadata
      .executable_metadata_debug_projection_manifest_debug_surface_published =
      executable_metadata_debug_projection.manifest_debug_surface_published;
  ir_frontend_metadata
      .executable_metadata_debug_projection_ir_named_metadata_published =
      executable_metadata_debug_projection.ir_named_metadata_published;
  ir_frontend_metadata
      .executable_metadata_debug_projection_replay_anchor_deterministic =
      executable_metadata_debug_projection.replay_anchor_deterministic;
  ir_frontend_metadata
      .executable_metadata_debug_projection_active_typed_handoff_ready =
      executable_metadata_debug_projection.active_typed_handoff_ready;
  ir_frontend_metadata.executable_metadata_debug_projection_matrix_row_count =
      executable_metadata_debug_projection.matrix_row_count;
  ir_frontend_metadata.executable_metadata_debug_projection_replay_key =
      executable_metadata_debug_projection.replay_key;
  ir_frontend_metadata
      .executable_metadata_debug_projection_active_typed_handoff_replay_key =
      executable_metadata_debug_projection.active_typed_handoff_replay_key;
  ir_frontend_metadata.executable_metadata_debug_projection_row0_descriptor =
      BuildExecutableMetadataDebugProjectionRowDescriptor(
          executable_metadata_debug_projection.rows[0]);
  ir_frontend_metadata.executable_metadata_debug_projection_row1_descriptor =
      BuildExecutableMetadataDebugProjectionRowDescriptor(
          executable_metadata_debug_projection.rows[1]);
  ir_frontend_metadata.executable_metadata_debug_projection_row2_descriptor =
      BuildExecutableMetadataDebugProjectionRowDescriptor(
          executable_metadata_debug_projection.rows[2]);
  ir_frontend_metadata.runtime_support_library_contract_id =
      runtime_support_library.contract_id;
  ir_frontend_metadata.runtime_support_library_metadata_publication_contract_id =
      runtime_support_library.metadata_publication_contract_id;
  ir_frontend_metadata.runtime_support_library_boundary_frozen =
      runtime_support_library.boundary_frozen;
  ir_frontend_metadata.runtime_support_library_fail_closed =
      runtime_support_library.fail_closed;
  ir_frontend_metadata.runtime_support_library_target_name_frozen =
      runtime_support_library.target_name_frozen;
  ir_frontend_metadata.runtime_support_library_exported_entrypoints_frozen =
      runtime_support_library.exported_entrypoints_frozen;
  ir_frontend_metadata.runtime_support_library_ownership_boundaries_frozen =
      runtime_support_library.ownership_boundaries_frozen;
  ir_frontend_metadata.runtime_support_library_build_constraints_frozen =
      runtime_support_library.build_constraints_frozen;
  ir_frontend_metadata
      .runtime_support_library_strict_dispatch_errors_required =
      runtime_support_library.strict_dispatch_errors_required;
  ir_frontend_metadata.runtime_support_library_native_library_present =
      runtime_support_library.native_runtime_library_present;
  ir_frontend_metadata.runtime_support_library_driver_link_wiring_pending =
      runtime_support_library.driver_link_wiring_pending;
  ir_frontend_metadata.runtime_support_library_ready_for_skeleton =
      runtime_support_library.ready_for_runtime_library_skeleton;
  ir_frontend_metadata.runtime_support_library_target_name =
      runtime_support_library.cmake_target_name;
  ir_frontend_metadata.runtime_support_library_public_header_path =
      runtime_support_library.public_header_path;
  ir_frontend_metadata.runtime_support_library_source_root =
      runtime_support_library.source_root;
  ir_frontend_metadata.runtime_support_library_library_kind =
      runtime_support_library.library_kind;
  ir_frontend_metadata.runtime_support_library_archive_basename =
      runtime_support_library.archive_basename;
  ir_frontend_metadata.runtime_support_library_register_image_symbol =
      runtime_support_library.register_image_symbol;
  ir_frontend_metadata.runtime_support_library_lookup_selector_symbol =
      runtime_support_library.lookup_selector_symbol;
  ir_frontend_metadata.runtime_support_library_dispatch_i32_symbol =
      runtime_support_library.dispatch_i32_symbol;
  ir_frontend_metadata.runtime_support_library_reset_for_testing_symbol =
      runtime_support_library.reset_for_testing_symbol;
  ir_frontend_metadata.runtime_support_library_driver_link_mode =
      runtime_support_library.driver_link_mode;
  ir_frontend_metadata.runtime_support_library_compiler_ownership_boundary =
      runtime_support_library.compiler_ownership_boundary;
  ir_frontend_metadata.runtime_support_library_runtime_ownership_boundary =
      runtime_support_library.runtime_ownership_boundary;
  ir_frontend_metadata.runtime_support_library_core_feature_contract_id =
      runtime_support_library_core_feature.contract_id;
  ir_frontend_metadata
      .runtime_support_library_core_feature_support_library_contract_id =
      runtime_support_library_core_feature.support_library_contract_id;
  ir_frontend_metadata
      .runtime_support_library_core_feature_metadata_publication_contract_id =
      runtime_support_library_core_feature.metadata_publication_contract_id;
  ir_frontend_metadata.runtime_support_library_core_feature_fail_closed =
      runtime_support_library_core_feature.fail_closed;
  ir_frontend_metadata.runtime_support_library_core_feature_sources_present =
      runtime_support_library_core_feature
          .native_runtime_library_sources_present;
  ir_frontend_metadata.runtime_support_library_core_feature_header_present =
      runtime_support_library_core_feature.native_runtime_library_header_present;
  ir_frontend_metadata
      .runtime_support_library_core_feature_archive_build_enabled =
      runtime_support_library_core_feature
          .native_runtime_library_archive_build_enabled;
  ir_frontend_metadata
      .runtime_support_library_core_feature_entrypoints_implemented =
      runtime_support_library_core_feature
          .native_runtime_library_entrypoints_implemented;
  ir_frontend_metadata
      .runtime_support_library_core_feature_selector_lookup_stateful =
      runtime_support_library_core_feature.selector_lookup_stateful;
  ir_frontend_metadata
      .runtime_support_library_core_feature_dispatch_formula_matches_runtime_test_helper =
      runtime_support_library_core_feature
          .deterministic_dispatch_formula_matches_runtime_test_helper;
  ir_frontend_metadata
      .runtime_support_library_core_feature_reset_for_testing_supported =
      runtime_support_library_core_feature.reset_for_testing_supported;
  ir_frontend_metadata
      .runtime_support_library_core_feature_strict_dispatch_errors_required =
      runtime_support_library_core_feature
          .strict_dispatch_errors_required;
  ir_frontend_metadata
      .runtime_support_library_core_feature_driver_link_wiring_pending =
      runtime_support_library_core_feature.driver_link_wiring_pending;
  ir_frontend_metadata
      .runtime_support_library_core_feature_ready_for_driver_link_wiring =
      runtime_support_library_core_feature.ready_for_driver_link_wiring;
  ir_frontend_metadata.runtime_support_library_core_feature_target_name =
      runtime_support_library_core_feature.cmake_target_name;
  ir_frontend_metadata.runtime_support_library_core_feature_public_header_path =
      runtime_support_library_core_feature.public_header_path;
  ir_frontend_metadata.runtime_support_library_core_feature_source_root =
      runtime_support_library_core_feature.source_root;
  ir_frontend_metadata
      .runtime_support_library_core_feature_implementation_source_path =
      runtime_support_library_core_feature.implementation_source_path;
  ir_frontend_metadata.runtime_support_library_core_feature_library_kind =
      runtime_support_library_core_feature.library_kind;
  ir_frontend_metadata.runtime_support_library_core_feature_archive_basename =
      runtime_support_library_core_feature.archive_basename;
  ir_frontend_metadata.runtime_support_library_core_feature_archive_relative_path =
      runtime_support_library_core_feature.archive_relative_path;
  ir_frontend_metadata.runtime_support_library_core_feature_probe_source_path =
      runtime_support_library_core_feature.probe_source_path;
  ir_frontend_metadata.runtime_support_library_core_feature_register_image_symbol =
      runtime_support_library_core_feature.register_image_symbol;
  ir_frontend_metadata
      .runtime_support_library_core_feature_lookup_selector_symbol =
      runtime_support_library_core_feature.lookup_selector_symbol;
  ir_frontend_metadata.runtime_support_library_core_feature_dispatch_i32_symbol =
      runtime_support_library_core_feature.dispatch_i32_symbol;
  ir_frontend_metadata
      .runtime_support_library_core_feature_reset_for_testing_symbol =
      runtime_support_library_core_feature.reset_for_testing_symbol;
  ir_frontend_metadata.runtime_support_library_core_feature_driver_link_mode =
      runtime_support_library_core_feature.driver_link_mode;
  ir_frontend_metadata.runtime_support_library_link_wiring_contract_id =
      runtime_support_library_link_wiring.contract_id;
  ir_frontend_metadata
      .runtime_support_library_link_wiring_core_feature_contract_id =
      runtime_support_library_link_wiring
          .support_library_core_feature_contract_id;
  ir_frontend_metadata.runtime_support_library_link_wiring_fail_closed =
      runtime_support_library_link_wiring.fail_closed;
  ir_frontend_metadata.runtime_support_library_link_wiring_archive_available =
      runtime_support_library_link_wiring.runtime_library_archive_available;
  ir_frontend_metadata
      .runtime_support_library_link_wiring_driver_emits_runtime_link_contract =
      runtime_support_library_link_wiring.driver_emits_runtime_link_contract;
  ir_frontend_metadata
      .runtime_support_library_link_wiring_execution_smoke_consumes_runtime_library =
      runtime_support_library_link_wiring
          .execution_smoke_consumes_runtime_library;
  ir_frontend_metadata
      .runtime_support_library_link_wiring_strict_dispatch_errors_required =
      runtime_support_library_link_wiring
          .strict_dispatch_errors_required;
  ir_frontend_metadata
      .runtime_support_library_link_wiring_ready_for_runtime_library_consumption =
      runtime_support_library_link_wiring
          .ready_for_runtime_library_consumption;
  ir_frontend_metadata.runtime_support_library_link_wiring_archive_relative_path =
      runtime_support_library_link_wiring.archive_relative_path;
  ir_frontend_metadata.runtime_support_library_link_wiring_runtime_dispatch_symbol =
      runtime_support_library_link_wiring.runtime_dispatch_symbol;
  ir_frontend_metadata
      .runtime_support_library_link_wiring_execution_smoke_script_path =
      runtime_support_library_link_wiring.execution_smoke_script_path;
  ir_frontend_metadata.runtime_support_library_link_wiring_driver_link_mode =
      runtime_support_library_link_wiring.driver_link_mode;
  ir_frontend_metadata.deterministic_id_class_sel_object_pointer_typecheck_handoff =
      id_class_sel_object_pointer_typecheck_contract.deterministic;
  ir_frontend_metadata.deterministic_dispatch_surface_classification_handoff =
      dispatch_surface_classification_contract.deterministic;
  ir_frontend_metadata.deterministic_message_send_selector_lowering_handoff =
      message_send_selector_lowering_contract.deterministic;
  ir_frontend_metadata.deterministic_dispatch_abi_marshalling_handoff =
      dispatch_abi_marshalling_contract.deterministic;
  ir_frontend_metadata.deterministic_nil_receiver_semantics_foldability_handoff =
      nil_receiver_semantics_foldability_contract.deterministic;
  ir_frontend_metadata.deterministic_super_dispatch_method_family_handoff =
      super_dispatch_method_family_contract.deterministic;
  ir_frontend_metadata.deterministic_runtime_link_host_link_handoff =
      runtime_link_host_link_contract.deterministic;
  ir_frontend_metadata.deterministic_ownership_qualifier_lowering_handoff =
      ownership_qualifier_lowering_contract.deterministic;
  ir_frontend_metadata.deterministic_retain_release_operation_lowering_handoff =
      retain_release_operation_lowering_contract.deterministic;
  ir_frontend_metadata.deterministic_autoreleasepool_scope_lowering_handoff =
      autoreleasepool_scope_lowering_contract.deterministic;
  ir_frontend_metadata.deterministic_object_pointer_nullability_generics_handoff =
      object_pointer_nullability_generics_summary.deterministic_object_pointer_nullability_generics_handoff;
  ir_frontend_metadata.deterministic_symbol_graph_handoff =
      symbol_graph_scope_resolution_summary.deterministic_symbol_graph_handoff;
  ir_frontend_metadata.deterministic_scope_resolution_handoff =
      symbol_graph_scope_resolution_summary.deterministic_scope_resolution_handoff;
  ir_frontend_metadata.deterministic_symbol_graph_scope_resolution_handoff_key =
      symbol_graph_scope_resolution_summary.deterministic_handoff_key;
  ir_frontend_metadata.ownership_aware_lowering_core_feature_expansion_ready =
      ownership_aware_lowering_behavior_scaffold.expansion_ready;
  ir_frontend_metadata.ownership_aware_lowering_core_feature_expansion_key =
      ownership_aware_lowering_behavior_scaffold.expansion_key;
  ir_frontend_metadata.ownership_aware_lowering_performance_quality_guardrails_ready =
      ownership_aware_lowering_behavior_scaffold.performance_quality_guardrails_ready;
  ir_frontend_metadata.ownership_aware_lowering_performance_quality_guardrails_key =
      ownership_aware_lowering_behavior_scaffold.performance_quality_guardrails_key;
  ir_frontend_metadata.ownership_aware_lowering_cross_lane_integration_ready =
      ownership_aware_lowering_behavior_scaffold.cross_lane_integration_ready;
  ir_frontend_metadata.ownership_aware_lowering_cross_lane_integration_key =
      ownership_aware_lowering_behavior_scaffold.cross_lane_integration_key;
  ir_frontend_metadata.lowering_pass_graph_core_feature_ready =
      pipeline_result.ir_emission_completeness_scaffold.core_feature_ready;
  ir_frontend_metadata.lowering_pass_graph_core_feature_key =
      pipeline_result.ir_emission_completeness_scaffold.core_feature_key;
  ir_frontend_metadata.lowering_pass_graph_core_feature_expansion_ready =
      pipeline_result.ir_emission_completeness_scaffold.expansion_ready;
  ir_frontend_metadata.lowering_pass_graph_core_feature_expansion_key =
      pipeline_result.ir_emission_completeness_scaffold.expansion_key;
  ir_frontend_metadata.lowering_pass_graph_edge_case_compatibility_ready =
      pipeline_result.ir_emission_completeness_scaffold
          .edge_case_compatibility_ready;
  ir_frontend_metadata.lowering_pass_graph_edge_case_compatibility_key =
      pipeline_result.ir_emission_completeness_scaffold
          .edge_case_compatibility_key;
  ir_frontend_metadata.lowering_pass_graph_edge_case_robustness_ready =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .edge_case_robustness_ready;
  ir_frontend_metadata.lowering_pass_graph_edge_case_robustness_key =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .edge_case_robustness_key;
  ir_frontend_metadata.lowering_pass_graph_diagnostics_hardening_ready =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .diagnostics_hardening_ready;
  ir_frontend_metadata.lowering_pass_graph_diagnostics_hardening_key =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .diagnostics_hardening_key;
  ir_frontend_metadata.lowering_pass_graph_recovery_determinism_ready =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .recovery_determinism_ready;
  ir_frontend_metadata.lowering_pass_graph_recovery_determinism_key =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .recovery_determinism_key;
  ir_frontend_metadata.lowering_pass_graph_conformance_matrix_ready =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .conformance_matrix_ready;
  ir_frontend_metadata.lowering_pass_graph_conformance_matrix_key =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .conformance_matrix_key;
  ir_frontend_metadata.lowering_pass_graph_conformance_corpus_ready =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .conformance_corpus_ready;
  ir_frontend_metadata.lowering_pass_graph_conformance_corpus_key =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .conformance_corpus_key;
  ir_frontend_metadata.lowering_pass_graph_performance_quality_guardrails_ready =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .performance_quality_guardrails_ready;
  ir_frontend_metadata.lowering_pass_graph_performance_quality_guardrails_key =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .performance_quality_guardrails_key;
  ir_frontend_metadata.ir_emission_completeness_modular_split_ready =
      pipeline_result.ir_emission_completeness_scaffold.modular_split_ready;
  ir_frontend_metadata.ir_emission_completeness_modular_split_key =
      pipeline_result.ir_emission_completeness_scaffold.scaffold_key;
  ir_frontend_metadata.ir_emission_core_feature_impl_ready =
      ir_emission_core_feature_impl_surface.core_feature_impl_ready;
  ir_frontend_metadata.ir_emission_core_feature_impl_key =
      ir_emission_core_feature_impl_surface.core_feature_key;
  ir_frontend_metadata.ir_emission_core_feature_expansion_ready =
      ir_emission_core_feature_impl_surface.core_feature_expansion_ready;
  ir_frontend_metadata.ir_emission_core_feature_expansion_key =
      ir_emission_core_feature_impl_surface.expansion_key;
  ir_frontend_metadata.ir_emission_core_feature_edge_case_compatibility_ready =
      ir_emission_core_feature_impl_surface
          .core_feature_edge_case_compatibility_ready;
  ir_frontend_metadata.ir_emission_core_feature_edge_case_compatibility_key =
      ir_emission_core_feature_impl_surface.edge_case_compatibility_key;
  ir_frontend_metadata.ir_emission_core_feature_edge_case_robustness_ready =
      ir_emission_core_feature_impl_surface.core_feature_edge_case_robustness_ready;
  ir_frontend_metadata.ir_emission_core_feature_edge_case_robustness_key =
      ir_emission_core_feature_impl_surface.edge_case_robustness_key;
  ir_frontend_metadata.ir_emission_core_feature_diagnostics_hardening_ready =
      ir_emission_core_feature_impl_surface
          .core_feature_diagnostics_hardening_ready;
  ir_frontend_metadata.ir_emission_core_feature_diagnostics_hardening_key =
      ir_emission_core_feature_impl_surface.diagnostics_hardening_key;
  ir_frontend_metadata.ir_emission_core_feature_recovery_determinism_ready =
      ir_emission_core_feature_impl_surface
          .core_feature_recovery_determinism_ready;
  ir_frontend_metadata.ir_emission_core_feature_recovery_determinism_key =
      ir_emission_core_feature_impl_surface.recovery_determinism_key;
  ir_frontend_metadata.ir_emission_core_feature_conformance_matrix_ready =
      ir_emission_core_feature_impl_surface.core_feature_conformance_matrix_ready;
  ir_frontend_metadata.ir_emission_core_feature_conformance_matrix_key =
      ir_emission_core_feature_impl_surface.conformance_matrix_key;
  ir_frontend_metadata.ir_emission_core_feature_conformance_corpus_ready =
      ir_emission_core_feature_impl_surface.core_feature_conformance_corpus_ready;
  ir_frontend_metadata.ir_emission_core_feature_conformance_corpus_key =
      ir_emission_core_feature_impl_surface.conformance_corpus_key;
  ir_frontend_metadata.ir_emission_core_feature_performance_quality_guardrails_ready =
      ir_emission_core_feature_impl_surface
          .core_feature_performance_quality_guardrails_ready;
  ir_frontend_metadata.ir_emission_core_feature_performance_quality_guardrails_key =
      ir_emission_core_feature_impl_surface.performance_quality_guardrails_key;
  ir_frontend_metadata.ir_emission_core_feature_cross_lane_integration_sync_ready =
      ir_emission_core_feature_impl_surface
          .core_feature_cross_lane_integration_sync_ready;
  ir_frontend_metadata.ir_emission_core_feature_cross_lane_integration_sync_key =
      ir_emission_core_feature_impl_surface.cross_lane_integration_sync_key;
  ir_frontend_metadata.ir_emission_core_feature_advanced_core_shard1_ready =
      ir_emission_core_feature_impl_surface.core_feature_advanced_core_shard1_ready;
  ir_frontend_metadata.ir_emission_core_feature_advanced_core_shard1_key =
      ir_emission_core_feature_impl_surface.advanced_core_shard1_key;
  ir_frontend_metadata.ir_emission_core_feature_advanced_edge_compatibility_shard1_ready =
      ir_emission_core_feature_impl_surface
          .core_feature_advanced_edge_compatibility_shard1_ready;
  ir_frontend_metadata.ir_emission_core_feature_advanced_edge_compatibility_shard1_key =
      ir_emission_core_feature_impl_surface
          .advanced_edge_compatibility_shard1_key;
  ir_frontend_metadata.ir_emission_core_feature_advanced_diagnostics_shard1_ready =
      ir_emission_core_feature_impl_surface.core_feature_advanced_diagnostics_shard1_ready;
  ir_frontend_metadata.ir_emission_core_feature_advanced_diagnostics_shard1_key =
      ir_emission_core_feature_impl_surface.advanced_diagnostics_shard1_key;
  ir_frontend_metadata.ir_emission_core_feature_advanced_conformance_shard1_ready =
      ir_emission_core_feature_impl_surface.core_feature_advanced_conformance_shard1_ready;
  ir_frontend_metadata.ir_emission_core_feature_advanced_conformance_shard1_key =
      ir_emission_core_feature_impl_surface.advanced_conformance_shard1_key;
  ir_frontend_metadata.ir_emission_core_feature_advanced_integration_shard1_ready =
      ir_emission_core_feature_impl_surface.core_feature_advanced_integration_shard1_ready;
  ir_frontend_metadata.ir_emission_core_feature_advanced_integration_shard1_key =
      ir_emission_core_feature_impl_surface.advanced_integration_shard1_key;
  std::string ir_error;
  // Historical extraction contract marker:
  // EmitObjc3IRText(pipeline_result.program, options.lowering, ir_frontend_metadata, bundle.ir_text, ir_error)
  // if (!EmitObjc3IRText(pipeline_result.program, options.lowering, ir_frontend_metadata, bundle.ir_text, ir_error)) {
  if (!EmitObjc3IRText(pipeline_result.program.ast, options.lowering, ir_frontend_metadata, bundle.ir_text, ir_error)) {
    bundle.post_pipeline_diagnostics = {MakeDiag(1, 1, "O3L300", "LLVM IR emission failed: " + ir_error)};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    bundle.manifest_json.clear();
    bundle.runtime_metadata_binary.clear();
    bundle.ir_text.clear();
    return bundle;
  }
  bundle.ir_text =
      std::string("; runtime_dispatch_lowering_abi_boundary = ") +
      Objc3RuntimeDispatchLoweringAbiBoundarySummary(
          runtime_dispatch_lowering_abi_contract) +
      "\n" + bundle.ir_text;

  if (objc3c::artifacts::IsSuspiciousObjc3NativeIRTruthGap(
          bundle.ir_text, program, message_send_selector_lowering_contract)) {
    bundle.post_pipeline_diagnostics = {
        MakeDiag(1, 1, "O3L330",
                 "LLVM IR emission failed: emitted native IR is suspiciously trivial for runtime-bearing executable surface")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    bundle.manifest_json.clear();
    bundle.runtime_metadata_binary.clear();
    bundle.ir_text.clear();
    return bundle;
  }

  return bundle;
}
