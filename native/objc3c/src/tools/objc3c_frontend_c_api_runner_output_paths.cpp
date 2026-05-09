#include "tools/objc3c_frontend_c_api_runner_output_paths.h"

#include "tools/objc3c_frontend_c_api_runner_result.h"

std::filesystem::path BuildFrontendCApiRunnerDiagnosticsOutputPath(
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result) {
  const std::string diagnostics_path_text =
      FrontendCApiResultArtifactPath(result,
                                     OBJC3C_FRONTEND_ARTIFACT_DIAGNOSTICS);
  return diagnostics_path_text.empty()
             ? (options.out_dir / (options.emit_prefix + ".diagnostics.json"))
             : std::filesystem::path(diagnostics_path_text);
}
