#pragma once

#include <iosfwd>

#include "artifacts/objc3_frontend_artifact_type_system_lowering_plan.h"

namespace objc3::artifacts::frontend {

void AppendObjc3FrontendArtifactLoweringHandoffTypeSystemManifestFields(
    std::ostringstream &manifest,
    const Objc3FrontendArtifactTypeSystemLoweringPlan
        &type_system_lowering_plan);

}  // namespace objc3::artifacts::frontend
