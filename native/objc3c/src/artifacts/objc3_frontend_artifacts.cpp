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
#include "artifacts/objc3_frontend_artifact_bundle_outputs.h"
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
#include "artifacts/objc3_frontend_artifact_dispatch_runtime_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_error_handling_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_error_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_error_metadata.h"
#include "artifacts/objc3_frontend_artifact_executable_metadata_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_function_manifest.h"
#include "artifacts/objc3_frontend_artifact_interop_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_interop_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_interop_metadata.h"
#include "artifacts/objc3_frontend_artifact_ir_emission_completion.h"
#include "artifacts/objc3_frontend_artifact_lowering_contracts.h"
#include "artifacts/objc3_frontend_artifact_lowering_replay_manifest.h"
#include "artifacts/objc3_frontend_artifact_manifest_header.h"
#include "artifacts/objc3_frontend_artifact_manifest_pipeline.h"
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
#include "artifacts/objc3_frontend_artifact_preservation_plan.h"
#include "artifacts/objc3_frontend_artifact_runtime_bootstrap_manifest_fields.h"
#include "artifacts/objc3_frontend_artifact_runtime_bootstrap_private_manifest_fields.h"
#include "artifacts/objc3_frontend_artifact_runtime_metadata_contract_metadata.h"
#include "artifacts/objc3_frontend_artifact_runtime_metadata_manifest_fields.h"
#include "artifacts/objc3_frontend_artifact_runtime_metadata_plan.h"
#include "artifacts/objc3_frontend_artifact_runtime_import_plan.h"
#include "artifacts/objc3_frontend_artifact_runtime_registration_plan.h"
#include "artifacts/objc3_frontend_artifact_runtime_startup_bootstrap_invariant_manifest_fields.h"
#include "artifacts/objc3_frontend_artifact_sanity.h"
#include "artifacts/objc3_frontend_artifact_semantic_closure_metadata.h"
#include "artifacts/objc3_frontend_artifact_semantic_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_semantic_summary_manifest_surfaces.h"
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
#include "ir/objc3_ir_emitter.h"
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
  const std::string
      &concurrency_actor_isolation_sendability_lowering_replay_key =
          semantic_lowering_plan
              .concurrency_actor_isolation_sendability_lowering_replay_key;
  const Objc3ActorLoweringMetadataContract
      &concurrency_actor_lowering_metadata_contract =
          semantic_lowering_plan.concurrency_actor_lowering_metadata_contract;
  const std::string &concurrency_actor_lowering_metadata_replay_key =
      semantic_lowering_plan.concurrency_actor_lowering_metadata_replay_key;
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
  const Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary
      &frontend_compatibility_strictness_claim_semantics =
          conformance_report_plan
              .frontend_compatibility_strictness_claim_semantics;
  const Objc3ToolingLegacyCanonicalMigrationSemanticsSummary
      &tooling_legacy_canonical_migration_semantics_summary =
          conformance_report_plan
              .tooling_legacy_canonical_migration_semantics_summary;
  const Objc3VersionedConformanceReportLoweringSummary
      &versioned_conformance_report_lowering =
          conformance_report_plan.versioned_conformance_report_lowering;
  const Objc3ToolingMachineReadableConformanceReportContractSummary
      &tooling_machine_readable_conformance_report_contract_summary =
          conformance_report_plan
              .tooling_machine_readable_conformance_report_contract_summary;
  const Objc3ToolingFeatureAwareConformanceReportEmissionSummary
      &tooling_feature_aware_conformance_report_emission_summary =
          conformance_report_plan
              .tooling_feature_aware_conformance_report_emission_summary;
  const Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary
      &tooling_corpus_sharding_release_evidence_packaging_summary =
          conformance_report_plan
              .tooling_corpus_sharding_release_evidence_packaging_summary;
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
  const std::string &id_class_sel_object_pointer_typecheck_replay_key =
      core_lowering_plan.id_class_sel_object_pointer_typecheck_replay_key;
  const Objc3DispatchSurfaceClassificationContract
      &dispatch_surface_classification_contract =
          core_lowering_plan.dispatch_surface_classification_contract;
  const std::string &dispatch_surface_classification_replay_key =
      core_lowering_plan.dispatch_surface_classification_replay_key;
  const Objc3MessageSendSelectorLoweringContract
      &message_send_selector_lowering_contract =
          core_lowering_plan.message_send_selector_lowering_contract;
  const std::string &message_send_selector_lowering_replay_key =
      core_lowering_plan.message_send_selector_lowering_replay_key;
  const Objc3DispatchAbiMarshallingContract &dispatch_abi_marshalling_contract =
      core_lowering_plan.dispatch_abi_marshalling_contract;
  const std::string &dispatch_abi_marshalling_replay_key =
      core_lowering_plan.dispatch_abi_marshalling_replay_key;
  const Objc3NilReceiverSemanticsFoldabilityContract
      &nil_receiver_semantics_foldability_contract =
          core_lowering_plan.nil_receiver_semantics_foldability_contract;
  const std::string &nil_receiver_semantics_foldability_replay_key =
      core_lowering_plan.nil_receiver_semantics_foldability_replay_key;
  const Objc3TypeSystemOptionalKeypathLoweringContract
      &type_system_optional_keypath_lowering_contract =
          core_lowering_plan.type_system_optional_keypath_lowering_contract;
  const std::string &type_system_optional_keypath_lowering_replay_key =
      core_lowering_plan.type_system_optional_keypath_lowering_replay_key;
  const Objc3ControlFlowControlFlowSafetyLoweringContract
      &control_flow_control_flow_safety_lowering_contract =
          core_lowering_plan.control_flow_control_flow_safety_lowering_contract;
  const std::string &control_flow_control_flow_safety_lowering_replay_key =
      core_lowering_plan.control_flow_control_flow_safety_lowering_replay_key;
  const Objc3SuperDispatchMethodFamilyContract
      &super_dispatch_method_family_contract =
          core_lowering_plan.super_dispatch_method_family_contract;
  const std::string &super_dispatch_method_family_replay_key =
      core_lowering_plan.super_dispatch_method_family_replay_key;
  const Objc3RuntimeLinkHostLinkContract &runtime_link_host_link_contract =
      core_lowering_plan.runtime_link_host_link_contract;
  const std::string &runtime_link_host_link_replay_key =
      core_lowering_plan.runtime_link_host_link_replay_key;
  // dispatch lowering ABI freeze anchor: lane-C now publishes the
  // canonical runtime-dispatch cutover boundary separately from the historical
  // dispatch-host-link packet so C002 can swap call emission over without
  // redefining the selector lookup/handle or argument-slot ABI ad hoc.
  const Objc3RuntimeDispatchLoweringAbiContract
      &runtime_dispatch_lowering_abi_contract =
          core_lowering_plan.runtime_dispatch_lowering_abi_contract;
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
  const Objc3RuntimeMetadataSourceRecordSet
      &serialized_runtime_metadata_reuse_records =
          runtime_import_plan.serialized_runtime_metadata_reuse_records;
  const Objc3SerializedRuntimeMetadataArtifactReuseSummary
      &serialized_runtime_metadata_artifact_reuse =
          runtime_import_plan.serialized_runtime_metadata_artifact_reuse;
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
  const auto &error_handling_result_and_bridging_artifact_replay_summary =
      interop_lowering_plan
          .error_handling_result_and_bridging_artifact_replay_summary;
  const Objc3InteropForeignSurfaceInterfacePreservationSummary
      &interop_foreign_surface_interface_preservation_summary =
          interop_lowering_plan
              .interop_foreign_surface_interface_preservation_summary;
  const Objc3InteropInteropLoweringContract
      &interop_interop_lowering_contract =
          interop_lowering_plan.interop_interop_lowering_contract;
  const std::string &interop_interop_lowering_replay_key =
      interop_lowering_plan.interop_interop_lowering_replay_key;
  const Objc3InteropForeignCallLifetimeLoweringContract
      &interop_foreign_call_lifetime_lowering_contract =
          interop_lowering_plan
              .interop_foreign_call_lifetime_lowering_contract;
  const std::string &interop_foreign_call_lifetime_lowering_replay_key =
      interop_lowering_plan
          .interop_foreign_call_lifetime_lowering_replay_key;
  const std::string &interop_ffi_metadata_interface_preservation_replay_key =
      interop_lowering_plan
          .interop_ffi_metadata_interface_preservation_replay_key;
  const Objc3InteropFfiMetadataInterfacePreservationContract
      &interop_ffi_metadata_interface_preservation_contract =
          interop_lowering_plan
              .interop_ffi_metadata_interface_preservation_contract;
  const Objc3InteropHeaderModuleBridgeGenerationSummary
      &interop_header_module_bridge_generation_summary =
          interop_lowering_plan
              .interop_header_module_bridge_generation_summary;
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
  const auto &runtime_block_ownership_artifact_preservation_summary =
      artifact_preservation_plan
          .runtime_block_ownership_artifact_preservation_summary;
  const auto &runtime_storage_reflection_artifact_preservation_summary =
      artifact_preservation_plan
          .runtime_storage_reflection_artifact_preservation_summary;
  const auto &metaprogramming_module_interface_replay_preservation_summary =
      artifact_preservation_plan
          .metaprogramming_module_interface_replay_preservation_summary;
  const auto
      &metaprogramming_macro_host_process_cache_runtime_integration_summary =
          artifact_preservation_plan
              .metaprogramming_macro_host_process_cache_runtime_integration_summary;
  const Objc3FrontendArtifactSourceShapePlan source_shape_plan =
      BuildObjc3FrontendArtifactSourceShapePlan(program, pipeline_result);
  for (const auto &failure : source_shape_plan.post_pipeline_failures) {
    record_post_pipeline_failure(failure.code.c_str(), failure.message);
  }
  const auto &interface_implementation_method_counts =
      source_shape_plan.interface_implementation_method_counts;
  const std::size_t interface_class_method_symbols =
      interface_implementation_method_counts.interface_class_method_symbols;
  const std::size_t interface_instance_method_symbols =
      interface_implementation_method_counts.interface_instance_method_symbols;
  const std::size_t implementation_class_method_symbols =
      interface_implementation_method_counts.implementation_class_method_symbols;
  const std::size_t implementation_instance_method_symbols =
      interface_implementation_method_counts
          .implementation_instance_method_symbols;
  const std::size_t implementation_methods_with_body =
      interface_implementation_method_counts.implementation_methods_with_body;
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
  manifest << ",\"deterministic_atomic_memory_order_mapping\":"
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
           << property_attribute_summary.property_setter_selector_entries;
  objc3::artifacts::frontend::WriteRuntimeMetadataPublicationManifestFields(
      manifest, runtime_metadata_source_ownership, runtime_export_legality,
      runtime_export_enforcement, runtime_metadata_section_abi,
      runtime_metadata_section_publication, runtime_metadata_object_inspection);
  manifest << ",\"executable_metadata_debug_projection_contract_id\":\""
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
          << "\"";
  objc3::artifacts::frontend::WriteRuntimeBootstrapManifestFields(
      manifest, runtime_bootstrap_failure_restart_semantics,
      runtime_bootstrap_api, runtime_bootstrap_semantics,
      runtime_bootstrap_lowering);
  objc3::artifacts::frontend::
      WriteRuntimeStartupBootstrapInvariantManifestFields(
          manifest, runtime_startup_bootstrap_invariants);
  manifest << ",\"deterministic_property_synthesis_ivar_binding_handoff\":"
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
                  type_system_type_semantic_model_summary);
  objc3::artifacts::frontend::
      WriteObjc3FrontendArtifactConformanceReportManifestSurfaces(
          manifest, conformance_report_plan);
  WriteExecutableRuntimeMetadataManifestSurfaces(
      manifest, executable_metadata_source_graph,
      runtime_metadata_source_to_section_matrix,
      executable_metadata_semantic_consistency_boundary,
      executable_metadata_semantic_validation_surface,
      executable_metadata_lowering_handoff_surface,
      executable_metadata_typed_lowering_handoff,
      executable_metadata_debug_projection,
      executable_metadata_runtime_ingest_packaging_contract,
      executable_metadata_runtime_ingest_binary_boundary);
  manifest
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
           << BuildRuntimeBootstrapApiSummaryJson(runtime_bootstrap_api);
  objc3::artifacts::frontend::WriteRuntimeBootstrapPrivateManifestFields(
      manifest, runtime_bootstrap_api, runtime_bootstrap_lowering,
      runtime_bootstrap_failure_restart_semantics,
      runtime_registration_descriptor_frontend_closure,
      translation_unit_identity_key);
  manifest
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
           << "}";
  WriteDispatchRuntimeAbiManifestSurfaces(
      manifest, dispatch_surface_classification_contract,
      dispatch_surface_classification_replay_key,
      message_send_selector_lowering_contract,
      message_send_selector_lowering_replay_key,
      dispatch_abi_marshalling_contract,
      dispatch_abi_marshalling_replay_key,
      nil_receiver_semantics_foldability_contract,
      nil_receiver_semantics_foldability_replay_key,
      super_dispatch_method_family_contract,
      super_dispatch_method_family_replay_key,
      runtime_link_host_link_contract,
      runtime_link_host_link_replay_key,
      runtime_dispatch_lowering_abi_contract,
      runtime_dispatch_lowering_abi_replay_key);
  WriteOwnershipReleaseManifestSurfaces(
      manifest, ownership_qualifier_lowering_contract,
      ownership_qualifier_lowering_replay_key,
      retain_release_operation_lowering_contract,
      retain_release_operation_lowering_replay_key,
      autoreleasepool_scope_lowering_contract,
      autoreleasepool_scope_lowering_replay_key,
      weak_unowned_semantics_lowering_contract,
      weak_unowned_semantics_lowering_replay_key,
      arc_diagnostics_fixit_lowering_contract,
      arc_diagnostics_fixit_lowering_replay_key);
  WriteBlockManifestSurfaces(
      manifest, block_literal_capture_lowering_contract,
      block_literal_capture_lowering_replay_key,
      block_source_model_completion_contract,
      block_source_model_completion_replay_key,
      block_source_storage_annotation_contract,
      block_source_storage_annotation_replay_key,
      block_abi_invoke_trampoline_lowering_contract,
      block_abi_invoke_trampoline_lowering_replay_key,
      block_storage_escape_lowering_contract,
      block_storage_escape_lowering_replay_key,
      block_copy_dispose_lowering_contract,
      block_copy_dispose_lowering_replay_key,
      block_determinism_perf_baseline_lowering_contract,
      block_determinism_perf_baseline_lowering_replay_key);
  WriteTypeSystemManifestSurfaces(
      manifest, lightweight_generic_constraint_lowering_contract,
      lightweight_generic_constraint_lowering_replay_key,
      nullability_flow_warning_precision_lowering_contract,
      nullability_flow_warning_precision_lowering_replay_key,
      protocol_qualified_object_type_lowering_contract,
      protocol_qualified_object_type_lowering_replay_key,
      variance_bridge_cast_lowering_contract,
      variance_bridge_cast_lowering_replay_key,
      generic_metadata_abi_lowering_contract,
      generic_metadata_abi_lowering_replay_key);
  manifest
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
                   type_system_optional_keypath_lowering_replay_key);
  WriteCrossModuleManifestSurfaces(
      manifest, program, pipeline_result.parser_contract_snapshot,
      module_import_graph_lowering_contract,
      module_import_graph_lowering_replay_key,
      runtime_aware_import_module_frontend_closure,
      cross_module_runtime_metadata_semantic_preservation,
      imported_runtime_metadata_semantic_rules,
      serialized_runtime_metadata_import_lowering,
      serialized_runtime_metadata_artifact_reuse,
      cross_module_build_runtime_orchestration,
      namespace_collision_shadowing_lowering_contract,
      namespace_collision_shadowing_lowering_replay_key,
      public_private_api_partition_lowering_contract,
      public_private_api_partition_lowering_replay_key,
      incremental_module_cache_invalidation_lowering_contract,
      incremental_module_cache_invalidation_lowering_replay_key,
      cross_module_conformance_lowering_contract,
      cross_module_conformance_lowering_replay_key);
  WriteErrorHandlingManifestSurfaces(
      manifest, throws_propagation_lowering_contract,
      throws_propagation_lowering_replay_key, result_like_lowering_contract,
      result_like_lowering_replay_key, ns_error_bridging_lowering_contract,
      ns_error_bridging_lowering_replay_key, unwind_cleanup_lowering_contract,
      unwind_cleanup_lowering_replay_key,
      deterministic_error_handling_throws_abi_propagation_lowering,
      error_handling_result_and_bridging_artifact_replay_summary);
  WriteSourceClosureManifestSurfaces(
      manifest, object_pointer_nullability_generics_summary,
      type_system_type_source_closure_summary,
      control_flow_control_flow_source_closure_summary,
      error_handling_error_source_closure_summary,
      concurrency_async_source_closure_summary,
      ownership_system_extension_source_closure_summary,
      ownership_cleanup_resource_capture_source_completion_summary,
      ownership_retainable_c_family_source_completion_summary,
      dispatch_dispatch_intent_source_closure_summary,
      dispatch_dispatch_intent_source_completion_summary,
      metaprogramming_metaprogramming_source_closure_summary,
      metaprogramming_macro_package_provenance_source_completion_summary,
      metaprogramming_property_behavior_source_completion_summary,
      interop_foreign_import_source_closure_summary,
      interop_cpp_swift_interop_annotation_source_completion_summary);
  WriteToolingManifestSurfaces(
      manifest, tooling_diagnostics_migrator_source_inventory_summary,
      tooling_migration_canonicalization_source_completion_summary,
      tooling_diagnostic_taxonomy_portability_contract_summary,
      tooling_feature_specific_fixit_synthesis_summary,
      tooling_legacy_canonical_migration_semantics_summary,
      tooling_machine_readable_conformance_report_contract_summary,
      tooling_feature_aware_conformance_report_emission_summary,
      tooling_corpus_sharding_release_evidence_packaging_summary);
  WriteInteropManifestSurfaces(
      manifest, interop_interop_semantic_model_summary,
      interop_interop_runtime_parity_summary,
      interop_cpp_interop_interaction_summary,
      interop_swift_interop_isolation_summary,
      interop_foreign_surface_interface_preservation_summary,
      interop_header_module_bridge_generation_summary,
      interop_interop_lowering_contract,
      interop_interop_lowering_replay_key,
      interop_foreign_call_lifetime_lowering_contract,
      interop_foreign_call_lifetime_lowering_replay_key,
      interop_ffi_metadata_interface_preservation_contract,
      interop_ffi_metadata_interface_preservation_replay_key);
  WriteMetaprogrammingManifestSurfaces(
      manifest, metaprogramming_expansion_behavior_semantic_model_summary,
      metaprogramming_derive_expansion_inventory_summary,
      metaprogramming_macro_safety_sandbox_determinism_summary,
      metaprogramming_property_behavior_legality_compatibility_summary,
      metaprogramming_property_behavior_source_completion_summary,
      metaprogramming_expansion_lowering_contract,
      metaprogramming_expansion_lowering_replay_key,
      metaprogramming_synthesized_artifact_emission_contract,
      metaprogramming_synthesized_artifact_emission_replay_key,
      metaprogramming_module_interface_replay_preservation_summary);
  WriteDispatchManifestSurfaces(
      manifest, dispatch_dispatch_intent_semantic_model_summary,
      dispatch_dispatch_intent_legality_summary,
      dispatch_dispatch_intent_compatibility_summary,
      dispatch_dispatch_control_lowering_contract,
      dispatch_dispatch_control_lowering_replay_key,
      dispatch_dispatch_metadata_interface_preservation_summary);
  WriteConcurrencyManifestSurfaces(
      manifest, concurrency_actor_member_isolation_source_closure_summary,
      concurrency_actor_isolation_sendable_semantic_model_summary,
      concurrency_actor_isolation_sendability_enforcement_summary,
      concurrency_actor_race_hazard_escape_diagnostics_summary,
      concurrency_actor_lowering_metadata_contract,
      concurrency_actor_lowering_metadata_replay_key,
      concurrency_task_group_cancellation_source_closure_summary,
      concurrency_async_effect_suspension_semantic_model_summary,
      concurrency_task_executor_cancellation_semantic_model_summary);
  WriteOwnershipManifestSurfaces(
      manifest, ownership_system_extension_semantic_model_summary,
      effects_ownership_semantic_model_summary,
      cross_module_semantic_contracts_diagnostics_summary,
      ownership_resource_move_use_after_move_semantics_summary,
      ownership_borrowed_pointer_escape_analysis_summary,
      ownership_capture_list_retainable_family_legality_completion_summary,
      ownership_system_extension_lowering_contract,
      ownership_system_extension_source_closure_summary,
      ownership_retainable_c_family_source_completion_summary,
      ownership_system_extension_lowering_replay_key,
      ownership_borrowed_retainable_abi_completion_replay_key);
  WriteConcurrencyRuntimeManifestSurfaces(
      manifest, concurrency_structured_task_cancellation_semantic_summary,
      concurrency_executor_hop_affinity_compatibility_summary,
      concurrency_await_suspension_resume_semantic_summary,
      concurrency_async_diagnostics_compatibility_summary,
      concurrency_async_continuation_lowering_contract,
      concurrency_await_lowering_suspension_state_lowering_contract,
      concurrency_async_continuation_lowering_replay_key,
      concurrency_await_lowering_suspension_state_lowering_replay_key,
      concurrency_task_executor_cancellation_semantic_model_summary,
      concurrency_actor_isolation_sendability_lowering_contract,
      concurrency_actor_isolation_sendability_lowering_replay_key,
      concurrency_task_runtime_interop_cancellation_lowering_contract,
      concurrency_task_runtime_interop_cancellation_lowering_replay_key,
      concurrency_concurrency_replay_race_guard_lowering_contract,
      concurrency_concurrency_replay_race_guard_lowering_replay_key,
      concurrency_async_source_closure_summary,
      control_flow_control_flow_safety_lowering_contract,
      control_flow_control_flow_safety_lowering_replay_key,
      autoreleasepool_scope_lowering_contract,
      autoreleasepool_scope_lowering_replay_key);
  WriteSemanticSummaryManifestSurfaces(
      manifest, error_handling_error_semantic_model_summary,
      error_handling_try_do_catch_semantic_summary,
      error_handling_error_bridge_legality_summary,
      control_flow_control_flow_semantic_model_summary,
      type_system_type_semantic_model_summary,
      symbol_graph_scope_resolution_summary, function_manifest);
  manifest << "    }\n";
  manifest << "  },\n";
  objc3::artifacts::frontend::
      AppendObjc3FrontendArtifactManifestLoweringHeader(
          manifest, options, vector_signature_functions,
          property_synthesis_ivar_binding_replay_key,
          property_synthesis_ivar_binding_contract);
  const objc3::artifacts::frontend::
      Objc3RuntimeDispatchTableReflectionRecordLoweringFields
          runtime_dispatch_table_reflection_record_lowering_fields{
              message_send_selector_lowering_contract.message_send_sites};
  const bool dispatch_accessor_deterministic_handoff =
      property_synthesis_ivar_binding_contract.deterministic &&
      dispatch_surface_classification_contract.deterministic &&
      message_send_selector_lowering_contract.deterministic &&
      runtime_link_host_link_contract.deterministic;
  objc3::artifacts::frontend::
      Objc3DispatchAndSynthesizedAccessorLoweringFields
          dispatch_and_synthesized_accessor_lowering_fields;
  dispatch_and_synthesized_accessor_lowering_fields.runtime_dispatch_symbol =
      runtime_link_host_link_contract.runtime_dispatch_symbol;
  dispatch_and_synthesized_accessor_lowering_fields.runtime_dispatch_arg_slots =
      runtime_link_host_link_contract.runtime_dispatch_arg_slots;
  dispatch_and_synthesized_accessor_lowering_fields
      .runtime_dispatch_declaration_parameter_count =
      runtime_link_host_link_contract
          .runtime_dispatch_declaration_parameter_count;
  dispatch_and_synthesized_accessor_lowering_fields
      .runtime_dispatch_symbol_matches_lowering =
      runtime_link_host_link_contract.runtime_dispatch_symbol ==
          options.lowering.runtime_dispatch_symbol &&
      runtime_link_host_link_contract.runtime_dispatch_symbol ==
          runtime_support_library_link_wiring.runtime_dispatch_symbol;
  dispatch_and_synthesized_accessor_lowering_fields.live_runtime_dispatch_sites =
      dispatch_surface_classification_contract.instance_dispatch_sites +
      dispatch_surface_classification_contract.class_dispatch_sites +
      dispatch_surface_classification_contract.super_dispatch_sites +
      dispatch_surface_classification_contract.dynamic_dispatch_sites;
  dispatch_and_synthesized_accessor_lowering_fields.direct_dispatch_sites =
      dispatch_surface_classification_contract.direct_dispatch_sites;
  dispatch_and_synthesized_accessor_lowering_fields.message_send_sites =
      message_send_selector_lowering_contract.message_send_sites;
  dispatch_and_synthesized_accessor_lowering_fields.property_synthesis_sites =
      property_synthesis_ivar_binding_contract.property_synthesis_sites;
  dispatch_and_synthesized_accessor_lowering_fields
      .property_synthesis_explicit_ivar_bindings =
      property_synthesis_ivar_binding_contract
          .property_synthesis_explicit_ivar_bindings;
  dispatch_and_synthesized_accessor_lowering_fields
      .property_synthesis_default_ivar_bindings =
      property_synthesis_ivar_binding_contract
          .property_synthesis_default_ivar_bindings;
  dispatch_and_synthesized_accessor_lowering_fields
      .interface_owned_property_synthesis_sites =
      property_synthesis_ivar_binding_contract
          .interface_owned_property_synthesis_sites;
  dispatch_and_synthesized_accessor_lowering_fields
      .implementation_property_redeclaration_sites =
      property_synthesis_ivar_binding_contract
          .implementation_property_redeclaration_sites;
  dispatch_and_synthesized_accessor_lowering_fields.ivar_binding_resolved =
      property_synthesis_ivar_binding_contract.ivar_binding_resolved;
  dispatch_and_synthesized_accessor_lowering_fields.deterministic_handoff =
      dispatch_accessor_deterministic_handoff;
  objc3::artifacts::frontend::Objc3DispatchAccessorRuntimeAbiFields
      dispatch_accessor_runtime_abi_fields;
  dispatch_accessor_runtime_abi_fields.runtime_dispatch_symbol =
      runtime_link_host_link_contract.runtime_dispatch_symbol;
  dispatch_accessor_runtime_abi_fields.deterministic =
      dispatch_accessor_deterministic_handoff;
  const objc3::artifacts::frontend::Objc3StorageAccessorRuntimeAbiFields
      storage_accessor_runtime_abi_fields{
          property_synthesis_ivar_binding_contract.deterministic &&
          runtime_link_host_link_contract.deterministic};
  objc3::artifacts::frontend::WriteObjc3FrontendRuntimeManifestSurfaces(
      manifest, runtime_translation_unit_registration_manifest,
      runtime_metadata_source_records, executable_metadata_source_graph,
      dispatch_and_synthesized_accessor_lowering_fields,
      dispatch_accessor_runtime_abi_fields,
      runtime_dispatch_table_reflection_record_lowering_fields,
      storage_accessor_runtime_abi_fields,
      runtime_metadata_section_publication, runtime_bootstrap_api,
      runtime_bootstrap_semantics,
      runtime_registration_descriptor_frontend_closure,
      runtime_registration_descriptor_image_root_source_surface,
      runtime_bootstrap_lowering, runtime_bootstrap_legality_semantics,
      runtime_bootstrap_failure_restart_semantics);
  objc3::artifacts::frontend::WriteObjc3FrontendDispatchReplayManifestEntries(
      manifest, id_class_sel_object_pointer_typecheck_replay_key,
      id_class_sel_object_pointer_typecheck_contract,
      dispatch_surface_classification_replay_key,
      dispatch_surface_classification_contract,
      message_send_selector_lowering_replay_key,
      message_send_selector_lowering_contract,
      dispatch_abi_marshalling_replay_key, dispatch_abi_marshalling_contract,
      nil_receiver_semantics_foldability_replay_key,
      nil_receiver_semantics_foldability_contract,
      control_flow_control_flow_safety_lowering_replay_key,
      control_flow_control_flow_safety_lowering_contract,
      super_dispatch_method_family_replay_key,
      super_dispatch_method_family_contract, runtime_link_host_link_replay_key,
      runtime_link_host_link_contract, runtime_support_library_link_wiring);
  objc3::artifacts::frontend::
      WriteObjc3FrontendOwnershipAndBlockReplayManifestEntries(
          manifest, ownership_qualifier_lowering_replay_key,
          ownership_qualifier_lowering_contract,
          retain_release_operation_lowering_replay_key,
          retain_release_operation_lowering_contract,
          autoreleasepool_scope_lowering_replay_key,
          autoreleasepool_scope_lowering_contract,
          weak_unowned_semantics_lowering_replay_key,
          weak_unowned_semantics_lowering_contract,
          arc_diagnostics_fixit_lowering_replay_key,
          arc_diagnostics_fixit_lowering_contract,
          block_literal_capture_lowering_replay_key,
          block_literal_capture_lowering_contract,
          block_abi_invoke_trampoline_lowering_replay_key,
          block_abi_invoke_trampoline_lowering_contract,
          block_storage_escape_lowering_replay_key,
          block_storage_escape_lowering_contract,
          block_copy_dispose_lowering_replay_key,
          block_copy_dispose_lowering_contract,
          block_determinism_perf_baseline_lowering_replay_key,
          block_determinism_perf_baseline_lowering_contract);
  objc3::artifacts::frontend::
      WriteObjc3FrontendTypeAndModuleReplayManifestEntries(
          manifest, lightweight_generic_constraint_lowering_replay_key,
          lightweight_generic_constraint_lowering_contract,
          nullability_flow_warning_precision_lowering_replay_key,
          nullability_flow_warning_precision_lowering_contract,
          protocol_qualified_object_type_lowering_replay_key,
          protocol_qualified_object_type_lowering_contract,
          variance_bridge_cast_lowering_replay_key,
          variance_bridge_cast_lowering_contract,
          generic_metadata_abi_lowering_replay_key,
          generic_metadata_abi_lowering_contract,
          module_import_graph_lowering_replay_key,
          module_import_graph_lowering_contract,
          namespace_collision_shadowing_lowering_replay_key,
          namespace_collision_shadowing_lowering_contract,
          public_private_api_partition_lowering_replay_key,
          public_private_api_partition_lowering_contract,
          incremental_module_cache_invalidation_lowering_replay_key,
          incremental_module_cache_invalidation_lowering_contract,
          cross_module_conformance_lowering_replay_key,
          cross_module_conformance_lowering_contract);
  objc3::artifacts::frontend::WriteObjc3FrontendErrorReplayManifestEntries(
      manifest, throws_propagation_lowering_replay_key,
      throws_propagation_lowering_contract, result_like_lowering_replay_key,
      result_like_lowering_contract, ns_error_bridging_lowering_replay_key,
      ns_error_bridging_lowering_contract, unwind_cleanup_lowering_replay_key,
      unwind_cleanup_lowering_contract,
      error_handling_throws_abi_propagation_lowering_replay_key,
      deterministic_error_handling_throws_abi_propagation_lowering,
      error_handling_result_and_bridging_artifact_replay_summary);
  objc3::artifacts::frontend::AppendObjc3FrontendArtifactManifestRecordArrays(
      manifest, program, resolved_global_values, manifest_functions,
      type_metadata_handoff, runtime_metadata_source_records);
  manifest << "}\n";
  bundle.manifest_json = manifest.str();
  bundle.runtime_metadata_binary = executable_metadata_runtime_ingest_binary_payload;
  objc3::artifacts::frontend::PopulateObjc3FrontendArtifactBundleOutputs(
      bundle, program, runtime_aware_import_module_frontend_closure,
      runtime_metadata_source_records, type_metadata_handoff,
      type_system_optional_keypath_lowering_contract,
      type_system_type_semantic_model_summary,
      message_send_selector_lowering_replay_key,
      dispatch_abi_marshalling_replay_key,
      nil_receiver_semantics_foldability_replay_key,
      type_system_optional_keypath_lowering_replay_key,
      runtime_support_library_link_wiring,
      error_handling_result_and_bridging_artifact_replay_summary,
      concurrency_actor_lowering_metadata_contract,
      concurrency_actor_lowering_metadata_replay_key,
      concurrency_actor_isolation_sendability_lowering_replay_key,
      interop_foreign_surface_interface_preservation_summary,
      interop_header_module_bridge_generation_summary,
      interop_foreign_call_lifetime_lowering_contract,
      interop_foreign_call_lifetime_lowering_replay_key,
      interop_ffi_metadata_interface_preservation_contract,
      interop_ffi_metadata_interface_preservation_replay_key,
      metaprogramming_module_interface_replay_preservation_summary,
      metaprogramming_macro_host_process_cache_runtime_integration_summary,
      dispatch_dispatch_metadata_interface_preservation_summary,
      runtime_block_ownership_artifact_preservation_summary,
      runtime_storage_reflection_artifact_preservation_summary,
      serialized_runtime_metadata_artifact_reuse,
      serialized_runtime_metadata_reuse_records,
      versioned_conformance_report_lowering, options, pipeline_result,
      frontend_compatibility_strictness_claim_semantics,
      tooling_feature_aware_conformance_report_emission_summary,
      tooling_corpus_sharding_release_evidence_packaging_summary,
      runtime_registration_descriptor_image_root_source_surface,
      runtime_registration_descriptor_frontend_closure,
      runtime_translation_unit_registration_manifest,
      runtime_bootstrap_legality_semantics,
      runtime_bootstrap_legality_failure_contract,
      runtime_bootstrap_failure_restart_semantics,
      tooling_legacy_canonical_migration_semantics_summary,
      tooling_machine_readable_conformance_report_contract_summary,
      runtime_bootstrap_api, runtime_bootstrap_semantics,
      runtime_bootstrap_lowering);

  if (objc3::artifacts::frontend::FinalizeObjc3FrontendPostPipelineFailure(
          bundle, options, post_pipeline_failure)) {
    return bundle;
  }

  if (!options.emit_ir && !options.emit_object) {
    return bundle;
  }

  Objc3IRFrontendMetadata ir_frontend_metadata;
  objc3::artifacts::frontend::ApplyObjc3FrontendSourceLinkageMetadata(
      ir_frontend_metadata, options, versioned_conformance_report_lowering,
      pipeline_result.canonical_literal_rejection_counts,
      interface_implementation_summary, protocol_category_summary,
      class_protocol_category_linking_summary, selector_normalization_summary,
      property_attribute_summary);
  objc3::artifacts::frontend::ApplyObjc3FrontendObjectDispatchMetadata(
      ir_frontend_metadata, property_synthesis_ivar_binding_replay_key,
      property_synthesis_ivar_binding_contract,
      id_class_sel_object_pointer_typecheck_replay_key,
      id_class_sel_object_pointer_typecheck_contract,
      dispatch_surface_classification_replay_key,
      dispatch_surface_classification_contract,
      message_send_selector_lowering_replay_key,
      message_send_selector_lowering_contract, dispatch_abi_marshalling_replay_key,
      dispatch_abi_marshalling_contract,
      nil_receiver_semantics_foldability_replay_key,
      nil_receiver_semantics_foldability_contract,
      super_dispatch_method_family_replay_key,
      super_dispatch_method_family_contract, runtime_link_host_link_replay_key,
      runtime_link_host_link_contract);
  objc3::artifacts::frontend::ApplyObjc3FrontendConcurrencyMetadata(
      ir_frontend_metadata, concurrency_async_continuation_lowering_replay_key,
      concurrency_async_continuation_lowering_contract,
      concurrency_await_lowering_suspension_state_lowering_replay_key,
      concurrency_await_lowering_suspension_state_lowering_contract,
      concurrency_actor_isolation_sendability_lowering_replay_key,
      concurrency_actor_isolation_sendability_lowering_contract,
      concurrency_actor_lowering_metadata_replay_key,
      concurrency_actor_lowering_metadata_contract);
  objc3::artifacts::frontend::ApplyObjc3FrontendMetaprogrammingMetadata(
      ir_frontend_metadata, metaprogramming_expansion_lowering_replay_key,
      metaprogramming_expansion_lowering_contract,
      metaprogramming_synthesized_artifact_emission_replay_key,
      metaprogramming_synthesized_artifact_emission_contract,
      metaprogramming_derived_method_bundles,
      metaprogramming_macro_artifact_bundles,
      metaprogramming_property_behavior_artifact_bundles,
      metaprogramming_module_interface_replay_preservation_summary);
  objc3::artifacts::frontend::ApplyObjc3FrontendInteropMetadata(
      ir_frontend_metadata, interop_interop_lowering_replay_key,
      interop_interop_lowering_contract,
      interop_foreign_call_lifetime_lowering_replay_key,
      interop_foreign_call_lifetime_lowering_contract,
      interop_ffi_metadata_interface_preservation_replay_key,
      interop_ffi_metadata_interface_preservation_contract,
      interop_header_module_bridge_generation_summary);
  objc3::artifacts::frontend::ApplyObjc3FrontendDispatchMetadata(
      ir_frontend_metadata, dispatch_dispatch_control_lowering_replay_key,
      dispatch_dispatch_control_lowering_contract,
      dispatch_dispatch_metadata_interface_preservation_summary);
  objc3::artifacts::frontend::ApplyObjc3FrontendOwnershipMetadata(
      ir_frontend_metadata, ownership_system_extension_lowering_replay_key,
      ownership_system_extension_lowering_contract,
      ownership_borrowed_retainable_abi_completion_replay_key,
      ownership_system_extension_source_closure_summary,
      ownership_retainable_c_family_source_completion_summary);
  objc3::artifacts::frontend::ApplyObjc3FrontendConcurrencyRuntimeMetadata(
      ir_frontend_metadata,
      concurrency_task_runtime_interop_cancellation_lowering_replay_key,
      concurrency_task_runtime_interop_cancellation_lowering_contract,
      concurrency_concurrency_replay_race_guard_lowering_replay_key,
      concurrency_concurrency_replay_race_guard_lowering_contract);
  objc3::artifacts::frontend::ApplyObjc3FrontendArcOwnershipMetadata(
      ir_frontend_metadata, ownership_qualifier_lowering_replay_key,
      ownership_qualifier_lowering_contract,
      retain_release_operation_lowering_replay_key,
      retain_release_operation_lowering_contract,
      autoreleasepool_scope_lowering_replay_key,
      autoreleasepool_scope_lowering_contract,
      weak_unowned_semantics_lowering_replay_key,
      weak_unowned_semantics_lowering_contract,
      arc_diagnostics_fixit_lowering_replay_key,
      arc_diagnostics_fixit_lowering_contract);
  objc3::artifacts::frontend::ApplyObjc3FrontendBlockMetadata(
      ir_frontend_metadata, block_literal_capture_lowering_replay_key,
      block_literal_capture_lowering_contract,
      block_source_model_completion_replay_key,
      block_source_model_completion_contract,
      block_source_storage_annotation_replay_key,
      block_source_storage_annotation_contract,
      block_abi_invoke_trampoline_lowering_replay_key,
      block_abi_invoke_trampoline_lowering_contract,
      block_storage_escape_lowering_replay_key,
      block_storage_escape_lowering_contract,
      block_copy_dispose_lowering_replay_key,
      block_copy_dispose_lowering_contract,
      block_determinism_perf_baseline_lowering_replay_key,
      block_determinism_perf_baseline_lowering_contract);
  objc3::artifacts::frontend::ApplyObjc3FrontendTypeSystemMetadata(
      ir_frontend_metadata,
      lightweight_generic_constraint_lowering_replay_key,
      lightweight_generic_constraint_lowering_contract,
      nullability_flow_warning_precision_lowering_replay_key,
      nullability_flow_warning_precision_lowering_contract,
      protocol_qualified_object_type_lowering_replay_key,
      protocol_qualified_object_type_lowering_contract,
      variance_bridge_cast_lowering_replay_key,
      variance_bridge_cast_lowering_contract,
      generic_metadata_abi_lowering_replay_key,
      generic_metadata_abi_lowering_contract);
  objc3::artifacts::frontend::ApplyObjc3FrontendModuleMetadata(
      ir_frontend_metadata,
      module_import_graph_lowering_replay_key,
      module_import_graph_lowering_contract,
      namespace_collision_shadowing_lowering_replay_key,
      namespace_collision_shadowing_lowering_contract,
      public_private_api_partition_lowering_replay_key,
      public_private_api_partition_lowering_contract,
      incremental_module_cache_invalidation_lowering_replay_key,
      incremental_module_cache_invalidation_lowering_contract,
      cross_module_conformance_lowering_replay_key,
      cross_module_conformance_lowering_contract);
  objc3::artifacts::frontend::ApplyObjc3FrontendErrorMetadata(
      ir_frontend_metadata,
      throws_propagation_lowering_replay_key,
      throws_propagation_lowering_contract,
      error_handling_throws_abi_propagation_lowering_replay_key,
      result_like_lowering_replay_key,
      result_like_lowering_contract,
      ns_error_bridging_lowering_replay_key,
      ns_error_bridging_lowering_contract,
      unwind_cleanup_lowering_replay_key,
      unwind_cleanup_lowering_contract,
      error_handling_result_and_bridging_artifact_replay_summary);
  objc3::artifacts::frontend::ApplyObjc3FrontendSemanticClosureMetadata(
      ir_frontend_metadata, object_pointer_nullability_generics_summary,
      symbol_graph_scope_resolution_summary,
      pipeline_result.sema_parity_surface
          .deterministic_interface_implementation_handoff,
      interface_implementation_summary, protocol_category_summary,
      class_protocol_category_linking_summary, selector_normalization_summary,
      property_attribute_summary);
  objc3::artifacts::frontend::ApplyObjc3FrontendRuntimeMetadataContractMetadata(
      ir_frontend_metadata, runtime_metadata_source_ownership,
      runtime_export_legality, runtime_export_enforcement,
      runtime_metadata_section_abi, runtime_metadata_section_publication,
      executable_metadata_typed_lowering_handoff, input_path,
      bundle.parse_lowering_readiness_surface.parse_artifact_replay_key,
      bundle.parse_lowering_readiness_surface.lowering_boundary_replay_key);
  objc3::artifacts::frontend::
      ApplyObjc3FrontendFinalRuntimeAndReadinessMetadata(
          ir_frontend_metadata, bundle,
          executable_metadata_typed_lowering_handoff,
          runtime_metadata_section_publication,
          runtime_metadata_object_inspection,
          executable_metadata_debug_projection,
          runtime_support_library, runtime_support_library_core_feature,
          runtime_support_library_link_wiring,
          ownership_aware_lowering_behavior_scaffold,
          pipeline_result.ir_emission_completeness_scaffold,
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface,
          ir_emission_core_feature_impl_surface);
  if (!objc3::artifacts::frontend::CompleteObjc3FrontendArtifactIREmission(
          bundle, pipeline_result, options, program, ir_frontend_metadata,
          runtime_dispatch_lowering_abi_contract,
          message_send_selector_lowering_contract)) {
    return bundle;
  }

  return bundle;
}
