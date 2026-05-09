#pragma once

#include <filesystem>

#include "io/objc3_manifest_artifacts.h"
#include "libobjc3c_frontend/objc3_cli_frontend.h"

Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs
BuildObjc3DriverRuntimeRegistrationManifestInputs(
    const Objc3FrontendArtifactBundle &artifacts,
    const std::filesystem::path &object_out,
    const std::filesystem::path &backend_out);
