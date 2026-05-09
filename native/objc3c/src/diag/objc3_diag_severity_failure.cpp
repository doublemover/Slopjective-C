#include "diag/objc3_diag_severity.h"

bool IsFailureDiagnosticSeverity(Objc3DiagnosticSeverity severity) {
  return severity == Objc3DiagnosticSeverity::kFatal ||
         severity == Objc3DiagnosticSeverity::kError;
}
