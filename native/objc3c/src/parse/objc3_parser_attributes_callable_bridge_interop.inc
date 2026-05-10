  template <typename TCallableDecl>
  CallableBridgeAttributeResult ParseInteropCallableBridgeAttribute(
      TCallableDecl &decl,
      const Token &attribute_name) {
    if (attribute_name.text == "objc_foreign") {
      if (decl.objc_foreign_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P349",
            "duplicate objc_foreign attribute"));
        return CallableBridgeAttributeResult::kFailed;
      }
      decl.objc_foreign_declared = true;
      return CallableBridgeAttributeResult::kParsed;
    }
    if (attribute_name.text == "objc_import_module") {
      if (decl.objc_import_module_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P350",
            "duplicate objc_import_module attribute"));
        return CallableBridgeAttributeResult::kFailed;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_import_module",
              decl.objc_import_module_name)) {
        return CallableBridgeAttributeResult::kFailed;
      }
      decl.objc_import_module_declared = true;
      return CallableBridgeAttributeResult::kParsed;
    }
    if (attribute_name.text == "objc_swift_name") {
      if (decl.objc_swift_name_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P351",
            "duplicate objc_swift_name attribute"));
        return CallableBridgeAttributeResult::kFailed;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_swift_name", decl.objc_swift_name)) {
        return CallableBridgeAttributeResult::kFailed;
      }
      decl.objc_swift_name_declared = true;
      return CallableBridgeAttributeResult::kParsed;
    }
    if (attribute_name.text == "objc_swift_private") {
      if (decl.objc_swift_private_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P352",
            "duplicate objc_swift_private attribute"));
        return CallableBridgeAttributeResult::kFailed;
      }
      decl.objc_swift_private_declared = true;
      return CallableBridgeAttributeResult::kParsed;
    }
    if (attribute_name.text == "objc_cxx_name") {
      if (decl.objc_cxx_name_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P353",
            "duplicate objc_cxx_name attribute"));
        return CallableBridgeAttributeResult::kFailed;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_cxx_name", decl.objc_cxx_name)) {
        return CallableBridgeAttributeResult::kFailed;
      }
      decl.objc_cxx_name_declared = true;
      return CallableBridgeAttributeResult::kParsed;
    }
    if (attribute_name.text == "objc_header_name") {
      if (decl.objc_header_name_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P354",
            "duplicate objc_header_name attribute"));
        return CallableBridgeAttributeResult::kFailed;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_header_name", decl.objc_header_name)) {
        return CallableBridgeAttributeResult::kFailed;
      }
      decl.objc_header_name_declared = true;
      return CallableBridgeAttributeResult::kParsed;
    }
    if (attribute_name.text == "objc_export_header") {
      if (decl.objc_export_header_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P356",
            "duplicate objc_export_header attribute"));
        return CallableBridgeAttributeResult::kFailed;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_export_header",
              decl.objc_export_header_name)) {
        return CallableBridgeAttributeResult::kFailed;
      }
      decl.objc_export_header_declared = true;
      return CallableBridgeAttributeResult::kParsed;
    }
    return CallableBridgeAttributeResult::kNotMatched;
  }
