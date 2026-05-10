  std::unique_ptr<Stmt> ParseStatement() {
    if (At(TokenKind::LBrace)) {
      const Token open = Peek();
      auto body = ParseBlock();
      if (block_failed_) {
        block_failed_ = false;
        return nullptr;
      }
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Block;
      stmt->line = open.line;
      stmt->column = open.column;
      stmt->block_stmt = std::make_unique<BlockStmt>();
      stmt->block_stmt->line = open.line;
      stmt->block_stmt->column = open.column;
      stmt->block_stmt->body = std::move(body);
      return stmt;
    }

    if (Match(TokenKind::Semicolon)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Empty;
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      return stmt;
    }

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) || At(TokenKind::KwAsync)) {
      const Token qualifier = Advance();
      const std::string message =
          qualifier.kind == TokenKind::KwPure ? "unexpected qualifier 'pure' in statement position"
          : qualifier.kind == TokenKind::KwExtern ? "unexpected qualifier 'extern' in statement position"
                                                  : "unexpected qualifier 'async' in statement position";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      return nullptr;
    }

    if (Match(TokenKind::KwAtAutoreleasePool)) {
      const Token marker = Previous();
      const unsigned scope_depth = autoreleasepool_scope_depth_ + 1u;
      ++autoreleasepool_scope_depth_;
      const unsigned scope_serial = ++autoreleasepool_scope_serial_;
      auto body = ParseBlock();
      --autoreleasepool_scope_depth_;
      if (block_failed_) {
        block_failed_ = false;
        return nullptr;
      }

      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Block;
      stmt->line = marker.line;
      stmt->column = marker.column;
      stmt->block_stmt = std::make_unique<BlockStmt>();
      stmt->block_stmt->line = marker.line;
      stmt->block_stmt->column = marker.column;
      stmt->block_stmt->body = std::move(body);
      stmt->block_stmt->is_autoreleasepool_scope = true;
      stmt->block_stmt->autoreleasepool_scope_depth = scope_depth;
      stmt->block_stmt->autoreleasepool_scope_symbol =
          BuildAutoreleasePoolScopeSymbol(scope_serial, scope_depth);
      return stmt;
    }

#include "parse/objc3_parser_core_statement_local_binding_parsing.inc"

    if (Match(TokenKind::KwReturn)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Return;
      stmt->return_stmt = std::make_unique<ReturnStmt>();
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      stmt->return_stmt->line = Previous().line;
      stmt->return_stmt->column = Previous().column;
      if (Match(TokenKind::Semicolon)) {
        return stmt;
      }
      stmt->return_stmt->value = ParseExpressionWithBlockLiteralSourceUse(
          BlockLiteralSourceUseKind::ReturnValue);
      if (stmt->return_stmt->value == nullptr) {
        return nullptr;
      }
      if (!Match(TokenKind::Semicolon)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            BuildObjc3MissingSemicolonDiagnostic(token, "return"));
        return nullptr;
      }
      return stmt;
    }

#include "parse/objc3_parser_core_statement_if_guard_parsing.inc"

    if (Match(TokenKind::KwDefer)) {
      const Token &token = Previous();
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Defer;
      stmt->block_stmt = std::make_unique<BlockStmt>();
      stmt->line = token.line;
      stmt->column = token.column;
      stmt->block_stmt->line = token.line;
      stmt->block_stmt->column = token.column;
      if (Peek().kind != TokenKind::LBrace) {
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P110", "defer requires braced body"));
        return nullptr;
      }
      stmt->block_stmt->body = ParseBlock();
      if (block_failed_) {
        block_failed_ = false;
        return nullptr;
      }
      return stmt;
    }

    if (Match(TokenKind::KwThrow)) {
      const Token &token = Previous();
      auto payload = ParseExpressionWithBlockLiteralSourceUse(
          BlockLiteralSourceUseKind::ExpressionSite);
      if (payload == nullptr) {
        return nullptr;
      }
      if (!Match(TokenKind::Semicolon)) {
        const Token &semicolon_token = Peek();
        diagnostics_.push_back(MakeDiag(semicolon_token.line,
                                        semicolon_token.column, "O3P104",
                                        "missing ';' after throw statement"));
        return nullptr;
      }
      auto call = std::make_unique<Expr>();
      call->kind = Expr::Kind::Call;
      call->ident = "__objc3_throw_stmt";
      call->line = token.line;
      call->column = token.column;
      call->throw_statement_enabled = true;
      call->throw_statement_is_normalized = true;
      call->args.push_back(std::move(payload));
      call->throw_statement_profile = BuildThrowStatementProfile(*call);

      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Expr;
      stmt->line = token.line;
      stmt->column = token.column;
      stmt->expr_stmt = std::make_unique<ExprStmt>();
      stmt->expr_stmt->line = token.line;
      stmt->expr_stmt->column = token.column;
      stmt->expr_stmt->value = std::move(call);
      return stmt;
    }

#include "parse/objc3_parser_core_statement_do_for_parsing.inc"

#include "parse/objc3_parser_core_statement_switch_parsing.inc"

#include "parse/objc3_parser_core_statement_tail_parsing.inc"
  }
