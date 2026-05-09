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

Objc3DiagnosticSubsystem DiagnosticSubsystemForPrefix(char prefix) {
  switch (prefix) {
    case 'C':
      return Objc3DiagnosticSubsystem::kCanonicalConfig;
    case 'L':
      return Objc3DiagnosticSubsystem::kLanguageSurface;
    case 'P':
      return Objc3DiagnosticSubsystem::kParser;
    case 'S':
      return Objc3DiagnosticSubsystem::kSemantic;
    case 'R':
      return Objc3DiagnosticSubsystem::kRuntime;
    case 'E':
      return Objc3DiagnosticSubsystem::kFrontendApi;
    case 'A':
      return Objc3DiagnosticSubsystem::kArtifact;
    case 'T':
      return Objc3DiagnosticSubsystem::kTooling;
    default:
      return Objc3DiagnosticSubsystem::kUnknown;
  }
}
