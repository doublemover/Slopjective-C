  void FinalizeObjcActorInterfaceDeclScopeAndSemanticLinks(
      Objc3InterfaceDecl &decl) {
    decl.adopted_protocols_lexicographic =
        BuildProtocolSemanticLinkTargetsLexicographic(decl.adopted_protocols);

    decl.semantic_link_symbol =
        BuildObjcContainerScopeOwner("interface", decl.name, false, "");
    if (!decl.super_name.empty()) {
      decl.semantic_link_super_symbol = "interface:" + decl.super_name;
    }
    decl.scope_owner_symbol =
        BuildObjcContainerScopeOwner("interface", decl.name, false, "");
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

  bool FinalizeObjcActorInterfaceDecl(Objc3InterfaceDecl &decl) {
    FinalizeObjcPropertySynthesisIvarBindingPackets(
        decl.properties, decl.property_synthesis_symbols_lexicographic,
        decl.ivar_binding_symbols_lexicographic);
    FinalizeObjcMethodLookupOverrideConflictPackets(
        decl.methods, decl.method_lookup_symbols_lexicographic,
        decl.override_lookup_symbols_lexicographic,
        decl.conflict_lookup_symbols_lexicographic);

    if (!Match(TokenKind::KwAtEnd)) {
      return DiagnoseMissingObjcActorInterfaceDeclEnd();
    }
    return true;
  }
