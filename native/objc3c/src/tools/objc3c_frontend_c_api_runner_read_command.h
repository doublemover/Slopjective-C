#pragma once

#include <string>

bool FrontendCApiRunnerPathExists(const std::string &path_text);
std::string BuildFrontendCApiRunnerReadCommand(const std::string &path_text);
