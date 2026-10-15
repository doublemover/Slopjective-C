  void ParseObjcImplementationDeclMethodMember(
      Objc3ImplementationDecl &decl,
      bool reification_marker_declared = false,
      unsigned reification_marker_line = 1,
      unsigned reification_marker_column = 1) {
    Objc3MethodDecl method;
    if (ParseObjcMethodDecl(method, true, reification_marker_declared,
                            reification_marker_line,
                            reification_marker_column)) {
      // executable object artifact lowering freeze anchor: parser
      // keeps raw implementation method bodies, selectors, and canonical
      // owner identities only. Emitted object binding and realization-record
      // wiring remain downstream lane-C responsibilities.
      method.scope_owner_symbol = decl.scope_owner_symbol;
      method.scope_path_symbol =
          decl.scope_owner_symbol + "::" +
          BuildObjcMethodScopePathSymbol(method);
      AssignObjcMethodLookupOverrideConflictSymbols(
          method, decl.semantic_link_symbol,
          decl.semantic_link_interface_symbol);
      decl.methods.push_back(std::move(method));
      return;
    }

    SynchronizeObjcContainer();
  }
