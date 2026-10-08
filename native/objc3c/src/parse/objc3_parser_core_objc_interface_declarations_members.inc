  void ParseObjcInterfaceDeclMembers(Objc3InterfaceDecl &decl) {
    while (!At(TokenKind::KwAtEnd) && !At(TokenKind::Eof)) {
      if (Match(TokenKind::KwAtReifyGenerics)) {
        const Token marker = Previous();
        if (!(At(TokenKind::Minus) || At(TokenKind::Plus))) {
          diagnostics_.push_back(MakeDiag(
              marker.line, marker.column, "O3P114",
              "declaration-scoped generic reification marker "
              "'@reify_generics' is only supported on Objective-C method "
              "members"));
          SynchronizeObjcContainer();
          continue;
        }
        Objc3MethodDecl method;
        if (ParseObjcMethodDecl(method, false, true, marker.line,
                                marker.column)) {
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
          continue;
        }
        SynchronizeObjcContainer();
        continue;
      }
      if (At(TokenKind::KwAtProperty)) {
        Objc3PropertyDecl property;
        if (ParseObjcPropertyDecl(property)) {
          property.scope_owner_symbol = decl.scope_owner_symbol;
          property.scope_path_symbol =
              decl.scope_owner_symbol + "::" +
              BuildObjcPropertyScopePathSymbol(property);
          AssignObjcPropertySynthesisIvarBindingSymbols(
              property, decl.semantic_link_symbol);
          AssignObjcExecutablePropertySourceModelFields(
              property, decl.semantic_link_symbol, decl.properties.size(),
              !decl.has_category);
          decl.properties.push_back(std::move(property));
          continue;
        }
        SynchronizeObjcContainer();
        continue;
      }
      if (!(At(TokenKind::Minus) || At(TokenKind::Plus))) {
        DiagnoseUnsupportedObjcInterfaceDeclMember();
        SynchronizeObjcContainer();
        continue;
      }

      Objc3MethodDecl method;
      if (ParseObjcMethodDecl(method, false)) {
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
        continue;
      }
      SynchronizeObjcContainer();
    }
  }
