  void ParseTopLevelDeclaration(Objc3ParsedProgram &program) {
    if (Match(TokenKind::KwModule)) {
      ParseModule(program);
    } else if (Match(TokenKind::KwLet)) {
      auto decl = ParseGlobalLet();
      if (decl != nullptr) {
        ast_builder_.AddGlobalDecl(program, std::move(*decl));
      }
    } else if (Match(TokenKind::KwAtInterface)) {
      auto decl = ParseObjcInterfaceDecl();
      if (decl != nullptr) {
        ast_builder_.AddInterfaceDecl(program, std::move(*decl));
      }
    } else if (Match(TokenKind::KwAtImplementation)) {
      auto decl = ParseObjcImplementationDecl();
      if (decl != nullptr) {
        ast_builder_.AddImplementationDecl(program, std::move(*decl));
      }
    } else if (Match(TokenKind::KwAtProtocol)) {
      auto decl = ParseObjcProtocolDecl();
      if (decl != nullptr) {
        ast_builder_.AddProtocolDecl(program, std::move(*decl));
      }
    } else if (IsObjc3ActorClassDeclarationLead(tokens_, index_)) {
      // source-surface anchor: `actor class` stays a contextual
      // parser form rather than a dedicated lexer keyword while lane-A
      // completes actor-member frontend admission.
      const Token actor_token = Advance();
      Advance();
      auto decl = ParseObjcActorInterfaceDecl(actor_token);
      if (decl != nullptr) {
        ast_builder_.AddInterfaceDecl(program, std::move(*decl));
      }
    } else if (AtIdentifierText("__attribute__")) {
      ParseAttributePrefixedTopLevelDeclaration(program);
    } else if (IsObjc3TopLevelFunctionQualifierLead(Peek().kind)) {
      ParseTopLevelFunctionDecl(program);
    } else if (AtRemovedOptionalTemplateAlias()) {
      RejectRemovedOptionalTemplateAlias(Peek());
      SynchronizeTopLevel();
    } else if (AtCStyleTopLevelFunctionDeclStart()) {
      ParseTopLevelCStyleFunctionDecl(program);
    } else {
      const Token &token = Peek();
      diagnostics_.push_back(BuildObjc3UnsupportedTopLevelDiagnostic(token));
      SynchronizeTopLevel();
    }
  }
