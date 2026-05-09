#pragma once

#include <array>
#include <string_view>

#include "contracts/objc3_diagnostic_owner_contract.h"
#include "contracts/objc3_frontend_diagnostic_stage_kind.h"

struct Objc3FrontendDiagnosticStageDescriptor {
  Objc3FrontendDiagnosticStage stage;
  std::string_view name;
  std::string_view owner_contract_id = kObjc3DiagnosticOwnerContractId;
  std::string_view stage_owner;
  bool hard_cutover_required = true;
  bool recovery_counts_as_success = false;
};

inline constexpr std::array<Objc3FrontendDiagnosticStageDescriptor, 4>
    kObjc3FrontendDiagnosticStageTable = {{
        {Objc3FrontendDiagnosticStage::kLexer,
         "lexer",
         kObjc3DiagnosticOwnerContractId,
         kObjc3LexerDiagnosticStageOwner},
        {Objc3FrontendDiagnosticStage::kParser,
         "parser",
         kObjc3DiagnosticOwnerContractId,
         kObjc3ParserDiagnosticStageOwner},
        {Objc3FrontendDiagnosticStage::kSemantic,
         "semantic",
         kObjc3DiagnosticOwnerContractId,
         kObjc3SemaDiagnosticStageOwner},
        {Objc3FrontendDiagnosticStage::kPostPipeline,
         "post-pipeline",
         kObjc3DiagnosticOwnerContractId,
         kObjc3PostPipelineDiagnosticStageOwner},
    }};
