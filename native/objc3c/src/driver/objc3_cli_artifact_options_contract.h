#pragma once

#include <string>

#include "driver/objc3_cli_options.h"

bool ValidateObjc3CliArtifactOptions(const Objc3CliOptions &options,
                                     std::string &error);
