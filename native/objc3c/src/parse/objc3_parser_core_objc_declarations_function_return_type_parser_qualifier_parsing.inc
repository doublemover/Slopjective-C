  bool ParseFunctionReturnTypeQualifiers(FunctionDecl &fn) {
    if (!RejectUnexpectedFunctionReturnTypeQualifier()) {
      return false;
    }

    while (At(TokenKind::Identifier) &&
           IsOwnershipQualifierSpelling(Peek().text)) {
      const Token qualifier = Advance();
      fn.has_return_ownership_qualifier = true;
      fn.return_ownership_qualifier_spelling = qualifier.text;
      fn.return_ownership_qualifier_tokens.push_back(
          MakeSemaTokenMetadata(
              Objc3SemaTokenKind::OwnershipQualifier, qualifier));
    }
    if (At(TokenKind::Identifier) &&
        IsBorrowedQualifierSpelling(Peek().text)) {
      Advance();
      fn.return_borrowed_pointer_qualified = true;
    }

    return true;
  }
