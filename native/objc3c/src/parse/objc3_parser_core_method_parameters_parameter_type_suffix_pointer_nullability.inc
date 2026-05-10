  bool ParseParameterPointerTypeSuffix(FuncParam &param) {
    if (!Match(TokenKind::Star)) {
      return false;
    }
    param.has_pointer_declarator = true;
    param.pointer_declarator_depth += 1;
    param.pointer_declarator_tokens.push_back(
        MakeSemaTokenMetadata(Objc3SemaTokenKind::PointerDeclarator, Previous()));
    return true;
  }

  bool ParseParameterNullabilityTypeSuffix(FuncParam &param) {
    if (!At(TokenKind::Question) && !At(TokenKind::Bang)) {
      return false;
    }
    param.nullability_suffix_tokens.push_back(
        MakeSemaTokenMetadata(Objc3SemaTokenKind::NullabilitySuffix, Advance()));
    return true;
  }
