#pragma once

#include "artifacts/objc3_frontend_artifacts.h"
#include "driver/objc3_cli_options.h"

struct Objc3DriverRuntimeImportArtifactPublicationResult {
  int status_code = 0;
  bool has_runtime_import_artifact = false;
};

Objc3DriverRuntimeImportArtifactPublicationResult
PublishObjc3DriverRuntimeImportArtifacts(
    const Objc3CliOptions &cli_options,
    const Objc3FrontendArtifactBundle &artifacts);
