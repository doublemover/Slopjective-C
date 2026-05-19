    if (Match(TokenKind::KwFor)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::For;
      stmt->for_stmt = std::make_unique<ForStmt>();
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      stmt->for_stmt->line = Previous().line;
      stmt->for_stmt->column = Previous().column;

#include "parse/objc3_parser_core_statement_do_for_parsing_for_header.inc"

#include "parse/objc3_parser_core_statement_do_for_parsing_for_body.inc"
      return stmt;
    }
