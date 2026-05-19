#pragma once

#include <filesystem>
#include <string>

#include "driver/objc3_cli_options.h"
#include "driver/objc3_driver_shell_paths.h"

bool ValidateObjc3DriverShellInputs(const Objc3CliOptions &cli_options,
                                    Objc3DriverInputKind input_kind,
                                    std::string &error);
