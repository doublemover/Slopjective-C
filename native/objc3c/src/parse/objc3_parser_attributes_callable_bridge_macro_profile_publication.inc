  template <typename TCallableDecl>
  CallableBridgeAttributeResult PublishObjcMacroCallableBridgeAttribute(
      TCallableDecl &decl,
      const Token &attribute_name) {
    if (decl.objc_macro_declared) {
      return RejectDuplicateMacroCallableBridgeAttribute(
          attribute_name, "O3P346", "objc_macro");
    }
    if (!ParseMacroCallableBridgeNamedStringPayload(
            attribute_name, "objc_macro", decl.objc_macro_name)) {
      return CallableBridgeAttributeResult::kFailed;
    }
    decl.objc_macro_declared = true;
    return CallableBridgeAttributeResult::kParsed;
  }

  template <typename TCallableDecl>
  CallableBridgeAttributeResult PublishObjcMacroPackageCallableBridgeAttribute(
      TCallableDecl &decl,
      const Token &attribute_name) {
    if (decl.objc_macro_package_declared) {
      return RejectDuplicateMacroCallableBridgeAttribute(
          attribute_name, "O3P347", "objc_macro_package");
    }
    if (!ParseMacroCallableBridgeNamedStringPayload(
            attribute_name, "objc_macro_package",
            decl.objc_macro_package_name)) {
      return CallableBridgeAttributeResult::kFailed;
    }
    decl.objc_macro_package_declared = true;
    return CallableBridgeAttributeResult::kParsed;
  }

  template <typename TCallableDecl>
  CallableBridgeAttributeResult
  PublishObjcMacroProvenanceCallableBridgeAttribute(
      TCallableDecl &decl,
      const Token &attribute_name) {
    if (decl.objc_macro_provenance_declared) {
      return RejectDuplicateMacroCallableBridgeAttribute(
          attribute_name, "O3P348", "objc_macro_provenance");
    }
    if (!ParseMacroCallableBridgeNamedStringPayload(
            attribute_name, "objc_macro_provenance",
            decl.objc_macro_provenance_name)) {
      return CallableBridgeAttributeResult::kFailed;
    }
    decl.objc_macro_provenance_declared = true;
    return CallableBridgeAttributeResult::kParsed;
  }

  template <typename TCallableDecl>
  CallableBridgeAttributeResult PublishObjcMacroCacheKeyCallableBridgeAttribute(
      TCallableDecl &decl,
      const Token &attribute_name) {
    if (decl.objc_macro_cache_key_declared) {
      return RejectDuplicateMacroCallableBridgeAttribute(
          attribute_name, "O3P367", "objc_macro_cache_key");
    }
    if (!ParseMacroCallableBridgeNamedStringPayload(
            attribute_name, "objc_macro_cache_key",
            decl.objc_macro_cache_key_name)) {
      return CallableBridgeAttributeResult::kFailed;
    }
    decl.objc_macro_cache_key_declared = true;
    return CallableBridgeAttributeResult::kParsed;
  }

  template <typename TCallableDecl>
  CallableBridgeAttributeResult PublishObjcMacroSandboxCallableBridgeAttribute(
      TCallableDecl &decl,
      const Token &attribute_name) {
    if (decl.objc_macro_sandbox_declared) {
      return RejectDuplicateMacroCallableBridgeAttribute(
          attribute_name, "O3P368", "objc_macro_sandbox");
    }
    if (!ParseMacroCallableBridgeNamedStringPayload(
            attribute_name, "objc_macro_sandbox",
            decl.objc_macro_sandbox_name)) {
      return CallableBridgeAttributeResult::kFailed;
    }
    decl.objc_macro_sandbox_declared = true;
    return CallableBridgeAttributeResult::kParsed;
  }
