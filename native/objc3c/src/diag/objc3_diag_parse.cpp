#include "diag/objc3_diag_parse.h"

#include <limits>

#include "diag/objc3_diag_text.h"

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
  if (second_colon + 1u >= diag_text.size() ||
      diag_text[second_colon + 1u] != ' ') {
    return false;
  }

  const std::size_t code_begin_marker = diag_text.rfind(" [");
  if (code_begin_marker == std::string_view::npos ||
      code_begin_marker + 3u >= diag_text.size() ||
      diag_text.back() != ']') {
    return false;
  }

  code.assign(
      diag_text.substr(code_begin_marker + 2u,
                       diag_text.size() - code_begin_marker - 3u));
  return !code.empty();
}
