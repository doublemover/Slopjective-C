  template <typename TCallableDecl>
  bool ParseDispatchIntentCallableAttribute(
      TCallableDecl &decl,
      const Token &attribute_name) {
    if (attribute_name.text == "objc_direct") {
      if (decl.objc_direct_declared) {
        return ReportDuplicateDispatchIntentCallableAttribute(
            attribute_name, "O3P330", "duplicate objc_direct attribute");
      }
      decl.objc_direct_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_final") {
      if (decl.objc_final_declared) {
        return ReportDuplicateDispatchIntentCallableAttribute(
            attribute_name, "O3P331", "duplicate objc_final attribute");
      }
      decl.objc_final_declared = true;
      return true;
    }
    if (attribute_name.text == "objc_dynamic") {
      if (decl.objc_dynamic_declared) {
        return ReportDuplicateDispatchIntentCallableAttribute(
            attribute_name, "O3P332", "duplicate objc_dynamic attribute");
      }
      decl.objc_dynamic_declared = true;
      return true;
    }
    return false;
  }
