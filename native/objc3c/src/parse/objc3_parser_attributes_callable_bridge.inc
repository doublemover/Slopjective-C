  template <typename TCallableDecl>
  bool ParseSingleCallableBridgeAttribute(TCallableDecl &decl) {
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P281",
          "invalid callable attribute name"));
      return false;
    }
    const Token attribute_name = Advance();
    if (attribute_name.text == "objc_nserror") {
      if (decl.objc_nserror_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P282",
            "duplicate objc_nserror attribute"));
        return false;
      }
      decl.objc_nserror_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_status_code") {
      if (decl.objc_status_code_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P283",
            "duplicate objc_status_code attribute"));
        return false;
      }
      return ParseStatusCodeBridgeAttributePayload(decl, attribute_name);
    }
    if (attribute_name.text == "objc_executor") {
      if (decl.executor_affinity_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P294",
            "duplicate objc_executor attribute"));
        return false;
      }
      return ParseExecutorAttributePayload(decl, attribute_name);
    }
    if (attribute_name.text == "objc_macro") {
      if (decl.objc_macro_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P346",
            "duplicate objc_macro attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_macro", decl.objc_macro_name)) {
        return false;
      }
      decl.objc_macro_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_macro_package") {
      if (decl.objc_macro_package_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P347",
            "duplicate objc_macro_package attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_macro_package",
              decl.objc_macro_package_name)) {
        return false;
      }
      decl.objc_macro_package_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_macro_provenance") {
      if (decl.objc_macro_provenance_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P348",
            "duplicate objc_macro_provenance attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_macro_provenance",
              decl.objc_macro_provenance_name)) {
        return false;
      }
      decl.objc_macro_provenance_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_macro_cache_key") {
      if (decl.objc_macro_cache_key_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P367",
            "duplicate objc_macro_cache_key attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_macro_cache_key",
              decl.objc_macro_cache_key_name)) {
        return false;
      }
      decl.objc_macro_cache_key_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_macro_sandbox") {
      if (decl.objc_macro_sandbox_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P368",
            "duplicate objc_macro_sandbox attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_macro_sandbox",
              decl.objc_macro_sandbox_name)) {
        return false;
      }
      decl.objc_macro_sandbox_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_foreign") {
      if (decl.objc_foreign_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P349",
            "duplicate objc_foreign attribute"));
        return false;
      }
      decl.objc_foreign_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_import_module") {
      if (decl.objc_import_module_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P350",
            "duplicate objc_import_module attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_import_module",
              decl.objc_import_module_name)) {
        return false;
      }
      decl.objc_import_module_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_swift_name") {
      if (decl.objc_swift_name_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P351",
            "duplicate objc_swift_name attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_swift_name", decl.objc_swift_name)) {
        return false;
      }
      decl.objc_swift_name_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_swift_private") {
      if (decl.objc_swift_private_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P352",
            "duplicate objc_swift_private attribute"));
        return false;
      }
      decl.objc_swift_private_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_cxx_name") {
      if (decl.objc_cxx_name_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P353",
            "duplicate objc_cxx_name attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_cxx_name", decl.objc_cxx_name)) {
        return false;
      }
      decl.objc_cxx_name_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_header_name") {
      if (decl.objc_header_name_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P354",
            "duplicate objc_header_name attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_header_name", decl.objc_header_name)) {
        return false;
      }
      decl.objc_header_name_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_export_header") {
      if (decl.objc_export_header_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P356",
            "duplicate objc_export_header attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_export_header",
              decl.objc_export_header_name)) {
        return false;
      }
      decl.objc_export_header_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_abi_align") {
      if (decl.objc_abi_align_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P357",
            "duplicate objc_abi_align attribute"));
        return false;
      }
      return ParseAbiAlignmentAttributePayload(decl, attribute_name);
    }
    if (attribute_name.text == "objc_foreign_type") {
      if (decl.objc_foreign_type_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P358",
            "duplicate objc_foreign_type attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_foreign_type",
              decl.objc_foreign_type_name)) {
        return false;
      }
      decl.objc_foreign_type_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_mixed_image") {
      if (decl.objc_mixed_image_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P359",
            "duplicate objc_mixed_image attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_mixed_image", decl.objc_mixed_image_name)) {
        return false;
      }
      decl.objc_mixed_image_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_package_entry") {
      if (decl.objc_package_entry_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P360",
            "duplicate objc_package_entry attribute"));
        return false;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_package_entry",
              decl.objc_package_entry_name)) {
        return false;
      }
      decl.objc_package_entry_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_nonisolated") {
      if (decl.objc_nonisolated_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P295",
            "duplicate objc_nonisolated attribute"));
        return false;
      }
      decl.objc_nonisolated_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_returns_borrowed") {
      if (decl.objc_returns_borrowed_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P312",
            "duplicate objc_returns_borrowed attribute"));
        return false;
      }
      return ParseReturnsBorrowedAttributePayload(decl);
    }
    if (ParseRetainableCFamilyCallableAttribute(decl, attribute_name)) {
      return true;
    }
    if (ParseDispatchIntentCallableAttribute(decl, attribute_name)) {
      return true;
    }

    diagnostics_.push_back(MakeDiag(
        attribute_name.line, attribute_name.column, "O3P284",
        "unsupported callable attribute '" + attribute_name.text + "'"));
    return false;
  }

  template <typename TCallableDecl>
  bool ParseOptionalCallableBridgeAttributesImpl(TCallableDecl &decl) {
    while (AtIdentifierText("__attribute__")) {
      Advance();
      if (!Match(TokenKind::LParen) || !Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P285",
            "malformed __attribute__ callable annotation"));
        return false;
      }

      do {
        if (!ParseSingleCallableBridgeAttribute(decl)) {
          return false;
        }
      } while (Match(TokenKind::Comma));

      if (!Match(TokenKind::RParen) || !Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P286",
            "missing '))' after callable attribute list"));
        return false;
      }
    }
    return true;
  }
