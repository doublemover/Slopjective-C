#pragma once

#include <array>
#include <string>
#include <string_view>
#include <vector>

#include "contracts/objc3_contract_helpers.h"
#include "contracts/objc3_native_contract_ids.h"
#include "parse/objc3_diagnostics_bus.h"

enum class Objc3FrontendDiagnosticStage {
  kLexer,
  kParser,
  kSemantic,
  kPostPipeline,
};

inline constexpr std::string_view Objc3FrontendDiagnosticsBusContractId() {
  return Objc3NativeContractIdSpelling(
      Objc3NativeContractId::kFrontendDiagnosticsBusV1);
}

struct Objc3FrontendDiagnosticStageSlice {
  Objc3FrontendDiagnosticStage stage = Objc3FrontendDiagnosticStage::kLexer;
  std::string_view contract_id = Objc3FrontendDiagnosticsBusContractId();
  const std::vector<std::string> *diagnostics = nullptr;
};

inline std::string_view Objc3FrontendDiagnosticStageName(
    Objc3FrontendDiagnosticStage stage) {
  switch (stage) {
    case Objc3FrontendDiagnosticStage::kLexer:
      return "lexer";
    case Objc3FrontendDiagnosticStage::kParser:
      return "parser";
    case Objc3FrontendDiagnosticStage::kSemantic:
      return "semantic";
    case Objc3FrontendDiagnosticStage::kPostPipeline:
      return "post-pipeline";
  }
  return {};
}

inline bool Objc3FrontendDiagnosticStageIsKnown(
    Objc3FrontendDiagnosticStage stage) {
  return !Objc3FrontendDiagnosticStageName(stage).empty();
}

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
