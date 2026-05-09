#pragma once

#include <filesystem>
#include <string>

#include "io/objc3_process.h"
#include "libobjc3c_frontend/objc3_cli_frontend.h"

namespace objc3c::frontend {

bool ValidateRuntimeRegistrationSummaries(
    const Objc3FrontendCompileProduct &product,
    std::string &backend_error);

Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs
BuildRuntimeRegistrationManifestInputs(
    const Objc3FrontendCompileProduct &product,
    const std::filesystem::path &object_out,
    const std::filesystem::path &backend_out);

Objc3RuntimeRegistrationDescriptorArtifactInputs
BuildRuntimeRegistrationDescriptorInputs(
    const Objc3FrontendCompileProduct &product,
    const std::filesystem::path &object_out,
    const std::filesystem::path &backend_out);

}  // namespace objc3c::frontend
