#include "tools/objc3c_frontend_c_api_runner_repro_command.h"

#include <sstream>

#include "tools/objc3c_frontend_c_api_runner_shell_quote.h"

std::string BuildFrontendCApiRunnerReproCommand(
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    bool dump_playground_repro_json) {
  std::ostringstream command;
  command << "& "
          << QuoteFrontendCApiRunnerPowerShellArg(
                 (std::filesystem::path("artifacts") / "bin" /
                  "objc3c-frontend-c-api-runner.exe")
                     .generic_string());
  command << " "
          << QuoteFrontendCApiRunnerPowerShellArg(
                 options.input_path.generic_string());
  command << " --out-dir "
          << QuoteFrontendCApiRunnerPowerShellArg(
                 options.out_dir.generic_string());
  command << " --emit-prefix "
          << QuoteFrontendCApiRunnerPowerShellArg(options.emit_prefix);
  command << " --clang "
          << QuoteFrontendCApiRunnerPowerShellArg(
                 options.clang_path.generic_string());
  command << " --llc "
          << QuoteFrontendCApiRunnerPowerShellArg(
                 options.llc_path.generic_string());
  command << " --summary-out "
          << QuoteFrontendCApiRunnerPowerShellArg(summary_path.generic_string());
  command << " --objc3-ir-object-backend "
          << QuoteFrontendCApiRunnerPowerShellArg(
                 options.ir_object_backend ==
                         OBJC3C_FRONTEND_IR_OBJECT_BACKEND_LLVM_DIRECT
                     ? "llvm-direct"
                     : "clang");
  if (options.max_message_send_args != 0) {
    command << " --objc3-max-message-args "
            << std::to_string(options.max_message_send_args);
  }
  if (!options.runtime_dispatch_symbol.empty()) {
    command << " --objc3-runtime-dispatch-symbol "
            << QuoteFrontendCApiRunnerPowerShellArg(
                   options.runtime_dispatch_symbol);
  }
  if (options.translation_unit_registration_order_ordinal != 0) {
    command << " --objc3-bootstrap-registration-order-ordinal "
            << std::to_string(
                   options.translation_unit_registration_order_ordinal);
  }
  if (!options.emit_manifest) {
    command << " --no-emit-manifest";
  }
  if (!options.emit_ir) {
    command << " --no-emit-ir";
  }
  if (!options.emit_object) {
    command << " --no-emit-object";
  }
  if (dump_playground_repro_json) {
    command << " --dump-playground-repro-json";
  }
  return command.str();
}
