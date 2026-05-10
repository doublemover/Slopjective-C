#include "tools/objc3c_frontend_c_api_runner_command_parser_dispatch_internal.h"

#include "tools/objc3c_frontend_c_api_runner_command_parser_internal.h"

FrontendCApiRunnerCommandOptionParseResult
DispatchFrontendCApiRunnerCommandOptionPassSequence(
    const std::string &arg,
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
  if (FrontendCApiRunnerDispatchShouldReturn(path_runtime_result)) {
    return path_runtime_result;
  }

  const FrontendCApiRunnerCommandOptionParseResult backend_result =
      DispatchFrontendCApiRunnerBackendOptionPass(arg,
                                                  argc,
                                                  argv,
                                                  index,
                                                  options,
                                                  error);
  if (FrontendCApiRunnerDispatchShouldReturn(backend_result)) {
    return backend_result;
  }

  return DispatchFrontendCApiRunnerEmissionHelpOptionPass(arg, options, error);
}
