  template <typename TCallableDecl>
  CallableBridgeAttributeResult ParseInteropCallableBridgeAttribute(
      TCallableDecl &decl,
      const Token &attribute_name) {
    if (IsObjcForeignCallableBridgeAttribute(attribute_name)) {
      return PublishObjcForeignCallableBridgeAttribute(decl, attribute_name);
    }
    if (IsObjcImportModuleCallableBridgeAttribute(attribute_name)) {
      return PublishObjcImportModuleCallableBridgeAttribute(
          decl, attribute_name);
    }
    if (IsObjcSwiftNameCallableBridgeAttribute(attribute_name)) {
      return PublishObjcSwiftNameCallableBridgeAttribute(decl, attribute_name);
    }
    if (IsObjcSwiftPrivateCallableBridgeAttribute(attribute_name)) {
      return PublishObjcSwiftPrivateCallableBridgeAttribute(
          decl, attribute_name);
    }
    if (IsObjcCxxNameCallableBridgeAttribute(attribute_name)) {
      return PublishObjcCxxNameCallableBridgeAttribute(decl, attribute_name);
    }
    if (IsObjcHeaderNameCallableBridgeAttribute(attribute_name)) {
      return PublishObjcHeaderNameCallableBridgeAttribute(decl, attribute_name);
    }
    if (IsObjcExportHeaderCallableBridgeAttribute(attribute_name)) {
      return PublishObjcExportHeaderCallableBridgeAttribute(
          decl, attribute_name);
    }
    return CallableBridgeAttributeResult::kNotMatched;
  }
