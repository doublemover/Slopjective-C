#pragma once

#include <filesystem>
#include <string>

struct Objc3FrontendArtifactBlockLoweringPlan;
struct Objc3FrontendArtifactBundle;
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
struct Objc3FrontendOptions;
struct Objc3FrontendPipelineResult;
struct Objc3IREmissionCoreFeatureImplementationSurface;
struct Objc3Program;
struct Objc3TypeSystemTypeSemanticModelSummary;

namespace objc3::artifacts::frontend {

struct Objc3FrontendArtifactConformanceReportPlan;
struct Objc3FrontendArtifactPostPipelineFailure;

struct Objc3FrontendArtifactBundlePublicationInputs {
  Objc3FrontendArtifactBundle &bundle;
  const std::filesystem::path &input_path;
  const Objc3FrontendPipelineResult &pipeline_result;
  const Objc3FrontendOptions &options;
  const Objc3Program &program;
  const std::string &manifest_json;
  const Objc3FrontendArtifactPostPipelineFailure &post_pipeline_failure;
  const Objc3IREmissionCoreFeatureImplementationSurface
      &ir_emission_core_feature_impl_surface;
  const Objc3TypeSystemTypeSemanticModelSummary
      &type_system_type_semantic_model_summary;
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

void PublishObjc3FrontendArtifactBundleOutputs(
    const Objc3FrontendArtifactBundlePublicationInputs &inputs);

}  // namespace objc3::artifacts::frontend
