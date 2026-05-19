#pragma once

#include <filesystem>

#include "io/objc3_manifest_artifacts.h"
#include "io/objc3_process_runtime_metadata_contracts.h"
#include "libobjc3c_frontend/objc3_cli_frontend.h"

Objc3RuntimeRegistrationDescriptorArtifactInputs
BuildObjc3DriverRuntimeRegistrationDescriptorInputs(
    const Objc3FrontendArtifactBundle &artifacts,
    const std::filesystem::path &object_out,
    const std::filesystem::path &backend_out);
