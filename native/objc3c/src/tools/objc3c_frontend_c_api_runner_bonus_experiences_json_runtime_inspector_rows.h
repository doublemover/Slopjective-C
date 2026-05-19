#pragma once

#include <iosfwd>
#include <string>

#include "tools/objc3c_frontend_c_api_runner_artifact_paths.h"

void WriteFrontendCApiRunnerBonusRuntimeInspectorAvailabilityRows(
    std::ostream &out,
    const std::string &grandchild_indent,
    bool runtime_inspector_ready);

void WriteFrontendCApiRunnerBonusRuntimeInspectorSectionFields(
    std::ostream &out,
    const std::string &grandchild_indent,
    const FrontendCApiRunnerArtifactPathView &paths);

void WriteFrontendCApiRunnerBonusRuntimeInspectorArtifactCommandRows(
    std::ostream &out,
    const std::string &grandchild_indent,
    const FrontendCApiRunnerArtifactPathView &paths);
