#include "tools/objc3c_frontend_c_api_runner_output_paths.h"

#include "tools/objc3c_frontend_c_api_runner_artifact_paths.h"

std::filesystem::path BuildFrontendCApiRunnerDiagnosticsOutputPath(
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result) {
  const FrontendCApiRunnerArtifactPathView paths =
      BuildFrontendCApiRunnerArtifactPathView(result, std::string());
  return paths.diagnostics.empty()
             ? (options.out_dir / (options.emit_prefix + ".diagnostics.json"))
             : std::filesystem::path(paths.diagnostics);
}
