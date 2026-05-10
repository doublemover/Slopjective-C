  template <typename TCallableDecl>
  CallableBridgeAttributeResult PublishObjcSwiftNameCallableBridgeAttribute(
      TCallableDecl &decl,
      const Token &attribute_name) {
    if (decl.objc_swift_name_declared) {
      return RejectDuplicateInteropCallableBridgeAttribute(
          attribute_name, "O3P351", "objc_swift_name");
    }
    if (!ParseInteropCallableBridgeNamedStringPayload(
            attribute_name, "objc_swift_name", decl.objc_swift_name)) {
      return CallableBridgeAttributeResult::kFailed;
    }
    decl.objc_swift_name_declared = true;
    return CallableBridgeAttributeResult::kParsed;
  }

  template <typename TCallableDecl>
  CallableBridgeAttributeResult PublishObjcSwiftPrivateCallableBridgeAttribute(
      TCallableDecl &decl,
      const Token &attribute_name) {
    if (decl.objc_swift_private_declared) {
      return RejectDuplicateInteropCallableBridgeAttribute(
          attribute_name, "O3P352", "objc_swift_private");
    }
    decl.objc_swift_private_declared = true;
    return CallableBridgeAttributeResult::kParsed;
  }
