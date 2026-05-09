#pragma once

#include <string>
#include <vector>

#include "contracts/objc3_frontend_diagnostic_stage_contract.h"
#include "parse/objc3_diagnostics_bus.h"

inline void AppendDiagnosticSlice(
    const Objc3FrontendDiagnosticStageSlice &slice,
    std::vector<std::string> &diagnostics) {
  if (!Objc3FrontendDiagnosticStageSliceIsValid(slice)) {
    return;
  }
  diagnostics.insert(diagnostics.end(), slice.diagnostics->begin(),
                     slice.diagnostics->end());
}

inline std::vector<std::string> FlattenStageDiagnostics(
    const Objc3FrontendDiagnosticsBus &stage_diagnostics) {
  std::vector<std::string> diagnostics;
  diagnostics.reserve(stage_diagnostics.size());
  for (const auto &slice : FrontendDiagnosticStageSlices(stage_diagnostics)) {
    AppendDiagnosticSlice(slice, diagnostics);
  }
  return diagnostics;
}

inline std::vector<std::string> FlattenStageDiagnostics(
    const Objc3FrontendDiagnosticsBus &stage_diagnostics,
    const std::vector<std::string> &post_pipeline_diagnostics) {
  std::vector<std::string> diagnostics =
      FlattenStageDiagnostics(stage_diagnostics);
  diagnostics.reserve(diagnostics.size() + post_pipeline_diagnostics.size());
  AppendDiagnosticSlice(PostPipelineDiagnosticSlice(post_pipeline_diagnostics),
                        diagnostics);
  return diagnostics;
}
