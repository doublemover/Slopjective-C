  std::unique_ptr<Objc3InterfaceDecl> ParseObjcInterfaceDeclHeader() {
    auto decl = std::make_unique<Objc3InterfaceDecl>();
    decl->line = Previous().line;
    decl->column = Previous().column;

    const Token &name_token = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(
          MakeDiag(name_token.line, name_token.column, "O3P101",
                   "invalid Objective-C interface identifier"));
      SynchronizeTopLevel();
      return nullptr;
    }
    decl->name = Previous().text;

    if (!ParseObjcInterfaceGenericParameterClause(*decl)) {
      SynchronizeObjcContainer();
    }

    return decl;
  }
