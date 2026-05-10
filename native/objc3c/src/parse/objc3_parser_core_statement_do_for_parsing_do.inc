    if (Match(TokenKind::KwDo)) {
      const Token do_token = Previous();
      auto body = ParseControlBody();
      if (block_failed_) {
        block_failed_ = false;
        return nullptr;
      }

      if (Match(TokenKind::KwCatch)) {
#include "parse/objc3_parser_core_statement_do_for_parsing_do_catch.inc"
      }

#include "parse/objc3_parser_core_statement_do_for_parsing_do_while.inc"
    }
