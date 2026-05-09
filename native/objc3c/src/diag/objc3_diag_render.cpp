#include "diag/objc3_diag_render.h"

#include "diag/objc3_diag_render_parts.h"

std::string RenderDiagnosticPayload(const Objc3DiagnosticPayload &payload) {
  std::string invalid_reason;
  if (!IsValidDiagnosticPayload(payload, &invalid_reason)) {
    return RenderDiagnosticPayloadUnchecked(
        MakeInvalidDiagnosticPayloadDiagnostic(invalid_reason));
  }
  return RenderDiagnosticPayloadUnchecked(payload);
}
