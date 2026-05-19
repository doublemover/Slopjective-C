#pragma once

#include <string>
#include <vector>

[[nodiscard]] std::string BuildDiagnosticsTextArtifact(
    const std::vector<std::string> &diagnostics);
[[nodiscard]] std::string BuildDiagnosticsJsonArtifact(
    const std::vector<std::string> &diagnostics);
