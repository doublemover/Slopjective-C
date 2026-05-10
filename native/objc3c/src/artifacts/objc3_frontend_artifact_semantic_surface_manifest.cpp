#include "artifacts/objc3_frontend_artifact_semantic_surface_manifest.h"

#include "artifacts/objc3_frontend_artifact_semantic_surface_manifest_fields.h"

namespace objc3::artifacts::frontend {

void WriteObjc3FrontendArtifactSemanticSurfaceManifest(
    std::ostream &manifest,
    const Objc3Program &program,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options,
    const Objc3FrontendArtifactFunctionManifest &function_manifest,
    const Objc3FrontendArtifactSourceShapePlan &source_shape_plan,
    const Objc3FrontendArtifactCoreLoweringPlan &core_lowering_plan,
    const Objc3FrontendArtifactSemanticLoweringPlan &semantic_lowering_plan,
    const Objc3FrontendArtifactOwnershipAwareLoweringPlan
        &ownership_aware_lowering_plan,
    const Objc3FrontendArtifactInteropLoweringPlan &interop_lowering_plan,
    const Objc3FrontendArtifactPreservationPlan &artifact_preservation_plan,
    const Objc3TypeSystemTypeSemanticModelSummary
        &type_system_type_semantic_model_summary) {
  const Objc3FrontendArtifactSemanticSurfaceManifestContext context{
      program,
      pipeline_result,
      options,
      function_manifest,
      source_shape_plan,
      core_lowering_plan,
      semantic_lowering_plan,
      ownership_aware_lowering_plan,
      interop_lowering_plan,
      artifact_preservation_plan,
      type_system_type_semantic_model_summary};

  WriteObjc3FrontendArtifactSemanticSurfaceVectorSignatureFields(
      manifest, function_manifest);
  WriteObjc3FrontendArtifactSemanticSurfaceCoreFields(manifest, context);
  WriteObjc3FrontendArtifactSemanticSurfaceSourceToolingFields(manifest,
                                                               context);
  WriteObjc3FrontendArtifactSemanticSurfaceInteropMetaprogrammingFields(
      manifest, context);
  WriteObjc3FrontendArtifactSemanticSurfaceDispatchConcurrencyOwnershipFields(
      manifest, context);
  WriteObjc3FrontendArtifactSemanticSurfaceTerminalSemanticFields(manifest,
                                                                  context);
}

}  // namespace objc3::artifacts::frontend
