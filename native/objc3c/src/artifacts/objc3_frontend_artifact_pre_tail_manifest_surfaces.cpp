#include "artifacts/objc3_frontend_artifact_pre_tail_manifest_surfaces.h"

#include <ostream>

#include "artifacts/objc3_frontend_artifact_block_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_block_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_concurrency_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_concurrency_runtime_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_conformance_report_plan.h"
#include "artifacts/objc3_frontend_artifact_core_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_cross_module_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_dispatch_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_dispatch_runtime_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_error_handling_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_error_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_function_manifest.h"
#include "artifacts/objc3_frontend_artifact_interop_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_interop_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_metaprogramming_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_module_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_ownership_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_ownership_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_ownership_release_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_preservation_plan.h"
#include "artifacts/objc3_frontend_artifact_runtime_import_plan.h"
#include "artifacts/objc3_frontend_artifact_runtime_manifest_orchestration.h"
#include "artifacts/objc3_frontend_artifact_runtime_metadata_plan.h"
#include "artifacts/objc3_frontend_artifact_runtime_registration_plan.h"
#include "artifacts/objc3_frontend_artifact_semantic_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_semantic_summary_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_source_closure_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_tooling_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_type_system_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_type_system_manifest_surfaces.h"
#include "artifacts/objc3_frontend_control_flow_semantic_artifacts.h"
#include "artifacts/objc3_frontend_type_system_contract_artifacts.h"
#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

#include "artifacts/objc3_frontend_artifact_pre_tail_manifest_surfaces_core_lowering.inc"
#include "artifacts/objc3_frontend_artifact_pre_tail_manifest_surfaces_module_source.inc"
#include "artifacts/objc3_frontend_artifact_pre_tail_manifest_surfaces_semantic_domains.inc"

void WriteObjc3FrontendArtifactPreTailManifestSurfaces(
    std::ostream &manifest,
    const Objc3Program &program,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendArtifactFunctionManifest &function_manifest,
    const Objc3FrontendArtifactCoreLoweringPlan &core_lowering_plan,
    const Objc3FrontendArtifactSemanticLoweringPlan &semantic_lowering_plan,
    const Objc3FrontendArtifactOwnershipAwareLoweringPlan
        &ownership_aware_lowering_plan,
    const Objc3FrontendArtifactBlockLoweringPlan &block_lowering_plan,
    const Objc3FrontendArtifactTypeSystemLoweringPlan
        &type_system_lowering_plan,
    const Objc3FrontendArtifactRuntimeImportPlan &runtime_import_plan,
    const Objc3FrontendArtifactModuleLoweringPlan &module_lowering_plan,
    const Objc3FrontendArtifactErrorLoweringPlan &error_lowering_plan,
    const Objc3FrontendArtifactInteropLoweringPlan &interop_lowering_plan,
    const Objc3FrontendArtifactPreservationPlan &artifact_preservation_plan,
    const Objc3FrontendArtifactConformanceReportPlan &conformance_report_plan,
    const Objc3FrontendArtifactRuntimeMetadataPlan &runtime_metadata_plan,
    const Objc3FrontendArtifactRuntimeRegistrationPlan
        &runtime_registration_plan,
    const Objc3TypeSystemTypeSemanticModelSummary
        &type_system_type_semantic_model_summary) {
  WriteObjc3FrontendArtifactPreTailCoreRuntimeSurfaces(
      manifest, pipeline_result, core_lowering_plan, conformance_report_plan,
      runtime_metadata_plan, runtime_registration_plan);
  WriteObjc3FrontendArtifactPreTailLanguageLoweringSurfaces(
      manifest, pipeline_result, core_lowering_plan,
      ownership_aware_lowering_plan, block_lowering_plan,
      type_system_lowering_plan, runtime_registration_plan,
      type_system_type_semantic_model_summary);
  WriteObjc3FrontendArtifactPreTailModuleAndErrorSurfaces(
      manifest, program, pipeline_result, runtime_import_plan,
      module_lowering_plan, error_lowering_plan, interop_lowering_plan);
  WriteObjc3FrontendArtifactPreTailSourceAndToolingSurfaces(
      manifest, pipeline_result, conformance_report_plan);
  WriteObjc3FrontendArtifactPreTailInteropAndMetaprogrammingSurfaces(
      manifest, pipeline_result, semantic_lowering_plan, interop_lowering_plan,
      artifact_preservation_plan);
  WriteObjc3FrontendArtifactPreTailDispatchConcurrencyOwnershipSurfaces(
      manifest, pipeline_result, semantic_lowering_plan,
      artifact_preservation_plan);
  WriteObjc3FrontendArtifactPreTailRuntimeAndSummarySurfaces(
      manifest, pipeline_result, function_manifest, core_lowering_plan,
      semantic_lowering_plan, ownership_aware_lowering_plan,
      type_system_type_semantic_model_summary);
}

}  // namespace objc3::artifacts::frontend
