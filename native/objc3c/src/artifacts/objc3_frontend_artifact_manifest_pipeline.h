#pragma once

#include <iosfwd>

#include "pipeline/objc3_frontend_types.h"

struct Objc3FrontendArtifactBundle;

namespace objc3::artifacts::frontend {

void AppendObjc3FrontendArtifactManifestPipelineStages(
    std::ostream &manifest,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options,
    const Objc3FrontendArtifactBundle &bundle);

}  // namespace objc3::artifacts::frontend
