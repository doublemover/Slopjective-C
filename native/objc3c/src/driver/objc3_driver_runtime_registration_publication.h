#pragma once

#include "driver/objc3_cli_options.h"
#include "driver/objc3_driver_object_backend.h"
#include "io/objc3_process.h"
#include "libobjc3c_frontend/objc3_cli_frontend.h"

struct Objc3DriverRuntimeRegistrationPublicationResult {
  int compile_status = 0;
  bool linker_retention_ready = false;
  Objc3RuntimeMetadataLinkerRetentionArtifacts linker_retention_artifacts;
};

Objc3DriverRuntimeRegistrationPublicationResult
PublishObjc3DriverRuntimeRegistrationArtifacts(
    const Objc3CliOptions &cli_options,
    const Objc3FrontendArtifactBundle &artifacts,
    const Objc3DriverObjectBackendResult &object_backend);
