#include "driver/objc3_driver_frontend_diagnostic_artifact_publication.h"

#include "io/objc3_diagnostics_artifacts.h"

bool PublishObjc3DriverFrontendDiagnosticArtifacts(
    const Objc3CliOptions &cli_options,
    const Objc3FrontendArtifactBundle &artifacts) {
  WriteDiagnosticsArtifacts(cli_options.out_dir,
                            cli_options.emit_prefix,
                            artifacts.stage_diagnostics,
                            artifacts.post_pipeline_diagnostics);
  return !artifacts.diagnostics.empty();
}
