  std::unique_ptr<Objc3ImplementationDecl> ParseObjcImplementationDecl() {
    auto decl = std::make_unique<Objc3ImplementationDecl>();
    decl->line = Previous().line;
    decl->column = Previous().column;

    const Token &name_token = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(
          MakeDiag(name_token.line, name_token.column, "O3P101", "invalid Objective-C implementation identifier"));
      SynchronizeTopLevel();
      return nullptr;
    }
    decl->name = Previous().text;

    if (!ParseObjcCategoryClause(decl->category_name, decl->has_category)) {
      SynchronizeObjcContainer();
    }
    // executable source-closure freeze anchor: implementation and
    // category source records keep the canonical interface/category attachment
    // identity for later realization and category-merge work; this freeze does
    // not yet add executable runtime realization semantics here.
    // class/metaclass completion anchor: implementation-owned
    // declaration closure still derives stable class/metaclass object identities
    // from the parser source record rather than ad hoc runtime reconstruction.
    // protocol/category completion anchor: category implementations
    // preserve the same canonical class(category) identity and interface
    // pairing so later attachment/conformance checks consume one source-owned
    // runtime closure.
    // object-model semantic-rule freeze anchor: parser-owned
    // implementation/category records keep canonical owner identities, while
    // realization legality and merge behavior remain sema-owned decisions.
    // diagnostic precision anchor: category implementation owners
    // mirror the canonical class(category) identity to keep attachment
    // collision diagnostics deterministic across parse and pipeline stages.
    decl->semantic_link_symbol =
        BuildObjcContainerScopeOwner("implementation", decl->name, decl->has_category, decl->category_name);
    decl->semantic_link_interface_symbol = BuildObjcContainerScopeOwner("interface", decl->name, false, "");
    if (decl->has_category) {
      decl->semantic_link_category_symbol = BuildObjcCategorySemanticLinkSymbol(decl->name, decl->category_name);
    }
    decl->scope_owner_symbol =
        BuildObjcContainerScopeOwner("implementation", decl->name, decl->has_category, decl->category_name);
    decl->scope_path_lexicographic =
        BuildScopePathLexicographic(decl->scope_owner_symbol, "implementation:" + decl->name);

    while (!At(TokenKind::KwAtEnd) && !At(TokenKind::Eof)) {
      if (At(TokenKind::KwAtProperty)) {
        Objc3PropertyDecl property;
        if (ParseObjcPropertyDecl(property)) {
          property.scope_owner_symbol = decl->scope_owner_symbol;
          property.scope_path_symbol = decl->scope_owner_symbol + "::" + BuildObjcPropertyScopePathSymbol(property);
          const std::string &storage_owner_symbol =
              decl->has_category ? decl->semantic_link_symbol
                                 : decl->semantic_link_interface_symbol;
          AssignObjcExecutablePropertySourceModelFields(
              property, storage_owner_symbol, decl->properties.size(),
              !decl->has_category);
          decl->properties.push_back(std::move(property));
          continue;
        }
        SynchronizeObjcContainer();
        continue;
      }
      if (!(At(TokenKind::Minus) || At(TokenKind::Plus))) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P100", "unsupported token inside @implementation declaration"));
        SynchronizeObjcContainer();
        continue;
      }

      Objc3MethodDecl method;
      if (ParseObjcMethodDecl(method, true)) {
        // executable object artifact lowering freeze anchor: parser
        // keeps raw implementation method bodies, selectors, and canonical
        // owner identities only. Emitted object binding and realization-record
        // wiring remain downstream lane-C responsibilities.
        method.scope_owner_symbol = decl->scope_owner_symbol;
        method.scope_path_symbol = decl->scope_owner_symbol + "::" + BuildObjcMethodScopePathSymbol(method);
        AssignObjcMethodLookupOverrideConflictSymbols(
            method, decl->semantic_link_symbol, decl->semantic_link_interface_symbol);
        decl->methods.push_back(std::move(method));
        continue;
      }
      SynchronizeObjcContainer();
    }

    FinalizeObjcPropertySynthesisIvarBindingPackets(
        decl->properties, decl->property_synthesis_symbols_lexicographic,
        decl->ivar_binding_symbols_lexicographic);
    FinalizeObjcMethodLookupOverrideConflictPackets(decl->methods,
                                                    decl->method_lookup_symbols_lexicographic,
                                                    decl->override_lookup_symbols_lexicographic,
                                                    decl->conflict_lookup_symbols_lexicographic);

    if (!Match(TokenKind::KwAtEnd)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P111", "missing '@end' after @implementation"));
      SynchronizeTopLevel();
      return nullptr;
    }
    return decl;
  }
