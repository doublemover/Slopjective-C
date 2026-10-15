#include "tools/objc3c_frontend_c_api_runner_command_parser_path_options_internal.h"

#include <filesystem>

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerToolchainPathStringOption(
    const std::string &arg,
    int argc,
    char **argv,
    int &index,
    FrontendCApiRunnerOptions &options) {
  if (arg == "--clang" && index + 1 < argc) {
    options.clang_path = std::filesystem::path(argv[++index]);
    options.clang_path_explicit = true;
    return FrontendCApiRunnerCommandOptionParseResult::kHandled;
  }
  if (arg == "--llc" && index + 1 < argc) {
    options.llc_path = std::filesystem::path(argv[++index]);
    options.llc_path_explicit = true;
    return FrontendCApiRunnerCommandOptionParseResult::kHandled;
  }
  return FrontendCApiRunnerCommandOptionParseResult::kNotHandled;
}
