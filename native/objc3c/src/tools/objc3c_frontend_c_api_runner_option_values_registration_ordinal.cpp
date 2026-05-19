#include "tools/objc3c_frontend_c_api_runner_option_values.h"

#include <cerrno>
#include <cstdlib>

bool ParseFrontendCApiRunnerRegistrationOrderOrdinal(
    const std::string &value,
    std::uint64_t &parsed_value,
    std::string &error) {
  errno = 0;
  char *end = nullptr;
  const unsigned long long parsed = std::strtoull(value.c_str(), &end, 10);
  if (value.empty() || end == value.c_str() || *end != '\0' ||
      errno == ERANGE || parsed == 0) {
    error =
        "invalid --objc3-bootstrap-registration-order-ordinal (expected "
        "positive integer): " +
        value;
    return false;
  }
  parsed_value = static_cast<std::uint64_t>(parsed);
  return true;
}
