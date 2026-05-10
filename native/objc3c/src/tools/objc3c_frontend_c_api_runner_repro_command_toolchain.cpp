#include "tools/objc3c_frontend_c_api_runner_repro_command_segments.h"

#include "tools/objc3c_frontend_c_api_runner_shell_quote.h"

void AppendFrontendCApiRunnerReproToolchainArgs(
    std::ostream &command,
    const FrontendCApiRunnerOptions &options) {
  command << " --clang "
          << QuoteFrontendCApiRunnerPowerShellArg(
                 options.clang_path.generic_string());
  command << " --llc "
          << QuoteFrontendCApiRunnerPowerShellArg(
                 options.llc_path.generic_string());
}

void AppendFrontendCApiRunnerReproBackendArg(
    std::ostream &command,
    const FrontendCApiRunnerOptions &options) {
  command << " --objc3-ir-object-backend "
          << QuoteFrontendCApiRunnerPowerShellArg(
                 options.ir_object_backend ==
                         OBJC3C_FRONTEND_IR_OBJECT_BACKEND_LLVM_DIRECT
                     ? "llvm-direct"
                     : "clang");
}
