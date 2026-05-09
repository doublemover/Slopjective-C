#pragma once

#include <string>

#include "driver/objc3_cli_options.h"

bool ParseObjc3CliIrObjectBackend(const std::string &value,
                                  Objc3IrObjectBackend &backend);
