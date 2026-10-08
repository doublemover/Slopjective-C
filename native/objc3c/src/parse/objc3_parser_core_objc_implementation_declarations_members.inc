  void ParseObjcImplementationDeclMembers(Objc3ImplementationDecl &decl) {
    while (!At(TokenKind::KwAtEnd) && !At(TokenKind::Eof)) {
      if (Match(TokenKind::KwAtReifyGenerics)) {
        const Token marker = Previous();
        if (!(At(TokenKind::Minus) || At(TokenKind::Plus))) {
          diagnostics_.push_back(MakeDiag(
              marker.line, marker.column, "O3P114",
              "declaration-scoped generic reification marker "
              "'@reify_generics' is only supported on Objective-C method "
              "members"));
          SynchronizeObjcContainer();
          continue;
        }
        ParseObjcImplementationDeclMethodMember(decl, true, marker.line,
                                                marker.column);
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
