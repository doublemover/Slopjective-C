#pragma once

#include <string>

#include "driver/objc3_cli_options.h"

bool TryApplyObjc3CliPrimaryOptionGroup(const std::string &flag,
                                        int &index,
                                        int argc,
                                        char **argv,
                                        Objc3CliOptions &options,
                                        std::string &error,
                                        bool &matched);
bool TryApplyObjc3CliExtendedOptionGroup(const std::string &flag,
                                         int &index,
                                         int argc,
                                         char **argv,
                                         Objc3CliOptions &options,
                                         std::string &error,
                                         bool &matched);
