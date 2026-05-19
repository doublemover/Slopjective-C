#include "diag/objc3_diag_severity.h"

#include <string>

#include "diag/objc3_diag_text.h"

Objc3DiagnosticSeverity ParseDiagnosticSeverity(std::string_view severity) {
  const std::string normalized = ToLower(std::string(severity));
  if (normalized == "fatal") {
    return Objc3DiagnosticSeverity::kFatal;
  }
  if (normalized == "error") {
    return Objc3DiagnosticSeverity::kError;
  }
  if (normalized == "warning") {
    return Objc3DiagnosticSeverity::kWarning;
  }
  if (normalized == "note") {
    return Objc3DiagnosticSeverity::kNote;
  }
  if (normalized == "ignored") {
    return Objc3DiagnosticSeverity::kIgnored;
  }
  return Objc3DiagnosticSeverity::kUnknown;
}
