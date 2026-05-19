#include "diag/objc3_diag_parse.h"

bool TryParseDiagnosticCoordinateAndCode(std::string_view diag_text,
                                         unsigned &line,
                                         unsigned &column,
                                         std::string &code) {
  Objc3DiagnosticPayload payload;
  if (!TryParseRenderedDiagnostic(diag_text, payload)) {
    return false;
  }
  line = payload.coordinate.line;
  column = payload.coordinate.column;
  code = payload.code;
  return true;
}
