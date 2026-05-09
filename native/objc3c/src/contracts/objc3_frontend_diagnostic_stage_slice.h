#pragma once

#include <array>
#include <string>
#include <string_view>
#include <vector>

#include "contracts/objc3_contract_helpers.h"
#include "contracts/objc3_frontend_diagnostic_stage_kind.h"
#include "contracts/objc3_native_contract_ids.h"
#include "parse/objc3_diagnostics_bus.h"

inline constexpr std::string_view Objc3FrontendDiagnosticsBusContractId() {
  return Objc3NativeContractIdSpelling(
      Objc3NativeContractId::kFrontendDiagnosticsBusV1);
}

struct Objc3FrontendDiagnosticStageSlice {
  Objc3FrontendDiagnosticStage stage = Objc3FrontendDiagnosticStage::kLexer;
  std::string_view contract_id = Objc3FrontendDiagnosticsBusContractId();
  const std::vector<std::string> *diagnostics = nullptr;
};

inline bool Objc3FrontendDiagnosticStageSliceIsValid(
    const Objc3FrontendDiagnosticStageSlice &slice) {
  return Objc3FrontendDiagnosticStageIsKnown(slice.stage) &&
         Objc3ContractIdMatches(
             slice.contract_id,
             Objc3NativeContractId::kFrontendDiagnosticsBusV1) &&
         slice.diagnostics != nullptr;
}

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

inline Objc3FrontendDiagnosticStageSlice PostPipelineDiagnosticSlice(
    const std::vector<std::string> &post_pipeline_diagnostics) {
  return Objc3FrontendDiagnosticStageSlice{
      Objc3FrontendDiagnosticStage::kPostPipeline,
      Objc3FrontendDiagnosticsBusContractId(),
      &post_pipeline_diagnostics};
}
