#include "tools/objc3c_frontend_c_api_runner_bonus_experiences_json_sections.h"

FrontendCApiRunnerArtifactPathView
BuildFrontendCApiRunnerBonusExperiencesArtifactPaths(
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &summary_path_text,
    const std::string &runtime_metadata_binary_path_text) {
  FrontendCApiRunnerArtifactPathView paths =
      BuildFrontendCApiRunnerArtifactPathView(result, summary_path_text);
  paths.runtime_metadata_binary = runtime_metadata_binary_path_text;
  return paths;
}
