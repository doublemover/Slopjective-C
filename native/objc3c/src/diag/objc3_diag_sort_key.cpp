#include "diag/objc3_diag_sort.h"

#include "diag/objc3_diag_catalog.h"
#include "diag/objc3_diag_parse.h"

DiagSortKey ParseDiagSortKey(const std::string &diag) {
  DiagSortKey key;
  key.raw = diag;

  Objc3DiagnosticPayload payload;
  if (TryParseRenderedDiagnostic(diag, payload)) {
    key.severity = std::string(DiagnosticSeveritySpelling(payload.severity));
    key.severity_rank = DiagnosticSeverityRank(payload.severity);
    key.line = payload.coordinate.line;
    key.column = payload.coordinate.column;
    key.code = payload.code;
    key.message = payload.message;
    return key;
  }
  key.message = diag;
  return key;
}
