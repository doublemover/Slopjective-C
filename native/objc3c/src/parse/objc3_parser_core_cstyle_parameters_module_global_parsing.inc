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

  void ParseDirectModuleImport(Objc3ParsedProgram &program) {
    const Token &import_token = Previous();
    std::vector<std::string> segments;
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3MOD8219",
          "malformed @import module identity; expected a dotted module identifier"));
      SynchronizeTopLevel();
      return;
    }
    segments.push_back(Advance().text);
    while (Match(TokenKind::Dot)) {
      if (!At(TokenKind::Identifier)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3MOD8219",
            "malformed @import module identity; expected a dotted module identifier"));
        SynchronizeTopLevel();
        return;
      }
      segments.push_back(Advance().text);
    }
    if (!Match(TokenKind::Semicolon)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3MOD8219",
          "missing ';' after @import module declaration"));
      SynchronizeTopLevel();
      return;
    }

    std::ostringstream module_name;
    for (std::size_t index = 0; index < segments.size(); ++index) {
      if (index != 0u) {
        module_name << ".";
      }
      module_name << segments[index];
    }
    Objc3ParsedModuleImportDecl decl;
    decl.module_name = module_name.str();
    decl.module_identity_key = "objc3.module-import:" + decl.module_name;
    decl.line = import_token.line;
    decl.column = import_token.column;
    ast_builder_.AddModuleImport(program, std::move(decl));
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
