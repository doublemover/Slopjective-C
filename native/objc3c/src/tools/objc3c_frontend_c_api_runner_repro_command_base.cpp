#include "tools/objc3c_frontend_c_api_runner_repro_command_segments.h"

#include "artifacts/identity/artifact_identity.h"
#include "tools/objc3c_frontend_c_api_runner_shell_quote.h"

void AppendFrontendCApiRunnerReproBaseInvocationAndPathArgs(
    std::ostream &command,
    const FrontendCApiRunnerOptions &options) {
  command << "& "
          << QuoteFrontendCApiRunnerPowerShellArg(
                 objc3::artifacts::identity::
                     kObjc3NativeFrontendRunnerRelativePath);
  command << " "
          << QuoteFrontendCApiRunnerPowerShellArg(
                 options.input_path.generic_string());
  command << " --out-dir "
          << QuoteFrontendCApiRunnerPowerShellArg(
                 options.out_dir.generic_string());
  command << " --emit-prefix "
          << QuoteFrontendCApiRunnerPowerShellArg(options.emit_prefix);
}

void AppendFrontendCApiRunnerReproSummaryPathArg(
    std::ostream &command,
    const std::filesystem::path &summary_path) {
  command << " --summary-out "
          << QuoteFrontendCApiRunnerPowerShellArg(summary_path.generic_string());
}
