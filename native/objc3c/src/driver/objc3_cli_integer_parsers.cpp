#include "driver/objc3_cli_integer_parsers.h"

#include <cerrno>
#include <cstdlib>
#include <limits>

bool ParseObjc3PositiveOrdinal(const std::string &value,
                               std::uint64_t &ordinal) {
  errno = 0;
  char *end = nullptr;
  const unsigned long long parsed = std::strtoull(value.c_str(), &end, 10);
  if (value.empty() || end == value.c_str() || *end != '\0' ||
      errno == ERANGE || parsed == 0 ||
      parsed > std::numeric_limits<std::uint64_t>::max()) {
    return false;
  }
  ordinal = static_cast<std::uint64_t>(parsed);
  return true;
}

bool ParseObjc3MessageSendArgLimit(const std::string &value,
                                   std::size_t &max_args) {
  errno = 0;
  char *end = nullptr;
  const unsigned long parsed = std::strtoul(value.c_str(), &end, 10);
  if (value.empty() || end == value.c_str() || *end != '\0' ||
      errno == ERANGE || parsed > kObjc3CliMaxMessageSendArgs) {
    return false;
  }
  max_args = static_cast<std::size_t>(parsed);
  return true;
}
