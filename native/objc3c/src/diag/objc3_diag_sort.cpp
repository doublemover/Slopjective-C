#include "diag/objc3_diag_sort.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <limits>

#include "diag/objc3_diag_catalog.h"
#include "diag/objc3_diag_parse.h"

DiagSortKey ParseDiagSortKey(const std::string &diag) {
  DiagSortKey key;
  key.raw = diag;

  const std::size_t severity_end = diag.find(':');
  if (severity_end == std::string::npos) {
    key.message = diag;
    return key;
  }
  key.severity = diag.substr(0, severity_end);
  key.severity_rank = DiagSeverityRank(key.severity);

  const std::size_t line_end = diag.find(':', severity_end + 1);
  const std::size_t column_end =
      line_end == std::string::npos ? std::string::npos
                                    : diag.find(':', line_end + 1);
  if (line_end == std::string::npos || column_end == std::string::npos) {
    key.message = diag;
    return key;
  }

  if (!TryParseUnsignedSegment(diag, severity_end + 1u, line_end, key.line) ||
      !TryParseUnsignedSegment(diag, line_end + 1u, column_end, key.column)) {
    key.line = std::numeric_limits<unsigned>::max();
    key.column = std::numeric_limits<unsigned>::max();
  }

  std::size_t message_begin = column_end + 1;
  while (message_begin < diag.size() &&
         std::isspace(static_cast<unsigned char>(diag[message_begin])) != 0) {
    ++message_begin;
  }
  const std::size_t code_begin = diag.rfind(" [");
  if (code_begin != std::string::npos && code_begin > message_begin &&
      diag.back() == ']') {
    const std::string candidate_code =
        diag.substr(code_begin + 2, diag.size() - (code_begin + 3));
    if (IsNativeDiagCode(candidate_code)) {
      key.message = diag.substr(message_begin, code_begin - message_begin);
      key.code = candidate_code;
      return key;
    }
  }
  key.message = diag.substr(message_begin);
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
