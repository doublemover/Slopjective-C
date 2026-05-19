  template <typename TCallableDecl>
  CallableBridgeAttributeResult ParseCoreCallableBridgeAttribute(
      TCallableDecl &decl,
      const Token &attribute_name) {
    if (attribute_name.text == "objc_nserror") {
      if (decl.objc_nserror_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P282",
            "duplicate objc_nserror attribute"));
        return CallableBridgeAttributeResult::kFailed;
      }
      decl.objc_nserror_declared = true;
      return CallableBridgeAttributeResult::kParsed;
    }
    if (attribute_name.text == "objc_status_code") {
      if (decl.objc_status_code_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P283",
            "duplicate objc_status_code attribute"));
        return CallableBridgeAttributeResult::kFailed;
      }
      return ParseStatusCodeBridgeAttributePayload(decl, attribute_name)
                 ? CallableBridgeAttributeResult::kParsed
                 : CallableBridgeAttributeResult::kFailed;
    }
    if (attribute_name.text == "objc_executor") {
      if (decl.executor_affinity_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P294",
            "duplicate objc_executor attribute"));
        return CallableBridgeAttributeResult::kFailed;
      }
      return ParseExecutorAttributePayload(decl, attribute_name)
                 ? CallableBridgeAttributeResult::kParsed
                 : CallableBridgeAttributeResult::kFailed;
    }
    if (attribute_name.text == "objc_nonisolated") {
      if (decl.objc_nonisolated_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P295",
            "duplicate objc_nonisolated attribute"));
        return CallableBridgeAttributeResult::kFailed;
      }
      decl.objc_nonisolated_declared = true;
      return CallableBridgeAttributeResult::kParsed;
    }
    if (attribute_name.text == "objc_returns_borrowed") {
      if (decl.objc_returns_borrowed_declared) {
        diagnostics_.push_back(MakeDiag(
            attribute_name.line, attribute_name.column, "O3P312",
            "duplicate objc_returns_borrowed attribute"));
        return CallableBridgeAttributeResult::kFailed;
      }
      return ParseReturnsBorrowedAttributePayload(decl)
                 ? CallableBridgeAttributeResult::kParsed
                 : CallableBridgeAttributeResult::kFailed;
    }
    return CallableBridgeAttributeResult::kNotMatched;
  }
