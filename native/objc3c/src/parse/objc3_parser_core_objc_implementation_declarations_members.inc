  void ParseObjcImplementationDeclMembers(Objc3ImplementationDecl &decl) {
    while (!At(TokenKind::KwAtEnd) && !At(TokenKind::Eof)) {
      if (At(TokenKind::KwAtProperty)) {
        ParseObjcImplementationDeclPropertyMember(decl);
        continue;
      }
      if (!(At(TokenKind::Minus) || At(TokenKind::Plus))) {
        DiagnoseUnsupportedObjcImplementationDeclMember();
        SynchronizeObjcContainer();
        continue;
      }

      ParseObjcImplementationDeclMethodMember(decl);
    }
  }
