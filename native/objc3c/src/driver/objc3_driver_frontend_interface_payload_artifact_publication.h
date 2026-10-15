#pragma once

#include "artifacts/objc3_frontend_artifacts.h"
#include "driver/objc3_cli_options.h"

void PublishObjc3DriverFrontendInterfacePayloadArtifact(
    const Objc3CliOptions &cli_options,
    const Objc3FrontendArtifactBundle &artifacts);
