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
