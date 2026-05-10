#pragma once

#include <iosfwd>

struct Objc3FrontendArtifactRuntimeRegistrationPlan;

namespace objc3::artifacts::frontend {

void AppendObjc3FrontendArtifactRuntimeRegistrationDescriptorManifestFields(
    std::ostream &manifest,
    const Objc3FrontendArtifactRuntimeRegistrationPlan
        &runtime_registration_plan);

}  // namespace objc3::artifacts::frontend
