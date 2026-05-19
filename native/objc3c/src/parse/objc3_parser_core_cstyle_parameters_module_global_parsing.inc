  void ParseModule(Objc3ParsedProgram &program) {
    // registration-descriptor/image-root source-surface and
    // frontend-closure anchor: module identity remains parser-owned here,
    // while the paired registration-descriptor and image-root prelude pragmas
    // are lexer-owned inputs that later frontend-manifest code resolves into
    // one bootstrap naming surface and emitted descriptor artifact boundary.
    const Token &name_token = Peek();
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P101", "invalid module identifier"));
      SynchronizeTopLevel();
      return;
    }
    const std::string module_name = Advance().text;
    if (!Match(TokenKind::Semicolon)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P104", "missing ';' after module declaration"));
      SynchronizeTopLevel();
      return;
    }
    if (saw_module_declaration_) {
      diagnostics_.push_back(
          MakeDiag(name_token.line, name_token.column, "O3S200", "duplicate module '" + module_name + "'"));
      return;
    }
    saw_module_declaration_ = true;
    ast_builder_.SetModuleName(program, module_name);
  }

  std::unique_ptr<GlobalDecl> ParseGlobalLet() {
    const Token &name_token = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(
          BuildObjc3InvalidDeclarationIdentifierDiagnostic(name_token));
      SynchronizeTopLevel();
      return nullptr;
    }
    auto decl = std::make_unique<GlobalDecl>(
        BuildObjc3GlobalLetDeclarationIdentity(Previous()));

    if (!Match(TokenKind::Equal)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P102", "missing '='"));
      SynchronizeTopLevel();
      return nullptr;
    }

    decl->value = ParseExpressionWithBlockLiteralSourceUse(
        BlockLiteralSourceUseKind::GlobalInitializer);
    if (decl->value == nullptr) {
      SynchronizeTopLevel();
      return nullptr;
    }

    if (!Match(TokenKind::Semicolon)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          BuildObjc3MissingSemicolonDiagnostic(token, "declaration"));
      SynchronizeTopLevel();
      return nullptr;
    }
    return decl;
  }
