#pragma once

#include <iosfwd>

#include "artifacts/objc3_frontend_artifact_error_lowering_plan.h"

namespace objc3::artifacts::frontend {

void AppendObjc3FrontendArtifactLoweringHandoffErrorManifestFields(
    std::ostringstream &manifest,
    const Objc3FrontendArtifactErrorLoweringPlan &error_lowering_plan);

}  // namespace objc3::artifacts::frontend
