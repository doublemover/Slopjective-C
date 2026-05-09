#include "diag/objc3_diag_code.h"

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
