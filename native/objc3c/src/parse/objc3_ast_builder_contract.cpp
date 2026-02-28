#include "parse/objc3_ast_builder_contract.h"

#include <utility>

#include "parse/objc3_parser.h"

Objc3AstBuilderResult BuildObjc3AstFromTokens(const Objc3LexTokenStream &tokens) {
  Objc3ParseResult parse_result = ParseObjc3Program(tokens);
  Objc3AstBuilderResult builder_result;
  builder_result.program = std::move(parse_result.program);
  builder_result.diagnostics = std::move(parse_result.diagnostics);
  return builder_result;
}
