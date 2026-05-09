#pragma once

#include <string>

#include "driver/objc3_cli_options.h"
#include "libobjc3c_frontend/objc3_cli_frontend.h"

int PublishObjc3DriverConformanceReleaseArtifacts(
    const Objc3CliOptions &cli_options,
    const Objc3FrontendArtifactBundle &artifacts,
    const std::string &conformance_publication_artifact_json);
