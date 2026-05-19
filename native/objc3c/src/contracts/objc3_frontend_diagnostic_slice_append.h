#pragma once

#include <string>
#include <vector>

#include "contracts/objc3_frontend_diagnostic_stage_slice.h"

inline void AppendDiagnosticSlice(
    const Objc3FrontendDiagnosticStageSlice &slice,
    std::vector<std::string> &diagnostics) {
  if (!Objc3FrontendDiagnosticStageSliceIsValid(slice)) {
    return;
  }
  diagnostics.insert(diagnostics.end(), slice.diagnostics->begin(),
                     slice.diagnostics->end());
}
