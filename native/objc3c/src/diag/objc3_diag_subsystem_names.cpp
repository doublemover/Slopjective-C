#include "diag/objc3_diag_code.h"

const char *DiagnosticSubsystemName(Objc3DiagnosticSubsystem subsystem) {
  switch (subsystem) {
    case Objc3DiagnosticSubsystem::kCanonicalConfig:
      return "canonical-config";
    case Objc3DiagnosticSubsystem::kLanguageSurface:
      return "language-surface";
    case Objc3DiagnosticSubsystem::kParser:
      return "parser";
    case Objc3DiagnosticSubsystem::kSemantic:
      return "semantic";
    case Objc3DiagnosticSubsystem::kRuntime:
      return "runtime";
    case Objc3DiagnosticSubsystem::kFrontendApi:
      return "frontend-api";
    case Objc3DiagnosticSubsystem::kArtifact:
      return "artifact";
    case Objc3DiagnosticSubsystem::kTooling:
      return "tooling";
    case Objc3DiagnosticSubsystem::kUnknown:
      break;
  }
  return "unknown";
}
