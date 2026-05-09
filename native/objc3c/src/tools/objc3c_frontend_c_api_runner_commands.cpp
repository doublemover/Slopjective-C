#include "tools/objc3c_frontend_c_api_runner_commands.h"

#include <cstddef>
#include <sstream>

#include "ast/objc3_ast.h"

bool FrontendCApiRunnerPathExists(const std::string &path_text) {
  return !path_text.empty() &&
         std::filesystem::exists(std::filesystem::path(path_text));
}

std::string QuoteFrontendCApiRunnerPowerShellArg(const std::string &value) {
  std::string quoted = "'";
  for (char c : value) {
    if (c == '\'') {
      quoted += "''";
    } else {
      quoted += c;
    }
  }
  quoted += "'";
  return quoted;
}

std::string BuildFrontendCApiRunnerReadCommand(
    const std::string &path_text) {
  if (!FrontendCApiRunnerPathExists(path_text)) {
    return "";
  }
  return "Get-Content -Raw " +
         QuoteFrontendCApiRunnerPowerShellArg(path_text);
}

std::string BuildFrontendCApiRunnerObjectInspectionCommand(
    const std::string &template_command,
    const std::string &object_path_text) {
  if (!FrontendCApiRunnerPathExists(object_path_text)) {
    return "";
  }
  const std::string placeholder =
      kObjc3RuntimeMetadataObjectInspectionObjectRelativePath;
  std::string command = template_command;
  const std::size_t placeholder_offset = command.find(placeholder);
  if (placeholder_offset != std::string::npos) {
    command.replace(
        placeholder_offset,
        placeholder.size(),
        QuoteFrontendCApiRunnerPowerShellArg(object_path_text));
    return command;
  }
  return command + " " +
         QuoteFrontendCApiRunnerPowerShellArg(object_path_text);
}

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
