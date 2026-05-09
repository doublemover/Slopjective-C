#include "diag/objc3_diag_severity.h"

std::string_view DiagnosticSeveritySpelling(
    Objc3DiagnosticSeverity severity) {
  switch (severity) {
    case Objc3DiagnosticSeverity::kFatal:
      return "fatal";
    case Objc3DiagnosticSeverity::kError:
      return "error";
    case Objc3DiagnosticSeverity::kWarning:
      return "warning";
    case Objc3DiagnosticSeverity::kNote:
      return "note";
    case Objc3DiagnosticSeverity::kIgnored:
      return "ignored";
    case Objc3DiagnosticSeverity::kUnknown:
      break;
  }
  return "unknown";
}
