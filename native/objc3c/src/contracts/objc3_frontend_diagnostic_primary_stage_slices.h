#pragma once

#include <array>

#include "contracts/objc3_frontend_diagnostic_stage_slice_record.h"
#include "parse/objc3_diagnostics_bus.h"

inline std::array<Objc3FrontendDiagnosticStageSlice, 3>
FrontendDiagnosticStageSlices(
    const Objc3FrontendDiagnosticsBus &stage_diagnostics) {
  return {{
      {Objc3FrontendDiagnosticStage::kLexer,
       Objc3FrontendDiagnosticsBusContractId(), &stage_diagnostics.lexer},
      {Objc3FrontendDiagnosticStage::kParser,
       Objc3FrontendDiagnosticsBusContractId(), &stage_diagnostics.parser},
      {Objc3FrontendDiagnosticStage::kSemantic,
       Objc3FrontendDiagnosticsBusContractId(), &stage_diagnostics.semantic},
  }};
}
