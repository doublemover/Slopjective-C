#pragma once

#include <string>

#include "driver/objc3_cli_options.h"

bool ParseObjc3CliOptions(int argc,
                          char **argv,
                          Objc3CliOptions &options,
                          std::string &error);
