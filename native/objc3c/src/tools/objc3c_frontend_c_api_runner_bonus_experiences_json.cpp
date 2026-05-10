#include "tools/objc3c_frontend_c_api_runner_bonus_experiences_json.h"

#include <filesystem>

#include "tools/objc3c_frontend_c_api_runner_artifact_paths.h"
#include "tools/objc3c_frontend_c_api_runner_bonus_experiences_json_contract.h"
#include "tools/objc3c_frontend_c_api_runner_bonus_experiences_json_playground.h"
#include "tools/objc3c_frontend_c_api_runner_bonus_experiences_json_runtime_inspector.h"
#include "tools/objc3c_frontend_c_api_runner_bonus_experiences_json_template_demo.h"

namespace fs = std::filesystem;

void WriteFrontendCApiRunnerBonusExperiencesJson(
    std::ostream &out,
    const std::string &indent,
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &summary_path_text,
    const std::string &runtime_metadata_binary_path_text) {
  FrontendCApiRunnerArtifactPathView paths =
      BuildFrontendCApiRunnerArtifactPathView(result, summary_path_text);
  paths.runtime_metadata_binary = runtime_metadata_binary_path_text;
  const std::string child_indent = indent + "  ";
  const std::string grandchild_indent = child_indent + "  ";
  const bool compile_surface_ready = result.emit.attempted != 0;
  const bool runtime_inspector_ready =
      FrontendCApiRunnerPathExists(paths.object) &&
      FrontendCApiRunnerPathExists(paths.runtime_metadata_binary);
  const bool showcase_surface_ready =
      fs::exists(fs::path("showcase") / "portfolio.json") &&
      fs::exists(fs::path("showcase") / "tutorial_walkthrough.json");
  const bool tutorial_surface_ready =
      fs::exists(fs::path("docs") / "tutorials" / "build_run_verify.md") &&
      fs::exists(fs::path("docs") / "tutorials" / "guided_walkthrough.md");

  out << "{\n";
  WriteFrontendCApiRunnerBonusExperiencesContractJsonRows(out, child_indent);
  WriteFrontendCApiRunnerBonusExperiencesPlaygroundJsonRows(
      out,
      child_indent,
      grandchild_indent,
      options,
      paths,
      compile_surface_ready);
  WriteFrontendCApiRunnerBonusExperiencesRuntimeInspectorJsonRows(
      out,
      child_indent,
      grandchild_indent,
      paths,
      runtime_inspector_ready);
  WriteFrontendCApiRunnerBonusExperiencesTemplateDemoJsonRows(
      out,
      child_indent,
      grandchild_indent,
      showcase_surface_ready,
      tutorial_surface_ready);
  out << indent << "}";
}
