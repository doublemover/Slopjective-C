#include "driver/objc3_cli_language_version.h"

#include <cerrno>
#include <cstdlib>
#include <limits>

bool ParseObjc3LanguageVersion(const std::string &value,
                               std::uint32_t &version) {
  errno = 0;
  char *end = nullptr;
  const unsigned long parsed = std::strtoul(value.c_str(), &end, 10);
  if (value.empty() || end == value.c_str() || *end != '\0' ||
      errno == ERANGE ||
      parsed > std::numeric_limits<std::uint32_t>::max()) {
    return false;
  }
  version = static_cast<std::uint32_t>(parsed);
  return true;
}
