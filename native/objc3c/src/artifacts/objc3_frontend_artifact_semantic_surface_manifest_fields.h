#pragma once

#include <iosfwd>

struct Objc3FrontendArtifactCoreLoweringPlan;
struct Objc3FrontendArtifactFunctionManifest;
struct Objc3FrontendArtifactInteropLoweringPlan;
struct Objc3FrontendArtifactOwnershipAwareLoweringPlan;
struct Objc3FrontendArtifactPreservationPlan;
struct Objc3FrontendArtifactSemanticLoweringPlan;
struct Objc3FrontendArtifactSourceShapePlan;
struct Objc3FrontendOptions;
struct Objc3FrontendPipelineResult;
struct Objc3Program;
struct Objc3TypeSystemTypeSemanticModelSummary;

namespace objc3::artifacts::frontend {

struct Objc3FrontendArtifactSemanticSurfaceManifestContext {
  const Objc3Program &program;
  const Objc3FrontendPipelineResult &pipeline_result;
  const Objc3FrontendOptions &options;
  const Objc3FrontendArtifactFunctionManifest &function_manifest;
  const Objc3FrontendArtifactSourceShapePlan &source_shape_plan;
  const Objc3FrontendArtifactCoreLoweringPlan &core_lowering_plan;
  const Objc3FrontendArtifactSemanticLoweringPlan &semantic_lowering_plan;
  const Objc3FrontendArtifactOwnershipAwareLoweringPlan
      &ownership_aware_lowering_plan;
  const Objc3FrontendArtifactInteropLoweringPlan &interop_lowering_plan;
  const Objc3FrontendArtifactPreservationPlan &artifact_preservation_plan;
  const Objc3TypeSystemTypeSemanticModelSummary
      &type_system_type_semantic_model_summary;
};

void WriteObjc3FrontendArtifactSemanticSurfaceVectorSignatureFields(
    std::ostream &manifest,
    const Objc3FrontendArtifactFunctionManifest &function_manifest);

void WriteObjc3FrontendArtifactSemanticSurfaceCoreFields(
    std::ostream &manifest,
    const Objc3FrontendArtifactSemanticSurfaceManifestContext &context);

void WriteObjc3FrontendArtifactSemanticSurfaceSourceToolingFields(
    std::ostream &manifest,
    const Objc3FrontendArtifactSemanticSurfaceManifestContext &context);

void WriteObjc3FrontendArtifactSemanticSurfaceInteropMetaprogrammingFields(
    std::ostream &manifest,
    const Objc3FrontendArtifactSemanticSurfaceManifestContext &context);

void WriteObjc3FrontendArtifactSemanticSurfaceDispatchConcurrencyOwnershipFields(
    std::ostream &manifest,
    const Objc3FrontendArtifactSemanticSurfaceManifestContext &context);

void WriteObjc3FrontendArtifactSemanticSurfaceTerminalSemanticFields(
    std::ostream &manifest,
    const Objc3FrontendArtifactSemanticSurfaceManifestContext &context);

}  // namespace objc3::artifacts::frontend
