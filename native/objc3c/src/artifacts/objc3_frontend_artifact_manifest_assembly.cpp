#include "artifacts/objc3_frontend_artifact_assembly.h"

#include <sstream>

#include "artifacts/objc3_frontend_artifact_executable_metadata_runtime_ingest_manifest_fields.h"
#include "artifacts/objc3_frontend_artifact_lowering_handoff_manifest_fields.h"
#include "artifacts/objc3_frontend_artifact_manifest_header.h"
#include "artifacts/objc3_frontend_artifact_manifest_pipeline.h"
#include "artifacts/objc3_frontend_artifact_manifest_readiness.h"
#include "artifacts/objc3_frontend_artifact_manifest_replay_tail.h"
#include "artifacts/objc3_frontend_artifact_pre_tail_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_runtime_bootstrap_legality_manifest_fields.h"
#include "artifacts/objc3_frontend_artifact_runtime_bootstrap_manifest_fields.h"
#include "artifacts/objc3_frontend_artifact_runtime_metadata_manifest_fields.h"
#include "artifacts/objc3_frontend_artifact_runtime_registration_descriptor_manifest_fields.h"
#include "artifacts/objc3_frontend_artifact_runtime_startup_bootstrap_invariant_manifest_fields.h"
#include "artifacts/objc3_frontend_artifact_runtime_support_registration_manifest_fields.h"
#include "artifacts/objc3_frontend_artifact_semantic_surface_manifest.h"
#include "artifacts/objc3_frontend_artifact_sema_parity_manifest_fields.h"
#include "artifacts/objc3_frontend_artifacts.h"
#include "pipeline/frontend_artifact_semantic_accessors.h"

namespace objc3::artifacts::frontend {

std::string BuildObjc3FrontendArtifactManifest(
    const std::filesystem::path &input_path,
    const Objc3Program &program,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options,
    const Objc3FrontendArtifactBundle &bundle,
    const Objc3FrontendArtifactAssemblyContext &context) {
  std::ostringstream manifest;
  AppendObjc3FrontendArtifactManifestHeader(
      manifest, input_path, program, pipeline_result, options);
  AppendObjc3FrontendArtifactManifestPipelineStages(
      manifest, pipeline_result, options, bundle);
  AppendObjc3FrontendArtifactManifestParseReadiness(manifest, bundle);
  AppendObjc3FrontendArtifactManifestSemaPassDiagnostics(manifest,
                                                         pipeline_result);
  AppendObjc3FrontendArtifactSemaParityManifestFields(
      manifest, pipeline_result,
      Objc3FrontendArtifactSemaParitySurfaceReady(pipeline_result));
  WriteRuntimeMetadataPublicationManifestFields(
      manifest, pipeline_result.runtime_metadata_source_ownership_boundary,
      pipeline_result.runtime_export_legality_boundary,
      pipeline_result.runtime_export_enforcement_summary,
      context.runtime_metadata_plan.runtime_metadata_section_abi,
      context.runtime_metadata_plan.runtime_metadata_section_publication,
      context.runtime_metadata_plan.runtime_metadata_object_inspection);
  AppendObjc3FrontendArtifactExecutableMetadataRuntimeIngestManifestFields(
      manifest, context.runtime_metadata_plan);
  AppendObjc3FrontendArtifactRuntimeSupportRegistrationManifestFields(
      manifest, context.runtime_registration_plan);
  AppendObjc3FrontendArtifactRuntimeRegistrationDescriptorManifestFields(
      manifest, context.runtime_registration_plan);
  AppendObjc3FrontendArtifactRuntimeBootstrapLegalityManifestFields(
      manifest, context.runtime_registration_plan);
  WriteRuntimeBootstrapManifestFields(
      manifest,
      context.runtime_registration_plan
          .runtime_bootstrap_failure_restart_semantics,
      context.runtime_registration_plan.runtime_bootstrap_api,
      context.runtime_registration_plan.runtime_bootstrap_semantics,
      context.runtime_registration_plan.runtime_bootstrap_lowering);
  WriteRuntimeStartupBootstrapInvariantManifestFields(
      manifest,
      context.runtime_registration_plan.runtime_startup_bootstrap_invariants);
  AppendObjc3FrontendArtifactLoweringHandoffManifestFields(
      manifest, context.core_lowering_plan,
      context.ownership_aware_lowering_plan, context.block_lowering_plan,
      context.type_system_lowering_plan, context.runtime_import_plan,
      context.module_lowering_plan, context.error_lowering_plan,
      pipeline_result.object_pointer_nullability_generics_summary,
      pipeline_result.symbol_graph_scope_resolution_summary);
  WriteObjc3FrontendArtifactSemanticSurfaceManifest(
      manifest, program, pipeline_result, options, context.function_manifest,
      context.source_shape_plan, context.core_lowering_plan,
      context.semantic_lowering_plan, context.ownership_aware_lowering_plan,
      context.interop_lowering_plan, context.artifact_preservation_plan,
      context.type_system_type_semantic_model_summary);
  WriteObjc3FrontendArtifactPreTailManifestSurfaces(
      manifest, program, pipeline_result, context.function_manifest,
      context.core_lowering_plan, context.semantic_lowering_plan,
      context.ownership_aware_lowering_plan, context.block_lowering_plan,
      context.type_system_lowering_plan, context.runtime_import_plan,
      context.module_lowering_plan, context.error_lowering_plan,
      context.interop_lowering_plan, context.artifact_preservation_plan,
      context.conformance_report_plan, context.runtime_metadata_plan,
      context.runtime_registration_plan,
      context.type_system_type_semantic_model_summary);
  AppendObjc3FrontendArtifactManifestReplayTail(
      manifest, program, pipeline_result, options, context.function_manifest,
      context.source_shape_plan, context.core_lowering_plan,
      context.ownership_aware_lowering_plan, context.block_lowering_plan,
      context.type_system_lowering_plan, context.runtime_import_plan,
      context.module_lowering_plan, context.error_lowering_plan,
      context.interop_lowering_plan, context.runtime_metadata_plan,
      context.runtime_registration_plan);

  return manifest.str();
}

}  // namespace objc3::artifacts::frontend
