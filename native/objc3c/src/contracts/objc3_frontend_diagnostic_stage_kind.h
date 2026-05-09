#pragma once

#include <string_view>

enum class Objc3FrontendDiagnosticStage {
  kLexer,
  kParser,
  kSemantic,
  kPostPipeline,
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
