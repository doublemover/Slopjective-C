#include "tools/objc3c_frontend_c_api_runner_command_parser_options.h"

#include <filesystem>

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerPathStringOption(const std::string &arg,
                                        int argc,
                                        char **argv,
                                        int &index,
                                        FrontendCApiRunnerOptions &options) {
  if (arg == "--out-dir" && index + 1 < argc) {
    options.out_dir = std::filesystem::path(argv[++index]);
    return FrontendCApiRunnerCommandOptionParseResult::kHandled;
  }
  if (arg == "--emit-prefix" && index + 1 < argc) {
    options.emit_prefix = argv[++index];
    return FrontendCApiRunnerCommandOptionParseResult::kHandled;
  }
  if (arg == "--clang" && index + 1 < argc) {
    options.clang_path = std::filesystem::path(argv[++index]);
    return FrontendCApiRunnerCommandOptionParseResult::kHandled;
  }
  if (arg == "--llc" && index + 1 < argc) {
    options.llc_path = std::filesystem::path(argv[++index]);
    return FrontendCApiRunnerCommandOptionParseResult::kHandled;
  }
  if (arg == "--summary-out" && index + 1 < argc) {
    options.summary_out = std::filesystem::path(argv[++index]);
    return FrontendCApiRunnerCommandOptionParseResult::kHandled;
  }
  return FrontendCApiRunnerCommandOptionParseResult::kNotHandled;
}
