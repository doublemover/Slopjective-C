#include "tools/objc3c_frontend_c_api_runner_command_parser.h"

#include <filesystem>

#include "tools/objc3c_frontend_c_api_runner_command_parser_options.h"
#include "tools/objc3c_frontend_c_api_runner_usage.h"

namespace {

bool FrontendCApiRunnerOptionParseFailed(
    FrontendCApiRunnerCommandOptionParseResult result) {
  return result == FrontendCApiRunnerCommandOptionParseResult::kError;
}

bool FrontendCApiRunnerOptionWasHandled(
    FrontendCApiRunnerCommandOptionParseResult result) {
  return result == FrontendCApiRunnerCommandOptionParseResult::kHandled;
}

}  // namespace

bool ParseFrontendCApiRunnerOptions(int argc,
                                    char **argv,
                                    FrontendCApiRunnerOptions &options,
                                    std::string &error) {
  if (argc < 2) {
    error = FrontendCApiRunnerUsage();
    return false;
  }

  options = FrontendCApiRunnerOptions{};
  options.input_path = std::filesystem::path(argv[1]);

  for (int i = 2; i < argc; ++i) {
    const std::string arg = argv[i];
    const FrontendCApiRunnerCommandOptionParseResult path_string_result =
        ParseFrontendCApiRunnerPathStringOption(arg, argc, argv, i, options);
    if (FrontendCApiRunnerOptionWasHandled(path_string_result)) {
      continue;
    }

    const FrontendCApiRunnerCommandOptionParseResult numeric_runtime_result =
        ParseFrontendCApiRunnerNumericRuntimeOption(
            arg,
            argc,
            argv,
            i,
            options,
            error);
    if (FrontendCApiRunnerOptionParseFailed(numeric_runtime_result)) {
      return false;
    }
    if (FrontendCApiRunnerOptionWasHandled(numeric_runtime_result)) {
      continue;
    }

    const FrontendCApiRunnerCommandOptionParseResult removed_mode_result =
        ParseFrontendCApiRunnerRemovedModeOption(arg, error);
    if (FrontendCApiRunnerOptionParseFailed(removed_mode_result)) {
      return false;
    }

    const FrontendCApiRunnerCommandOptionParseResult backend_result =
        ParseFrontendCApiRunnerBackendSelectionOption(
            arg,
            argc,
            argv,
            i,
            options,
            error);
    if (FrontendCApiRunnerOptionParseFailed(backend_result)) {
      return false;
    }
    if (FrontendCApiRunnerOptionWasHandled(backend_result)) {
      continue;
    }

    const FrontendCApiRunnerCommandOptionParseResult dump_help_result =
        ParseFrontendCApiRunnerEmissionDumpHelpOption(arg, options, error);
    if (FrontendCApiRunnerOptionParseFailed(dump_help_result)) {
      return false;
    }
    if (FrontendCApiRunnerOptionWasHandled(dump_help_result)) {
      continue;
    }

    error = "unknown arg: " + arg;
    return false;
  }

  return true;
}
