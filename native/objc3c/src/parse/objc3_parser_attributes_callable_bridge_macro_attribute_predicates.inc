  bool IsObjcMacroCallableBridgeAttribute(
      const Token &attribute_name) const {
    return attribute_name.text == "objc_macro";
  }

  bool IsObjcMacroPackageCallableBridgeAttribute(
      const Token &attribute_name) const {
    return attribute_name.text == "objc_macro_package";
  }

  bool IsObjcMacroProvenanceCallableBridgeAttribute(
      const Token &attribute_name) const {
    return attribute_name.text == "objc_macro_provenance";
  }

  bool IsObjcMacroCacheKeyCallableBridgeAttribute(
      const Token &attribute_name) const {
    return attribute_name.text == "objc_macro_cache_key";
  }

  bool IsObjcMacroSandboxCallableBridgeAttribute(
      const Token &attribute_name) const {
    return attribute_name.text == "objc_macro_sandbox";
  }
