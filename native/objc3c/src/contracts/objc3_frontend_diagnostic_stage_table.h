#pragma once

#include <array>
#include <string_view>

#include "contracts/objc3_frontend_diagnostic_stage_kind.h"

struct Objc3FrontendDiagnosticStageDescriptor {
  Objc3FrontendDiagnosticStage stage;
  std::string_view name;
};

inline constexpr std::array<Objc3FrontendDiagnosticStageDescriptor, 4>
    kObjc3FrontendDiagnosticStageTable = {{
        {Objc3FrontendDiagnosticStage::kLexer, "lexer"},
        {Objc3FrontendDiagnosticStage::kParser, "parser"},
        {Objc3FrontendDiagnosticStage::kSemantic, "semantic"},
        {Objc3FrontendDiagnosticStage::kPostPipeline, "post-pipeline"},
    }};
