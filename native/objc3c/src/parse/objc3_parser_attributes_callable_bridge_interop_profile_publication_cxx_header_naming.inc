  template <typename TCallableDecl>
  CallableBridgeAttributeResult PublishObjcCxxNameCallableBridgeAttribute(
      TCallableDecl &decl,
      const Token &attribute_name) {
    if (decl.objc_cxx_name_declared) {
      return RejectDuplicateInteropCallableBridgeAttribute(
          attribute_name, "O3P353", "objc_cxx_name");
    }
    if (!ParseInteropCallableBridgeNamedStringPayload(
            attribute_name, "objc_cxx_name", decl.objc_cxx_name)) {
      return CallableBridgeAttributeResult::kFailed;
    }
    decl.objc_cxx_name_declared = true;
    return CallableBridgeAttributeResult::kParsed;
  }

  template <typename TCallableDecl>
  CallableBridgeAttributeResult PublishObjcHeaderNameCallableBridgeAttribute(
      TCallableDecl &decl,
      const Token &attribute_name) {
    if (decl.objc_header_name_declared) {
      return RejectDuplicateInteropCallableBridgeAttribute(
          attribute_name, "O3P354", "objc_header_name");
    }
    if (!ParseInteropCallableBridgeNamedStringPayload(
            attribute_name, "objc_header_name", decl.objc_header_name)) {
      return CallableBridgeAttributeResult::kFailed;
    }
    decl.objc_header_name_declared = true;
    return CallableBridgeAttributeResult::kParsed;
  }
