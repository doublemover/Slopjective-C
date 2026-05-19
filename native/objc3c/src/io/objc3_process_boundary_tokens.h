#pragma once

#include <string>

bool ExtractBoundaryTokenValue(const std::string &line,
                               const std::string &key,
                               std::string &value);
bool ExtractHexBoundaryTokenValue(const std::string &line,
                                  const std::string &key,
                                  std::string &value);
