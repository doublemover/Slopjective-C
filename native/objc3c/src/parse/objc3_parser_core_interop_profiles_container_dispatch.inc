  void ParseAttributePrefixedTopLevelDeclaration(Objc3ParsedProgram &program) {
    // source-completion anchor: prefixed Part 9 container attributes are
    // parser-owned wrappers around the existing interface / actor container
    // forms rather than a widened top-level grammar family.
    Objc3InterfaceDecl pending_container_attributes;
    if (!ParseOptionalContainerDispatchAttributes(
            pending_container_attributes)) {
      SynchronizeTopLevel();
      return;
    }
    if (Match(TokenKind::KwAtInterface)) {
      auto decl = ParseObjcInterfaceDecl(&pending_container_attributes);
      if (decl != nullptr) {
        ast_builder_.AddInterfaceDecl(program, std::move(*decl));
      }
    } else if (IsObjc3ActorClassDeclarationLead(tokens_, index_)) {
      const Token actor_token = Advance();
      Advance();
      auto decl = ParseObjcActorInterfaceDecl(actor_token,
                                              &pending_container_attributes);
      if (decl != nullptr) {
        ast_builder_.AddInterfaceDecl(program, std::move(*decl));
      }
    } else {
      const Token &token = Peek();
      diagnostics_.push_back(BuildObjc3UnsupportedTopLevelDiagnostic(token));
      SynchronizeTopLevel();
    }
  }
