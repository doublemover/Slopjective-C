      if (!Match(TokenKind::LParen)) {
        const Token &token = Peek();
        emit_do_for_diagnostic(token, "O3P106", "missing '(' after for");
        return nullptr;
      }

      if (At(TokenKind::Identifier) &&
          index_ + 1 < tokens_.size() &&
          ((tokens_[index_ + 1].kind == TokenKind::Identifier &&
            tokens_[index_ + 1].text == "in") ||
           (tokens_[index_ + 1].kind == TokenKind::Comma &&
            index_ + 3 < tokens_.size() &&
            tokens_[index_ + 2].kind == TokenKind::Identifier &&
            tokens_[index_ + 3].kind == TokenKind::Identifier &&
            tokens_[index_ + 3].text == "in"))) {
        stmt->kind = Stmt::Kind::ForIn;
        stmt->for_stmt.reset();
        stmt->for_in_stmt = std::make_unique<ForInStmt>();
        stmt->for_in_stmt->line = stmt->line;
        stmt->for_in_stmt->column = stmt->column;
        const Token first_binding = Advance();
        if (Match(TokenKind::Comma)) {
          stmt->for_in_stmt->has_key_binding = true;
          stmt->for_in_stmt->key_name = first_binding.text;
          const Token value_binding = Advance();
          stmt->for_in_stmt->value_name = value_binding.text;
        } else {
          stmt->for_in_stmt->value_name = first_binding.text;
        }
        (void)Advance();
        stmt->for_in_stmt->collection = ParseExpressionWithBlockLiteralSourceUse(
            BlockLiteralSourceUseKind::ExpressionSite);
        if (stmt->for_in_stmt->collection == nullptr) {
          return nullptr;
        }
        if (!Match(TokenKind::RParen)) {
          const Token &token = Peek();
          emit_do_for_diagnostic(token, "O3P109",
                                 "missing ')' after for-in clause");
          return nullptr;
        }
        stmt->for_in_stmt->body = ParseControlBody();
        if (block_failed_) {
          block_failed_ = false;
          return nullptr;
        }
        return stmt;
      }

#include "parse/objc3_parser_core_statement_do_for_parsing_for_header_init.inc"

#include "parse/objc3_parser_core_statement_do_for_parsing_for_header_condition.inc"

#include "parse/objc3_parser_core_statement_do_for_parsing_for_header_step.inc"
