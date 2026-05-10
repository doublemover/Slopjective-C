#pragma once

#include <iosfwd>

struct Objc3FrontendPipelineResult;

namespace objc3::artifacts::frontend {

void AppendObjc3FrontendArtifactSemaParityManifestFields(
    std::ostream &manifest,
    const Objc3FrontendPipelineResult &pipeline_result,
    bool parity_ready);

}  // namespace objc3::artifacts::frontend
