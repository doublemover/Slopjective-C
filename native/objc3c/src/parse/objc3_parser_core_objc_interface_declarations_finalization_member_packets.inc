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
