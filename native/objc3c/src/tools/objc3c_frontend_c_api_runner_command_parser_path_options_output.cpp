#include "tools/objc3c_frontend_c_api_runner_command_parser_path_options_internal.h"

#include <filesystem>

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerOutputPathStringOption(
    const std::string &arg,
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
  if (arg == "--objc3-metaprogramming-cache-root" && index + 1 < argc) {
    options.metaprogramming_cache_root = std::filesystem::path(argv[++index]);
    return FrontendCApiRunnerCommandOptionParseResult::kHandled;
  }
  if (arg == "--objc3-import-runtime-surface" && index + 1 < argc) {
    options.imported_runtime_surface_paths.push_back(
        std::filesystem::path(argv[++index]));
    return FrontendCApiRunnerCommandOptionParseResult::kHandled;
  }
  return FrontendCApiRunnerCommandOptionParseResult::kNotHandled;
}

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerSummaryOutputPathStringOption(
    const std::string &arg,
    int argc,
    char **argv,
    int &index,
    FrontendCApiRunnerOptions &options) {
  if (arg == "--summary-out" && index + 1 < argc) {
    options.summary_out = std::filesystem::path(argv[++index]);
    return FrontendCApiRunnerCommandOptionParseResult::kHandled;
  }
  return FrontendCApiRunnerCommandOptionParseResult::kNotHandled;
}
