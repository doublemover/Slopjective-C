#include "parse/objc3_parser_diagnostics.h"

#include "diag/objc3_diag_utils.h"

namespace objc3c::parse {

std::string BuildObjc3ParserDiagnostic(
    const Objc3LexToken &token,
    const char *code,
    const std::string &message) {
  return MakeDiag(token.line, token.column, code, message);
}

std::string BuildObjc3RemovedOptionalTemplateAliasDiagnostic(
    const Objc3LexToken &token) {
  return BuildObjc3ParserDiagnostic(
      token,
      "O3C004",
      "optional<T> aliases are rejected; use canonical Optional<T> spelling");
}

std::string BuildObjc3UnsupportedTopLevelDiagnostic(
    const Objc3LexToken &token) {
  return BuildObjc3ParserDiagnostic(
      token, "O3P100", "unsupported Objective-C 3 statement");
}

}  // namespace objc3c::parse
