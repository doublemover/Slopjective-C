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

    bool saw_local_storage_annotation = false;
    LetStmt local_storage_annotation;
    if (AtIdentifierText("__attribute__")) {
      Advance();
      if (!ParseLocalStorageAttribute(local_storage_annotation)) {
        return nullptr;
      }
      saw_local_storage_annotation = true;
    } else if (At(TokenKind::KwAtCleanup) || At(TokenKind::KwAtResource)) {
      if (!ParseLocalStorageSugar(local_storage_annotation)) {
        return nullptr;
      }
      saw_local_storage_annotation = true;
    }

    if (Match(TokenKind::KwLet)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Let;
      stmt->let_stmt = std::make_unique<LetStmt>();
      const Token &name_token = Peek();
      if (!Match(TokenKind::Identifier)) {
        diagnostics_.push_back(
            BuildObjc3InvalidDeclarationIdentifierDiagnostic(name_token));
        return nullptr;
      }
      stmt->let_stmt->name = Previous().text;
      stmt->let_stmt->line = Previous().line;
      stmt->let_stmt->column = Previous().column;
      if (saw_local_storage_annotation) {
        stmt->let_stmt->cleanup_attribute_declared =
            local_storage_annotation.cleanup_attribute_declared;
        stmt->let_stmt->cleanup_sugar_declared =
            local_storage_annotation.cleanup_sugar_declared;
        stmt->let_stmt->cleanup_function_symbol =
            local_storage_annotation.cleanup_function_symbol;
        stmt->let_stmt->cleanup_profile_is_normalized =
            local_storage_annotation.cleanup_profile_is_normalized;
        stmt->let_stmt->cleanup_profile =
            local_storage_annotation.cleanup_profile;
        stmt->let_stmt->resource_attribute_declared =
            local_storage_annotation.resource_attribute_declared;
        stmt->let_stmt->resource_sugar_declared =
            local_storage_annotation.resource_sugar_declared;
        stmt->let_stmt->resource_close_symbol =
            local_storage_annotation.resource_close_symbol;
        stmt->let_stmt->resource_invalid_expression =
            local_storage_annotation.resource_invalid_expression;
        stmt->let_stmt->resource_profile_is_normalized =
            local_storage_annotation.resource_profile_is_normalized;
        stmt->let_stmt->resource_profile =
            local_storage_annotation.resource_profile;
      }
      stmt->line = Previous().line;
      stmt->column = Previous().column;

      if (!Match(TokenKind::Equal)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P102", "missing '='"));
        return nullptr;
      }

      stmt->let_stmt->value = ParseExpressionWithBlockLiteralSourceUse(
          BlockLiteralSourceUseKind::LocalBindingInitializer);
      if (stmt->let_stmt->value == nullptr) {
        return nullptr;
      }

      if (!Match(TokenKind::Semicolon)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            BuildObjc3MissingSemicolonDiagnostic(token, "declaration"));
        return nullptr;
      }
      return stmt;
    }

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

    if (Match(TokenKind::KwWhile)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::While;
      stmt->while_stmt = std::make_unique<WhileStmt>();
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      stmt->while_stmt->line = Previous().line;
      stmt->while_stmt->column = Previous().column;

      if (!Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P106", "missing '(' after while"));
        return nullptr;
      }
      stmt->while_stmt->condition = ParseExpressionWithBlockLiteralSourceUse(
          BlockLiteralSourceUseKind::ExpressionSite);
      if (stmt->while_stmt->condition == nullptr) {
        return nullptr;
      }
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after while condition"));
        return nullptr;
      }

      stmt->while_stmt->body = ParseControlBody();
      if (block_failed_) {
        block_failed_ = false;
        return nullptr;
      }
      return stmt;
    }

    if (Match(TokenKind::KwBreak)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Break;
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      if (!Match(TokenKind::Semicolon)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104", "missing ';' after break"));
        return nullptr;
      }
      return stmt;
    }

    if (Match(TokenKind::KwContinue)) {
      auto stmt = BuildObjc3LoopControlStatement(Stmt::Kind::Continue, Previous());
      if (!Match(TokenKind::Semicolon)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            BuildObjc3MissingSemicolonDiagnostic(token, "continue"));
        return nullptr;
      }
      return stmt;
    }

    if (AtIdentifierAssignment() || AtIdentifierUpdate()) {
      const Token name = Advance();
      std::string op = "=";
      if (!MatchAssignmentOperator(op)) {
        (void)MatchUpdateOperator(op);
      }
      std::unique_ptr<Expr> value;
      if (op == "++" || op == "--") {
        value = nullptr;
      } else {
        value = ParseExpressionWithBlockLiteralSourceUse(
            BlockLiteralSourceUseKind::AssignmentValue);
        if (value == nullptr) {
          return nullptr;
        }
      }
      auto stmt = BuildObjc3AssignmentStatement(name, op, std::move(value));
      if (!Match(TokenKind::Semicolon)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            BuildObjc3MissingSemicolonDiagnostic(token, "assignment"));
        return nullptr;
      }
      return stmt;
    }

    if (AtPrefixUpdate()) {
      std::string op = "++";
      const Token op_token = Peek();
      (void)MatchUpdateOperator(op);
      const Token name = Peek();
      if (!Match(TokenKind::Identifier)) {
        diagnostics_.push_back(MakeDiag(op_token.line, op_token.column, "O3P101", "invalid assignment target"));
        return nullptr;
      }
      auto stmt = BuildObjc3AssignmentStatement(name, op, nullptr);
      if (!Match(TokenKind::Semicolon)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            BuildObjc3MissingSemicolonDiagnostic(token, "assignment"));
        return nullptr;
      }
      return stmt;
    }

    const Token expr_anchor = Peek();
    auto expr = ParseExpressionWithBlockLiteralSourceUse(
        BlockLiteralSourceUseKind::ExpressionSite);
    if (expr == nullptr) {
      return nullptr;
    }
    auto stmt = BuildObjc3ExpressionStatement(expr_anchor, std::move(expr));
    if (!Match(TokenKind::Semicolon)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          BuildObjc3MissingSemicolonDiagnostic(token, "expression"));
      return nullptr;
    }
    return stmt;
  }
