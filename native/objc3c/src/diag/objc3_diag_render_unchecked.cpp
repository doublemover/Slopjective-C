#include "diag/objc3_diag_render_parts.h"

#include <sstream>

std::string RenderDiagnosticPayloadUnchecked(
    const Objc3DiagnosticPayload &payload) {
  std::ostringstream out;
  out << DiagnosticSeveritySpelling(payload.severity) << ":"
      << payload.coordinate.line << ":" << payload.coordinate.column << ": "
      << payload.message << " [" << payload.code << "]";
  return out.str();
}
