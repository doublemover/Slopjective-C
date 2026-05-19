#include "libobjc3c_frontend/objc3c_frontend_diagnostics.h"

#include <cctype>
#include <exception>
#include <limits>
#include <sstream>

#include "io/json/json_writer.h"

namespace objc3c::frontend {

namespace {

using objc3::io::json::JsonObjectWriter;

std::string ToLowerCopy(std::string value) {
  for (char &ch : value) {
    ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
  }
  return value;
}

struct ParsedFrontendDiagnostic {
  std::string severity = "unknown";
  unsigned line = std::numeric_limits<unsigned>::max();
  unsigned column = std::numeric_limits<unsigned>::max();
  std::string code;
  std::string message;
  std::string raw;
};

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

ParsedFrontendDiagnostic ParseFrontendDiagnostic(const std::string &diag) {
  ParsedFrontendDiagnostic parsed;
  parsed.raw = diag;
  parsed.message = diag;

  const std::size_t severity_end = diag.find(':');
  if (severity_end == std::string::npos) {
    return parsed;
  }
  parsed.severity = ToLowerCopy(diag.substr(0, severity_end));

  const std::size_t line_end = diag.find(':', severity_end + 1);
  const std::size_t column_end =
      line_end == std::string::npos ? std::string::npos
                                    : diag.find(':', line_end + 1);
  if (line_end == std::string::npos || column_end == std::string::npos) {
    return parsed;
  }

  try {
    parsed.line = static_cast<unsigned>(
        std::stoul(diag.substr(severity_end + 1,
                               line_end - (severity_end + 1))));
    parsed.column = static_cast<unsigned>(
        std::stoul(diag.substr(line_end + 1, column_end - (line_end + 1))));
  } catch (const std::exception &) {
    parsed.line = std::numeric_limits<unsigned>::max();
    parsed.column = std::numeric_limits<unsigned>::max();
  }

  std::size_t message_begin = column_end + 1;
  while (message_begin < diag.size() &&
         std::isspace(static_cast<unsigned char>(diag[message_begin])) != 0) {
    ++message_begin;
  }

  const std::size_t code_begin = diag.rfind(" [");
  if (code_begin != std::string::npos && code_begin > message_begin &&
      !diag.empty() && diag.back() == ']') {
    const std::string candidate_code =
        diag.substr(code_begin + 2, diag.size() - (code_begin + 3));
    if (IsNativeDiagCode(candidate_code)) {
      parsed.message = diag.substr(message_begin, code_begin - message_begin);
      parsed.code = candidate_code;
      return parsed;
    }
  }

  parsed.message = diag.substr(message_begin);
  return parsed;
}

}  // namespace

std::string BuildFrontendDiagnosticsJson(
    const std::vector<std::string> &diagnostics) {
  std::ostringstream out;
  out << "{\n";
  out << "  \"schema_version\": \"1.0.0\",\n";
  out << "  \"diagnostics\": [\n";
  for (std::size_t i = 0; i < diagnostics.size(); ++i) {
    const ParsedFrontendDiagnostic parsed =
        ParseFrontendDiagnostic(diagnostics[i]);
    const unsigned line =
        parsed.line == std::numeric_limits<unsigned>::max() ? 0U : parsed.line;
    const unsigned column =
        parsed.column == std::numeric_limits<unsigned>::max() ? 0U
                                                              : parsed.column;
    out << "    ";
    JsonObjectWriter diagnostic(out);
    diagnostic.StringField("severity", parsed.severity);
    diagnostic.UnsignedField("line", line);
    diagnostic.UnsignedField("column", column);
    diagnostic.StringField("code", parsed.code);
    diagnostic.StringField("message", parsed.message);
    diagnostic.StringField("raw", parsed.raw);
    diagnostic.End();
    if (i + 1 != diagnostics.size()) {
      out << ",";
    }
    out << "\n";
  }
  out << "  ]\n";
  out << "}\n";
  return out.str();
}

}  // namespace objc3c::frontend
