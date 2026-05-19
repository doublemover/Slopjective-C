#pragma once

#include "driver/objc3_cli_options.h"
#include "libobjc3c_frontend/objc3_cli_frontend.h"

int PublishObjc3DriverConformanceArtifacts(
    const Objc3CliOptions &cli_options,
    const Objc3FrontendArtifactBundle &artifacts);
