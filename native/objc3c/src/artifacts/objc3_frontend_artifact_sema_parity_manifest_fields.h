#pragma once

#include <iosfwd>

#include "pipeline/results/pipeline_result_model.h"

namespace objc3::artifacts::frontend {

void AppendObjc3FrontendArtifactSemaParityManifestFields(
    std::ostream &manifest,
    const Objc3FrontendPipelineResult &pipeline_result,
    bool parity_ready);

}  // namespace objc3::artifacts::frontend
