  template <typename TCallableDecl>
  CallableBridgeAttributeResult ParseMacroCallableBridgeAttribute(
      TCallableDecl &decl,
      const Token &attribute_name) {
    if (IsObjcMacroCallableBridgeAttribute(attribute_name)) {
      return PublishObjcMacroCallableBridgeAttribute(decl, attribute_name);
    }
    if (IsObjcMacroPackageCallableBridgeAttribute(attribute_name)) {
      return PublishObjcMacroPackageCallableBridgeAttribute(
          decl, attribute_name);
    }
    if (IsObjcMacroProvenanceCallableBridgeAttribute(attribute_name)) {
      return PublishObjcMacroProvenanceCallableBridgeAttribute(
          decl, attribute_name);
    }
    if (IsObjcMacroCacheKeyCallableBridgeAttribute(attribute_name)) {
      return PublishObjcMacroCacheKeyCallableBridgeAttribute(
          decl, attribute_name);
    }
    if (IsObjcMacroSandboxCallableBridgeAttribute(attribute_name)) {
      return PublishObjcMacroSandboxCallableBridgeAttribute(
          decl, attribute_name);
    }
    return CallableBridgeAttributeResult::kNotMatched;
  }
