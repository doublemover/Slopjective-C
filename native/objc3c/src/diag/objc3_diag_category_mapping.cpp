#include "diag/objc3_diag_category.h"

Objc3DiagnosticCategory DiagnosticCategoryForSubsystem(
    Objc3DiagnosticSubsystem subsystem) {
  switch (subsystem) {
    case Objc3DiagnosticSubsystem::kCanonicalConfig:
      return Objc3DiagnosticCategory::kConfiguration;
    case Objc3DiagnosticSubsystem::kLanguageSurface:
      return Objc3DiagnosticCategory::kLexical;
    case Objc3DiagnosticSubsystem::kParser:
      return Objc3DiagnosticCategory::kParsing;
    case Objc3DiagnosticSubsystem::kSemantic:
      return Objc3DiagnosticCategory::kSemanticAnalysis;
    case Objc3DiagnosticSubsystem::kRuntime:
      return Objc3DiagnosticCategory::kRuntime;
    case Objc3DiagnosticSubsystem::kFrontendApi:
      return Objc3DiagnosticCategory::kFrontendApi;
    case Objc3DiagnosticSubsystem::kArtifact:
      return Objc3DiagnosticCategory::kArtifact;
    case Objc3DiagnosticSubsystem::kTooling:
      return Objc3DiagnosticCategory::kTooling;
    case Objc3DiagnosticSubsystem::kUnknown:
      break;
  }
  return Objc3DiagnosticCategory::kUnknown;
}

Objc3DiagnosticCategory DiagnosticCategoryForCode(std::string_view code) {
  Objc3DiagnosticCode parsed;
  if (!TryParseNativeDiagCode(code, parsed)) {
    return Objc3DiagnosticCategory::kUnknown;
  }
  return DiagnosticCategoryForSubsystem(parsed.subsystem);
}

bool DiagnosticCategoryIsFrontendCompiler(Objc3DiagnosticCategory category) {
  return category == Objc3DiagnosticCategory::kConfiguration ||
         category == Objc3DiagnosticCategory::kLexical ||
         category == Objc3DiagnosticCategory::kParsing ||
         category == Objc3DiagnosticCategory::kSemanticAnalysis;
}
