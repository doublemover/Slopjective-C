#include "tools/objc3c_frontend_c_api_runner_command_parser.h"

#include "tools/objc3c_frontend_c_api_runner_command_parser_internal.h"
#include "tools/objc3c_frontend_c_api_runner_usage.h"

bool ParseFrontendCApiRunnerOptions(int argc,
                                    char **argv,
                                    FrontendCApiRunnerOptions &options,
                                    std::string &error) {
  if (argc < 2) {
    error = FrontendCApiRunnerUsage();
    return false;
  }

  InitializeFrontendCApiRunnerParsedOptions(argv, options);

  for (int i = 2; i < argc; ++i) {
    const std::string arg = argv[i];
    const FrontendCApiRunnerCommandOptionParseResult option_result =
        DispatchFrontendCApiRunnerCommandOption(arg, argc, argv, i, options,
                                                error);
    if (FrontendCApiRunnerOptionParseFailed(option_result)) {
      return false;
    }
    if (FrontendCApiRunnerOptionWasHandled(option_result)) {
      continue;
    }

    error = "unknown arg: " + arg;
    return false;
  }

  NormalizeFrontendCApiRunnerParsedOptions(options);
  return true;
}
