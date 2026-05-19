#include "tools/objc3c_frontend_c_api_runner_observability_publication.h"

#include "tools/objc3c_frontend_c_api_runner_dump_publication.h"
#include "tools/objc3c_frontend_c_api_runner_public_result.h"
#include "tools/objc3c_frontend_c_api_runner_stage_selection.h"

namespace {

FrontendCApiRunnerObservabilityPublication
BuildFrontendCApiRunnerObservabilityPublicationFromFields(
    const std::string &summary_path_text,
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_status_t status,
    const std::string &result_error_message,
    const std::string &runtime_metadata_binary_path_text) {
  FrontendCApiRunnerObservabilityPublication publication;
  publication.status = status;
  publication.paths =
      BuildFrontendCApiRunnerArtifactPathView(result, summary_path_text);
  publication.paths.runtime_metadata_binary =
      runtime_metadata_binary_path_text;
  publication.diagnostics.totals = BuildFrontendCApiDiagnosticTotals(result);
  publication.diagnostics.result_error_message = result_error_message;
  publication.diagnostics.result_error_message_present =
      !result_error_message.empty();
  publication.last_attempted_stage = LastAttemptedFrontendCApiStageName(result);
  publication.blocking_stage = BlockingFrontendCApiStageName(result);
  return publication;
}

}  // namespace

FrontendCApiRunnerObservabilityPublication
BuildFrontendCApiRunnerObservabilityPublication(
    const FrontendCApiRunnerDumpPublication &publication) {
  return BuildFrontendCApiRunnerObservabilityPublicationFromFields(
      publication.summary_path.generic_string(),
      FrontendCApiRunnerDumpCompileResult(publication),
      publication.status,
      publication.result_error_message,
      publication.runtime_metadata_binary_path_text);
}

FrontendCApiRunnerObservabilityPublication
BuildFrontendCApiRunnerObservabilityPublication(
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_status_t status,
    const FrontendCApiRunnerPublicResultView &public_result) {
  return BuildFrontendCApiRunnerObservabilityPublicationFromFields(
      public_result.paths.summary,
      result,
      status,
      public_result.diagnostics.result_error_message,
      public_result.paths.runtime_metadata_binary);
}
