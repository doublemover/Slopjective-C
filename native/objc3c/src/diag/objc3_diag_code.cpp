#include "diag/objc3_diag_code.h"

#include <cctype>
#include <cstddef>

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

bool TryParseNativeDiagCode(std::string_view candidate,
                            Objc3DiagnosticCode &code) {
  code = Objc3DiagnosticCode{};
  if (candidate.size() != 6) {
    return false;
  }
  if (candidate[0] != 'O' || candidate[1] != '3') {
    return false;
  }
  if (std::isupper(static_cast<unsigned char>(candidate[2])) == 0) {
    return false;
  }

  unsigned ordinal = 0;
  for (std::size_t i = 3; i < candidate.size(); ++i) {
    if (std::isdigit(static_cast<unsigned char>(candidate[i])) == 0) {
      return false;
    }
    ordinal = ordinal * 10u + static_cast<unsigned>(candidate[i] - '0');
  }

  const Objc3DiagnosticSubsystem subsystem =
      DiagnosticSubsystemForPrefix(candidate[2]);
  if (subsystem == Objc3DiagnosticSubsystem::kUnknown) {
    return false;
  }

  code.text = std::string(candidate);
  code.subsystem = subsystem;
  code.ordinal = ordinal;
  code.valid = true;
  return true;
}

bool IsNativeDiagCode(std::string_view candidate) {
  Objc3DiagnosticCode code;
  return TryParseNativeDiagCode(candidate, code);
}
