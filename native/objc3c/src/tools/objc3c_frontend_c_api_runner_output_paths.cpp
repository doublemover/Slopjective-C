#include "tools/objc3c_frontend_c_api_runner_output_paths.h"

#include "tools/objc3c_frontend_c_api_runner_result_artifact_snapshot.h"

bool ResolveFrontendCApiRunnerDiagnosticsOutputPath(
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result,
    std::filesystem::path &diagnostics_path,
    std::string &error) {
  (void)options;
  const FrontendCApiRunnerResultArtifactSnapshot diagnostics =
      CaptureFrontendCApiRunnerResultArtifactSnapshot(
          result,
          OBJC3C_FRONTEND_ARTIFACT_DIAGNOSTICS);
  if (!diagnostics.path.present) {
    error =
        "diagnostics output path contract fail-closed: result-owned "
        "diagnostics artifact path is absent";
    return false;
  }
  if (diagnostics.path.text.empty()) {
    error =
        "diagnostics output path contract fail-closed: result-owned "
        "diagnostics artifact path is empty";
    return false;
  }
  diagnostics_path = std::filesystem::path(diagnostics.path.text);
  error.clear();
  return true;
}
