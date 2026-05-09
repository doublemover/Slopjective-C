#include "diag/objc3_diag_category.h"

const char *DiagnosticCategoryName(Objc3DiagnosticCategory category) {
  switch (category) {
    case Objc3DiagnosticCategory::kConfiguration:
      return "configuration";
    case Objc3DiagnosticCategory::kLexical:
      return "lexical";
    case Objc3DiagnosticCategory::kParsing:
      return "parsing";
    case Objc3DiagnosticCategory::kSemanticAnalysis:
      return "semantic-analysis";
    case Objc3DiagnosticCategory::kRuntime:
      return "runtime";
    case Objc3DiagnosticCategory::kFrontendApi:
      return "frontend-api";
    case Objc3DiagnosticCategory::kArtifact:
      return "artifact";
    case Objc3DiagnosticCategory::kTooling:
      return "tooling";
    case Objc3DiagnosticCategory::kUnknown:
      break;
  }
  return "unknown";
}
