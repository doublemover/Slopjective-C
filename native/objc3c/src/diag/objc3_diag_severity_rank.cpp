#include "diag/objc3_diag_severity.h"

unsigned DiagnosticSeverityRank(Objc3DiagnosticSeverity severity) {
  switch (severity) {
    case Objc3DiagnosticSeverity::kFatal:
      return 0;
    case Objc3DiagnosticSeverity::kError:
      return 1;
    case Objc3DiagnosticSeverity::kWarning:
      return 2;
    case Objc3DiagnosticSeverity::kNote:
      return 3;
    case Objc3DiagnosticSeverity::kIgnored:
      return 4;
    case Objc3DiagnosticSeverity::kUnknown:
      break;
  }
  return 5;
}

unsigned DiagSeverityRank(std::string_view severity) {
  return DiagnosticSeverityRank(ParseDiagnosticSeverity(severity));
}
