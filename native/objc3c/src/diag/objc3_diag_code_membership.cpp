#include "diag/objc3_diag_code.h"

bool IsNativeDiagCode(std::string_view candidate) {
  Objc3DiagnosticCode code;
  return TryParseNativeDiagCode(candidate, code);
}
