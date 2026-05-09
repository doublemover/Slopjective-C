#include "driver/objc3_driver_frontend_artifact_handoff.h"

#include <iostream>

#include "io/objc3_diagnostics_artifacts.h"
#include "io/objc3_manifest_artifacts.h"
#include "pipeline/objc3_runtime_import_surface.h"

Objc3DriverFrontendArtifactHandoffResult
PublishObjc3DriverFrontendArtifactHandoff(
    const Objc3CliOptions &cli_options,
    const Objc3FrontendArtifactBundle &artifacts) {
  WriteDiagnosticsArtifacts(cli_options.out_dir,
                            cli_options.emit_prefix,
                            artifacts.stage_diagnostics,
                            artifacts.post_pipeline_diagnostics);
  if (!artifacts.manifest_json.empty()) {
    WriteManifestArtifact(cli_options.out_dir,
                          cli_options.emit_prefix,
                          artifacts.manifest_json);
  }
  if (!artifacts.runtime_metadata_binary.empty()) {
    WriteRuntimeMetadataBinaryArtifact(cli_options.out_dir,
                                       cli_options.emit_prefix,
                                       artifacts.runtime_metadata_binary);
  }
  if (!artifacts.error_handling_result_bridge_artifact_replay_json.empty()) {
    WriteErrorHandlingResultBridgeArtifactReplay(
        cli_options.out_dir,
        cli_options.emit_prefix,
        artifacts.error_handling_result_bridge_artifact_replay_json);
  }
  if (!artifacts.diagnostics.empty()) {
    return {.status_code = 1, .has_runtime_import_artifact = false};
  }

  const bool has_runtime_import_artifact =
      !artifacts.runtime_aware_import_module_artifact_json.empty();
  if (has_runtime_import_artifact &&
      !IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(
          artifacts.runtime_aware_import_module_frontend_closure_summary)) {
    std::cerr << "runtime-aware import/module frontend closure not ready\n";
    return {.status_code = 125,
            .has_runtime_import_artifact = has_runtime_import_artifact};
  }
  if (has_runtime_import_artifact) {
    WriteRuntimeAwareImportModuleArtifact(
        cli_options.out_dir,
        cli_options.emit_prefix,
        artifacts.runtime_aware_import_module_artifact_json);
  }
  if (!artifacts.interop_bridge_header_artifact_text.empty()) {
    WriteInteropBridgeHeaderArtifact(
        cli_options.out_dir,
        cli_options.emit_prefix,
        artifacts.interop_bridge_header_artifact_text);
  }
  if (!artifacts.interop_bridge_module_artifact_text.empty()) {
    WriteInteropBridgeModuleArtifact(
        cli_options.out_dir,
        cli_options.emit_prefix,
        artifacts.interop_bridge_module_artifact_text);
  }
  if (!artifacts.interop_bridge_artifact_json.empty()) {
    WriteInteropBridgeArtifact(cli_options.out_dir,
                               cli_options.emit_prefix,
                               artifacts.interop_bridge_artifact_json);
  }
  return {.status_code = 0,
          .has_runtime_import_artifact = has_runtime_import_artifact};
}
