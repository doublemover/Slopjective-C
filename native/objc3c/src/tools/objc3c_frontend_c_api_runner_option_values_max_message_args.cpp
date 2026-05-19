#include "tools/objc3c_frontend_c_api_runner_option_values.h"

#include <cerrno>
#include <cstdlib>

bool ParseFrontendCApiRunnerMaxMessageSendArgs(const std::string &value,
                                               std::uint32_t &parsed_value,
                                               std::string &error) {
  errno = 0;
  char *end = nullptr;
  const unsigned long parsed = std::strtoul(value.c_str(), &end, 10);
  if (value.empty() || end == value.c_str() || *end != '\0' ||
      errno == ERANGE || parsed > kFrontendCApiRunnerMaxMessageSendArgs) {
    error = "invalid --objc3-max-message-args (expected integer 0-" +
            std::to_string(kFrontendCApiRunnerMaxMessageSendArgs) + "): " +
            value;
    return false;
  }
  parsed_value = static_cast<std::uint32_t>(parsed);
  return true;
}
