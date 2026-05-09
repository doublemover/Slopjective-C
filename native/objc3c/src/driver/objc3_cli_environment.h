#pragma once

#include <filesystem>
#include <string>

#include "driver/objc3_cli_options.h"

std::string ReadObjc3DriverEnvironmentVariable(const char *name);
std::filesystem::path DefaultObjc3DriverLlcPath();
void ApplyObjc3CliEnvironmentDefaults(Objc3CliOptions &options);
