#include "tools/objc3c_frontend_c_api_runner_command_parser_internal.h"

FrontendCApiRunnerCommandOptionParseResult
DispatchFrontendCApiRunnerCommandOption(const std::string &arg,
                                        int argc,
                                        char **argv,
                                        int &index,
                                        FrontendCApiRunnerOptions &options,
                                        std::string &error) {
  const FrontendCApiRunnerCommandOptionParseResult path_runtime_result =
      DispatchFrontendCApiRunnerPathRuntimeOptionPass(arg,
                                                      argc,
                                                      argv,
                                                      index,
                                                      options,
                                                      error);
  if (FrontendCApiRunnerOptionParseFailed(path_runtime_result) ||
      FrontendCApiRunnerOptionWasHandled(path_runtime_result)) {
    return path_runtime_result;
  }

  const FrontendCApiRunnerCommandOptionParseResult backend_result =
      DispatchFrontendCApiRunnerBackendOptionPass(arg,
                                                  argc,
                                                  argv,
                                                  index,
                                                  options,
                                                  error);
  if (FrontendCApiRunnerOptionParseFailed(backend_result) ||
      FrontendCApiRunnerOptionWasHandled(backend_result)) {
    return backend_result;
  }

  return DispatchFrontendCApiRunnerEmissionHelpOptionPass(arg, options, error);
}
