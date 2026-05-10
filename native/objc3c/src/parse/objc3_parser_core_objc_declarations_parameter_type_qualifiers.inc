    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) ||
        At(TokenKind::KwAsync)) {
      const Token qualifier = Advance();
      const std::string message =
          qualifier.kind == TokenKind::KwPure
              ? "unexpected qualifier 'pure' in parameter type annotation"
          : qualifier.kind == TokenKind::KwExtern
              ? "unexpected qualifier 'extern' in parameter type annotation"
              : "unexpected qualifier 'async' in parameter type annotation";
      diagnostics_.push_back(
          MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      return false;
    }

    while (At(TokenKind::Identifier) &&
           IsOwnershipQualifierSpelling(Peek().text)) {
      const Token qualifier = Advance();
      param.has_ownership_qualifier = true;
      param.ownership_qualifier_spelling = qualifier.text;
      param.ownership_qualifier_tokens.push_back(
          MakeSemaTokenMetadata(
              Objc3SemaTokenKind::OwnershipQualifier, qualifier));
    }
    if (At(TokenKind::Identifier) && IsBorrowedQualifierSpelling(Peek().text)) {
      Advance();
      param.borrowed_pointer_qualified = true;
    }
