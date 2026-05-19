#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

[[nodiscard]] std::string
BuildRuntimeRegistrationDescriptorImageRootSourceSurfaceReplayKey(
    const Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary
        &summary);

[[nodiscard]] Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary
BuildRuntimeRegistrationDescriptorImageRootSourceSurfaceSummary(
    const Objc3Program &program,
    const Objc3FrontendBootstrapRegistrationSourcePragmaContract
        &bootstrap_registration_source_pragma_contract,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &registration_manifest);

[[nodiscard]] std::string
BuildRuntimeRegistrationDescriptorImageRootSourceSurfaceSummaryJson(
    const Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary
        &summary);

[[nodiscard]] std::string BuildRuntimeRegistrationDescriptorFrontendClosureReplayKey(
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary &summary);

[[nodiscard]] Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
BuildRuntimeRegistrationDescriptorFrontendClosureSummary(
    const Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary
        &source_surface_summary,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &registration_manifest_summary);

[[nodiscard]] std::string BuildRuntimeRegistrationDescriptorFrontendClosureSummaryJson(
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary &summary);

}  // namespace objc3::artifacts::frontend
