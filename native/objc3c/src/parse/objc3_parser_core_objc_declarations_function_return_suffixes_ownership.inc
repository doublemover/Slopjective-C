  bool ParseFunctionReturnOwnershipQualifierSuffix(FunctionDecl &fn) {
    if (!(At(TokenKind::Identifier) && IsOwnershipQualifierSpelling(Peek().text))) {
      return false;
    }
    const Token qualifier = Advance();
    fn.has_return_ownership_qualifier = true;
    fn.return_ownership_qualifier_spelling = qualifier.text;
    fn.return_ownership_qualifier_tokens.push_back(
        MakeSemaTokenMetadata(Objc3SemaTokenKind::OwnershipQualifier, qualifier));
    return true;
  }
