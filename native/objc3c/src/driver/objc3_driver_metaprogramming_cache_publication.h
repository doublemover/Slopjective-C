#pragma once

#include <string>

#include "driver/objc3_cli_options.h"
#include "libobjc3c_frontend/objc3_cli_frontend.h"

int PublishObjc3DriverMetaprogrammingCacheArtifact(
    const Objc3CliOptions &cli_options,
    const Objc3FrontendArtifactBundle &artifacts,
    std::string &error_message);
