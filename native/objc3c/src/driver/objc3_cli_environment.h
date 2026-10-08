#pragma once

#include <filesystem>
#include <string>

#include "driver/objc3_cli_options.h"

std::string ReadObjc3DriverEnvironmentVariable(const char *name);
std::filesystem::path DefaultObjc3DriverClangPath();
std::filesystem::path DefaultObjc3DriverLlcPath();
std::filesystem::path DefaultObjc3DriverLlvmToolPath(const char *tool_name);
void ApplyObjc3CliEnvironmentDefaults(Objc3CliOptions &options);
