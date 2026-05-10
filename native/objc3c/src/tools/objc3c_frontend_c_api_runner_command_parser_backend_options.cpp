#include "tools/objc3c_frontend_c_api_runner_command_parser_options.h"

#include "tools/objc3c_frontend_c_api_runner_option_values.h"

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerBackendSelectionOption(
    const std::string &arg,
    int argc,
    char **argv,
    int &index,
    FrontendCApiRunnerOptions &options,
    std::string &error) {
  if (arg != "--objc3-ir-object-backend" || index + 1 >= argc) {
    return FrontendCApiRunnerCommandOptionParseResult::kNotHandled;
  }

  const std::string backend = argv[++index];
  if (!ParseFrontendCApiRunnerIrObjectBackend(backend,
                                              options.ir_object_backend)) {
    error =
        "invalid --objc3-ir-object-backend (expected clang|llvm-direct): " +
        backend;
    return FrontendCApiRunnerCommandOptionParseResult::kError;
  }
  return FrontendCApiRunnerCommandOptionParseResult::kHandled;
}
