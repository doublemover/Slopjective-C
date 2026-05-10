#include "tools/objc3c_frontend_c_api_runner_observability_json_sections.h"

#include "tools/objc3c_frontend_c_api_runner_observability_json_artifacts.h"
#include "tools/objc3c_frontend_c_api_runner_observability_json_diagnostics.h"
#include "tools/objc3c_frontend_c_api_runner_observability_json_dump_commands.h"
#include "tools/objc3c_frontend_c_api_runner_observability_json_status.h"

void WriteFrontendCApiRunnerObservabilitySections(
    std::ostream &out,
    objc3c_frontend_c_status_t status,
    const FrontendCApiRunnerObservabilityContext &context) {
  WriteFrontendCApiRunnerObservabilityStatusStageJsonRows(
      out,
      context.child_indent,
      status,
      context.last_attempted_stage,
      context.blocking_stage);
  WriteFrontendCApiRunnerObservabilityDiagnosticTotalJsonRows(
      out,
      context.child_indent,
      context.diagnostic_totals,
      context.result_error_message_present);
  WriteFrontendCApiRunnerObservabilityArtifactPresenceJsonRows(
      out,
      context.child_indent,
      context.grandchild_indent,
      context.paths);
  WriteFrontendCApiRunnerObservabilityDumpCommandJsonRows(
      out,
      context.child_indent,
      context.grandchild_indent,
      context.paths);
}
