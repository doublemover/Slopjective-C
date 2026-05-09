#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

constexpr std::size_t kObjc3CliMaxMessageSendArgs = 16;

bool ParseObjc3PositiveOrdinal(const std::string &value,
                               std::uint64_t &ordinal);
bool ParseObjc3MessageSendArgLimit(const std::string &value,
                                   std::size_t &max_args);
