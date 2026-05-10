#include "parse/objc3_parser_profile_symbol_walk.h"

namespace objc3c::parse {

void Objc3ProfileSymbolWalker::Visit(const std::string &symbol) const {
  if (visit != nullptr) {
    visit(symbol, context);
  }
}

#include "parse/objc3_parser_profile_symbol_walk_expression_traversal.inc"

namespace {

#include "parse/objc3_parser_profile_symbol_walk_for_clause_traversal.inc"

}  // namespace

#include "parse/objc3_parser_profile_symbol_walk_statement_traversal.inc"

#include "parse/objc3_parser_profile_symbol_walk_body_traversal.inc"

#include "parse/objc3_parser_profile_symbol_walk_opaque_method_handling.inc"

}  // namespace objc3c::parse
