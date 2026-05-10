#pragma once

#include <iosfwd>

struct Objc3FrontendArtifactBlockLoweringPlan;

namespace objc3::artifacts::frontend {

void AppendObjc3FrontendArtifactLoweringHandoffBlockManifestFields(
    std::ostringstream &manifest,
    const Objc3FrontendArtifactBlockLoweringPlan &block_lowering_plan);

}  // namespace objc3::artifacts::frontend
