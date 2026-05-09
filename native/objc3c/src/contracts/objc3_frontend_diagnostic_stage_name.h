#pragma once

#include <string_view>

#include "contracts/objc3_frontend_diagnostic_stage_kind.h"

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
