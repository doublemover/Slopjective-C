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
