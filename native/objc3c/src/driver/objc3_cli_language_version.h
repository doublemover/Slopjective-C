#pragma once

#include <cstdint>
#include <string>

bool ParseObjc3LanguageVersion(const std::string &value,
                               std::uint32_t &version);
