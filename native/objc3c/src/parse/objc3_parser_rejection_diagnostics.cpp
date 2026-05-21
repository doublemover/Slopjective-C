#include "parse/objc3_parser_diagnostics.h"

namespace objc3c::parse {

std::string BuildObjc3MissingSemicolonDiagnostic(
    const Objc3LexToken &token,
    const std::string &context) {
  return BuildObjc3MissingSemicolonDiagnostic(token, token, context);
}

std::string BuildObjc3MissingSemicolonDiagnostic(
    const Objc3LexToken &token,
    const Objc3LexToken &insertion_anchor,
    const std::string &context) {
  const unsigned insertion_column =
      insertion_anchor.column +
      static_cast<unsigned>(insertion_anchor.text.size());
  return BuildObjc3ParserDiagnosticWithFixItAndRecovery(
      token,
      "O3P104",
      "missing ';' after " + context,
      Objc3ParserDiagnosticFixIt{
          insertion_anchor.line,
          insertion_column,
          insertion_anchor.line,
          insertion_column,
          "insert-missing-semicolon-after-" + context,
          ";",
          true},
      "parser-statement-boundary-synchronization",
      "next statement token");
}

std::string BuildObjc3InvalidDeclarationIdentifierDiagnostic(
    const Objc3LexToken &token) {
  return BuildObjc3ParserDiagnostic(
      token, "O3P101", "invalid declaration identifier");
}

std::string BuildObjc3RemovedOptionalTemplateAliasDiagnostic(
    const Objc3LexToken &token) {
  const unsigned end_column =
      token.column + static_cast<unsigned>(token.text.empty() ? 8u : token.text.size());
  return BuildObjc3ParserDiagnosticWithFixItAndRecovery(
      token,
      "O3C004",
      "optional<T> aliases are rejected; use canonical Optional<T> spelling",
      Objc3ParserDiagnosticFixIt{
          token.line,
          token.column,
          token.line,
          end_column,
          "replace-optional-alias-with-Optional",
          "Optional",
          true},
      "parser-canonical-spelling-rejection",
      "canonical type spelling");
}

std::string BuildObjc3UnsupportedTopLevelDiagnostic(
    const Objc3LexToken &token) {
  return BuildObjc3ParserDiagnosticWithRecovery(
      token,
      "O3P100",
      "unsupported Objective-C 3 statement",
      "skip-unsupported-top-level-fragment",
      "next-top-level-declaration-or-semicolon");
}

}  // namespace objc3c::parse
