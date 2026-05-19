#pragma once

#include <iosfwd>
#include <string>

#include "tools/objc3c_frontend_c_api_runner_artifact_paths.h"
#include "tools/objc3c_frontend_c_api_runner_options.h"

void WriteFrontendCApiRunnerBonusExperiencesPlaygroundAvailabilitySourceRows(
    std::ostream &out,
    const std::string &grandchild_indent,
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerArtifactPathView &paths,
    bool compile_surface_ready);

void WriteFrontendCApiRunnerBonusExperiencesPlaygroundArrayRows(
    std::ostream &out,
    const std::string &grandchild_indent);

void WriteFrontendCApiRunnerBonusExperiencesPlaygroundDumpCommandRows(
    std::ostream &out,
    const std::string &grandchild_indent,
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerArtifactPathView &paths);
