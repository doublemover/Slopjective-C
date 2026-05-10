  template <typename TCallableDecl>
  CallableBridgeAttributeResult PublishObjcForeignCallableBridgeAttribute(
      TCallableDecl &decl,
      const Token &attribute_name) {
    if (decl.objc_foreign_declared) {
      return RejectDuplicateInteropCallableBridgeAttribute(
          attribute_name, "O3P349", "objc_foreign");
    }
    decl.objc_foreign_declared = true;
    return CallableBridgeAttributeResult::kParsed;
  }

  template <typename TCallableDecl>
  CallableBridgeAttributeResult PublishObjcImportModuleCallableBridgeAttribute(
      TCallableDecl &decl,
      const Token &attribute_name) {
    if (decl.objc_import_module_declared) {
      return RejectDuplicateInteropCallableBridgeAttribute(
          attribute_name, "O3P350", "objc_import_module");
    }
    if (!ParseInteropCallableBridgeNamedStringPayload(
            attribute_name, "objc_import_module",
            decl.objc_import_module_name)) {
      return CallableBridgeAttributeResult::kFailed;
    }
    decl.objc_import_module_declared = true;
    return CallableBridgeAttributeResult::kParsed;
  }

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
