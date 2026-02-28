#pragma once

#include <string>

#include "driver/objc3_cli_options.h"

// Applies fail-closed capability summary routing for backend/tool selection.
bool ApplyObjc3LLVMCabilityRouting(Objc3CliOptions &options, std::string &error);
