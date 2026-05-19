#pragma once

#include <iosfwd>

#include "artifacts/objc3_frontend_artifact_ownership_lowering_plan.h"

namespace objc3::artifacts::frontend {

void AppendObjc3FrontendArtifactLoweringHandoffOwnershipManifestFields(
    std::ostringstream &manifest,
    const Objc3FrontendArtifactOwnershipAwareLoweringPlan
        &ownership_aware_lowering_plan);

}  // namespace objc3::artifacts::frontend
