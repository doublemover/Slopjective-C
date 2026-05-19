#pragma once

#include <iosfwd>
#include <string>

#include "tools/objc3c_frontend_c_api_runner_artifact_paths.h"

void WriteFrontendCApiRunnerRuntimeInspectorObjectInspectionJsonRows(
    std::ostream &out,
    const std::string &child_indent,
    const FrontendCApiRunnerArtifactPathView &paths);
