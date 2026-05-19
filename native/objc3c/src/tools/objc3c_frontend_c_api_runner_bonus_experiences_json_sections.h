#pragma once

#include <ostream>
#include <string>

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_artifact_paths.h"
#include "tools/objc3c_frontend_c_api_runner_options.h"

struct FrontendCApiRunnerBonusExperienceReadiness {
  bool compile_surface_ready = false;
  bool runtime_inspector_ready = false;
  bool showcase_surface_ready = false;
  bool tutorial_surface_ready = false;
};

FrontendCApiRunnerArtifactPathView
BuildFrontendCApiRunnerBonusExperiencesArtifactPaths(
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &summary_path_text,
    const std::string &runtime_metadata_binary_path_text);

FrontendCApiRunnerBonusExperienceReadiness
ProbeFrontendCApiRunnerBonusExperienceReadiness(
    const objc3c_frontend_c_compile_result_t &result,
    const FrontendCApiRunnerArtifactPathView &paths);

void WriteFrontendCApiRunnerBonusExperienceSections(
    std::ostream &out,
    const std::string &child_indent,
    const std::string &grandchild_indent,
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerArtifactPathView &paths,
    const FrontendCApiRunnerBonusExperienceReadiness &readiness);
