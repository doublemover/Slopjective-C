#include "diag/objc3_diag_render.h"

#include <sstream>

namespace {

std::string RenderValidDiagnosticPayload(
    const Objc3DiagnosticPayload &payload) {
  std::ostringstream out;
  out << DiagnosticSeveritySpelling(payload.severity) << ":"
      << payload.coordinate.line << ":" << payload.coordinate.column << ": "
      << payload.message << " [" << payload.code << "]";
  return out.str();
}

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

}  // namespace

std::string RenderDiagnosticPayload(const Objc3DiagnosticPayload &payload) {
  std::string invalid_reason;
  if (!IsValidDiagnosticPayload(payload, &invalid_reason)) {
    return RenderValidDiagnosticPayload(
        MakeInvalidDiagnosticPayloadDiagnostic(invalid_reason));
  }
  return RenderValidDiagnosticPayload(payload);
}
