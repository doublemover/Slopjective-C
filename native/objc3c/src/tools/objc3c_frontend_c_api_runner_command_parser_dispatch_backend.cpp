#include "tools/objc3c_frontend_c_api_runner_command_parser_internal.h"

FrontendCApiRunnerCommandOptionParseResult
DispatchFrontendCApiRunnerBackendOptionPass(const std::string &arg,
                                            int argc,
                                            char **argv,
                                            int &index,
                                            FrontendCApiRunnerOptions &options,
                                            std::string &error) {
  const FrontendCApiRunnerCommandOptionParseResult removed_mode_result =
      ParseFrontendCApiRunnerRemovedModeOption(arg, error);
  if (FrontendCApiRunnerOptionParseFailed(removed_mode_result)) {
    return removed_mode_result;
  }

  return ParseFrontendCApiRunnerBackendSelectionOption(arg, argc, argv, index,
                                                       options, error);
}
