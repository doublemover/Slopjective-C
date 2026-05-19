      if (Match(TokenKind::Semicolon)) {
        stmt->for_stmt->init.kind = ForClause::Kind::None;
      } else {
#include "parse/objc3_parser_core_statement_do_for_parsing_for_header_init_kind_dispatch.inc"

#include "parse/objc3_parser_core_statement_do_for_parsing_for_header_init_final_clause_publication.inc"
      }
