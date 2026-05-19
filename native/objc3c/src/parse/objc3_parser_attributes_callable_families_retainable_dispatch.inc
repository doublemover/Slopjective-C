  template <typename TCallableDecl>
  bool ParseRetainableCFamilyCallableAttribute(
      TCallableDecl &decl,
      const Token &attribute_name) {
    if (IsRetainableCFamilyPayloadCallableAttribute(attribute_name)) {
      return ParseRetainableCFamilyCallablePayload(decl, attribute_name);
    }
    if (IsRetainableCFamilyFlagCallableAttribute(attribute_name)) {
      RecordRetainableCFamilyCallableFlagAttribute(decl, attribute_name);
      return true;
    }
    return false;
  }
