#include "tools/objc3c_frontend_c_api_runner_read_command.h"

#include <filesystem>

#include "tools/objc3c_frontend_c_api_runner_shell_quote.h"

bool FrontendCApiRunnerPathExists(const std::string &path_text) {
  return !path_text.empty() &&
         std::filesystem::exists(std::filesystem::path(path_text));
}

std::string BuildFrontendCApiRunnerReadCommand(
    const std::string &path_text) {
  if (!FrontendCApiRunnerPathExists(path_text)) {
    return "";
  }
  return "Get-Content -Raw " +
         QuoteFrontendCApiRunnerPowerShellArg(path_text);
}
