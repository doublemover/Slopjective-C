  std::unique_ptr<Objc3InterfaceDecl> ParseObjcInterfaceDecl(
      const Objc3InterfaceDecl *prefixed_dispatch_attributes = nullptr) {
    auto decl = std::make_unique<Objc3InterfaceDecl>();
    decl->line = Previous().line;
    decl->column = Previous().column;

    const Token &name_token = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(
          MakeDiag(name_token.line, name_token.column, "O3P101", "invalid Objective-C interface identifier"));
      SynchronizeTopLevel();
      return nullptr;
    }
    decl->name = Previous().text;

    if (!ParseObjcInterfaceGenericParameterClause(*decl)) {
      SynchronizeObjcContainer();
    }

    if (Match(TokenKind::Colon)) {
      const Token &super_token = Peek();
      if (!Match(TokenKind::Identifier)) {
        diagnostics_.push_back(
            MakeDiag(super_token.line, super_token.column, "O3P101", "invalid Objective-C superclass identifier"));
        SynchronizeObjcContainer();
      } else {
        decl->super_name = Previous().text;
      }
    }

    if (!ParseObjcCategoryClause(decl->category_name, decl->has_category)) {
      SynchronizeObjcContainer();
    }

    if (!ParseObjcProtocolCompositionClause(decl->adopted_protocols)) {
      SynchronizeObjcContainer();
    }
    if (prefixed_dispatch_attributes != nullptr) {
      decl->prefixed_dispatch_control_attributes_declared = true;
      decl->objc_direct_members_declared =
          prefixed_dispatch_attributes->objc_direct_members_declared;
      decl->objc_derive_declared =
          prefixed_dispatch_attributes->objc_derive_declared;
      decl->objc_derive_name = prefixed_dispatch_attributes->objc_derive_name;
      decl->objc_final_declared =
          prefixed_dispatch_attributes->objc_final_declared;
      decl->objc_sealed_declared =
          prefixed_dispatch_attributes->objc_sealed_declared;
    }
    if (!ParseOptionalContainerDispatchAttributes(*decl)) {
      SynchronizeObjcContainer();
    }
    // anchor: interface superclass names plus the canonical
    // lexicographic adopted-protocol list remain the parser-owned inputs for
    // graph-backed inheritance and protocol-composition validation.
    decl->adopted_protocols_lexicographic = BuildProtocolSemanticLinkTargetsLexicographic(decl->adopted_protocols);
    // executable source-closure freeze anchor: parser-owned
    // interface records remain the canonical source surface for inheritance,
    // metaclass derivation, adopted protocol lists, and category attachments
    // until later realization work binds the same identities to runnable
    // class/protocol/category behavior.
    // class/metaclass completion anchor: the same parser-owned
    // superclass spelling continues to feed declaration-level class and
    // metaclass parent identities plus stable runtime method-owner identities.
    // protocol/category completion anchor: the same parser-owned
    // adopted-protocol list and canonical category owner identity now also
    // feed fail-closed protocol inheritance and category attachment/conformance
    // closure for executable runtime metadata.
    // object-model semantic-rule freeze anchor: parser-owned
    // superclass/adopted-protocol spellings remain raw inputs only; semantic
    // legality for realization, overrides, conformance, and category merge
    // stays outside parser policy.
    // inheritance-legality source anchor: parser still preserves raw
    // superclass spellings plus deterministic member identities only; realized
    // inheritance/override legality now fails closed in sema rather than being
    // inferred or normalized here.
    // diagnostic precision anchor: category owners keep a canonical
    // class(category) semantic-link identity so attachment-collision and
    // ambiguity diagnostics can report the exact runtime metadata owner.
    // binary-boundary anchor: lane-D packages this same semantic-link
    // identity into the runtime-facing metadata envelope so later binary
    // consumers do not reconstruct owner strings from ad hoc parser state.
    decl->semantic_link_symbol =
        BuildObjcContainerScopeOwner("interface", decl->name, decl->has_category, decl->category_name);
    if (!decl->super_name.empty()) {
      decl->semantic_link_super_symbol = "interface:" + decl->super_name;
    }
    if (decl->has_category) {
      decl->semantic_link_category_symbol = BuildObjcCategorySemanticLinkSymbol(decl->name, decl->category_name);
    }
    decl->scope_owner_symbol =
        BuildObjcContainerScopeOwner("interface", decl->name, decl->has_category, decl->category_name);
    decl->scope_path_lexicographic =
        BuildScopePathLexicographic(decl->scope_owner_symbol, "interface:" + decl->name);
    if (!decl->super_name.empty()) {
      decl->scope_path_lexicographic.push_back("super:" + decl->super_name);
      std::sort(decl->scope_path_lexicographic.begin(), decl->scope_path_lexicographic.end());
      decl->scope_path_lexicographic.erase(
          std::unique(decl->scope_path_lexicographic.begin(), decl->scope_path_lexicographic.end()),
          decl->scope_path_lexicographic.end());
    }

    while (!At(TokenKind::KwAtEnd) && !At(TokenKind::Eof)) {
      if (At(TokenKind::KwAtProperty)) {
        Objc3PropertyDecl property;
        if (ParseObjcPropertyDecl(property)) {
          property.scope_owner_symbol = decl->scope_owner_symbol;
          property.scope_path_symbol = decl->scope_owner_symbol + "::" + BuildObjcPropertyScopePathSymbol(property);
          AssignObjcPropertySynthesisIvarBindingSymbols(property, decl->semantic_link_symbol);
          AssignObjcExecutablePropertySourceModelFields(
              property, decl->semantic_link_symbol, decl->properties.size(),
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
            MakeDiag(token.line, token.column, "O3P100", "unsupported token inside @interface declaration"));
        SynchronizeObjcContainer();
        continue;
      }

      Objc3MethodDecl method;
      if (ParseObjcMethodDecl(method, false)) {
        method.scope_owner_symbol = decl->scope_owner_symbol;
        method.scope_path_symbol = decl->scope_owner_symbol + "::" + BuildObjcMethodScopePathSymbol(method);
        const std::string override_owner_symbol =
            decl->semantic_link_super_symbol.empty() ? decl->semantic_link_symbol : decl->semantic_link_super_symbol;
        AssignObjcMethodLookupOverrideConflictSymbols(method, decl->semantic_link_symbol, override_owner_symbol);
        decl->methods.push_back(std::move(method));
        continue;
      }
      SynchronizeObjcContainer();
    }

    FinalizeObjcPropertySynthesisIvarBindingPackets(decl->properties,
                                                    decl->property_synthesis_symbols_lexicographic,
                                                    decl->ivar_binding_symbols_lexicographic);
    FinalizeObjcMethodLookupOverrideConflictPackets(decl->methods,
                                                    decl->method_lookup_symbols_lexicographic,
                                                    decl->override_lookup_symbols_lexicographic,
                                                    decl->conflict_lookup_symbols_lexicographic);

    if (!Match(TokenKind::KwAtEnd)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P111", "missing '@end' after @interface"));
      SynchronizeTopLevel();
      return nullptr;
    }
    return decl;
  }
