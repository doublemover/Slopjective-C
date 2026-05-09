#include "diag/objc3_diag_sort.h"

#include <algorithm>
#include <cstddef>
#include <limits>

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

void NormalizeDiagnostics(std::vector<std::string> &diagnostics) {
  std::vector<DiagSortKey> rows;
  rows.reserve(diagnostics.size());
  for (const auto &diag : diagnostics) {
    rows.push_back(ParseDiagSortKey(diag));
  }

  std::stable_sort(rows.begin(), rows.end(),
                   [](const DiagSortKey &a, const DiagSortKey &b) {
                     if (a.line != b.line) {
                       return a.line < b.line;
                     }
                     if (a.column != b.column) {
                       return a.column < b.column;
                     }
                     if (a.severity_rank != b.severity_rank) {
                       return a.severity_rank < b.severity_rank;
                     }
                     if (a.code != b.code) {
                       return a.code < b.code;
                     }
                     if (a.message != b.message) {
                       return a.message < b.message;
                     }
                     return a.raw < b.raw;
                   });

  diagnostics.clear();
  diagnostics.reserve(rows.size());
  for (const auto &row : rows) {
    if (diagnostics.empty() || diagnostics.back() != row.raw) {
      diagnostics.push_back(row.raw);
    }
  }
}
