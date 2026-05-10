#include "tools/objc3c_frontend_c_api_runner_bonus_experiences_json_sections.h"

#include "tools/objc3c_frontend_c_api_runner_bonus_experiences_json_contract.h"
#include "tools/objc3c_frontend_c_api_runner_bonus_experiences_json_playground.h"
#include "tools/objc3c_frontend_c_api_runner_bonus_experiences_json_runtime_inspector.h"
#include "tools/objc3c_frontend_c_api_runner_bonus_experiences_json_template_demo.h"

void WriteFrontendCApiRunnerBonusExperienceSections(
    std::ostream &out,
    const std::string &child_indent,
    const std::string &grandchild_indent,
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerArtifactPathView &paths,
    const FrontendCApiRunnerBonusExperienceReadiness &readiness) {
  WriteFrontendCApiRunnerBonusExperiencesContractJsonRows(out, child_indent);
  WriteFrontendCApiRunnerBonusExperiencesPlaygroundJsonRows(
      out,
      child_indent,
      grandchild_indent,
      options,
      paths,
      readiness.compile_surface_ready);
  WriteFrontendCApiRunnerBonusExperiencesRuntimeInspectorJsonRows(
      out,
      child_indent,
      grandchild_indent,
      paths,
      readiness.runtime_inspector_ready);
  WriteFrontendCApiRunnerBonusExperiencesTemplateDemoJsonRows(
      out,
      child_indent,
      grandchild_indent,
      readiness.showcase_surface_ready,
      readiness.tutorial_surface_ready);
}
