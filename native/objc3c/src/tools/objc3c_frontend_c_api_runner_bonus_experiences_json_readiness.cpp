#include "tools/objc3c_frontend_c_api_runner_bonus_experiences_json_sections.h"

#include <filesystem>

#include "tools/objc3c_frontend_c_api_runner_read_command.h"

namespace fs = std::filesystem;

FrontendCApiRunnerBonusExperienceReadiness
ProbeFrontendCApiRunnerBonusExperienceReadiness(
    const objc3c_frontend_c_compile_result_t &result,
    const FrontendCApiRunnerArtifactPathView &paths) {
  FrontendCApiRunnerBonusExperienceReadiness readiness;
  readiness.compile_surface_ready = result.emit.attempted != 0;
  readiness.runtime_inspector_ready =
      FrontendCApiRunnerPathExists(paths.object) &&
      FrontendCApiRunnerPathExists(paths.runtime_metadata_binary);
  readiness.showcase_surface_ready =
      fs::exists(fs::path("showcase") / "portfolio.json") &&
      fs::exists(fs::path("showcase") / "tutorial_walkthrough.json");
  readiness.tutorial_surface_ready =
      fs::exists(fs::path("docs") / "tutorials" / "build_run_verify.md") &&
      fs::exists(fs::path("docs") / "tutorials" / "guided_walkthrough.md");
  return readiness;
}
