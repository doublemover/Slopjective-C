  void FinalizeObjcInterfaceDeclScopeAndSemanticLinks(
      Objc3InterfaceDecl &decl) {
    // anchor: interface superclass names plus the canonical
    // lexicographic adopted-protocol list remain the parser-owned inputs for
    // graph-backed inheritance and protocol-composition validation.
    decl.adopted_protocols_lexicographic =
        BuildProtocolSemanticLinkTargetsLexicographic(decl.adopted_protocols);
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
    decl.semantic_link_symbol = BuildObjcContainerScopeOwner(
        "interface", decl.name, decl.has_category, decl.category_name);
    if (!decl.super_name.empty()) {
      decl.semantic_link_super_symbol = "interface:" + decl.super_name;
    }
    if (decl.has_category) {
      decl.semantic_link_category_symbol =
          BuildObjcCategorySemanticLinkSymbol(decl.name, decl.category_name);
    }
    decl.scope_owner_symbol = BuildObjcContainerScopeOwner(
        "interface", decl.name, decl.has_category, decl.category_name);
    decl.scope_path_lexicographic =
        BuildScopePathLexicographic(decl.scope_owner_symbol,
                                    "interface:" + decl.name);
    if (!decl.super_name.empty()) {
      decl.scope_path_lexicographic.push_back("super:" + decl.super_name);
      std::sort(decl.scope_path_lexicographic.begin(),
                decl.scope_path_lexicographic.end());
      decl.scope_path_lexicographic.erase(
          std::unique(decl.scope_path_lexicographic.begin(),
                      decl.scope_path_lexicographic.end()),
          decl.scope_path_lexicographic.end());
    }
  }

  bool FinalizeObjcInterfaceDecl(Objc3InterfaceDecl &decl) {
    FinalizeObjcPropertySynthesisIvarBindingPackets(
        decl.properties, decl.property_synthesis_symbols_lexicographic,
        decl.ivar_binding_symbols_lexicographic);
    FinalizeObjcMethodLookupOverrideConflictPackets(
        decl.methods, decl.method_lookup_symbols_lexicographic,
        decl.override_lookup_symbols_lexicographic,
        decl.conflict_lookup_symbols_lexicographic);

    if (!Match(TokenKind::KwAtEnd)) {
      return DiagnoseMissingObjcInterfaceDeclEnd();
    }
    return true;
  }
