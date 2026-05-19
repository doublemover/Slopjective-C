#pragma once

#include <filesystem>

#include "driver/objc3_cli_options.h"
#include "io/objc3_process.h"
#include "libobjc3c_frontend/objc3_cli_frontend.h"

int PublishObjc3DriverCrossModuleRuntimeLinkArtifacts(
    const Objc3CliOptions &cli_options,
    const Objc3FrontendArtifactBundle &artifacts,
    const Objc3RuntimeMetadataLinkerRetentionArtifacts
        &linker_retention_artifacts,
    const std::filesystem::path &object_out);
