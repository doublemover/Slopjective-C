  bool ParseFunctionReturnPointerDeclaratorSuffix(FunctionDecl &fn) {
    if (!Match(TokenKind::Star)) {
      return false;
    }
    fn.has_return_pointer_declarator = true;
    fn.return_pointer_declarator_depth += 1;
    fn.return_pointer_declarator_tokens.push_back(
        MakeSemaTokenMetadata(Objc3SemaTokenKind::PointerDeclarator, Previous()));
    return true;
  }

  bool ParseFunctionReturnNullabilitySuffix(FunctionDecl &fn) {
    if (!(At(TokenKind::Question) || At(TokenKind::Bang))) {
      return false;
    }
    fn.return_nullability_suffix_tokens.push_back(
        MakeSemaTokenMetadata(Objc3SemaTokenKind::NullabilitySuffix, Advance()));
    return true;
  }
