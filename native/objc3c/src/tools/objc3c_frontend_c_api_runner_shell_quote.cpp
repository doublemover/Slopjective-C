#include "tools/objc3c_frontend_c_api_runner_shell_quote.h"

std::string QuoteFrontendCApiRunnerPowerShellArg(const std::string &value) {
  std::string quoted = "'";
  for (char c : value) {
    if (c == '\'') {
      quoted += "''";
    } else {
      quoted += c;
    }
  }
  quoted += "'";
  return quoted;
}
