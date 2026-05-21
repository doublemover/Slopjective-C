  void ParseObjcProtocolRequirementMembers(Objc3ProtocolDecl &decl) {
    // protocol requirement parsing anchor: @protocol containers now
    // preserve explicit @required/@optional partitions on parser-owned method
    // and property source records. The @required/@optional partition is parser-owned
    // so lane-B sema can enforce conformance without reinterpreting raw
    // declaration text.
    Objc3ProtocolRequirementKind requirement_kind =
        Objc3ProtocolRequirementKind::Required;
    while (!At(TokenKind::KwAtEnd) && !At(TokenKind::Eof)) {
      if (Match(TokenKind::KwAtRequired)) {
        requirement_kind = Objc3ProtocolRequirementKind::Required;
        continue;
      }
      if (Match(TokenKind::KwAtOptional)) {
        requirement_kind = Objc3ProtocolRequirementKind::Optional;
        continue;
      }
      if (At(TokenKind::KwAtProperty)) {
        Objc3PropertyDecl property;
        if (ParseObjcPropertyDecl(property)) {
          property.protocol_requirement_kind = requirement_kind;
          property.scope_owner_symbol = decl.scope_owner_symbol;
          property.scope_path_symbol = decl.scope_owner_symbol + "::" + BuildObjcPropertyScopePathSymbol(property);
          AssignObjcExecutablePropertySourceModelFields(
              property, decl.semantic_link_symbol, decl.properties.size(),
              false);
          decl.properties.push_back(std::move(property));
          continue;
        }
        SynchronizeObjcContainer();
        continue;
      }
      if (At(TokenKind::Identifier) &&
          (Peek().text == "associatedtype" || Peek().text == "typealias")) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P100",
            "unsupported protocol associated-type declaration; public "
            "protocol existentials require explicit id<Protocol> requirements"));
        SynchronizeObjcContainer();
        continue;
      }
      if (!(At(TokenKind::Minus) || At(TokenKind::Plus))) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P100", "unsupported token inside @protocol declaration"));
        SynchronizeObjcContainer();
        continue;
      }

      Objc3MethodDecl method;
      if (ParseObjcMethodDecl(method, false)) {
        method.protocol_requirement_kind = requirement_kind;
        method.scope_owner_symbol = decl.scope_owner_symbol;
        method.scope_path_symbol = decl.scope_owner_symbol + "::" + BuildObjcMethodScopePathSymbol(method);
        AssignObjcMethodLookupOverrideConflictSymbols(method, decl.semantic_link_symbol, decl.semantic_link_symbol);
        decl.methods.push_back(std::move(method));
        continue;
      }
      SynchronizeObjcContainer();
    }
  }
