#include "diag/objc3_diag_record.h"

Objc3DiagnosticPayload MakeErrorDiagnosticPayload(
    unsigned line,
    unsigned column,
    const std::string &code,
    const std::string &message) {
  Objc3DiagnosticPayload payload;
  payload.severity = Objc3DiagnosticSeverity::kError;
  payload.coordinate = Objc3DiagnosticCoordinate{line, column};
  payload.code = code;
  payload.message = message;
  return payload;
}
