      while (!At(TokenKind::RBrace) && !At(TokenKind::Eof)) {
        if (Match(TokenKind::KwCase)) {
          SwitchCase case_stmt;
          case_stmt.line = Previous().line;
          case_stmt.column = Previous().column;
          case_stmt.is_default = false;
          if (!ParseMatchPattern(case_stmt)) {
            return nullptr;
          }
#include "parse/objc3_parser_core_statement_switch_parsing_match_case_diagnostics.inc"
#include "parse/objc3_parser_core_statement_switch_parsing_match_case_body.inc"
          stmt->switch_stmt->cases.push_back(std::move(case_stmt));
          continue;
        }

        if (Match(TokenKind::KwDefault)) {
          SwitchCase default_stmt;
          default_stmt.line = Previous().line;
          default_stmt.column = Previous().column;
          default_stmt.is_default = true;
          default_stmt.match_pattern_enabled = true;
          default_stmt.match_pattern_kind = MatchPatternKind::Wildcard;
#include "parse/objc3_parser_core_statement_switch_parsing_match_default_diagnostics.inc"
#include "parse/objc3_parser_core_statement_switch_parsing_match_default_body.inc"
          stmt->switch_stmt->cases.push_back(std::move(default_stmt));
          continue;
        }

#include "parse/objc3_parser_core_statement_switch_parsing_match_body_diagnostics.inc"
      }
