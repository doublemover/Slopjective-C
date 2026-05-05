        continue;
      }

      if (Match(TokenKind::Greater)) {
        return true;
      }

      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P112", "missing '>' after Objective-C protocol composition list"));
      return false;
    }
  }

  bool ParseObjcCategoryClause(std::string &category_name, bool &has_category) {
    if (!Match(TokenKind::LParen)) {
      return true;
    }
    // category-merge source-order anchor: parser preserves category
    // attachment spelling and declaration order verbatim so sema can build one
    // deterministic realized-class merge surface without reparsing tokens.
    has_category = true;
    if (Match(TokenKind::Identifier)) {
      category_name = Previous().text;
    }
    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P109", "missing ')' after Objective-C category name"));
      return false;
    }
    return true;
  }

  std::unique_ptr<Objc3ProtocolDecl> ParseObjcProtocolDecl() {
    auto decl = std::make_unique<Objc3ProtocolDecl>();
    decl->line = Previous().line;
    decl->column = Previous().column;

    const Token &name_token = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(
          MakeDiag(name_token.line, name_token.column, "O3P101", "invalid Objective-C protocol identifier"));
      SynchronizeTopLevel();
      return nullptr;
    }
    decl->name = Previous().text;

    if (!ParseObjcProtocolCompositionClause(decl->inherited_protocols)) {
      SynchronizeObjcContainer();
    }
    decl->inherited_protocols_lexicographic =
        BuildProtocolSemanticLinkTargetsLexicographic(decl->inherited_protocols);
    decl->semantic_link_symbol = "protocol:" + decl->name;
    decl->scope_owner_symbol = BuildObjcContainerScopeOwner("protocol", decl->name, false, "");
    decl->scope_path_lexicographic =
        BuildScopePathLexicographic(decl->scope_owner_symbol, decl->semantic_link_symbol);

    if (Match(TokenKind::Semicolon)) {
      decl->is_forward_declaration = true;
      return decl;
    }

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
          property.scope_owner_symbol = decl->scope_owner_symbol;
          property.scope_path_symbol = decl->scope_owner_symbol + "::" + BuildObjcPropertyScopePathSymbol(property);
          AssignObjcExecutablePropertySourceModelFields(
              property, decl->semantic_link_symbol, decl->properties.size(),
              false);
          decl->properties.push_back(std::move(property));
          continue;
        }
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
        method.scope_owner_symbol = decl->scope_owner_symbol;
        method.scope_path_symbol = decl->scope_owner_symbol + "::" + BuildObjcMethodScopePathSymbol(method);
        AssignObjcMethodLookupOverrideConflictSymbols(method, decl->semantic_link_symbol, decl->semantic_link_symbol);
        decl->methods.push_back(std::move(method));
        continue;
      }
      SynchronizeObjcContainer();
    }

    FinalizeObjcMethodLookupOverrideConflictPackets(decl->methods,
                                                    decl->method_lookup_symbols_lexicographic,
                                                    decl->override_lookup_symbols_lexicographic,
                                                    decl->conflict_lookup_symbols_lexicographic);

    if (!Match(TokenKind::KwAtEnd)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P111", "missing '@end' after @protocol"));
      SynchronizeTopLevel();
      return nullptr;
    }
    return decl;
  }

  bool IsGenericVarianceMarker(const std::string &text) const {
    return text == "__covariant" || text == "__contravariant" ||
           text == "__invariant";
  }

  bool LooksLikeObjcInterfaceGenericParameterClause() const {
    if (!At(TokenKind::Less)) {
      return false;
    }
    int depth = 0;
    for (std::size_t offset = 0; index_ + offset < tokens_.size(); ++offset) {
      const Token &token = tokens_[index_ + offset];
      if (token.kind == TokenKind::Less) {
        ++depth;
        continue;
      }
      if (token.kind == TokenKind::Greater) {
        --depth;
        if (depth == 0) {
          return false;
        }
        continue;
      }
      if (depth == 1 &&
          (token.kind == TokenKind::Colon ||
           (token.kind == TokenKind::Identifier &&
            IsGenericVarianceMarker(token.text)))) {
        return true;
      }
      if (token.kind == TokenKind::Eof) {
        return false;
      }
    }
    return false;
  }

  bool ParseRawGenericSuffixText(std::string &suffix_text,
                                 bool &suffix_terminated,
                                 unsigned &line,
                                 unsigned &column,
                                 const std::string &diagnostic_code,
                                 const std::string &diagnostic_message,
                                 unsigned *split_closing_greater_tokens = nullptr) {
    if (!Match(TokenKind::Less)) {
      return false;
    }
    const Token &open = Previous();
    line = open.line;
    column = open.column;
    suffix_text = "<";
    suffix_terminated = false;
    int depth = 1;
    while (depth > 0 && !At(TokenKind::Eof)) {
      if (Match(TokenKind::Less)) {
        suffix_text += "<";
        ++depth;
        continue;
      }
      if (Match(TokenKind::Greater)) {
        suffix_text += ">";
        --depth;
        if (depth == 0) {
          suffix_terminated = true;
        }
        continue;
      }
      if (Match(TokenKind::GreaterGreater)) {
        if (depth >= 2) {
          suffix_text += ">>";
          depth -= 2;
          if (depth == 0) {
            suffix_terminated = true;
          }
          continue;
        }
        if (split_closing_greater_tokens != nullptr) {
          suffix_text += ">";
          --depth;
          *split_closing_greater_tokens += 1;
          suffix_terminated = true;
          continue;
        }
        suffix_text += ">>";
        depth = -1;
        break;
      }
      suffix_text += Advance().text;
    }
    if (!suffix_terminated) {
      diagnostics_.push_back(
          MakeDiag(open.line, open.column, diagnostic_code, diagnostic_message));
      return false;
    }
    return true;
  }

  bool ParseObjcInterfaceGenericParameterClause(Objc3InterfaceDecl &decl) {
    if (!LooksLikeObjcInterfaceGenericParameterClause()) {
      return true;
    }
    Match(TokenKind::Less);
    bool clause_closed_by_constraint_suffix = false;
    while (!At(TokenKind::Greater) && !At(TokenKind::Eof) &&
           !clause_closed_by_constraint_suffix) {
      Objc3GenericParamDecl param;
      if (At(TokenKind::Identifier) &&
          IsGenericVarianceMarker(Peek().text)) {
        param.variance_spelling = Advance().text;
      }

      const Token &name_token = Peek();
      if (!Match(TokenKind::Identifier)) {
        diagnostics_.push_back(MakeDiag(
            name_token.line, name_token.column, "O3P101",
            "invalid Objective-C interface generic parameter identifier"));
        return false;
      }
      param.name = Previous().text;
      param.line = Previous().line;
      param.column = Previous().column;

      if (Match(TokenKind::Colon)) {
        param.has_constraint = true;
        param.constraint_line = Previous().line;
        param.constraint_column = Previous().column;
        if (Match(TokenKind::KwId) || Match(TokenKind::KwClass) ||
            Match(TokenKind::KwInstancetype) || Match(TokenKind::Identifier)) {
          param.constraint_type_name = Previous().text;
        } else {
          const Token &constraint_token = Peek();
          diagnostics_.push_back(MakeDiag(
              constraint_token.line, constraint_token.column, "O3P101",
              "invalid Objective-C interface generic constraint type"));
          return false;
        }
        if (At(TokenKind::Less)) {
          unsigned split_closing_greater_tokens = 0;
          param.has_constraint_generic_suffix =
              ParseRawGenericSuffixText(param.constraint_generic_suffix_text,
                                        param.constraint_generic_suffix_terminated,
                                        param.constraint_line,
                                        param.constraint_column, "O3P112",
                                        "unterminated Objective-C interface generic constraint suffix",
                                        &split_closing_greater_tokens);
          if (!param.has_constraint_generic_suffix) {
            return false;
          }
          clause_closed_by_constraint_suffix =
              split_closing_greater_tokens > 0u;
        }
      }

      decl.generic_params.push_back(std::move(param));
      if (clause_closed_by_constraint_suffix) {
        break;
      }
      if (Match(TokenKind::Comma)) {
        continue;
      }
      break;
    }

    if (!clause_closed_by_constraint_suffix && !Match(TokenKind::Greater)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P112",
          "missing '>' after Objective-C interface generic parameter list"));
      return false;
    }
    return true;
  }

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

  std::unique_ptr<Objc3InterfaceDecl> ParseObjcActorInterfaceDecl(
      const Token &actor_token,
      const Objc3InterfaceDecl *prefixed_dispatch_attributes = nullptr) {
    // actor-member source-closure anchor: actor interfaces now ride
    // the same interface parsing path while publishing one dedicated frontend
    // semantic packet for member/isolation annotations.
    auto decl = std::make_unique<Objc3InterfaceDecl>();
    decl->line = actor_token.line;
    decl->column = actor_token.column;
    decl->is_actor = true;

    const Token &name_token = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(
          MakeDiag(name_token.line, name_token.column, "O3P101",
                   "invalid Objective-C actor identifier"));
      SynchronizeTopLevel();
      return nullptr;
    }
    decl->name = Previous().text;

    if (Match(TokenKind::Colon)) {
      const Token &super_token = Peek();
      if (!Match(TokenKind::Identifier)) {
        diagnostics_.push_back(MakeDiag(
            super_token.line, super_token.column, "O3P109",
            "invalid Objective-C superclass identifier"));
        SynchronizeTopLevel();
        return nullptr;
      }
      decl->super_name = Previous().text;
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
    decl->adopted_protocols_lexicographic =
        BuildProtocolSemanticLinkTargetsLexicographic(decl->adopted_protocols);

    decl->semantic_link_symbol =
        BuildObjcContainerScopeOwner("interface", decl->name, false, "");
    if (!decl->super_name.empty()) {
      decl->semantic_link_super_symbol = "interface:" + decl->super_name;
    }
    decl->scope_owner_symbol =
        BuildObjcContainerScopeOwner("interface", decl->name, false, "");
    decl->scope_path_lexicographic =
        BuildScopePathLexicographic(decl->scope_owner_symbol,
                                    "interface:" + decl->name);
    if (!decl->super_name.empty()) {
      decl->scope_path_lexicographic.push_back("super:" + decl->super_name);
      std::sort(decl->scope_path_lexicographic.begin(),
                decl->scope_path_lexicographic.end());
      decl->scope_path_lexicographic.erase(
          std::unique(decl->scope_path_lexicographic.begin(),
                      decl->scope_path_lexicographic.end()),
          decl->scope_path_lexicographic.end());
    }

    while (!At(TokenKind::KwAtEnd) && !At(TokenKind::Eof)) {
      if (At(TokenKind::KwAtProperty)) {
        Objc3PropertyDecl property;
        if (ParseObjcPropertyDecl(property)) {
          property.scope_owner_symbol = decl->scope_owner_symbol;
          property.scope_path_symbol =
              decl->scope_owner_symbol + "::" +
              BuildObjcPropertyScopePathSymbol(property);
          AssignObjcPropertySynthesisIvarBindingSymbols(
              property, decl->semantic_link_symbol);
          AssignObjcExecutablePropertySourceModelFields(
              property, decl->semantic_link_symbol, decl->properties.size(),
              true);
          decl->properties.push_back(std::move(property));
          continue;
        }
        SynchronizeObjcContainer();
        continue;
      }
      if (!(At(TokenKind::Minus) || At(TokenKind::Plus))) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P100",
            "unsupported token inside actor class declaration"));
        SynchronizeObjcContainer();
        continue;
      }

      Objc3MethodDecl method;
      if (ParseObjcMethodDecl(method, false)) {
        method.scope_owner_symbol = decl->scope_owner_symbol;
        method.scope_path_symbol =
            decl->scope_owner_symbol + "::" +
            BuildObjcMethodScopePathSymbol(method);
        const std::string override_owner_symbol =
            decl->semantic_link_super_symbol.empty()
                ? decl->semantic_link_symbol
                : decl->semantic_link_super_symbol;
        AssignObjcMethodLookupOverrideConflictSymbols(
            method, decl->semantic_link_symbol, override_owner_symbol);
        decl->methods.push_back(std::move(method));
        continue;
      }
      SynchronizeObjcContainer();
    }

    FinalizeObjcPropertySynthesisIvarBindingPackets(
        decl->properties, decl->property_synthesis_symbols_lexicographic,
        decl->ivar_binding_symbols_lexicographic);
    FinalizeObjcMethodLookupOverrideConflictPackets(
        decl->methods, decl->method_lookup_symbols_lexicographic,
        decl->override_lookup_symbols_lexicographic,
        decl->conflict_lookup_symbols_lexicographic);

    if (!Match(TokenKind::KwAtEnd)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P111",
                                      "missing '@end' after actor class"));
      SynchronizeTopLevel();
      return nullptr;
    }
    return decl;
  }

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

  std::unique_ptr<FunctionDecl> ParseFunction() {
    auto fn = std::make_unique<FunctionDecl>();
    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) || At(TokenKind::KwAsync)) {
      const Token qualifier = Advance();
      const std::string message =
          qualifier.kind == TokenKind::KwPure ? "unexpected qualifier 'pure' after 'fn'"
          : qualifier.kind == TokenKind::KwExtern ? "unexpected qualifier 'extern' after 'fn'"
                                                  : "unexpected qualifier 'async' after 'fn'";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      SynchronizeTopLevel();
      return nullptr;
    }

    const Token &name_token = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(MakeDiag(name_token.line, name_token.column, "O3P101",
                                      "invalid function identifier"));
      SynchronizeTopLevel();
      return nullptr;
    }
    fn->name = Previous().text;
    fn->line = Previous().line;
    fn->column = Previous().column;
    fn->scope_owner_symbol = "global";
    fn->scope_path_lexicographic =
        BuildScopePathLexicographic(fn->scope_owner_symbol, "function:" + fn->name);

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) || At(TokenKind::KwAsync)) {
      const Token qualifier = Advance();
      const std::string message =
          qualifier.kind == TokenKind::KwPure ? "unexpected qualifier 'pure' after function name"
          : qualifier.kind == TokenKind::KwExtern ? "unexpected qualifier 'extern' after function name"
                                                  : "unexpected qualifier 'async' after function name";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      SynchronizeTopLevel();
      return nullptr;
    }

    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P106", "missing '(' after function name"));
      SynchronizeTopLevel();
      return nullptr;
    }

    if (!ParseFunctionParameters(*fn)) {
      SynchronizeTopLevel();
      return nullptr;
    }

    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after parameters"));
      SynchronizeTopLevel();
      return nullptr;
    }

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) || At(TokenKind::KwAsync)) {
      const Token qualifier = Advance();
      const std::string message =
          qualifier.kind == TokenKind::KwPure ? "unexpected qualifier 'pure' after parameter list"
          : qualifier.kind == TokenKind::KwExtern ? "unexpected qualifier 'extern' after parameter list"
                                                  : "unexpected qualifier 'async' after parameter list";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      SynchronizeTopLevel();
      return nullptr;
    }

    bool has_return_annotation = false;
    if (!ParseOptionalAsyncClause(*fn)) {
      SynchronizeTopLevel();
      return nullptr;
    }
    if (!ParseOptionalThrowsClause(*fn)) {
      SynchronizeTopLevel();
      return nullptr;
    }

    if (Match(TokenKind::Minus)) {
      const Token arrow_start = Previous();
      if (!Match(TokenKind::Greater)) {
        diagnostics_.push_back(
            MakeDiag(arrow_start.line, arrow_start.column, "O3P114", "missing '>' in function return annotation"));
        SynchronizeFunctionTail();
        return nullptr;
      }
      if (!ParseFunctionReturnType(*fn)) {
        SynchronizeFunctionTail();
        return nullptr;
      }
      has_return_annotation = true;
    }

    if (!ParseOptionalThrowsClause(*fn)) {
      SynchronizeTopLevel();
      return nullptr;
    }

    if (!ParseOptionalAsyncClause(*fn)) {
      SynchronizeTopLevel();
      return nullptr;
    }

    if (!ParseOptionalCallableBridgeAttributes(*fn)) {
      SynchronizeTopLevel();
      return nullptr;
    }
    fn->returns_borrowed_profile = BuildReturnsBorrowedProfile(
        fn->objc_returns_borrowed_declared,
        fn->objc_returns_borrowed_owner_index,
        fn->return_borrowed_pointer_qualified);

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) || At(TokenKind::KwAsync)) {
      const Token qualifier = Advance();
      const std::string message =
          qualifier.kind == TokenKind::KwPure ? "unexpected qualifier 'pure' after function return annotation"
          : qualifier.kind == TokenKind::KwExtern ? "unexpected qualifier 'extern' after function return annotation"
                                                  : "unexpected qualifier 'async' after function return annotation";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      SynchronizeTopLevel();
      return nullptr;
    }

    if (Match(TokenKind::Semicolon)) {
      fn->is_prototype = true;
      FinalizeThrowsDeclarationProfile(*fn, has_return_annotation);
      FinalizeResultLikeProfile(*fn);
      FinalizeNSErrorBridgingProfile(*fn);
      FinalizeErrorBridgeMarkerProfile(*fn);
      FinalizeUnwindCleanupProfile(*fn);
      FinalizeErrorDiagnosticsRecoveryProfile(*fn);
      FinalizeAsyncContinuationProfile(*fn);
      FinalizeAwaitSuspensionProfile(*fn);
      FinalizeActorIsolationSendabilityProfile(*fn);
      FinalizeTaskRuntimeCancellationProfile(*fn);
      FinalizeConcurrencyReplayRaceGuardProfile(*fn);
      FinalizeUnsafePointerExtensionProfile(*fn);
      FinalizeInlineAsmIntrinsicGovernanceProfile(*fn);
      return fn;
    }

    if (!At(TokenKind::LBrace)) {
      const Token &token = Peek();
      if (At(TokenKind::KwModule) || At(TokenKind::KwLet) || At(TokenKind::KwFn) || At(TokenKind::KwPure) ||
          At(TokenKind::KwExtern) || At(TokenKind::KwAsync) || At(TokenKind::KwAtInterface) || At(TokenKind::KwAtImplementation) ||
          At(TokenKind::KwAtProtocol) || At(TokenKind::KwAtProperty) ||
          At(TokenKind::Eof)) {
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P104", "missing ';' after function prototype declaration"));
      } else {
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P110", "missing '{' to start block"));
      }
      SynchronizeTopLevel();
      return nullptr;
    }

    fn->body = ParseBlock();
    if (block_failed_) {
      block_failed_ = false;
      SynchronizeTopLevel();
      return nullptr;
    }
    FinalizeThrowsDeclarationProfile(*fn, has_return_annotation);
    FinalizeResultLikeProfile(*fn);
    FinalizeNSErrorBridgingProfile(*fn);
    FinalizeErrorBridgeMarkerProfile(*fn);
    FinalizeUnwindCleanupProfile(*fn);
    FinalizeErrorDiagnosticsRecoveryProfile(*fn);
    FinalizeAsyncContinuationProfile(*fn);
    FinalizeAwaitSuspensionProfile(*fn);
    FinalizeActorIsolationSendabilityProfile(*fn);
    FinalizeTaskRuntimeCancellationProfile(*fn);
    FinalizeConcurrencyReplayRaceGuardProfile(*fn);
    FinalizeUnsafePointerExtensionProfile(*fn);
    FinalizeInlineAsmIntrinsicGovernanceProfile(*fn);
    return fn;
  }

  bool ParseFunctionParameters(FunctionDecl &fn) {
    if (At(TokenKind::RParen)) {
      return true;
    }

    while (true) {
      if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) || At(TokenKind::KwAsync)) {
        const Token qualifier = Advance();
        const std::string message =
            qualifier.kind == TokenKind::KwPure ? "unexpected qualifier 'pure' in parameter identifier position"
            : qualifier.kind == TokenKind::KwExtern ? "unexpected qualifier 'extern' in parameter identifier position"
                                                    : "unexpected qualifier 'async' in parameter identifier position";
        diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
        return false;
      }

      if (!At(TokenKind::Identifier)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P101", "invalid parameter identifier"));
        return false;
      }

      FuncParam param;
      param.name = Advance().text;
      param.line = Previous().line;
      param.column = Previous().column;

      if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) || At(TokenKind::KwAsync)) {
        const Token qualifier = Advance();
        const std::string message =
            qualifier.kind == TokenKind::KwPure ? "unexpected qualifier 'pure' after parameter name"
            : qualifier.kind == TokenKind::KwExtern ? "unexpected qualifier 'extern' after parameter name"
                                                    : "unexpected qualifier 'async' after parameter name";
        diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
        return false;
      }

      if (!Match(TokenKind::Colon)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P107", "missing ':' after parameter name"));
        return false;
      }
      if (!ParseParameterType(param)) {
        return false;
      }

      if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) || At(TokenKind::KwAsync)) {
        const Token qualifier = Advance();
        const std::string message =
            qualifier.kind == TokenKind::KwPure ? "unexpected qualifier 'pure' after parameter type annotation"
            : qualifier.kind == TokenKind::KwExtern ? "unexpected qualifier 'extern' after parameter type annotation"
                                                    : "unexpected qualifier 'async' after parameter type annotation";
        diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
        return false;
      }

      fn.params.push_back(std::move(param));
      if (!Match(TokenKind::Comma)) {
        return true;
      }
    }
  }

  bool ParseFunctionReturnType(FunctionDecl &fn) {
    fn.return_id_spelling = false;
    fn.return_class_spelling = false;
    fn.return_sel_spelling = false;
    fn.return_instancetype_spelling = false;
    fn.return_object_pointer_type_spelling = false;
    fn.return_object_pointer_type_name.clear();
    fn.return_typecheck_family_symbol.clear();
    fn.return_vector_spelling = false;
    fn.return_vector_base_spelling.clear();
    fn.return_vector_lane_count = 1;
    fn.has_return_generic_suffix = false;
    fn.return_generic_suffix_terminated = true;
    fn.return_generic_suffix_text.clear();
    fn.return_generic_line = 1;
    fn.return_generic_column = 1;
    fn.return_lightweight_generic_constraint_profile_is_normalized = false;
    fn.return_lightweight_generic_constraint_profile.clear();
    fn.return_nullability_flow_profile_is_normalized = false;
    fn.return_nullability_flow_profile.clear();
    fn.return_protocol_qualified_object_type_profile_is_normalized = false;
    fn.return_protocol_qualified_object_type_profile.clear();
    fn.return_variance_bridge_cast_profile_is_normalized = false;
    fn.return_variance_bridge_cast_profile.clear();
    fn.return_generic_metadata_abi_profile_is_normalized = false;
    fn.return_generic_metadata_abi_profile.clear();
    fn.return_module_import_graph_profile_is_normalized = false;
    fn.return_module_import_graph_profile.clear();
    fn.return_namespace_collision_shadowing_profile_is_normalized = false;
    fn.return_namespace_collision_shadowing_profile.clear();
    fn.return_public_private_api_partition_profile_is_normalized = false;
    fn.return_public_private_api_partition_profile.clear();
    fn.return_incremental_module_cache_invalidation_profile_is_normalized = false;
    fn.return_incremental_module_cache_invalidation_profile.clear();
    fn.return_cross_module_conformance_profile_is_normalized = false;
    fn.return_cross_module_conformance_profile.clear();
    fn.has_return_pointer_declarator = false;
    fn.return_pointer_declarator_depth = 0;
    fn.return_pointer_declarator_tokens.clear();
    fn.return_nullability_suffix_tokens.clear();
    fn.has_return_ownership_qualifier = false;
    fn.return_ownership_qualifier_spelling.clear();
    fn.return_ownership_qualifier_symbol.clear();
    fn.return_ownership_qualifier_tokens.clear();
    fn.return_ownership_insert_retain = false;
    fn.return_ownership_insert_release = false;
    fn.return_ownership_insert_autorelease = false;
    fn.return_ownership_operation_profile.clear();
    fn.return_ownership_is_weak_reference = false;
    fn.return_ownership_is_unowned_reference = false;
    fn.return_ownership_is_unowned_safe_reference = false;
    fn.return_ownership_lifetime_profile.clear();
    fn.return_ownership_runtime_hook_profile.clear();
    fn.return_ownership_arc_diagnostic_candidate = false;
    fn.return_ownership_arc_fixit_available = false;
    fn.return_ownership_arc_diagnostic_profile.clear();
    fn.return_ownership_arc_fixit_hint.clear();
    fn.return_borrowed_pointer_qualified = false;

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) || At(TokenKind::KwAsync)) {
      const Token qualifier = Advance();
      const std::string message =
          qualifier.kind == TokenKind::KwPure ? "unexpected qualifier 'pure' in function return type annotation"
          : qualifier.kind == TokenKind::KwExtern ? "unexpected qualifier 'extern' in function return type annotation"
                                                  : "unexpected qualifier 'async' in function return type annotation";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      return false;
    }

    while (At(TokenKind::Identifier) && IsOwnershipQualifierSpelling(Peek().text)) {
      const Token qualifier = Advance();
      fn.has_return_ownership_qualifier = true;
      fn.return_ownership_qualifier_spelling = qualifier.text;
      fn.return_ownership_qualifier_tokens.push_back(
          MakeSemaTokenMetadata(Objc3SemaTokenKind::OwnershipQualifier, qualifier));
    }
    if (At(TokenKind::Identifier) && IsBorrowedQualifierSpelling(Peek().text)) {
      Advance();
      fn.return_borrowed_pointer_qualified = true;
    }

    if (Match(TokenKind::KwI32)) {
      fn.return_type = ValueType::I32;
    } else if (Match(TokenKind::KwBool)) {
      fn.return_type = ValueType::Bool;
    } else if (Match(TokenKind::KwBOOL)) {
      fn.return_type = ValueType::Bool;
    } else if (Match(TokenKind::KwNSInteger) || Match(TokenKind::KwNSUInteger)) {
      fn.return_type = ValueType::I32;
    } else if (Match(TokenKind::KwVoid)) {
      fn.return_type = ValueType::Void;
    } else if (Match(TokenKind::KwId)) {
      fn.return_type = ValueType::ObjCId;
      fn.return_id_spelling = true;
    } else if (Match(TokenKind::KwClass)) {
      fn.return_type = ValueType::ObjCClass;
      fn.return_class_spelling = true;
    } else if (Match(TokenKind::KwSEL)) {
      fn.return_type = ValueType::ObjCSel;
      fn.return_sel_spelling = true;
    } else if (Match(TokenKind::KwProtocol)) {
      fn.return_type = ValueType::ObjCProtocol;
    } else if (Match(TokenKind::KwInstancetype)) {
      fn.return_type = ValueType::ObjCInstancetype;
      fn.return_instancetype_spelling = true;
    } else if (AtRemovedOptionalTemplateAlias()) {
      return RejectRemovedOptionalTemplateAlias(Peek());
    } else {
      if (At(TokenKind::Identifier)) {
        const Token type_token = Advance();
        ValueType vector_type = ValueType::Unknown;
        std::string vector_base_spelling;
        unsigned vector_lane_count = 1;
        if (TryParseVectorTypeSpelling(type_token, vector_type, vector_base_spelling, vector_lane_count)) {
          fn.return_type = vector_type;
          fn.return_vector_spelling = true;
          fn.return_vector_base_spelling = vector_base_spelling;
          fn.return_vector_lane_count = vector_lane_count;
        } else {
          fn.return_type = ValueType::ObjCObjectPtr;
          fn.return_object_pointer_type_spelling = true;
          fn.return_object_pointer_type_name = type_token.text;
        }
      } else {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P114",
                     "expected function return type 'i32', 'bool', 'BOOL', 'NSInteger', 'NSUInteger', 'void', 'id', "
                     "'Class', 'SEL', 'Protocol', 'instancetype', object pointer spelling, ownership qualifiers "
                     "'__strong/__weak/__autoreleasing/__unsafe_unretained', or vector forms "
                     "'i32x2/i32x4/i32x8/i32x16' and 'boolx2/boolx4/boolx8/boolx16'"));
        return false;
      }
    }

    fn.return_typecheck_family_symbol = BuildObjcTypecheckReturnFamilySymbol(fn);

    bool parsed_generic_suffix = false;
    while (true) {
      if (At(TokenKind::Less) && !parsed_generic_suffix) {
        Match(TokenKind::Less);
        const Token &open = Previous();
        fn.has_return_generic_suffix = true;
        fn.return_generic_suffix_terminated = false;
        fn.return_generic_line = open.line;
        fn.return_generic_column = open.column;
        fn.return_generic_suffix_text = "<";
        int depth = 1;
        while (depth > 0 && !At(TokenKind::Eof)) {
          if (Match(TokenKind::Less)) {
            fn.return_generic_suffix_text += "<";
            ++depth;
            continue;
          }
          if (Match(TokenKind::Greater)) {
            fn.return_generic_suffix_text += ">";
            --depth;
            if (depth == 0) {
              fn.return_generic_suffix_terminated = true;
            }
            continue;
          }
          if (Match(TokenKind::GreaterGreater)) {
            if (depth >= 2) {
              fn.return_generic_suffix_text += ">>";
              depth -= 2;
              if (depth == 0) {
                fn.return_generic_suffix_terminated = true;
              }
              continue;
            }
            fn.return_generic_suffix_text += ">>";
            depth = -1;
            break;
          }
          fn.return_generic_suffix_text += Advance().text;
        }
        if (!fn.return_generic_suffix_terminated) {
          diagnostics_.push_back(
              MakeDiag(fn.return_generic_line, fn.return_generic_column, "O3P114",
                       "unterminated generic function return type suffix"));
          return false;
        }
        parsed_generic_suffix = true;
        continue;
      }

      if (Match(TokenKind::Star)) {
        fn.has_return_pointer_declarator = true;
        fn.return_pointer_declarator_depth += 1;
        fn.return_pointer_declarator_tokens.push_back(
            MakeSemaTokenMetadata(Objc3SemaTokenKind::PointerDeclarator, Previous()));
        continue;
      }

      if (At(TokenKind::Question) || At(TokenKind::Bang)) {
        fn.return_nullability_suffix_tokens.push_back(
            MakeSemaTokenMetadata(Objc3SemaTokenKind::NullabilitySuffix, Advance()));
        continue;
      }

      if (At(TokenKind::Identifier) && IsOwnershipQualifierSpelling(Peek().text)) {
        const Token qualifier = Advance();
        fn.has_return_ownership_qualifier = true;
        fn.return_ownership_qualifier_spelling = qualifier.text;
        fn.return_ownership_qualifier_tokens.push_back(
            MakeSemaTokenMetadata(Objc3SemaTokenKind::OwnershipQualifier, qualifier));
        continue;
      }

      break;
    }

    fn.return_ownership_qualifier_symbol =
        BuildOwnershipQualifierSymbol(fn.return_ownership_qualifier_spelling, true);
    const Objc3OwnershipOperationProfile return_ownership_profile =
        BuildReturnOwnershipOperationProfile(fn.return_ownership_qualifier_spelling);
    fn.return_ownership_insert_retain = return_ownership_profile.insert_retain;
    fn.return_ownership_insert_release = return_ownership_profile.insert_release;
    fn.return_ownership_insert_autorelease = return_ownership_profile.insert_autorelease;
    fn.return_ownership_operation_profile = return_ownership_profile.profile;
    const Objc3WeakUnownedLifetimeProfile return_lifetime_profile =
        BuildWeakUnownedLifetimeProfile(fn.return_ownership_qualifier_spelling, false);
    fn.return_ownership_is_weak_reference = return_lifetime_profile.is_weak_reference;
    fn.return_ownership_is_unowned_reference = return_lifetime_profile.is_unowned_reference;
    fn.return_ownership_is_unowned_safe_reference = return_lifetime_profile.is_unowned_safe_reference;
    fn.return_ownership_lifetime_profile = return_lifetime_profile.lifetime_profile;
    fn.return_ownership_runtime_hook_profile = return_lifetime_profile.runtime_hook_profile;
    const Objc3ArcDiagnosticFixitProfile return_arc_diagnostic_profile =
        BuildArcDiagnosticFixitProfile(fn.return_ownership_qualifier_spelling, true, false, false);
    fn.return_ownership_arc_diagnostic_candidate = return_arc_diagnostic_profile.diagnostic_candidate;
    fn.return_ownership_arc_fixit_available = return_arc_diagnostic_profile.fixit_available;
    fn.return_ownership_arc_diagnostic_profile = return_arc_diagnostic_profile.diagnostic_profile;
    fn.return_ownership_arc_fixit_hint = return_arc_diagnostic_profile.fixit_hint;
    fn.return_lightweight_generic_constraint_profile =
        BuildLightweightGenericConstraintProfile(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text);
    fn.return_lightweight_generic_constraint_profile_is_normalized =
        IsLightweightGenericConstraintProfileNormalized(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated);
    fn.return_nullability_flow_profile =
        BuildNullabilityFlowProfile(
            fn.return_object_pointer_type_spelling,
            fn.return_nullability_suffix_tokens.size(),
            fn.has_return_pointer_declarator,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated);
    fn.return_nullability_flow_profile_is_normalized =
        IsNullabilityFlowProfileNormalized(
            fn.return_object_pointer_type_spelling,
            fn.return_nullability_suffix_tokens.size());
    fn.return_protocol_qualified_object_type_profile =
        BuildProtocolQualifiedObjectTypeProfile(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text);
    fn.return_protocol_qualified_object_type_profile_is_normalized =
        IsProtocolQualifiedObjectTypeProfileNormalized(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated);
    fn.return_variance_bridge_cast_profile =
        BuildVarianceBridgeCastProfile(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text,
            fn.return_ownership_qualifier_spelling);
    fn.return_variance_bridge_cast_profile_is_normalized =
        IsVarianceBridgeCastProfileNormalized(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.return_generic_suffix_text,
            fn.return_ownership_qualifier_spelling);
    fn.return_generic_metadata_abi_profile =
        BuildGenericMetadataAbiProfile(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text,
            fn.return_ownership_qualifier_spelling);
    fn.return_generic_metadata_abi_profile_is_normalized =
        IsGenericMetadataAbiProfileNormalized(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text);
    fn.return_module_import_graph_profile =
        BuildModuleImportGraphProfile(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text,
            fn.return_object_pointer_type_name);
    fn.return_module_import_graph_profile_is_normalized =
        IsModuleImportGraphProfileNormalized(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.return_generic_suffix_text);
    fn.return_namespace_collision_shadowing_profile =
        BuildNamespaceCollisionShadowingProfile(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text,
            fn.return_object_pointer_type_name);
    fn.return_namespace_collision_shadowing_profile_is_normalized =
        IsNamespaceCollisionShadowingProfileNormalized(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.return_generic_suffix_text,
            fn.return_object_pointer_type_name);
    fn.return_public_private_api_partition_profile =
        BuildPublicPrivateApiPartitionProfile(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text,
            fn.return_object_pointer_type_name);
    fn.return_public_private_api_partition_profile_is_normalized =
        IsPublicPrivateApiPartitionProfileNormalized(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.return_generic_suffix_text,
            fn.return_object_pointer_type_name);
    fn.return_incremental_module_cache_invalidation_profile =
        BuildIncrementalModuleCacheInvalidationProfile(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text,
            fn.return_object_pointer_type_name);
    fn.return_incremental_module_cache_invalidation_profile_is_normalized =
        IsIncrementalModuleCacheInvalidationProfileNormalized(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text,
            fn.return_object_pointer_type_name);
    fn.return_cross_module_conformance_profile =
        BuildCrossModuleConformanceProfile(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text,
            fn.return_object_pointer_type_name);
    fn.return_cross_module_conformance_profile_is_normalized =
        IsCrossModuleConformanceProfileNormalized(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text,
            fn.return_object_pointer_type_name);
    fn.returns_borrowed_profile = BuildReturnsBorrowedProfile(
        fn.objc_returns_borrowed_declared,
        fn.objc_returns_borrowed_owner_index,
        fn.return_borrowed_pointer_qualified);

    return true;
  }

  bool ParseParameterType(FuncParam &param) {
    param.vector_spelling = false;
    param.vector_base_spelling.clear();
    param.vector_lane_count = 1;
    param.id_spelling = false;
    param.class_spelling = false;
    param.sel_spelling = false;
    param.instancetype_spelling = false;
    param.object_pointer_type_spelling = false;
    param.object_pointer_type_name.clear();
    param.typecheck_family_symbol.clear();
    param.has_generic_suffix = false;
    param.generic_suffix_terminated = true;
    param.generic_suffix_text.clear();
    param.generic_line = 1;
    param.generic_column = 1;
    param.lightweight_generic_constraint_profile_is_normalized = false;
    param.lightweight_generic_constraint_profile.clear();
    param.nullability_flow_profile_is_normalized = false;
    param.nullability_flow_profile.clear();
    param.protocol_qualified_object_type_profile_is_normalized = false;
    param.protocol_qualified_object_type_profile.clear();
    param.variance_bridge_cast_profile_is_normalized = false;
    param.variance_bridge_cast_profile.clear();
    param.generic_metadata_abi_profile_is_normalized = false;
    param.generic_metadata_abi_profile.clear();
    param.module_import_graph_profile_is_normalized = false;
    param.module_import_graph_profile.clear();
    param.namespace_collision_shadowing_profile_is_normalized = false;
    param.namespace_collision_shadowing_profile.clear();
    param.public_private_api_partition_profile_is_normalized = false;
    param.public_private_api_partition_profile.clear();
    param.incremental_module_cache_invalidation_profile_is_normalized = false;
    param.incremental_module_cache_invalidation_profile.clear();
    param.cross_module_conformance_profile_is_normalized = false;
    param.cross_module_conformance_profile.clear();
    param.has_pointer_declarator = false;
    param.pointer_declarator_depth = 0;
    param.pointer_declarator_tokens.clear();
    param.nullability_suffix_tokens.clear();
    param.has_ownership_qualifier = false;
    param.ownership_qualifier_spelling.clear();
    param.ownership_qualifier_symbol.clear();
    param.ownership_qualifier_tokens.clear();
    param.ownership_insert_retain = false;
    param.ownership_insert_release = false;
    param.ownership_insert_autorelease = false;
    param.ownership_operation_profile.clear();
    param.ownership_is_weak_reference = false;
    param.ownership_is_unowned_reference = false;
    param.ownership_is_unowned_safe_reference = false;
    param.ownership_lifetime_profile.clear();
    param.ownership_runtime_hook_profile.clear();
    param.ownership_arc_diagnostic_candidate = false;
    param.ownership_arc_fixit_available = false;
    param.ownership_arc_diagnostic_profile.clear();
    param.ownership_arc_fixit_hint.clear();
    param.borrowed_pointer_qualified = false;
    param.borrowed_pointer_profile.clear();
    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) || At(TokenKind::KwAsync)) {
      const Token qualifier = Advance();
      const std::string message =
          qualifier.kind == TokenKind::KwPure ? "unexpected qualifier 'pure' in parameter type annotation"
          : qualifier.kind == TokenKind::KwExtern ? "unexpected qualifier 'extern' in parameter type annotation"
                                                  : "unexpected qualifier 'async' in parameter type annotation";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      return false;
    }

    while (At(TokenKind::Identifier) && IsOwnershipQualifierSpelling(Peek().text)) {
      const Token qualifier = Advance();
      param.has_ownership_qualifier = true;
      param.ownership_qualifier_spelling = qualifier.text;
      param.ownership_qualifier_tokens.push_back(
          MakeSemaTokenMetadata(Objc3SemaTokenKind::OwnershipQualifier, qualifier));
    }
    if (At(TokenKind::Identifier) && IsBorrowedQualifierSpelling(Peek().text)) {
      Advance();
