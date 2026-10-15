#include "tools/objc3c_frontend_c_api_runner_command_parser_internal.h"

#include <filesystem>

#include "driver/objc3_cli_environment.h"

void InitializeFrontendCApiRunnerParsedOptions(
    char **argv,
    FrontendCApiRunnerOptions &options) {
  options = FrontendCApiRunnerOptions{};
  options.input_path = std::filesystem::path(argv[1]);
}

void NormalizeFrontendCApiRunnerParsedOptions(
    FrontendCApiRunnerOptions &options) {
  if (!options.clang_path_explicit) {
    options.clang_path = DefaultObjc3DriverClangPath();
  }
  if (!options.llc_path_explicit) {
    options.llc_path = DefaultObjc3DriverLlcPath();
  }
}
