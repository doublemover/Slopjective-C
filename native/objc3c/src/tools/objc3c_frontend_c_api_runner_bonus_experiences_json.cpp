#include "tools/objc3c_frontend_c_api_runner_bonus_experiences_json.h"

#include "tools/objc3c_frontend_c_api_runner_bonus_experiences_json_sections.h"

void WriteFrontendCApiRunnerBonusExperiencesJson(
    std::ostream &out,
    const std::string &indent,
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &summary_path_text,
    const std::string &runtime_metadata_binary_path_text) {
  const FrontendCApiRunnerArtifactPathView paths =
      BuildFrontendCApiRunnerBonusExperiencesArtifactPaths(
          result,
          summary_path_text,
          runtime_metadata_binary_path_text);
  const FrontendCApiRunnerBonusExperienceReadiness readiness =
      ProbeFrontendCApiRunnerBonusExperienceReadiness(result, paths);
  const std::string child_indent = indent + "  ";
  const std::string grandchild_indent = child_indent + "  ";

  out << "{\n";
  WriteFrontendCApiRunnerBonusExperienceSections(
      out,
      child_indent,
      grandchild_indent,
      options,
      paths,
      readiness);
  out << indent << "}";
}
