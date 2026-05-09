#pragma once

#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

#include "contracts/objc3_diagnostic_payload_contract.h"
#include "contracts/objc3_native_contract_ids.h"
#include "parse/objc3_diagnostics_bus.h"

enum class Objc3FrontendDiagnosticStage {
  kLexer,
  kParser,
  kSemantic,
  kPostPipeline,
};

struct Objc3FrontendDiagnosticStageSlice {
  Objc3FrontendDiagnosticStage stage = Objc3FrontendDiagnosticStage::kLexer;
  std::string_view contract_id =
      Objc3NativeContractIdSpelling(
          Objc3NativeContractId::kFrontendDiagnosticsBusV1);
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
  return "unknown";
}

inline std::array<Objc3FrontendDiagnosticStageSlice, 3>
FrontendDiagnosticStageSlices(
    const Objc3FrontendDiagnosticsBus &stage_diagnostics) {
  return {{
      {Objc3FrontendDiagnosticStage::kLexer,
       Objc3NativeContractIdSpelling(
           Objc3NativeContractId::kFrontendDiagnosticsBusV1),
       &stage_diagnostics.lexer},
      {Objc3FrontendDiagnosticStage::kParser,
       Objc3NativeContractIdSpelling(
           Objc3NativeContractId::kFrontendDiagnosticsBusV1),
       &stage_diagnostics.parser},
      {Objc3FrontendDiagnosticStage::kSemantic,
       Objc3NativeContractIdSpelling(
           Objc3NativeContractId::kFrontendDiagnosticsBusV1),
       &stage_diagnostics.semantic},
  }};
}

inline void AppendDiagnosticSlice(
    const Objc3FrontendDiagnosticStageSlice &slice,
    std::vector<std::string> &diagnostics) {
  if (slice.diagnostics == nullptr) {
    return;
  }
  diagnostics.insert(diagnostics.end(), slice.diagnostics->begin(),
                     slice.diagnostics->end());
}

inline Objc3FrontendDiagnosticStageSlice PostPipelineDiagnosticSlice(
    const std::vector<std::string> &post_pipeline_diagnostics) {
  return Objc3FrontendDiagnosticStageSlice{
      Objc3FrontendDiagnosticStage::kPostPipeline,
      Objc3NativeContractIdSpelling(
          Objc3NativeContractId::kFrontendDiagnosticsBusV1),
      &post_pipeline_diagnostics};
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
