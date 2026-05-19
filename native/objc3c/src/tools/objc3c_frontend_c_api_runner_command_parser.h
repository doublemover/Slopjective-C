#pragma once

#include <string>

#include "tools/objc3c_frontend_c_api_runner_options.h"

bool ParseFrontendCApiRunnerOptions(int argc,
                                    char **argv,
                                    FrontendCApiRunnerOptions &options,
                                    std::string &error);
