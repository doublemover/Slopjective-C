  bool ParseObjcActorInterfaceDeclPropertyMember(
      Objc3InterfaceDecl &decl) {
    Objc3PropertyDecl property;
    if (ParseObjcPropertyDecl(property)) {
      property.scope_owner_symbol = decl.scope_owner_symbol;
      property.scope_path_symbol =
          decl.scope_owner_symbol + "::" +
          BuildObjcPropertyScopePathSymbol(property);
      AssignObjcPropertySynthesisIvarBindingSymbols(
          property, decl.semantic_link_symbol);
      AssignObjcExecutablePropertySourceModelFields(
          property, decl.semantic_link_symbol, decl.properties.size(), true);
      decl.properties.push_back(std::move(property));
      return true;
    }

    SynchronizeObjcContainer();
    return false;
  }

  bool ParseObjcActorInterfaceDeclMethodMember(
      Objc3InterfaceDecl &decl,
      bool reification_marker_declared = false,
      unsigned reification_marker_line = 1,
      unsigned reification_marker_column = 1) {
    Objc3MethodDecl method;
    if (ParseObjcMethodDecl(method, false, reification_marker_declared,
                            reification_marker_line,
                            reification_marker_column)) {
      method.scope_owner_symbol = decl.scope_owner_symbol;
      method.scope_path_symbol =
          decl.scope_owner_symbol + "::" +
          BuildObjcMethodScopePathSymbol(method);
      const std::string override_owner_symbol =
          decl.semantic_link_super_symbol.empty()
              ? decl.semantic_link_symbol
              : decl.semantic_link_super_symbol;
      AssignObjcMethodLookupOverrideConflictSymbols(
          method, decl.semantic_link_symbol, override_owner_symbol);
      decl.methods.push_back(std::move(method));
      return true;
    }

    SynchronizeObjcContainer();
    return false;
  }

  void ParseObjcActorInterfaceDeclMembers(Objc3InterfaceDecl &decl) {
    while (!At(TokenKind::KwAtEnd) && !At(TokenKind::Eof)) {
      if (Match(TokenKind::KwAtReifyGenerics)) {
        const Token marker = Previous();
        if (!(At(TokenKind::Minus) || At(TokenKind::Plus))) {
          diagnostics_.push_back(MakeDiag(
              marker.line, marker.column, "O3P114",
              "declaration-scoped generic reification marker "
              "'@reify_generics' is only supported on Objective-C actor "
              "method members"));
          SynchronizeObjcContainer();
          continue;
        }
        ParseObjcActorInterfaceDeclMethodMember(decl, true, marker.line,
                                                marker.column);
        continue;
      }
      if (At(TokenKind::KwAtProperty)) {
        ParseObjcActorInterfaceDeclPropertyMember(decl);
        continue;
      }

      if (!(At(TokenKind::Minus) || At(TokenKind::Plus))) {
        DiagnoseUnsupportedObjcActorInterfaceDeclMember();
        SynchronizeObjcContainer();
        continue;
      }

      ParseObjcActorInterfaceDeclMethodMember(decl);
    }
  }
