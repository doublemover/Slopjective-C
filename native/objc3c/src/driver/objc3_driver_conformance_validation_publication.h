#pragma once

#include <filesystem>
#include <string>

#include "driver/objc3_cli_options.h"

int PublishObjc3DriverConformanceValidationArtifacts(
    const Objc3CliOptions &cli_options,
    const std::filesystem::path &publication_path,
    const std::string &report_json,
    const std::string &publication_json);
