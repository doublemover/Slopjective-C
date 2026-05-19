      if (!Match(TokenKind::LParen)) {
        const Token &token = Peek();
        emit_do_for_diagnostic(token, "O3P106", "missing '(' after for");
        return nullptr;
      }

#include "parse/objc3_parser_core_statement_do_for_parsing_for_header_init.inc"

#include "parse/objc3_parser_core_statement_do_for_parsing_for_header_condition.inc"

#include "parse/objc3_parser_core_statement_do_for_parsing_for_header_step.inc"
