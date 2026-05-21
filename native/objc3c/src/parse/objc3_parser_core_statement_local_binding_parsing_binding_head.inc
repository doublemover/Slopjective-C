    if (At(TokenKind::KwLet) || At(TokenKind::KwVar)) {
      const bool mutable_binding = At(TokenKind::KwVar);
      Advance();
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Let;
      stmt->let_stmt = std::make_unique<LetStmt>();
      stmt->let_stmt->mutable_binding = mutable_binding;
      const Token &name_token = Peek();
      if (!Match(TokenKind::Identifier)) {
        diagnostics_.push_back(
            BuildObjc3InvalidDeclarationIdentifierDiagnostic(name_token));
        return nullptr;
      }
      stmt->let_stmt->name = Previous().text;
      stmt->let_stmt->line = Previous().line;
      stmt->let_stmt->column = Previous().column;
