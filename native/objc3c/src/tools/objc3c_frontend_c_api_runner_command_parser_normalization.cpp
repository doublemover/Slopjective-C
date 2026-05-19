#include "tools/objc3c_frontend_c_api_runner_command_parser_internal.h"

#include <filesystem>

void InitializeFrontendCApiRunnerParsedOptions(
    char **argv,
    FrontendCApiRunnerOptions &options) {
  options = FrontendCApiRunnerOptions{};
  options.input_path = std::filesystem::path(argv[1]);
}

void NormalizeFrontendCApiRunnerParsedOptions(
    FrontendCApiRunnerOptions &) {}
