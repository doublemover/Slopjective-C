  void ParseObjcImplementationDeclMembers(Objc3ImplementationDecl &decl) {
    while (!At(TokenKind::KwAtEnd) && !At(TokenKind::Eof)) {
      if (Match(TokenKind::KwAtReifyGenerics)) {
        RejectReservedReificationControl(Previous(),
                                         "an Objective-C implementation member");
        SynchronizeObjcContainer();
        continue;
      }
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
