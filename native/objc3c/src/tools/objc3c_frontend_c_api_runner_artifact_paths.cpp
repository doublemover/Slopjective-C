#include "tools/objc3c_frontend_c_api_runner_artifact_paths.h"

#include "tools/objc3c_frontend_c_api_runner_result_artifact_snapshot.h"

FrontendCApiRunnerArtifactPathView BuildFrontendCApiRunnerArtifactPathView(
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &summary_path_text) {
  FrontendCApiRunnerArtifactPathView paths;
  paths.summary = summary_path_text;
  paths.diagnostics = FrontendCApiRunnerResultArtifactPathText(
      result,
      OBJC3C_FRONTEND_ARTIFACT_DIAGNOSTICS);
  paths.manifest = FrontendCApiRunnerResultArtifactPathText(
      result,
      OBJC3C_FRONTEND_ARTIFACT_MANIFEST);
  paths.ir = FrontendCApiRunnerResultArtifactPathText(
      result,
      OBJC3C_FRONTEND_ARTIFACT_IR);
  paths.object = FrontendCApiRunnerResultArtifactPathText(
      result,
      OBJC3C_FRONTEND_ARTIFACT_OBJECT);
  paths.runtime_metadata_binary = FrontendCApiRunnerResultArtifactPathText(
      result,
      OBJC3C_FRONTEND_ARTIFACT_RUNTIME_METADATA);
  return paths;
}

FrontendCApiRunnerArtifactPathView BuildFrontendCApiRunnerArtifactPathView(
    const objc3c_frontend_c_compile_result_t &result,
    const std::filesystem::path &summary_path) {
  return BuildFrontendCApiRunnerArtifactPathView(
      result,
      summary_path.generic_string());
}
