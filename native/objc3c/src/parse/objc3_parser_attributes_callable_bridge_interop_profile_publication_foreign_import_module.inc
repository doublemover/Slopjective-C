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
