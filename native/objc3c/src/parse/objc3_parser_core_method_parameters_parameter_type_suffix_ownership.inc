  bool ParseParameterOwnershipQualifierTypeSuffix(FuncParam &param) {
    if (!At(TokenKind::Identifier) || !IsOwnershipQualifierSpelling(Peek().text)) {
      return false;
    }
    const Token qualifier = Advance();
    param.has_ownership_qualifier = true;
    param.ownership_qualifier_spelling = qualifier.text;
    param.ownership_qualifier_tokens.push_back(
        MakeSemaTokenMetadata(Objc3SemaTokenKind::OwnershipQualifier, qualifier));
    return true;
  }
