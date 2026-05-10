  template <typename TCallableDecl>
  CallableBridgeAttributeResult ParseMacroCallableBridgeAttribute(
      TCallableDecl &decl,
      const Token &attribute_name) {
    if (attribute_name.text == "objc_macro") {
      if (decl.objc_macro_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P346",
            "duplicate objc_macro attribute"));
        return CallableBridgeAttributeResult::kFailed;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_macro", decl.objc_macro_name)) {
        return CallableBridgeAttributeResult::kFailed;
      }
      decl.objc_macro_declared = true;
      return CallableBridgeAttributeResult::kParsed;
    }
    if (attribute_name.text == "objc_macro_package") {
      if (decl.objc_macro_package_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P347",
            "duplicate objc_macro_package attribute"));
        return CallableBridgeAttributeResult::kFailed;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_macro_package",
              decl.objc_macro_package_name)) {
        return CallableBridgeAttributeResult::kFailed;
      }
      decl.objc_macro_package_declared = true;
      return CallableBridgeAttributeResult::kParsed;
    }
    if (attribute_name.text == "objc_macro_provenance") {
      if (decl.objc_macro_provenance_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P348",
            "duplicate objc_macro_provenance attribute"));
        return CallableBridgeAttributeResult::kFailed;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_macro_provenance",
              decl.objc_macro_provenance_name)) {
        return CallableBridgeAttributeResult::kFailed;
      }
      decl.objc_macro_provenance_declared = true;
      return CallableBridgeAttributeResult::kParsed;
    }
    if (attribute_name.text == "objc_macro_cache_key") {
      if (decl.objc_macro_cache_key_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P367",
            "duplicate objc_macro_cache_key attribute"));
        return CallableBridgeAttributeResult::kFailed;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_macro_cache_key",
              decl.objc_macro_cache_key_name)) {
        return CallableBridgeAttributeResult::kFailed;
      }
      decl.objc_macro_cache_key_declared = true;
      return CallableBridgeAttributeResult::kParsed;
    }
    if (attribute_name.text == "objc_macro_sandbox") {
      if (decl.objc_macro_sandbox_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P368",
            "duplicate objc_macro_sandbox attribute"));
        return CallableBridgeAttributeResult::kFailed;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_macro_sandbox",
              decl.objc_macro_sandbox_name)) {
        return CallableBridgeAttributeResult::kFailed;
      }
      decl.objc_macro_sandbox_declared = true;
      return CallableBridgeAttributeResult::kParsed;
    }
    return CallableBridgeAttributeResult::kNotMatched;
  }
