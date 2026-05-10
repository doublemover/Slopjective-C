  template <typename TCallableDecl>
  CallableBridgeAttributeResult PublishObjcExportHeaderCallableBridgeAttribute(
      TCallableDecl &decl,
      const Token &attribute_name) {
    if (decl.objc_export_header_declared) {
      return RejectDuplicateInteropCallableBridgeAttribute(
          attribute_name, "O3P356", "objc_export_header");
    }
    if (!ParseInteropCallableBridgeNamedStringPayload(
            attribute_name, "objc_export_header",
            decl.objc_export_header_name)) {
      return CallableBridgeAttributeResult::kFailed;
    }
    decl.objc_export_header_declared = true;
    return CallableBridgeAttributeResult::kParsed;
  }
