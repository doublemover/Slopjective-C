#include "diag/objc3_diag_utils.h"

#include <algorithm>
#include <cctype>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

std::string MakeDiag(unsigned line, unsigned column, const std::string &code, const std::string &message) {
  std::ostringstream out;
  out << "error:" << line << ":" << column << ": " << message << " [" << code << "]";
  return out.str();
}

bool StartsWith(std::string_view value, std::string_view prefix) {
  return value.size() >= prefix.size() && value.compare(0u, prefix.size(), prefix) == 0;
}

bool TryParseUnsignedSegment(std::string_view text,
                             std::size_t begin,
                             std::size_t delimiter_offset,
                             unsigned &value) {
  if (delimiter_offset <= begin || delimiter_offset > text.size()) {
    return false;
  }
  unsigned parsed = 0;
  for (std::size_t i = begin; i < delimiter_offset; ++i) {
    const char c = text[i];
    if (c < '0' || c > '9') {
      return false;
    }
    const unsigned digit = static_cast<unsigned>(c - '0');
    if (parsed > (std::numeric_limits<unsigned>::max() - digit) / 10u) {
      return false;
    }
    parsed = parsed * 10u + digit;
  }
  value = parsed;
  return true;
}

bool TryParseDiagnosticCoordinateAndCode(std::string_view diag_text,
                                         unsigned &line,
                                         unsigned &column,
                                         std::string &code) {
  constexpr std::string_view kPrefix = "error:";
  if (!StartsWith(diag_text, kPrefix)) {
    return false;
  }

  const std::size_t line_begin = kPrefix.size();
  const std::size_t first_colon = diag_text.find(':', line_begin);
  if (first_colon == std::string_view::npos ||
      !TryParseUnsignedSegment(diag_text, line_begin, first_colon, line)) {
    return false;
  }

  const std::size_t column_begin = first_colon + 1u;
  const std::size_t second_colon = diag_text.find(':', column_begin);
  if (second_colon == std::string_view::npos ||
      !TryParseUnsignedSegment(diag_text, column_begin, second_colon, column)) {
    return false;
  }
  if (second_colon + 1u >= diag_text.size() || diag_text[second_colon + 1u] != ' ') {
    return false;
  }

  const std::size_t code_begin_marker = diag_text.rfind(" [");
  if (code_begin_marker == std::string_view::npos ||
      code_begin_marker + 3u >= diag_text.size() ||
      diag_text.back() != ']') {
    return false;
  }

  code.assign(diag_text.substr(code_begin_marker + 2u, diag_text.size() - code_begin_marker - 3u));
  return !code.empty();
}

std::string ToLower(std::string value) {
  std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
    return static_cast<char>(std::tolower(c));
  });
  return value;
}

namespace {

unsigned DiagSeverityRank(const std::string &severity) {
  const std::string normalized = ToLower(severity);
  if (normalized == "fatal") {
    return 0;
  }
  if (normalized == "error") {
    return 1;
  }
  if (normalized == "warning") {
    return 2;
  }
  if (normalized == "note") {
    return 3;
  }
  if (normalized == "ignored") {
    return 4;
  }
  return 5;
}

bool IsNativeDiagCode(const std::string &candidate) {
  if (candidate.size() != 6) {
    return false;
  }
  if (candidate[0] != 'O' || candidate[1] != '3') {
    return false;
  }
  if (std::isupper(static_cast<unsigned char>(candidate[2])) == 0) {
    return false;
  }
  return std::isdigit(static_cast<unsigned char>(candidate[3])) != 0 &&
         std::isdigit(static_cast<unsigned char>(candidate[4])) != 0 &&
         std::isdigit(static_cast<unsigned char>(candidate[5])) != 0;
}

}  // namespace

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
      line_end == std::string::npos ? std::string::npos : diag.find(':', line_end + 1);
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
  while (message_begin < diag.size() && std::isspace(static_cast<unsigned char>(diag[message_begin])) != 0) {
    ++message_begin;
  }
  const std::size_t code_begin = diag.rfind(" [");
  if (code_begin != std::string::npos && code_begin > message_begin && diag.back() == ']') {
    const std::string candidate_code = diag.substr(code_begin + 2, diag.size() - (code_begin + 3));
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

  std::stable_sort(rows.begin(), rows.end(), [](const DiagSortKey &a, const DiagSortKey &b) {
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
