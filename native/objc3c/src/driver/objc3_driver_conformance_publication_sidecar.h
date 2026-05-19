#pragma once

#include <string>

#include "driver/objc3_cli_options.h"

int PublishObjc3DriverConformancePublicationSidecar(
    const Objc3CliOptions &cli_options,
    std::string &conformance_publication_artifact_json);
