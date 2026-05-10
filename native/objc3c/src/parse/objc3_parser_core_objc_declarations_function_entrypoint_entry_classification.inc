  bool AtFunctionDeclarationQualifier() const {
    return At(TokenKind::KwPure) || At(TokenKind::KwExtern) ||
           At(TokenKind::KwAsync);
  }

  bool AtFunctionTerminatorRecoveryLead() const {
    return At(TokenKind::KwModule) || At(TokenKind::KwLet) ||
           At(TokenKind::KwFn) || At(TokenKind::KwPure) ||
           At(TokenKind::KwExtern) || At(TokenKind::KwAsync) ||
           At(TokenKind::KwAtInterface) ||
           At(TokenKind::KwAtImplementation) ||
           At(TokenKind::KwAtProtocol) || At(TokenKind::KwAtProperty) ||
           At(TokenKind::Eof);
  }
