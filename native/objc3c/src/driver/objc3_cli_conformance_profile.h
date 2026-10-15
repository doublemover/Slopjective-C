#pragma once

#include <string>

#include "driver/objc3_cli_options.h"

bool ParseObjc3ConformanceProfile(const std::string &value,
                                  Objc3ConformanceProfile &profile);
std::string ConformanceProfileName(Objc3ConformanceProfile profile);
std::string ConformanceProfileLanguageProfileName(
    Objc3ConformanceProfile profile);
