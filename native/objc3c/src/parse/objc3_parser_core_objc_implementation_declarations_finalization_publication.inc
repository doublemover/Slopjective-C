  void FinalizeObjcImplementationDeclScopeAndSemanticLinks(
      Objc3ImplementationDecl &decl) {
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
    decl.semantic_link_symbol = BuildObjcContainerScopeOwner(
        "implementation", decl.name, decl.has_category, decl.category_name);
    decl.semantic_link_interface_symbol =
        BuildObjcContainerScopeOwner("interface", decl.name, false, "");
    if (decl.has_category) {
      decl.semantic_link_category_symbol =
          BuildObjcCategorySemanticLinkSymbol(decl.name, decl.category_name);
    }
    decl.scope_owner_symbol = BuildObjcContainerScopeOwner(
        "implementation", decl.name, decl.has_category, decl.category_name);
    decl.scope_path_lexicographic = BuildScopePathLexicographic(
        decl.scope_owner_symbol, "implementation:" + decl.name);
  }

  bool FinalizeObjcImplementationDecl(Objc3ImplementationDecl &decl) {
    FinalizeObjcPropertySynthesisIvarBindingPackets(
        decl.properties, decl.property_synthesis_symbols_lexicographic,
        decl.ivar_binding_symbols_lexicographic);
    FinalizeObjcMethodLookupOverrideConflictPackets(
        decl.methods, decl.method_lookup_symbols_lexicographic,
        decl.override_lookup_symbols_lexicographic,
        decl.conflict_lookup_symbols_lexicographic);

    if (!Match(TokenKind::KwAtEnd)) {
      return DiagnoseMissingObjcImplementationDeclEnd();
    }
    return true;
  }
