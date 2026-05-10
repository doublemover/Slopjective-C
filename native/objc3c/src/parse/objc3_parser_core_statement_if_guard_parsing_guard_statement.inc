    if (Match(TokenKind::KwGuard)) {
#include "parse/objc3_parser_core_statement_if_guard_parsing_guard_statement_setup.inc"
      auto clauses = ParseGuardConditionClauses();
      if (clauses.empty()) {
        return nullptr;
      }
      stmt->if_stmt->guard_condition_list_surface_enabled = true;
      stmt->if_stmt->condition =
          BuildObjc3BoolLiteralExprAt(false, stmt->line, stmt->column);
#include "parse/objc3_parser_core_statement_if_guard_parsing_guard_clause_application.inc"
#include "parse/objc3_parser_core_statement_if_guard_parsing_guard_else_body.inc"
      return stmt;
    }
