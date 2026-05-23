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
      kObjc3ParserDiagnosticRemovedOptionalAliasCode,
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

std::string BuildObjc3ReservedValueOptionalTypeDiagnostic(
    const Objc3LexToken &token) {
  return BuildObjc3ParserDiagnosticWithRecovery(
      token,
      kObjc3ParserDiagnosticReservedValueOptionalCode,
      "Optional<T> value optionals are reserved until value-optional ABI, lowering, and interface roundtrip are implemented; Optional<T> remains distinct from nullable object pointers and nil-to-scalar conversions",
      "parser-reserved-value-optional-type-rejection",
      "type spelling");
}

std::string BuildObjc3ReservedTypedThrowsDiagnostic(
    const Objc3LexToken &token,
    std::string_view payload_shape) {
  return BuildObjc3ParserDiagnosticWithRecovery(
      token,
      kObjc3ParserDiagnosticReservedTypedThrowsCode,
      "typed throws payloads are reserved; use bare throws or remove the parenthesized error type; typed-payload-shape=" +
          std::string(payload_shape) +
          "; parenthesized payloads are not silently erased into untyped throws",
      "parser-reserved-typed-throws-rejection",
      "throws clause");
}

std::string BuildObjc3ReservedMatchExpressionDiagnostic(
    const Objc3LexToken &token) {
  return BuildObjc3ParserDiagnosticWithRecovery(
      token,
      kObjc3ParserDiagnosticReservedMatchExpressionCode,
      "statement-form match uses ':' braced arms; use expression-form match only in expression position with '=>' value arms",
      "parser-match-statement-fat-arrow-rejection",
      "match statement arm");
}

std::string BuildObjc3ReservedMatchTypeTestPatternDiagnostic(
    const Objc3LexToken &token) {
  return BuildObjc3ParserDiagnosticWithRecovery(
      token,
      kObjc3ParserDiagnosticReservedMatchTypeTestPatternCode,
      "match type-test patterns are reserved; statement-form match currently admits wildcard, literal, binding, and Result case patterns only",
      "parser-reserved-match-type-test-pattern-rejection",
      "match case pattern");
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
