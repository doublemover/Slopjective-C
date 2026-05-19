  template <typename TCallableDecl>
  CallableBridgeAttributeResult ParseAbiCallableBridgeAttribute(
      TCallableDecl &decl,
      const Token &attribute_name) {
    if (attribute_name.text == "objc_abi_align") {
      if (decl.objc_abi_align_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P357",
            "duplicate objc_abi_align attribute"));
        return CallableBridgeAttributeResult::kFailed;
      }
      return ParseAbiAlignmentAttributePayload(decl, attribute_name)
                 ? CallableBridgeAttributeResult::kParsed
                 : CallableBridgeAttributeResult::kFailed;
    }
    if (attribute_name.text == "objc_foreign_type") {
      if (decl.objc_foreign_type_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P358",
            "duplicate objc_foreign_type attribute"));
        return CallableBridgeAttributeResult::kFailed;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_foreign_type",
              decl.objc_foreign_type_name)) {
        return CallableBridgeAttributeResult::kFailed;
      }
      decl.objc_foreign_type_declared = true;
      return CallableBridgeAttributeResult::kParsed;
    }
    if (attribute_name.text == "objc_mixed_image") {
      if (decl.objc_mixed_image_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P359",
            "duplicate objc_mixed_image attribute"));
        return CallableBridgeAttributeResult::kFailed;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_mixed_image", decl.objc_mixed_image_name)) {
        return CallableBridgeAttributeResult::kFailed;
      }
      decl.objc_mixed_image_declared = true;
      return CallableBridgeAttributeResult::kParsed;
    }
    if (attribute_name.text == "objc_package_entry") {
      if (decl.objc_package_entry_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P360",
            "duplicate objc_package_entry attribute"));
        return CallableBridgeAttributeResult::kFailed;
      }
      if (!ParseNamedStringAttributePayload(
              attribute_name, "objc_package_entry",
              decl.objc_package_entry_name)) {
        return CallableBridgeAttributeResult::kFailed;
      }
      decl.objc_package_entry_declared = true;
      return CallableBridgeAttributeResult::kParsed;
    }
    return CallableBridgeAttributeResult::kNotMatched;
  }
