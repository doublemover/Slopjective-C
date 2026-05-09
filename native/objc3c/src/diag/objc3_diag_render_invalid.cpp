#include "diag/objc3_diag_render_parts.h"

Objc3DiagnosticPayload MakeInvalidDiagnosticPayloadDiagnostic(
    const std::string &reason) {
  Objc3DiagnosticPayload payload;
  payload.severity = Objc3DiagnosticSeverity::kFatal;
  payload.coordinate = Objc3DiagnosticCoordinate{0u, 0u};
  payload.code = "O3E001";
  payload.message = "invalid diagnostic payload";
  if (!reason.empty()) {
    payload.message += ": ";
    payload.message += reason;
  }
  return payload;
}
