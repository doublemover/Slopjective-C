  bool ParseObjcActorInterfaceDeclSuperClause(Objc3InterfaceDecl &decl) {
    if (!Match(TokenKind::Colon)) {
      return true;
    }

    const Token &super_token = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(MakeDiag(
          super_token.line, super_token.column, "O3P109",
          "invalid Objective-C superclass identifier"));
      SynchronizeTopLevel();
      return false;
    }
    decl.super_name = Previous().text;
    return true;
  }

  void ApplyObjcActorInterfaceDeclPrefixedDispatchAttributes(
      Objc3InterfaceDecl &decl,
      const Objc3InterfaceDecl *prefixed_dispatch_attributes) {
    if (prefixed_dispatch_attributes == nullptr) {
      return;
    }

    decl.prefixed_dispatch_control_attributes_declared = true;
    decl.objc_direct_members_declared =
        prefixed_dispatch_attributes->objc_direct_members_declared;
    decl.objc_derive_declared =
        prefixed_dispatch_attributes->objc_derive_declared;
    decl.objc_derive_name = prefixed_dispatch_attributes->objc_derive_name;
    decl.objc_final_declared =
        prefixed_dispatch_attributes->objc_final_declared;
    decl.objc_sealed_declared =
        prefixed_dispatch_attributes->objc_sealed_declared;
  }

  bool ParseObjcActorInterfaceDeclProtocolSuperAndDispatchClauses(
      Objc3InterfaceDecl &decl,
      const Objc3InterfaceDecl *prefixed_dispatch_attributes) {
    if (!ParseObjcActorInterfaceDeclSuperClause(decl)) {
      return false;
    }

    if (!ParseObjcProtocolCompositionClause(decl.adopted_protocols)) {
      SynchronizeObjcContainer();
    }

    ApplyObjcActorInterfaceDeclPrefixedDispatchAttributes(
        decl, prefixed_dispatch_attributes);

    if (!ParseOptionalContainerDispatchAttributes(decl)) {
      SynchronizeObjcContainer();
    }

    return true;
  }
