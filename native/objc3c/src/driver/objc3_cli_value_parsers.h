#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "driver/objc3_cli_options.h"

constexpr std::size_t kObjc3CliMaxMessageSendArgs = 16;

bool ParseObjc3CliIrObjectBackend(const std::string &value,
                                  Objc3IrObjectBackend &backend);
bool ParseObjc3ConformanceProfile(const std::string &value,
                                  Objc3ConformanceProfile &profile);
bool ParseObjc3LanguageVersion(const std::string &value,
                               std::uint32_t &version);
bool ParseObjc3PositiveOrdinal(const std::string &value,
                               std::uint64_t &ordinal);
bool ParseObjc3MessageSendArgLimit(const std::string &value,
                                   std::size_t &max_args);
std::string ConformanceProfileName(Objc3ConformanceProfile profile);
std::string Objc3CliUsage();
