#include "diag/objc3_diag_category.h"

Objc3DiagnosticCategory DiagnosticCategoryForCode(std::string_view code) {
  Objc3DiagnosticCode parsed;
  if (!TryParseNativeDiagCode(code, parsed)) {
    return Objc3DiagnosticCategory::kUnknown;
  }
  return DiagnosticCategoryForSubsystem(parsed.subsystem);
}
