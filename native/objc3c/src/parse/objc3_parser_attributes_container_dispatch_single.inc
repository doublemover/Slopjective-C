  bool ParseSingleContainerDispatchAttribute(Objc3InterfaceDecl &decl) {
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P333",
          "invalid Objective-C container attribute name"));
      return false;
    }

    const Token attribute_name = Advance();
    auto record_duplicate = [&](const char *code, const std::string &message) {
      diagnostics_.push_back(MakeDiag(
          attribute_name.line, attribute_name.column, code, message));
      return false;
    };

    if (attribute_name.text == "objc_direct_members") {
      if (decl.objc_direct_members_declared) {
        return record_duplicate(
            "O3P334", "duplicate objc_direct_members attribute");
      }
      decl.objc_direct_members_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_derive") {
      if (decl.objc_derive_declared) {
        return record_duplicate("O3P345", "duplicate objc_derive attribute");
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_derive", decl.objc_derive_name)) {
        return false;
      }
      decl.objc_derive_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_final") {
      if (decl.objc_final_declared) {
        return record_duplicate("O3P335", "duplicate objc_final attribute");
      }
      decl.objc_final_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_sealed") {
      if (decl.objc_sealed_declared) {
        return record_duplicate("O3P336", "duplicate objc_sealed attribute");
      }
      decl.objc_sealed_declared = true;
      return true;
    }

    diagnostics_.push_back(MakeDiag(
        attribute_name.line, attribute_name.column, "O3P337",
        "unsupported Objective-C container attribute '" + attribute_name.text +
            "'"));
    return false;
  }
