#pragma once

#include "driver/objc3_cli_options.h"
#include "driver/objc3_driver_shell.h"

int DispatchObjc3DriverCommand(const Objc3CliOptions &cli_options,
                               Objc3DriverInputKind input_kind);
