  CallableBridgeAttributeResult RejectDuplicateMacroCallableBridgeAttribute(
      const Token &attribute_name,
      const char *diagnostic_code,
      const char *attribute_spelling) {
    diagnostics_.push_back(MakeDiag(
        attribute_name.line, attribute_name.column, diagnostic_code,
        std::string("duplicate ") + attribute_spelling + " attribute"));
    return CallableBridgeAttributeResult::kFailed;
  }
