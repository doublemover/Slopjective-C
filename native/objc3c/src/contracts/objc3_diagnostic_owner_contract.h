#pragma once

#include <array>
#include <cstddef>
#include <string_view>

#include "contracts/objc3_frontend_diagnostic_stage_kind.h"

inline constexpr std::string_view kObjc3DiagnosticOwnerContractId =
    "objc3c.native.diagnostic.owner-split.hard-cutover.v1";
inline constexpr std::string_view kObjc3DiagnosticNoRetiredRouteOwnerModel =
    "strict-hard-cutover-no-retired-route-no-compatibility-gate";

inline constexpr std::string_view kObjc3LexerDiagnosticCatalogOwner =
    "native.frontend.lexer.diagnostic-catalog";
inline constexpr std::string_view kObjc3LexerDiagnosticStageOwner =
    "native.frontend.lexer.diagnostic-stage";
inline constexpr std::string_view kObjc3ParserDiagnosticCatalogOwner =
    "native.frontend.parser.diagnostic-catalog";
inline constexpr std::string_view kObjc3ParserDiagnosticStageOwner =
    "native.frontend.parser.diagnostic-stage";
inline constexpr std::string_view kObjc3ParserDiagnosticFixitOwner =
    "native.frontend.parser.diagnostic-fixit";
inline constexpr std::string_view kObjc3ParserDiagnosticRecoveryOwner =
    "native.frontend.parser.diagnostic-recovery";
inline constexpr std::string_view kObjc3SemaDiagnosticCatalogOwner =
    "native.frontend.sema.diagnostic-catalog";
inline constexpr std::string_view kObjc3SemaDiagnosticStageOwner =
    "native.frontend.sema.diagnostic-stage";
inline constexpr std::string_view kObjc3SemaDiagnosticFixitOwner =
    "native.frontend.sema.diagnostic-fixit";
inline constexpr std::string_view kObjc3SemaDiagnosticRecoveryOwner =
    "native.frontend.sema.diagnostic-recovery";
inline constexpr std::string_view kObjc3PostPipelineDiagnosticStageOwner =
    "native.frontend.post-pipeline.diagnostic-stage";
inline constexpr std::string_view kObjc3RuntimeDiagnosticCatalogOwner =
    "native.runtime.diagnostic-catalog";
inline constexpr std::string_view kObjc3FrontendApiDiagnosticCatalogOwner =
    "native.frontend.api.diagnostic-catalog";
inline constexpr std::string_view kObjc3ArtifactDiagnosticCatalogOwner =
    "native.artifact.diagnostic-catalog";
inline constexpr std::string_view kObjc3ToolingDiagnosticCatalogOwner =
    "native.tooling.diagnostic-catalog";

struct Objc3DiagnosticStageOwnerContract {
  Objc3FrontendDiagnosticStage stage = Objc3FrontendDiagnosticStage::kLexer;
  std::string_view stage_name;
  std::string_view catalog_owner;
  std::string_view stage_owner;
  std::string_view fixit_owner;
  std::string_view recovery_owner;
  std::string_view owner_model = kObjc3DiagnosticNoRetiredRouteOwnerModel;
  bool retired_route_allowed = false;
  bool compatibility_gate_allowed = false;
  bool legacy_positive_allowed = false;
  bool recovery_counts_as_success = false;
};

inline constexpr std::array<Objc3DiagnosticStageOwnerContract, 4>
    kObjc3DiagnosticStageOwnerContracts = {{
        {Objc3FrontendDiagnosticStage::kLexer,
         "lexer",
         kObjc3LexerDiagnosticCatalogOwner,
         kObjc3LexerDiagnosticStageOwner,
         kObjc3ParserDiagnosticFixitOwner,
         kObjc3ParserDiagnosticRecoveryOwner},
        {Objc3FrontendDiagnosticStage::kParser,
         "parser",
         kObjc3ParserDiagnosticCatalogOwner,
         kObjc3ParserDiagnosticStageOwner,
         kObjc3ParserDiagnosticFixitOwner,
         kObjc3ParserDiagnosticRecoveryOwner},
        {Objc3FrontendDiagnosticStage::kSemantic,
         "semantic",
         kObjc3SemaDiagnosticCatalogOwner,
         kObjc3SemaDiagnosticStageOwner,
         kObjc3SemaDiagnosticFixitOwner,
         kObjc3SemaDiagnosticRecoveryOwner},
        {Objc3FrontendDiagnosticStage::kPostPipeline,
         "post-pipeline",
         kObjc3SemaDiagnosticCatalogOwner,
         kObjc3PostPipelineDiagnosticStageOwner,
         kObjc3SemaDiagnosticFixitOwner,
         kObjc3SemaDiagnosticRecoveryOwner},
    }};

inline bool Objc3DiagnosticOwnerIsExplicit(std::string_view owner) {
  constexpr std::string_view kNativePrefix = "native.";
  return owner.size() > kNativePrefix.size() &&
         owner.substr(0, kNativePrefix.size()) == kNativePrefix;
}

inline const Objc3DiagnosticStageOwnerContract *
FindObjc3DiagnosticStageOwnerContract(Objc3FrontendDiagnosticStage stage) {
  for (const auto &contract : kObjc3DiagnosticStageOwnerContracts) {
    if (contract.stage == stage) {
      return &contract;
    }
  }
  return nullptr;
}

inline bool Objc3DiagnosticStageOwnerContractIsHardCutover(
    const Objc3DiagnosticStageOwnerContract &contract) {
  return Objc3DiagnosticOwnerIsExplicit(contract.catalog_owner) &&
         Objc3DiagnosticOwnerIsExplicit(contract.stage_owner) &&
         Objc3DiagnosticOwnerIsExplicit(contract.fixit_owner) &&
         Objc3DiagnosticOwnerIsExplicit(contract.recovery_owner) &&
         contract.owner_model == kObjc3DiagnosticNoRetiredRouteOwnerModel &&
         !contract.retired_route_allowed && !contract.compatibility_gate_allowed &&
         !contract.legacy_positive_allowed &&
         !contract.recovery_counts_as_success;
}

inline bool Objc3DiagnosticStageIsHardCutover(
    Objc3FrontendDiagnosticStage stage) {
  const Objc3DiagnosticStageOwnerContract *contract =
      FindObjc3DiagnosticStageOwnerContract(stage);
  return contract != nullptr &&
         Objc3DiagnosticStageOwnerContractIsHardCutover(*contract);
}

inline bool Objc3DiagnosticCodeFamilyMatchesStage(
    Objc3FrontendDiagnosticStage stage,
    char family) {
  switch (stage) {
  case Objc3FrontendDiagnosticStage::kLexer:
    return family == 'L' || family == 'C';
  case Objc3FrontendDiagnosticStage::kParser:
    return family == 'P' || family == 'C';
  case Objc3FrontendDiagnosticStage::kSemantic:
    return family == 'S';
  case Objc3FrontendDiagnosticStage::kPostPipeline:
    return family == 'A' || family == 'T' || family == 'L';
  }
  return false;
}

inline bool Objc3RenderedDiagnosticCodeMatchesStage(
    Objc3FrontendDiagnosticStage stage,
    std::string_view code) {
  return code.size() >= 4u && code[0] == 'O' && code[1] == '3' &&
         Objc3DiagnosticCodeFamilyMatchesStage(stage, code[2]);
}
