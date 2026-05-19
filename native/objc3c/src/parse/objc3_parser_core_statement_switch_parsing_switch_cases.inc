      while (!At(TokenKind::RBrace) && !At(TokenKind::Eof)) {
        if (Match(TokenKind::KwCase)) {
          SwitchCase case_stmt;
          case_stmt.line = Previous().line;
          case_stmt.column = Previous().column;
          case_stmt.is_default = false;

#include "parse/objc3_parser_core_statement_switch_parsing_switch_case_label.inc"
#include "parse/objc3_parser_core_statement_switch_parsing_switch_case_body.inc"
          stmt->switch_stmt->cases.push_back(std::move(case_stmt));
          continue;
        }

        if (Match(TokenKind::KwDefault)) {
          SwitchCase default_stmt;
          default_stmt.line = Previous().line;
          default_stmt.column = Previous().column;
          default_stmt.is_default = true;
          default_stmt.value = 0;
          default_stmt.value_line = Previous().line;
          default_stmt.value_column = Previous().column;

#include "parse/objc3_parser_core_statement_switch_parsing_switch_default_body.inc"
          stmt->switch_stmt->cases.push_back(std::move(default_stmt));
          continue;
        }

#include "parse/objc3_parser_core_statement_switch_parsing_switch_body_diagnostics.inc"
      }
