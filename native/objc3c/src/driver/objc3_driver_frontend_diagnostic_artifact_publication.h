#pragma once

#include "artifacts/objc3_frontend_artifacts.h"
#include "driver/objc3_cli_options.h"

bool PublishObjc3DriverFrontendDiagnosticArtifacts(
    const Objc3CliOptions &cli_options,
    const Objc3FrontendArtifactBundle &artifacts);
