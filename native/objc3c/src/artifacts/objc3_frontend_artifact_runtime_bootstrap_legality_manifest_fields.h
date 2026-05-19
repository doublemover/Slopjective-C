#pragma once

#include <iosfwd>

#include "artifacts/objc3_frontend_artifact_runtime_registration_plan.h"

namespace objc3::artifacts::frontend {

void AppendObjc3FrontendArtifactRuntimeBootstrapLegalityManifestFields(
    std::ostream &manifest,
    const Objc3FrontendArtifactRuntimeRegistrationPlan
        &runtime_registration_plan);

}  // namespace objc3::artifacts::frontend
