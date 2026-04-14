#pragma once

#include <string>
#include <vector>

#include "parse/objc3_diagnostics_bus.h"

inline std::vector<std::string> FlattenStageDiagnostics(const Objc3FrontendDiagnosticsBus &stage_diagnostics) {
  std::vector<std::string> diagnostics;
  diagnostics.reserve(stage_diagnostics.size());
  diagnostics.insert(diagnostics.end(), stage_diagnostics.lexer.begin(), stage_diagnostics.lexer.end());
  diagnostics.insert(diagnostics.end(), stage_diagnostics.parser.begin(), stage_diagnostics.parser.end());
  diagnostics.insert(diagnostics.end(), stage_diagnostics.semantic.begin(), stage_diagnostics.semantic.end());
  return diagnostics;
}

inline std::vector<std::string> FlattenStageDiagnostics(
    const Objc3FrontendDiagnosticsBus &stage_diagnostics,
    const std::vector<std::string> &post_pipeline_diagnostics) {
  std::vector<std::string> diagnostics = FlattenStageDiagnostics(stage_diagnostics);
  diagnostics.reserve(diagnostics.size() + post_pipeline_diagnostics.size());
  diagnostics.insert(diagnostics.end(), post_pipeline_diagnostics.begin(), post_pipeline_diagnostics.end());
  return diagnostics;
}
