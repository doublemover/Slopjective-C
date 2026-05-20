#include "tools/objc3c_frontend_c_api_runner_compile_options_fields.h"

void ApplyFrontendCApiRunnerCompilePathInputOptions(
    objc3c_frontend_c_compile_options_t &compile_options,
    const FrontendCApiRunnerOptions &runner_options,
    const std::string &input_path_text,
    const std::string &out_dir_text,
    const std::string &metaprogramming_cache_root_text) {
  compile_options.input_path = input_path_text.c_str();
  compile_options.out_dir = out_dir_text.c_str();
  compile_options.metaprogramming_cache_root =
      runner_options.metaprogramming_cache_root.empty()
          ? nullptr
          : metaprogramming_cache_root_text.c_str();
  compile_options.emit_prefix = runner_options.emit_prefix.c_str();
}
