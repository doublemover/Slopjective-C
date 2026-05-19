#pragma once

#include <filesystem>

#include "artifacts/objc3_frontend_artifact_diagnostics.h"

struct Objc3FrontendArtifactBlockLoweringPlan;
struct Objc3FrontendArtifactCoreLoweringPlan;
struct Objc3FrontendArtifactErrorLoweringPlan;
struct Objc3FrontendArtifactInteropLoweringPlan;
struct Objc3FrontendArtifactModuleLoweringPlan;
struct Objc3FrontendArtifactOwnershipAwareLoweringPlan;
struct Objc3FrontendArtifactPreservationPlan;
struct Objc3FrontendArtifactRuntimeImportPlan;
struct Objc3FrontendArtifactRuntimeMetadataPlan;
struct Objc3FrontendArtifactRuntimeRegistrationPlan;
struct Objc3FrontendArtifactSemanticLoweringPlan;
struct Objc3FrontendArtifactTypeSystemLoweringPlan;
struct Objc3FrontendArtifactBundle;
struct Objc3FrontendOptions;
struct Objc3FrontendPipelineResult;
struct Objc3IREmissionCoreFeatureImplementationSurface;
struct Objc3Program;

namespace objc3::artifacts::frontend {

struct Objc3FrontendArtifactConformanceReportPlan;

struct Objc3FrontendArtifactIrApplicationInputs {
  Objc3FrontendArtifactBundle &bundle;
  const std::filesystem::path &input_path;
  const Objc3FrontendPipelineResult &pipeline_result;
  const Objc3FrontendOptions &options;
  const Objc3Program &program;
  const Objc3FrontendArtifactPostPipelineFailure &post_pipeline_failure;
  const Objc3IREmissionCoreFeatureImplementationSurface
      &ir_emission_core_feature_impl_surface;
  const Objc3FrontendArtifactConformanceReportPlan &conformance_report_plan;
  const Objc3FrontendArtifactSemanticLoweringPlan &semantic_lowering_plan;
  const Objc3FrontendArtifactCoreLoweringPlan &core_lowering_plan;
  const Objc3FrontendArtifactOwnershipAwareLoweringPlan
      &ownership_aware_lowering_plan;
  const Objc3FrontendArtifactBlockLoweringPlan &block_lowering_plan;
  const Objc3FrontendArtifactTypeSystemLoweringPlan &type_system_lowering_plan;
  const Objc3FrontendArtifactRuntimeImportPlan &runtime_import_plan;
  const Objc3FrontendArtifactModuleLoweringPlan &module_lowering_plan;
  const Objc3FrontendArtifactErrorLoweringPlan &error_lowering_plan;
  const Objc3FrontendArtifactInteropLoweringPlan &interop_lowering_plan;
  const Objc3FrontendArtifactPreservationPlan &artifact_preservation_plan;
  const Objc3FrontendArtifactRuntimeMetadataPlan &runtime_metadata_plan;
  const Objc3FrontendArtifactRuntimeRegistrationPlan &runtime_registration_plan;
};

void FinalizeObjc3FrontendArtifactIrApplication(
    const Objc3FrontendArtifactIrApplicationInputs &inputs);

}  // namespace objc3::artifacts::frontend
