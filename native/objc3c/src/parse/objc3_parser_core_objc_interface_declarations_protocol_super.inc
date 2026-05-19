  void ParseObjcInterfaceDeclProtocolAndSuperClauses(
      Objc3InterfaceDecl &decl,
      const Objc3InterfaceDecl *prefixed_dispatch_attributes) {
    if (Match(TokenKind::Colon)) {
      const Token &super_token = Peek();
      if (!Match(TokenKind::Identifier)) {
        diagnostics_.push_back(
            MakeDiag(super_token.line, super_token.column, "O3P101",
                     "invalid Objective-C superclass identifier"));
        SynchronizeObjcContainer();
      } else {
        decl.super_name = Previous().text;
      }
    }

    if (!ParseObjcCategoryClause(decl.category_name, decl.has_category)) {
      SynchronizeObjcContainer();
    }

    if (!ParseObjcProtocolCompositionClause(decl.adopted_protocols)) {
      SynchronizeObjcContainer();
    }
    if (prefixed_dispatch_attributes != nullptr) {
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
    if (!ParseOptionalContainerDispatchAttributes(decl)) {
      SynchronizeObjcContainer();
    }
  }
