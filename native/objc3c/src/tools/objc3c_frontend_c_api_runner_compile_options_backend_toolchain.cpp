#include "tools/objc3c_frontend_c_api_runner_compile_options_fields.h"

void ApplyFrontendCApiRunnerCompileBackendToolchainOptions(
    objc3c_frontend_c_compile_options_t &compile_options,
    const FrontendCApiRunnerOptions &runner_options,
    const std::string &clang_path_text,
    const std::string &llc_path_text) {
  compile_options.clang_path =
      runner_options.emit_object &&
              runner_options.ir_object_backend ==
                  OBJC3C_FRONTEND_IR_OBJECT_BACKEND_CLANG
          ? clang_path_text.c_str()
          : nullptr;
  compile_options.llc_path =
      runner_options.emit_object &&
              runner_options.ir_object_backend ==
                  OBJC3C_FRONTEND_IR_OBJECT_BACKEND_LLVM_DIRECT
          ? llc_path_text.c_str()
          : nullptr;
  compile_options.ir_object_backend = runner_options.ir_object_backend;
  compile_options.language_version =
      OBJC3C_FRONTEND_LANGUAGE_VERSION_OBJECTIVE_C_3;
}
