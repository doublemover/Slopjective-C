#pragma once

#include <array>

#include "contracts/objc3_frontend_diagnostic_stage_slice_record.h"
#include "parse/objc3_diagnostics_bus.h"

inline std::array<Objc3FrontendDiagnosticStageSlice, 3>
FrontendDiagnosticStageSlices(
    const Objc3FrontendDiagnosticsBus &stage_diagnostics) {
  return {{
      {Objc3FrontendDiagnosticStage::kLexer,
       Objc3FrontendDiagnosticsBusContractId(),
       &stage_diagnostics.lexer,
       kObjc3DiagnosticOwnerContractId,
       kObjc3DiagnosticNoFallbackOwnerModel,
       false,
       false,
       false},
      {Objc3FrontendDiagnosticStage::kParser,
       Objc3FrontendDiagnosticsBusContractId(),
       &stage_diagnostics.parser,
       kObjc3DiagnosticOwnerContractId,
       kObjc3DiagnosticNoFallbackOwnerModel,
       false,
       false,
       false},
      {Objc3FrontendDiagnosticStage::kSemantic,
       Objc3FrontendDiagnosticsBusContractId(),
       &stage_diagnostics.semantic,
       kObjc3DiagnosticOwnerContractId,
       kObjc3DiagnosticNoFallbackOwnerModel,
       false,
       false,
       false},
  }};
}
