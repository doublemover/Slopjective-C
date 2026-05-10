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

