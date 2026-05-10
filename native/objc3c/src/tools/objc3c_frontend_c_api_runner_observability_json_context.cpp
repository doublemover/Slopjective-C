#include "tools/objc3c_frontend_c_api_runner_observability_json_context.h"

#include "tools/objc3c_frontend_c_api_runner_stage_selection.h"

FrontendCApiRunnerObservabilityContext
BuildFrontendCApiRunnerObservabilityContext(
    const std::string &indent,
    const std::string &summary_path_text,
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &result_error_message,
    const std::string &runtime_metadata_binary_path_text) {
  FrontendCApiRunnerObservabilityContext context;
  context.paths =
      BuildFrontendCApiRunnerArtifactPathView(result, summary_path_text);
  context.paths.runtime_metadata_binary = runtime_metadata_binary_path_text;
  context.diagnostic_totals = BuildFrontendCApiDiagnosticTotals(result);
  context.last_attempted_stage = LastAttemptedFrontendCApiStageName(result);
  context.blocking_stage = BlockingFrontendCApiStageName(result);
  context.child_indent = indent + "  ";
  context.grandchild_indent = context.child_indent + "  ";
  context.result_error_message_present = !result_error_message.empty();
  return context;
}
