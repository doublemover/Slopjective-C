#include "parse/objc3_parser_diagnostics.h"

namespace objc3c::parse {

std::string BuildObjc3MissingSemicolonDiagnostic(
    const Objc3LexToken &token,
    const std::string &context) {
  return BuildObjc3ParserDiagnostic(
      token, "O3P104", "missing ';' after " + context);
}

std::string BuildObjc3InvalidDeclarationIdentifierDiagnostic(
    const Objc3LexToken &token) {
  return BuildObjc3ParserDiagnostic(
      token, "O3P101", "invalid declaration identifier");
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
