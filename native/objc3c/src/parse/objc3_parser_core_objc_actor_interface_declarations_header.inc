  std::unique_ptr<Objc3InterfaceDecl> ParseObjcActorInterfaceDeclHeader(
      const Token &actor_token) {
    // actor-member source-closure anchor: actor interfaces now ride
    // the same interface parsing path while publishing one dedicated frontend
    // semantic packet for member/isolation annotations.
    auto decl = std::make_unique<Objc3InterfaceDecl>();
    decl->line = actor_token.line;
    decl->column = actor_token.column;
    decl->is_actor = true;

    const Token &name_token = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(
          MakeDiag(name_token.line, name_token.column, "O3P101",
                   "invalid Objective-C actor identifier"));
      SynchronizeTopLevel();
      return nullptr;
    }
    decl->name = Previous().text;
    return decl;
  }
