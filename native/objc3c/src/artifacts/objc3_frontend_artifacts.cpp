#include "artifacts/objc3_frontend_artifacts.h"

#include "artifacts/objc3_frontend_artifacts_runtime_manifest_surfaces.h"

#include <cctype>
#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

#include "artifacts/evidence/error_handling_replay_evidence.h"
#include "artifacts/objc3_frontend_actor_semantic_artifacts.h"
#include "artifacts/objc3_frontend_artifact_block_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_arc_ownership_metadata.h"
#include "artifacts/objc3_frontend_artifact_bundle_publication.h"
#include "artifacts/objc3_frontend_artifact_block_metadata.h"
#include "artifacts/objc3_frontend_artifact_block_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_conformance_report_plan.h"
#include "artifacts/objc3_frontend_artifact_concurrency_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_concurrency_metadata.h"
#include "artifacts/objc3_frontend_artifact_concurrency_runtime_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_concurrency_runtime_metadata.h"
#include "artifacts/objc3_frontend_artifact_core_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_cross_module_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_dispatch_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_dispatch_metadata.h"
#include "artifacts/objc3_frontend_artifact_dispatch_metadata_application.h"
#include "artifacts/objc3_frontend_artifact_dispatch_runtime_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_error_handling_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_error_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_error_metadata.h"
#include "artifacts/objc3_frontend_artifact_executable_metadata_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_executable_metadata_runtime_ingest_manifest_fields.h"
#include "artifacts/objc3_frontend_artifact_function_manifest.h"
#include "artifacts/objc3_frontend_artifact_interop_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_interop_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_interop_metadata.h"
#include "artifacts/objc3_frontend_artifact_ir_emission_completion.h"
#include "artifacts/objc3_frontend_artifact_lowering_contracts.h"
#include "artifacts/objc3_frontend_artifact_lowering_handoff_manifest_fields.h"
#include "artifacts/objc3_frontend_artifact_lowering_replay_manifest.h"
#include "artifacts/objc3_frontend_artifact_manifest_header.h"
#include "artifacts/objc3_frontend_artifact_manifest_pipeline.h"
#include "artifacts/objc3_frontend_artifact_manifest_replay_tail.h"
#include "artifacts/objc3_frontend_artifact_manifest_readiness.h"
#include "artifacts/objc3_frontend_artifact_metaprogramming_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_metaprogramming_metadata.h"
#include "artifacts/objc3_frontend_artifact_metadata_mode.h"
#include "artifacts/objc3_frontend_artifact_module_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_module_metadata.h"
#include "artifacts/objc3_frontend_artifact_object_dispatch_metadata.h"
#include "artifacts/objc3_frontend_artifact_ownership_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_ownership_metadata.h"
#include "artifacts/objc3_frontend_artifact_ownership_release_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_ownership_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_pre_tail_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_preservation_plan.h"
#include "artifacts/objc3_frontend_artifact_runtime_bootstrap_legality_manifest_fields.h"
#include "artifacts/objc3_frontend_artifact_runtime_bootstrap_manifest_fields.h"
#include "artifacts/objc3_frontend_artifact_runtime_bootstrap_private_manifest_fields.h"
#include "artifacts/objc3_frontend_artifact_runtime_metadata_contract_metadata.h"
#include "artifacts/objc3_frontend_artifact_runtime_metadata_manifest_fields.h"
#include "artifacts/objc3_frontend_artifact_runtime_metadata_plan.h"
#include "artifacts/objc3_frontend_artifact_runtime_import_plan.h"
#include "artifacts/objc3_frontend_artifact_runtime_manifest_orchestration.h"
#include "artifacts/objc3_frontend_artifact_runtime_registration_descriptor_manifest_fields.h"
#include "artifacts/objc3_frontend_artifact_runtime_registration_plan.h"
#include "artifacts/objc3_frontend_artifact_runtime_support_registration_manifest_fields.h"
#include "artifacts/objc3_frontend_artifact_runtime_startup_bootstrap_invariant_manifest_fields.h"
#include "artifacts/objc3_frontend_artifact_sanity.h"
#include "artifacts/objc3_frontend_artifact_semantic_closure_metadata.h"
#include "artifacts/objc3_frontend_artifact_semantic_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_sema_parity_manifest_fields.h"
#include "artifacts/objc3_frontend_artifact_semantic_summary_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_semantic_surface_manifest.h"
#include "artifacts/objc3_frontend_artifact_source_closure_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_source_linkage_metadata.h"
#include "artifacts/objc3_frontend_artifact_source_shape_plan.h"
#include "artifacts/objc3_frontend_artifact_tooling_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_type_system_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_type_system_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_type_system_metadata.h"
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
#include "artifacts/objc3_frontend_runtime_bootstrap_artifacts.h"
#include "artifacts/objc3_frontend_runtime_descriptor_artifacts.h"
#include "artifacts/objc3_frontend_runtime_import_artifacts.h"
#include "artifacts/objc3_frontend_runtime_ingest_binary_artifacts.h"
#include "artifacts/objc3_frontend_runtime_metadata_section_artifacts.h"
#include "artifacts/objc3_frontend_runtime_registration_artifacts.h"
#include "artifacts/objc3_frontend_source_closure_artifacts.h"
#include "artifacts/objc3_frontend_tooling_source_artifacts.h"
#include "artifacts/objc3_frontend_type_system_contract_artifacts.h"
#include "artifacts/objc3_frontend_type_system_semantic_artifacts.h"
#include "contracts/objc3_frontend_diagnostics_bus_contract.h"
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
#include "support/objc3_identifier_safe_suffix.h"
#include "support/objc3_frontend_dispatch_metadata_ir_application.h"
#include "support/objc3_runtime_metadata_record_set.h"
#include "support/objc3_value_type_names.h"

namespace {

using objc3::io::EscapeJsonString;
using objc3::artifacts::frontend::
    BuildDispatchDispatchIntentCompatibilitySummaryJson;
using objc3::artifacts::frontend::
    BuildDispatchDispatchControlLoweringContractJson;
using objc3::artifacts::frontend::BuildDispatchDispatchControlLoweringContract;
using objc3::artifacts::frontend::
    BuildControlFlowControlFlowSemanticModelSummaryJson;
using objc3::artifacts::frontend::BuildDispatchDispatchIntentLegalitySummaryJson;
using objc3::artifacts::frontend::
    BuildDispatchDispatchIntentSemanticModelSummaryJson;
using objc3::artifacts::frontend::WriteDispatchRuntimeAbiManifestSurfaces;
using objc3::artifacts::frontend::WriteOwnershipReleaseManifestSurfaces;
using objc3::artifacts::frontend::
    BuildErrorHandlingErrorBridgeLegalitySummaryJson;
using objc3::artifacts::frontend::
    BuildErrorHandlingErrorSemanticModelSummaryJson;
using objc3::artifacts::frontend::
    BuildErrorHandlingTryDoCatchSemanticSummaryJson;
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
    BuildInteropFfiMetadataInterfacePreservationContractJson;
using objc3::artifacts::frontend::BuildInteropInteropSemanticModelSummaryJson;
using objc3::artifacts::frontend::BuildInteropInteropLoweringContractJson;
using objc3::artifacts::frontend::BuildInteropSwiftInteropIsolationSummaryJson;
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
    BuildMetaprogrammingMacroHostProcessCacheRuntimeIntegrationSummaryJson;
using objc3::artifacts::frontend::
    BuildMetaprogrammingMacroSafetySandboxDeterminismSummaryJson;
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
using objc3::artifacts::frontend::
    BuildConcurrencyActorIsolationSendabilityEnforcementSummaryJson;
using objc3::artifacts::frontend::
    BuildConcurrencyActorIsolationSendabilityLoweringContract;
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
    BuildConcurrencyTaskRuntimeInteropCancellationLoweringContract;
using objc3::artifacts::frontend::
    BuildConcurrencyTaskRuntimeAbiCompletionJson;
using objc3::artifacts::frontend::
    BuildConcurrencyTaskRuntimeLoweringContractJson;
using objc3::artifacts::frontend::
    BuildConcurrencyConcurrencyReplayRaceGuardLoweringContract;
using objc3::artifacts::frontend::
    WriteExecutableRuntimeMetadataManifestSurfaces;
using objc3::artifacts::frontend::WriteBlockManifestSurfaces;
using objc3::artifacts::frontend::WriteConcurrencyManifestSurfaces;
using objc3::artifacts::frontend::WriteConcurrencyRuntimeManifestSurfaces;
using objc3::artifacts::frontend::WriteCrossModuleManifestSurfaces;
using objc3::artifacts::frontend::WriteDispatchManifestSurfaces;
using objc3::artifacts::frontend::WriteErrorHandlingManifestSurfaces;
using objc3::artifacts::frontend::WriteInteropManifestSurfaces;
using objc3::artifacts::frontend::WriteMetaprogrammingManifestSurfaces;
using objc3::artifacts::frontend::WriteOwnershipManifestSurfaces;
using objc3::artifacts::frontend::WriteSemanticSummaryManifestSurfaces;
using objc3::artifacts::frontend::WriteSourceClosureManifestSurfaces;
using objc3::artifacts::frontend::WriteToolingManifestSurfaces;
using objc3::artifacts::frontend::WriteTypeSystemManifestSurfaces;
using objc3::artifacts::frontend::
    BuildTypeSystemGenericContractPreservationJson;
using objc3::artifacts::frontend::
    BuildTypeSystemNullabilityContractPreservationJson;
using objc3::artifacts::frontend::
    BuildTypeSystemOptionalKeypathLoweringContractJson;
using objc3::artifacts::frontend::
    BuildTypeSystemOptionalKeypathRuntimeHelperContractJson;
using objc3::artifacts::frontend::
    BuildTypeSystemProtocolContractPreservationJson;
using objc3::artifacts::frontend::BuildTypeSystemTypeSemanticModelSummaryJson;
using objc3::artifacts::frontend::
    BuildToolingDiagnosticTaxonomyPortabilityContractSummaryJson;
using objc3::artifacts::frontend::
    BuildToolingDiagnosticsMigratorSourceInventorySummaryJson;
using objc3::artifacts::frontend::
    BuildToolingFeatureSpecificFixitSynthesisSummaryJson;
using objc3::artifacts::frontend::
    BuildToolingMigrationCanonicalizationSourceCompletionSummaryJson;
using objc3::artifacts::frontend::BuildConcurrencyAsyncSourceClosureSummaryJson;
using objc3::artifacts::frontend::
    BuildConcurrencyTaskGroupCancellationSourceClosureSummaryJson;
using objc3::artifacts::frontend::
    BuildControlFlowControlFlowSafetyLoweringContractJson;
using objc3::artifacts::frontend::
    BuildControlFlowControlFlowSourceClosureSummaryJson;
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
using objc3::artifacts::frontend::
    BuildOwnershipCaptureListRetainableFamilyLegalityCompletionSummaryJson;
using objc3::artifacts::frontend::
    BuildOwnershipResourceMoveUseAfterMoveSemanticsSummaryJson;
using objc3::artifacts::frontend::
    BuildOwnershipSystemExtensionSemanticModelSummaryJson;
using objc3::artifacts::frontend::
    BuildOwnershipSystemExtensionLoweringContractJson;
using objc3::artifacts::frontend::BuildOwnershipSystemExtensionLoweringContract;
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
using objc3::artifacts::frontend::
    BuildConcurrencyActorMailboxRuntimeImportSummary;
using objc3::artifacts::frontend::
    BuildConcurrencyActorMailboxRuntimeImportSummaryJson;
using objc3::artifacts::frontend::
    BuildDispatchDispatchMetadataInterfacePreservationSummaryJson;
using objc3::artifacts::frontend::
    BuildRuntimeBlockOwnershipArtifactPreservationSummaryJson;
using objc3::artifacts::frontend::
    BuildRuntimeStorageReflectionArtifactPreservationSummaryJson;
using objc3c::support::CountRuntimeMetadataSourceRecordSetDeclarations;
using objc3c::support::CountRuntimeMetadataSourceRecordSetReferences;

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
  Objc3FrontendArtifactPostPipelineFailure post_pipeline_failure;
  const auto record_post_pipeline_failure = [&](const char *code,
                                                std::string message) {
    objc3::artifacts::frontend::RecordObjc3FrontendArtifactPostPipelineFailure(
        post_pipeline_failure, code, std::move(message));
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
  const std::size_t vector_signature_functions =
      function_manifest.vector_signature_functions;
  const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff = pipeline_result.sema_type_metadata_handoff;
  const Objc3InterfaceImplementationSummary &interface_implementation_summary =
      type_metadata_handoff.interface_implementation_summary;
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
  const Objc3FrontendArtifactSemanticLoweringPlan semantic_lowering_plan =
      BuildObjc3FrontendArtifactSemanticLoweringPlan(program, pipeline_result);
  if (!semantic_lowering_plan.post_pipeline_failure.empty()) {
    record_post_pipeline_failure(
        semantic_lowering_plan.post_pipeline_failure.code.c_str(),
        semantic_lowering_plan.post_pipeline_failure.message);
  }
  const Objc3DispatchDispatchControlLoweringContract
      &dispatch_dispatch_control_lowering_contract =
          semantic_lowering_plan.dispatch_dispatch_control_lowering_contract;
  const auto &dispatch_dispatch_control_lowering_snapshot =
      semantic_lowering_plan.dispatch_dispatch_control_lowering_snapshot;
  const std::string &dispatch_dispatch_control_lowering_replay_key =
      semantic_lowering_plan.dispatch_dispatch_control_lowering_replay_key;
  const Objc3MetaprogrammingExpansionLoweringContract
      &metaprogramming_expansion_lowering_contract =
          semantic_lowering_plan.metaprogramming_expansion_lowering_contract;
  const std::string &metaprogramming_expansion_lowering_replay_key =
      semantic_lowering_plan.metaprogramming_expansion_lowering_replay_key;
  const std::vector<Objc3IRMetaprogrammingDerivedMethodBundle>
      &metaprogramming_derived_method_bundles =
          semantic_lowering_plan.metaprogramming_derived_method_bundles;
  const std::vector<Objc3IRMetaprogrammingMacroArtifactBundle>
      &metaprogramming_macro_artifact_bundles =
          semantic_lowering_plan.metaprogramming_macro_artifact_bundles;
  const std::vector<Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle>
      &metaprogramming_property_behavior_artifact_bundles =
          semantic_lowering_plan.metaprogramming_property_behavior_artifact_bundles;
  const Objc3MetaprogrammingSynthesizedArtifactEmissionContract
      &metaprogramming_synthesized_artifact_emission_contract =
          semantic_lowering_plan
              .metaprogramming_synthesized_artifact_emission_contract;
  const std::string &metaprogramming_synthesized_artifact_emission_replay_key =
      semantic_lowering_plan
          .metaprogramming_synthesized_artifact_emission_replay_key;
  const Objc3OwnershipSystemExtensionLoweringContract
      &ownership_system_extension_lowering_contract =
          semantic_lowering_plan.ownership_system_extension_lowering_contract;
  const std::string &ownership_system_extension_lowering_replay_key =
      semantic_lowering_plan.ownership_system_extension_lowering_replay_key;
  const std::string &ownership_borrowed_retainable_abi_completion_replay_key =
      semantic_lowering_plan
          .ownership_borrowed_retainable_abi_completion_replay_key;
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
      &concurrency_async_continuation_lowering_contract =
          semantic_lowering_plan
              .concurrency_async_continuation_lowering_contract;
  const Objc3AwaitLoweringSuspensionStateLoweringContract
      &concurrency_await_lowering_suspension_state_lowering_contract =
          semantic_lowering_plan
              .concurrency_await_lowering_suspension_state_lowering_contract;
  const std::string &concurrency_async_continuation_lowering_replay_key =
      semantic_lowering_plan.concurrency_async_continuation_lowering_replay_key;
  const std::string
      &concurrency_await_lowering_suspension_state_lowering_replay_key =
          semantic_lowering_plan
              .concurrency_await_lowering_suspension_state_lowering_replay_key;
  const Objc3ActorIsolationSendabilityLoweringContract
      &concurrency_actor_isolation_sendability_lowering_contract =
          semantic_lowering_plan
              .concurrency_actor_isolation_sendability_lowering_contract;
  const Objc3TaskRuntimeInteropCancellationLoweringContract
      &concurrency_task_runtime_interop_cancellation_lowering_contract =
          semantic_lowering_plan
              .concurrency_task_runtime_interop_cancellation_lowering_contract;
  const std::string
      &concurrency_task_runtime_interop_cancellation_lowering_replay_key =
          semantic_lowering_plan
              .concurrency_task_runtime_interop_cancellation_lowering_replay_key;
  const Objc3ConcurrencyReplayRaceGuardLoweringContract
      &concurrency_concurrency_replay_race_guard_lowering_contract =
          semantic_lowering_plan
              .concurrency_concurrency_replay_race_guard_lowering_contract;
  const std::string
      &concurrency_concurrency_replay_race_guard_lowering_replay_key =
          semantic_lowering_plan
              .concurrency_concurrency_replay_race_guard_lowering_replay_key;
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
  const Objc3FrontendArtifactRuntimeRegistrationPlan runtime_registration_plan =
      BuildObjc3FrontendArtifactRuntimeRegistrationPlan(
          input_path, program, pipeline_result, options, runtime_metadata_plan);
  const Objc3RuntimeSupportLibraryLinkWiringSummary
      &runtime_support_library_link_wiring =
          runtime_registration_plan.runtime_support_library_link_wiring;
  const Objc3RuntimeTranslationUnitRegistrationManifestSummary
      &runtime_translation_unit_registration_manifest =
          runtime_registration_plan
              .runtime_translation_unit_registration_manifest;
  const std::string &translation_unit_identity_key =
      runtime_registration_plan.translation_unit_identity_key;
  const Objc3RuntimeStartupBootstrapInvariantSummary
      &runtime_startup_bootstrap_invariants =
          runtime_registration_plan.runtime_startup_bootstrap_invariants;
  const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api =
      runtime_registration_plan.runtime_bootstrap_api;
  const Objc3RuntimeBootstrapSemanticsSummary &runtime_bootstrap_semantics =
      runtime_registration_plan.runtime_bootstrap_semantics;
  const Objc3RuntimeBootstrapLoweringSummary &runtime_bootstrap_lowering =
      runtime_registration_plan.runtime_bootstrap_lowering;
  const Objc3RuntimeBootstrapFailureRestartSemanticsSummary
      &runtime_bootstrap_failure_restart_semantics =
          runtime_registration_plan
              .runtime_bootstrap_failure_restart_semantics;
  const objc3::artifacts::frontend::Objc3FrontendArtifactConformanceReportPlan
      conformance_report_plan =
          objc3::artifacts::frontend::
              BuildObjc3FrontendArtifactConformanceReportPlan(
                  options, pipeline_result,
                  tooling_feature_specific_fixit_synthesis_summary);
  if (!conformance_report_plan.post_pipeline_failure.empty()) {
    record_post_pipeline_failure(
        conformance_report_plan.post_pipeline_failure.code.c_str(),
        conformance_report_plan.post_pipeline_failure.message);
  }
  const Objc3ToolingLegacyCanonicalMigrationSemanticsSummary
      &tooling_legacy_canonical_migration_semantics_summary =
          conformance_report_plan
              .tooling_legacy_canonical_migration_semantics_summary;
  const Objc3ToolingMachineReadableConformanceReportContractSummary
      &tooling_machine_readable_conformance_report_contract_summary =
          conformance_report_plan
              .tooling_machine_readable_conformance_report_contract_summary;
  const Objc3FrontendArtifactCoreLoweringPlan core_lowering_plan =
      BuildObjc3FrontendArtifactCoreLoweringPlan(
          program, pipeline_result, options,
          type_system_type_semantic_model_summary,
          control_flow_control_flow_semantic_model_summary,
          runtime_bootstrap_api);
  for (const auto &failure : core_lowering_plan.post_pipeline_failures) {
    record_post_pipeline_failure(failure.code.c_str(), failure.message);
  }
  const Objc3PropertySynthesisIvarBindingContract
      &property_synthesis_ivar_binding_contract =
          core_lowering_plan.property_synthesis_ivar_binding_contract;
  const auto &property_synthesis_ivar_binding_snapshot =
      core_lowering_plan.property_synthesis_ivar_binding_snapshot;
  const std::string &property_synthesis_ivar_binding_replay_key =
      core_lowering_plan.property_synthesis_ivar_binding_replay_key;
  const Objc3PropertySynthesisIvarBindingSummary
      &property_synthesis_ivar_binding_summary =
          core_lowering_plan.property_synthesis_ivar_binding_summary;
  // export-legality anchor: manifest sema surfaces must publish the
  // canonical sema property-synthesis/ivar-binding summary rather than the
  // lowering fail-closed contract used for later replay keys.
  const bool property_synthesis_ivar_binding_handoff_deterministic =
      core_lowering_plan.property_synthesis_ivar_binding_handoff_deterministic;
  const Objc3IdClassSelObjectPointerTypecheckContract
      &id_class_sel_object_pointer_typecheck_contract =
          core_lowering_plan.id_class_sel_object_pointer_typecheck_contract;
  const auto &id_class_sel_object_pointer_typecheck_snapshot =
      core_lowering_plan.id_class_sel_object_pointer_typecheck_snapshot;
  const std::string &id_class_sel_object_pointer_typecheck_replay_key =
      core_lowering_plan.id_class_sel_object_pointer_typecheck_replay_key;
  const Objc3DispatchSurfaceClassificationContract
      &dispatch_surface_classification_contract =
          core_lowering_plan.dispatch_surface_classification_contract;
  const auto &dispatch_surface_classification_snapshot =
      core_lowering_plan.dispatch_surface_classification_snapshot;
  const std::string &dispatch_surface_classification_replay_key =
      core_lowering_plan.dispatch_surface_classification_replay_key;
  const Objc3MessageSendSelectorLoweringContract
      &message_send_selector_lowering_contract =
          core_lowering_plan.message_send_selector_lowering_contract;
  const auto &message_send_selector_lowering_snapshot =
      core_lowering_plan.message_send_selector_lowering_snapshot;
  const Objc3DispatchAbiMarshallingContract &dispatch_abi_marshalling_contract =
      core_lowering_plan.dispatch_abi_marshalling_contract;
  const auto &dispatch_abi_marshalling_snapshot =
      core_lowering_plan.dispatch_abi_marshalling_snapshot;
  const Objc3NilReceiverSemanticsFoldabilityContract
      &nil_receiver_semantics_foldability_contract =
          core_lowering_plan.nil_receiver_semantics_foldability_contract;
  const auto &nil_receiver_semantics_foldability_snapshot =
      core_lowering_plan.nil_receiver_semantics_foldability_snapshot;
  const Objc3ControlFlowControlFlowSafetyLoweringContract
      &control_flow_control_flow_safety_lowering_contract =
          core_lowering_plan.control_flow_control_flow_safety_lowering_contract;
  const std::string &control_flow_control_flow_safety_lowering_replay_key =
      core_lowering_plan.control_flow_control_flow_safety_lowering_replay_key;
  const Objc3SuperDispatchMethodFamilyContract
      &super_dispatch_method_family_contract =
          core_lowering_plan.super_dispatch_method_family_contract;
  const auto &super_dispatch_method_family_snapshot =
      core_lowering_plan.super_dispatch_method_family_snapshot;
  const std::string &super_dispatch_method_family_replay_key =
      core_lowering_plan.super_dispatch_method_family_replay_key;
  const Objc3RuntimeLinkHostLinkContract &runtime_link_host_link_contract =
      core_lowering_plan.runtime_link_host_link_contract;
  const auto &runtime_link_host_link_snapshot =
      core_lowering_plan.runtime_link_host_link_snapshot;
  const std::string &runtime_link_host_link_replay_key =
      core_lowering_plan.runtime_link_host_link_replay_key;
  // dispatch lowering ABI freeze anchor: lane-C now publishes the
  // canonical runtime-dispatch cutover boundary separately from the historical
  // dispatch-host-link packet so C002 can swap call emission over without
  // redefining the selector lookup/handle or argument-slot ABI ad hoc.
  const Objc3RuntimeDispatchLoweringAbiContract
      &runtime_dispatch_lowering_abi_contract =
          core_lowering_plan.runtime_dispatch_lowering_abi_contract;
  const auto &runtime_dispatch_lowering_abi_snapshot =
      core_lowering_plan.runtime_dispatch_lowering_abi_snapshot;
  const std::string &runtime_dispatch_lowering_abi_replay_key =
      core_lowering_plan.runtime_dispatch_lowering_abi_replay_key;
  const Objc3FrontendArtifactOwnershipAwareLoweringPlan
      ownership_aware_lowering_plan =
          BuildObjc3FrontendArtifactOwnershipAwareLoweringPlan(
              pipeline_result, metadata_only_ir_emission_mode);
  for (const auto &failure :
       ownership_aware_lowering_plan.post_pipeline_failures) {
    record_post_pipeline_failure(failure.code.c_str(), failure.message);
  }
  const Objc3OwnershipQualifierLoweringContract
      &ownership_qualifier_lowering_contract =
          ownership_aware_lowering_plan
              .ownership_qualifier_lowering_contract;
  const std::string &ownership_qualifier_lowering_replay_key =
      ownership_aware_lowering_plan.ownership_qualifier_lowering_replay_key;
  const Objc3RetainReleaseOperationLoweringContract
      &retain_release_operation_lowering_contract =
          ownership_aware_lowering_plan
              .retain_release_operation_lowering_contract;
  const std::string &retain_release_operation_lowering_replay_key =
      ownership_aware_lowering_plan
          .retain_release_operation_lowering_replay_key;
  const Objc3AutoreleasePoolScopeLoweringContract
      &autoreleasepool_scope_lowering_contract =
          ownership_aware_lowering_plan
              .autoreleasepool_scope_lowering_contract;
  const std::string &autoreleasepool_scope_lowering_replay_key =
      ownership_aware_lowering_plan
          .autoreleasepool_scope_lowering_replay_key;
  const Objc3WeakUnownedSemanticsLoweringContract
      &weak_unowned_semantics_lowering_contract =
          ownership_aware_lowering_plan
              .weak_unowned_semantics_lowering_contract;
  const std::string &weak_unowned_semantics_lowering_replay_key =
      ownership_aware_lowering_plan
          .weak_unowned_semantics_lowering_replay_key;
  const Objc3ArcDiagnosticsFixitLoweringContract
      &arc_diagnostics_fixit_lowering_contract =
          ownership_aware_lowering_plan
              .arc_diagnostics_fixit_lowering_contract;
  const std::string &arc_diagnostics_fixit_lowering_replay_key =
      ownership_aware_lowering_plan
          .arc_diagnostics_fixit_lowering_replay_key;
  const Objc3OwnershipAwareLoweringBehaviorScaffold
      &ownership_aware_lowering_behavior_scaffold =
          ownership_aware_lowering_plan
              .ownership_aware_lowering_behavior_scaffold;
  const Objc3FrontendArtifactBlockLoweringPlan block_lowering_plan =
      BuildObjc3FrontendArtifactBlockLoweringPlan(pipeline_result);
  for (const auto &failure : block_lowering_plan.post_pipeline_failures) {
    record_post_pipeline_failure(failure.code.c_str(), failure.message);
  }
  const Objc3BlockLiteralCaptureLoweringContract
      &block_literal_capture_lowering_contract =
          block_lowering_plan.block_literal_capture_lowering_contract;
  const std::string &block_literal_capture_lowering_replay_key =
      block_lowering_plan.block_literal_capture_lowering_replay_key;
  const Objc3BlockSourceModelCompletionContract
      &block_source_model_completion_contract =
          block_lowering_plan.block_source_model_completion_contract;
  const std::string &block_source_model_completion_replay_key =
      block_lowering_plan.block_source_model_completion_replay_key;
  const Objc3BlockSourceStorageAnnotationContract
      &block_source_storage_annotation_contract =
          block_lowering_plan.block_source_storage_annotation_contract;
  const std::string &block_source_storage_annotation_replay_key =
      block_lowering_plan.block_source_storage_annotation_replay_key;
  const Objc3BlockAbiInvokeTrampolineLoweringContract
      &block_abi_invoke_trampoline_lowering_contract =
          block_lowering_plan.block_abi_invoke_trampoline_lowering_contract;
  const std::string &block_abi_invoke_trampoline_lowering_replay_key =
      block_lowering_plan.block_abi_invoke_trampoline_lowering_replay_key;
  const Objc3BlockStorageEscapeLoweringContract
      &block_storage_escape_lowering_contract =
          block_lowering_plan.block_storage_escape_lowering_contract;
  const std::string &block_storage_escape_lowering_replay_key =
      block_lowering_plan.block_storage_escape_lowering_replay_key;
  const Objc3BlockCopyDisposeLoweringContract
      &block_copy_dispose_lowering_contract =
          block_lowering_plan.block_copy_dispose_lowering_contract;
  const std::string &block_copy_dispose_lowering_replay_key =
      block_lowering_plan.block_copy_dispose_lowering_replay_key;
  const Objc3BlockDeterminismPerfBaselineLoweringContract
      &block_determinism_perf_baseline_lowering_contract =
          block_lowering_plan
              .block_determinism_perf_baseline_lowering_contract;
  const std::string &block_determinism_perf_baseline_lowering_replay_key =
      block_lowering_plan
          .block_determinism_perf_baseline_lowering_replay_key;
  const Objc3FrontendArtifactTypeSystemLoweringPlan
      type_system_lowering_plan =
          BuildObjc3FrontendArtifactTypeSystemLoweringPlan(pipeline_result);
  for (const auto &failure :
       type_system_lowering_plan.post_pipeline_failures) {
    record_post_pipeline_failure(failure.code.c_str(), failure.message);
  }
  const Objc3LightweightGenericsConstraintLoweringContract
      &lightweight_generic_constraint_lowering_contract =
          type_system_lowering_plan
              .lightweight_generic_constraint_lowering_contract;
  const std::string &lightweight_generic_constraint_lowering_replay_key =
      type_system_lowering_plan
          .lightweight_generic_constraint_lowering_replay_key;
  const Objc3NullabilityFlowWarningPrecisionLoweringContract
      &nullability_flow_warning_precision_lowering_contract =
          type_system_lowering_plan
              .nullability_flow_warning_precision_lowering_contract;
  const std::string &nullability_flow_warning_precision_lowering_replay_key =
      type_system_lowering_plan
          .nullability_flow_warning_precision_lowering_replay_key;
  const Objc3ProtocolQualifiedObjectTypeLoweringContract
      &protocol_qualified_object_type_lowering_contract =
          type_system_lowering_plan
              .protocol_qualified_object_type_lowering_contract;
  const std::string &protocol_qualified_object_type_lowering_replay_key =
      type_system_lowering_plan
          .protocol_qualified_object_type_lowering_replay_key;
  const Objc3VarianceBridgeCastLoweringContract
      &variance_bridge_cast_lowering_contract =
          type_system_lowering_plan.variance_bridge_cast_lowering_contract;
  const std::string &variance_bridge_cast_lowering_replay_key =
      type_system_lowering_plan.variance_bridge_cast_lowering_replay_key;
  const Objc3GenericMetadataAbiLoweringContract
      &generic_metadata_abi_lowering_contract =
          type_system_lowering_plan.generic_metadata_abi_lowering_contract;
  const std::string &generic_metadata_abi_lowering_replay_key =
      type_system_lowering_plan.generic_metadata_abi_lowering_replay_key;
  const Objc3FrontendArtifactRuntimeImportPlan runtime_import_plan =
      BuildObjc3FrontendArtifactRuntimeImportPlan(
          program,
          pipeline_result,
          options,
          runtime_metadata_source_records,
          runtime_translation_unit_registration_manifest,
          !post_pipeline_failure.empty());
  for (const auto &failure : runtime_import_plan.post_pipeline_failures) {
    record_post_pipeline_failure(failure.code.c_str(), failure.message);
  }
  const Objc3ModuleImportGraphLoweringContract
      &module_import_graph_lowering_contract =
          runtime_import_plan.module_import_graph_lowering_contract;
  const std::string &module_import_graph_lowering_replay_key =
      runtime_import_plan.module_import_graph_lowering_replay_key;
  const Objc3RuntimeAwareImportModuleFrontendClosureSummary
      &runtime_aware_import_module_frontend_closure =
          runtime_import_plan.runtime_aware_import_module_frontend_closure;
  const Objc3CrossModuleRuntimeMetadataSemanticPreservationSummary
      &cross_module_runtime_metadata_semantic_preservation =
          runtime_import_plan
              .cross_module_runtime_metadata_semantic_preservation;
  const std::vector<Objc3ImportedRuntimeModuleSurface>
      &imported_runtime_module_surfaces =
          runtime_import_plan.imported_runtime_module_surfaces;
  const Objc3ImportedRuntimeMetadataSemanticRulesSummary
      &imported_runtime_metadata_semantic_rules =
          runtime_import_plan.imported_runtime_metadata_semantic_rules;
  const bool has_imported_runtime_surface_inputs =
      runtime_import_plan.has_imported_runtime_surface_inputs;
  const Objc3SerializedRuntimeMetadataImportLoweringSummary
      &serialized_runtime_metadata_import_lowering =
          runtime_import_plan.serialized_runtime_metadata_import_lowering;
  const Objc3CrossModuleBuildRuntimeOrchestrationSummary
      &cross_module_build_runtime_orchestration =
          runtime_import_plan.cross_module_build_runtime_orchestration;
  const Objc3FrontendArtifactModuleLoweringPlan module_lowering_plan =
      BuildObjc3FrontendArtifactModuleLoweringPlan(pipeline_result);
  for (const auto &failure : module_lowering_plan.post_pipeline_failures) {
    record_post_pipeline_failure(failure.code.c_str(), failure.message);
  }
  const Objc3NamespaceCollisionShadowingLoweringContract
      &namespace_collision_shadowing_lowering_contract =
          module_lowering_plan.namespace_collision_shadowing_lowering_contract;
  const std::string &namespace_collision_shadowing_lowering_replay_key =
      module_lowering_plan.namespace_collision_shadowing_lowering_replay_key;
  const Objc3PublicPrivateApiPartitionLoweringContract
      &public_private_api_partition_lowering_contract =
          module_lowering_plan.public_private_api_partition_lowering_contract;
  const std::string &public_private_api_partition_lowering_replay_key =
      module_lowering_plan.public_private_api_partition_lowering_replay_key;
  const Objc3IncrementalModuleCacheInvalidationLoweringContract
      &incremental_module_cache_invalidation_lowering_contract =
          module_lowering_plan
              .incremental_module_cache_invalidation_lowering_contract;
  const std::string
      &incremental_module_cache_invalidation_lowering_replay_key =
          module_lowering_plan
              .incremental_module_cache_invalidation_lowering_replay_key;
  const Objc3CrossModuleConformanceLoweringContract
      &cross_module_conformance_lowering_contract =
          module_lowering_plan.cross_module_conformance_lowering_contract;
  const std::string &cross_module_conformance_lowering_replay_key =
      module_lowering_plan.cross_module_conformance_lowering_replay_key;
  const Objc3FrontendArtifactErrorLoweringPlan error_lowering_plan =
      BuildObjc3FrontendArtifactErrorLoweringPlan(pipeline_result);
  for (const auto &failure : error_lowering_plan.post_pipeline_failures) {
    record_post_pipeline_failure(failure.code.c_str(), failure.message);
  }
  const Objc3ThrowsPropagationLoweringContract
      &throws_propagation_lowering_contract =
          error_lowering_plan.throws_propagation_lowering_contract;
  const std::string &throws_propagation_lowering_replay_key =
      error_lowering_plan.throws_propagation_lowering_replay_key;
  const Objc3ResultLikeLoweringContract &result_like_lowering_contract =
      error_lowering_plan.result_like_lowering_contract;
  const std::string &result_like_lowering_replay_key =
      error_lowering_plan.result_like_lowering_replay_key;
  const Objc3NSErrorBridgingLoweringContract
      &ns_error_bridging_lowering_contract =
          error_lowering_plan.ns_error_bridging_lowering_contract;
  const std::string &ns_error_bridging_lowering_replay_key =
      error_lowering_plan.ns_error_bridging_lowering_replay_key;
  const Objc3UnwindCleanupLoweringContract &unwind_cleanup_lowering_contract =
      error_lowering_plan.unwind_cleanup_lowering_contract;
  const std::string &unwind_cleanup_lowering_replay_key =
      error_lowering_plan.unwind_cleanup_lowering_replay_key;
  const bool deterministic_error_handling_throws_abi_propagation_lowering =
      error_lowering_plan
          .deterministic_error_handling_throws_abi_propagation_lowering;
  const std::string
      &error_handling_throws_abi_propagation_lowering_replay_key =
          error_lowering_plan
              .error_handling_throws_abi_propagation_lowering_replay_key;
  const bool runtime_import_artifact_ready =
      IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(
          runtime_aware_import_module_frontend_closure);
  const Objc3FrontendArtifactInteropLoweringPlan interop_lowering_plan =
      BuildObjc3FrontendArtifactInteropLoweringPlan(
          program,
          interop_foreign_import_source_closure_summary,
          interop_cpp_swift_interop_annotation_source_completion_summary,
          interop_interop_semantic_model_summary,
          interop_interop_runtime_parity_summary,
          interop_cpp_interop_interaction_summary,
          interop_swift_interop_isolation_summary,
          error_handling_throws_abi_propagation_lowering_replay_key,
          throws_propagation_lowering_replay_key,
          result_like_lowering_replay_key,
          ns_error_bridging_lowering_replay_key,
          unwind_cleanup_lowering_replay_key,
          deterministic_error_handling_throws_abi_propagation_lowering,
          runtime_import_artifact_ready,
          imported_runtime_module_surfaces);
  for (const auto &failure : interop_lowering_plan.post_pipeline_failures) {
    record_post_pipeline_failure(failure.code.c_str(), failure.message);
  }
  const Objc3InteropInteropLoweringContract
      &interop_interop_lowering_contract =
          interop_lowering_plan.interop_interop_lowering_contract;
  const std::string &interop_interop_lowering_replay_key =
      interop_lowering_plan.interop_interop_lowering_replay_key;
  const Objc3FrontendArtifactPreservationPlan artifact_preservation_plan =
      BuildObjc3FrontendArtifactPreservationPlan(
          runtime_metadata_source_records,
          dispatch_dispatch_control_lowering_replay_key,
          runtime_import_artifact_ready,
          imported_runtime_module_surfaces,
          block_abi_invoke_trampoline_lowering_contract,
          block_storage_escape_lowering_contract,
          block_copy_dispose_lowering_contract,
          runtime_support_library_link_wiring,
          metaprogramming_expansion_lowering_contract,
          metaprogramming_expansion_lowering_replay_key,
          metaprogramming_synthesized_artifact_emission_contract,
          metaprogramming_synthesized_artifact_emission_replay_key,
          metaprogramming_property_behavior_artifact_bundles,
          options);
  const auto &dispatch_dispatch_metadata_interface_preservation_summary =
      artifact_preservation_plan
          .dispatch_dispatch_metadata_interface_preservation_summary;
  const auto dispatch_dispatch_metadata_interface_preservation_snapshot =
      objc3::artifacts::frontend::BuildDispatchMetadataPreservationSnapshot(
          dispatch_dispatch_metadata_interface_preservation_summary);
  const Objc3FrontendArtifactSourceShapePlan source_shape_plan =
      BuildObjc3FrontendArtifactSourceShapePlan(program, pipeline_result);
  for (const auto &failure : source_shape_plan.post_pipeline_failures) {
    record_post_pipeline_failure(failure.code.c_str(), failure.message);
  }
  const std::vector<int> &resolved_global_values =
      source_shape_plan.resolved_global_values;

  std::ostringstream manifest;
  objc3::artifacts::frontend::AppendObjc3FrontendArtifactManifestHeader(
      manifest, input_path, program, pipeline_result, options);
  objc3::artifacts::frontend::AppendObjc3FrontendArtifactManifestPipelineStages(
      manifest, pipeline_result, options, bundle);
  objc3::artifacts::frontend::AppendObjc3FrontendArtifactManifestParseReadiness(
      manifest, bundle);
  objc3::artifacts::frontend::
      AppendObjc3FrontendArtifactManifestSemaPassDiagnostics(manifest,
                                                             pipeline_result);
  objc3::artifacts::frontend::AppendObjc3FrontendArtifactSemaParityManifestFields(
      manifest, pipeline_result,
      IsReadyObjc3SemaParityContractSurface(pipeline_result.sema_parity_surface));
  objc3::artifacts::frontend::WriteRuntimeMetadataPublicationManifestFields(
      manifest, runtime_metadata_source_ownership, runtime_export_legality,
      runtime_export_enforcement, runtime_metadata_section_abi,
      runtime_metadata_section_publication, runtime_metadata_object_inspection);
  objc3::artifacts::frontend::
      AppendObjc3FrontendArtifactExecutableMetadataRuntimeIngestManifestFields(
          manifest, runtime_metadata_plan);
  objc3::artifacts::frontend::
      AppendObjc3FrontendArtifactRuntimeSupportRegistrationManifestFields(
          manifest, runtime_registration_plan);
  objc3::artifacts::frontend::
      AppendObjc3FrontendArtifactRuntimeRegistrationDescriptorManifestFields(
          manifest, runtime_registration_plan);
  objc3::artifacts::frontend::
      AppendObjc3FrontendArtifactRuntimeBootstrapLegalityManifestFields(
          manifest, runtime_registration_plan);
  objc3::artifacts::frontend::WriteRuntimeBootstrapManifestFields(
      manifest, runtime_bootstrap_failure_restart_semantics,
      runtime_bootstrap_api, runtime_bootstrap_semantics,
      runtime_bootstrap_lowering);
  objc3::artifacts::frontend::
      WriteRuntimeStartupBootstrapInvariantManifestFields(
          manifest, runtime_startup_bootstrap_invariants);
  objc3::artifacts::frontend::
      AppendObjc3FrontendArtifactLoweringHandoffManifestFields(
          manifest, core_lowering_plan, ownership_aware_lowering_plan,
          block_lowering_plan, type_system_lowering_plan, runtime_import_plan,
          module_lowering_plan, error_lowering_plan,
          object_pointer_nullability_generics_summary,
          symbol_graph_scope_resolution_summary);
  objc3::artifacts::frontend::WriteObjc3FrontendArtifactSemanticSurfaceManifest(
      manifest, program, pipeline_result, options, function_manifest,
      source_shape_plan, core_lowering_plan, semantic_lowering_plan,
      ownership_aware_lowering_plan, interop_lowering_plan,
      artifact_preservation_plan, type_system_type_semantic_model_summary);
  objc3::artifacts::frontend::WriteObjc3FrontendArtifactPreTailManifestSurfaces(
      manifest, program, pipeline_result, function_manifest,
      core_lowering_plan, semantic_lowering_plan, ownership_aware_lowering_plan,
      block_lowering_plan, type_system_lowering_plan, runtime_import_plan,
      module_lowering_plan, error_lowering_plan, interop_lowering_plan,
      artifact_preservation_plan, conformance_report_plan,
      runtime_metadata_plan, runtime_registration_plan,
      type_system_type_semantic_model_summary);
  objc3::artifacts::frontend::AppendObjc3FrontendArtifactManifestReplayTail(
      manifest, program, pipeline_result, options, function_manifest,
      source_shape_plan, core_lowering_plan, ownership_aware_lowering_plan,
      block_lowering_plan, type_system_lowering_plan, runtime_import_plan,
      module_lowering_plan, error_lowering_plan, interop_lowering_plan,
      runtime_metadata_plan, runtime_registration_plan);

  objc3::artifacts::frontend::PublishObjc3FrontendArtifactBundleOutputs({
      .bundle = bundle,
      .input_path = input_path,
      .pipeline_result = pipeline_result,
      .options = options,
      .program = program,
      .manifest_json = manifest.str(),
      .post_pipeline_failure = post_pipeline_failure,
      .ir_emission_core_feature_impl_surface =
          ir_emission_core_feature_impl_surface,
      .type_system_type_semantic_model_summary =
          type_system_type_semantic_model_summary,
      .conformance_report_plan = conformance_report_plan,
      .semantic_lowering_plan = semantic_lowering_plan,
      .core_lowering_plan = core_lowering_plan,
      .ownership_aware_lowering_plan = ownership_aware_lowering_plan,
      .block_lowering_plan = block_lowering_plan,
      .type_system_lowering_plan = type_system_lowering_plan,
      .runtime_import_plan = runtime_import_plan,
      .module_lowering_plan = module_lowering_plan,
      .error_lowering_plan = error_lowering_plan,
      .interop_lowering_plan = interop_lowering_plan,
      .artifact_preservation_plan = artifact_preservation_plan,
      .runtime_metadata_plan = runtime_metadata_plan,
      .runtime_registration_plan = runtime_registration_plan});

  return bundle;
}
