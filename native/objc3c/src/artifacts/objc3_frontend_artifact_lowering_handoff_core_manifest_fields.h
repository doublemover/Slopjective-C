#pragma once

#include <iosfwd>

#include "artifacts/objc3_frontend_artifact_core_lowering_plan.h"

namespace objc3::artifacts::frontend {

void AppendObjc3FrontendArtifactLoweringHandoffCoreManifestFields(
    std::ostringstream &manifest,
    const Objc3FrontendArtifactCoreLoweringPlan &core_lowering_plan);

}  // namespace objc3::artifacts::frontend
