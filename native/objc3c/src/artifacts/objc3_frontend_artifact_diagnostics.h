#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"
#include "pipeline/objc3_ir_emission_core_feature_implementation_surface.h"
#include "pipeline/objc3_parse_lowering_readiness_surface.h"

struct Objc3FrontendArtifactBundle;

namespace objc3::artifacts::frontend {

struct Objc3FrontendArtifactPostPipelineFailure {
  bool present = false;
  std::string code;
  std::string message;

  [[nodiscard]] bool empty() const { return !present; }
};

Objc3FrontendArtifactPostPipelineFailure
BuildObjc3FrontendArtifactInitialPostPipelineFailure(
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3ParseLoweringReadinessSurface &parse_lowering_readiness_surface,
    const Objc3IREmissionCoreFeatureImplementationSurface
        &ir_emission_core_feature_impl_surface,
    bool metadata_only_ir_emission_mode);

void RecordObjc3FrontendArtifactPostPipelineFailure(
    Objc3FrontendArtifactPostPipelineFailure &failure,
    const char *code,
    std::string message);

bool FinalizeObjc3FrontendPostPipelineFailure(
    Objc3FrontendArtifactBundle &bundle,
    const Objc3FrontendOptions &options,
    const Objc3FrontendArtifactPostPipelineFailure &failure);

}  // namespace objc3::artifacts::frontend
