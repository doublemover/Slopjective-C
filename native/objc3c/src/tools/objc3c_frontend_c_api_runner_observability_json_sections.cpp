#include "tools/objc3c_frontend_c_api_runner_observability_json_sections.h"

#include "tools/objc3c_frontend_c_api_runner_observability_json_artifacts.h"
#include "tools/objc3c_frontend_c_api_runner_observability_json_diagnostics.h"
#include "tools/objc3c_frontend_c_api_runner_observability_json_dump_commands.h"
#include "tools/objc3c_frontend_c_api_runner_observability_json_status.h"

void WriteFrontendCApiRunnerObservabilitySections(
    std::ostream &out,
    const FrontendCApiRunnerObservabilityContext &context) {
  WriteFrontendCApiRunnerObservabilityStatusStageJsonRows(
      out,
      context.child_indent,
      context.publication.status,
      context.publication.last_attempted_stage,
      context.publication.blocking_stage);
  WriteFrontendCApiRunnerObservabilityDiagnosticTotalJsonRows(
      out,
      context.child_indent,
      context.publication.diagnostics.totals,
      context.publication.diagnostics.result_error_message_present);
  WriteFrontendCApiRunnerObservabilityArtifactPresenceJsonRows(
      out,
      context.child_indent,
      context.grandchild_indent,
      context.publication.paths);
  WriteFrontendCApiRunnerObservabilityDumpCommandJsonRows(
      out,
      context.child_indent,
      context.grandchild_indent,
      context.publication.paths);
}
