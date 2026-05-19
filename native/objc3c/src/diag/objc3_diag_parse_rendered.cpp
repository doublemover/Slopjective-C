#include "diag/objc3_diag_parse.h"

#include "diag/objc3_diag_catalog.h"
#include "diag/objc3_diag_text.h"

bool TryParseRenderedDiagnostic(std::string_view diag_text,
                                Objc3DiagnosticPayload &payload) {
  payload = Objc3DiagnosticPayload{};
  const std::size_t severity_end = diag_text.find(':');
  if (severity_end == std::string_view::npos) {
    return false;
  }
  const Objc3DiagnosticSeverity severity =
      ParseDiagnosticSeverity(diag_text.substr(0, severity_end));
  if (severity == Objc3DiagnosticSeverity::kUnknown) {
    return false;
  }

  unsigned line = 0;
  unsigned column = 0;
  std::string code;
  const std::size_t line_begin = severity_end + 1u;
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
  if (!NativeDiagCodeIsWithinCatalog(code)) {
    return false;
  }

  std::size_t message_begin = second_colon + 1u;
  while (message_begin < diag_text.size() && diag_text[message_begin] == ' ') {
    ++message_begin;
  }
  if (code_begin_marker <= message_begin) {
    return false;
  }

  payload.severity = severity;
  payload.coordinate = Objc3DiagnosticCoordinate{line, column};
  payload.code = code;
  payload.message = std::string(
      diag_text.substr(message_begin, code_begin_marker - message_begin));
  return !payload.message.empty();
}
