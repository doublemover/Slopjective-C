#include "tools/objc3c_frontend_c_api_runner_command_parser_internal.h"

bool FrontendCApiRunnerOptionParseFailed(
    FrontendCApiRunnerCommandOptionParseResult result) {
  return result == FrontendCApiRunnerCommandOptionParseResult::kError;
}

bool FrontendCApiRunnerOptionWasHandled(
    FrontendCApiRunnerCommandOptionParseResult result) {
  return result == FrontendCApiRunnerCommandOptionParseResult::kHandled;
}

FrontendCApiRunnerCommandOptionParseResult
DispatchFrontendCApiRunnerCommandOption(const std::string &arg,
                                        int argc,
                                        char **argv,
                                        int &index,
                                        FrontendCApiRunnerOptions &options,
                                        std::string &error) {
  const FrontendCApiRunnerCommandOptionParseResult path_string_result =
      ParseFrontendCApiRunnerPathStringOption(arg, argc, argv, index, options);
  if (FrontendCApiRunnerOptionWasHandled(path_string_result)) {
    return path_string_result;
  }

  const FrontendCApiRunnerCommandOptionParseResult numeric_runtime_result =
      ParseFrontendCApiRunnerNumericRuntimeOption(arg, argc, argv, index,
                                                  options, error);
  if (FrontendCApiRunnerOptionParseFailed(numeric_runtime_result) ||
      FrontendCApiRunnerOptionWasHandled(numeric_runtime_result)) {
    return numeric_runtime_result;
  }

  const FrontendCApiRunnerCommandOptionParseResult removed_mode_result =
      ParseFrontendCApiRunnerRemovedModeOption(arg, error);
  if (FrontendCApiRunnerOptionParseFailed(removed_mode_result)) {
    return removed_mode_result;
  }

  const FrontendCApiRunnerCommandOptionParseResult backend_result =
      ParseFrontendCApiRunnerBackendSelectionOption(arg, argc, argv, index,
                                                    options, error);
  if (FrontendCApiRunnerOptionParseFailed(backend_result) ||
      FrontendCApiRunnerOptionWasHandled(backend_result)) {
    return backend_result;
  }

  return ParseFrontendCApiRunnerEmissionDumpHelpOption(arg, options, error);
}
