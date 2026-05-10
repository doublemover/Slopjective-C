#pragma once

#include <filesystem>
#include <string>

#include "artifacts/objc3_frontend_artifact_block_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_conformance_report_plan.h"
#include "artifacts/objc3_frontend_artifact_core_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_diagnostics.h"
#include "artifacts/objc3_frontend_artifact_error_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_function_manifest.h"
#include "artifacts/objc3_frontend_artifact_interop_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_module_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_ownership_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_preservation_plan.h"
#include "artifacts/objc3_frontend_artifact_runtime_import_plan.h"
#include "artifacts/objc3_frontend_artifact_runtime_metadata_plan.h"
#include "artifacts/objc3_frontend_artifact_runtime_registration_plan.h"
#include "artifacts/objc3_frontend_artifact_semantic_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_source_shape_plan.h"
#include "artifacts/objc3_frontend_artifact_type_system_lowering_plan.h"
#include "pipeline/objc3_frontend_types.h"
#include "pipeline/objc3_ir_emission_core_feature_implementation_surface.h"
#include "pipeline/objc3_parse_lowering_readiness_surface.h"

struct Objc3FrontendArtifactBundle;

namespace objc3::artifacts::frontend {

struct Objc3FrontendArtifactAssemblyContext {
  Objc3FrontendArtifactPostPipelineFailure post_pipeline_failure;
  Objc3IREmissionCoreFeatureImplementationSurface
      ir_emission_core_feature_impl_surface;
  Objc3FrontendArtifactFunctionManifest function_manifest;
  Objc3FrontendArtifactSemanticLoweringPlan semantic_lowering_plan;
  Objc3TypeSystemTypeSemanticModelSummary
      type_system_type_semantic_model_summary;
  Objc3FrontendArtifactRuntimeMetadataPlan runtime_metadata_plan;
  Objc3FrontendArtifactRuntimeRegistrationPlan runtime_registration_plan;
  Objc3FrontendArtifactConformanceReportPlan conformance_report_plan;
  Objc3FrontendArtifactCoreLoweringPlan core_lowering_plan;
  Objc3FrontendArtifactOwnershipAwareLoweringPlan ownership_aware_lowering_plan;
  Objc3FrontendArtifactBlockLoweringPlan block_lowering_plan;
  Objc3FrontendArtifactTypeSystemLoweringPlan type_system_lowering_plan;
  Objc3FrontendArtifactRuntimeImportPlan runtime_import_plan;
  Objc3FrontendArtifactModuleLoweringPlan module_lowering_plan;
  Objc3FrontendArtifactErrorLoweringPlan error_lowering_plan;
  Objc3FrontendArtifactInteropLoweringPlan interop_lowering_plan;
  Objc3FrontendArtifactPreservationPlan artifact_preservation_plan;
  Objc3FrontendArtifactSourceShapePlan source_shape_plan;
};

Objc3FrontendArtifactAssemblyContext BuildObjc3FrontendArtifactAssemblyContext(
    const std::filesystem::path &input_path,
    const Objc3Program &program,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options,
    const Objc3ParseLoweringReadinessSurface &parse_lowering_readiness_surface);

std::string BuildObjc3FrontendArtifactManifest(
    const std::filesystem::path &input_path,
    const Objc3Program &program,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options,
    const Objc3FrontendArtifactBundle &bundle,
    const Objc3FrontendArtifactAssemblyContext &context);

}  // namespace objc3::artifacts::frontend
