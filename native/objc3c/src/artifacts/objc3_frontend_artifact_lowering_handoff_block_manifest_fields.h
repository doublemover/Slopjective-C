#pragma once

#include <iosfwd>

#include "artifacts/objc3_frontend_artifact_block_lowering_plan.h"

namespace objc3::artifacts::frontend {

void AppendObjc3FrontendArtifactLoweringHandoffBlockManifestFields(
    std::ostringstream &manifest,
    const Objc3FrontendArtifactBlockLoweringPlan &block_lowering_plan);

}  // namespace objc3::artifacts::frontend
