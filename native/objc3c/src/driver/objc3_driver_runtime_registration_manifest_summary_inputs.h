#pragma once

#include <filesystem>

#include "io/objc3_process.h"
#include "libobjc3c_frontend/objc3_cli_frontend.h"

void PopulateObjc3DriverRuntimeRegistrationManifestSummaryInputs(
    Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs &inputs,
    const Objc3FrontendArtifactBundle &artifacts);
void PopulateObjc3DriverRuntimeRegistrationDescriptorSourceInputs(
    Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs &inputs,
    const Objc3FrontendArtifactBundle &artifacts);
void PopulateObjc3DriverRuntimeRegistrationOutputArtifactInputs(
    Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs &inputs,
    const Objc3FrontendArtifactBundle &artifacts,
    const std::filesystem::path &object_out,
    const std::filesystem::path &backend_out);
