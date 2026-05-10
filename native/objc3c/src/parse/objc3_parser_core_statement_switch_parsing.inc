    if (Match(TokenKind::KwMatch)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Switch;
      stmt->switch_stmt = std::make_unique<SwitchStmt>();
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      stmt->switch_stmt->line = Previous().line;
      stmt->switch_stmt->column = Previous().column;
      stmt->switch_stmt->match_surface_enabled = true;

#include "parse/objc3_parser_core_statement_switch_parsing_match_header.inc"
#include "parse/objc3_parser_core_statement_switch_parsing_match_cases.inc"
#include "parse/objc3_parser_core_statement_switch_parsing_match_closing.inc"
      return stmt;
    }

    if (Match(TokenKind::KwSwitch)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Switch;
      stmt->switch_stmt = std::make_unique<SwitchStmt>();
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      stmt->switch_stmt->line = Previous().line;
      stmt->switch_stmt->column = Previous().column;

#include "parse/objc3_parser_core_statement_switch_parsing_switch_header.inc"
#include "parse/objc3_parser_core_statement_switch_parsing_switch_cases.inc"
#include "parse/objc3_parser_core_statement_switch_parsing_switch_closing.inc"
      return stmt;
    }
