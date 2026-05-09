#pragma once

#include <string>

bool FailFrontendCApiRunnerOutputContract(const char *boundary,
                                          const std::string &reason,
                                          std::string &error);
