  bool IsObjcForeignCallableBridgeAttribute(
      const Token &attribute_name) const {
    return attribute_name.text == "objc_foreign";
  }

  bool IsObjcImportModuleCallableBridgeAttribute(
      const Token &attribute_name) const {
    return attribute_name.text == "objc_import_module";
  }

  bool IsObjcSwiftNameCallableBridgeAttribute(
      const Token &attribute_name) const {
    return attribute_name.text == "objc_swift_name";
  }

  bool IsObjcSwiftPrivateCallableBridgeAttribute(
      const Token &attribute_name) const {
    return attribute_name.text == "objc_swift_private";
  }

  bool IsObjcCxxNameCallableBridgeAttribute(
      const Token &attribute_name) const {
    return attribute_name.text == "objc_cxx_name";
  }

  bool IsObjcHeaderNameCallableBridgeAttribute(
      const Token &attribute_name) const {
    return attribute_name.text == "objc_header_name";
  }

  bool IsObjcExportHeaderCallableBridgeAttribute(
      const Token &attribute_name) const {
    return attribute_name.text == "objc_export_header";
  }
