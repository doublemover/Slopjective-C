#pragma once

#include "artifacts/objc3_frontend_artifacts.h"
#include "driver/objc3_cli_options.h"

struct Objc3DriverFrontendCoreArtifactPublicationResult {
  bool diagnostics_present = false;
};

Objc3DriverFrontendCoreArtifactPublicationResult
PublishObjc3DriverFrontendCoreArtifacts(
    const Objc3CliOptions &cli_options,
    const Objc3FrontendArtifactBundle &artifacts);
