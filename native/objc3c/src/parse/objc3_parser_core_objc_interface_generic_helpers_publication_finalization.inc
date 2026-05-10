  void PublishObjcInterfaceGenericParameter(Objc3InterfaceDecl &decl,
                                            Objc3GenericParamDecl &&param) {
    decl.generic_params.push_back(std::move(param));
  }

  bool FinishObjcInterfaceGenericParameterClause(
      bool clause_closed_by_constraint_suffix) {
    if (!clause_closed_by_constraint_suffix && !Match(TokenKind::Greater)) {
      const Token &token = Peek();
      return RejectMissingObjcInterfaceGenericParameterListCloser(token);
    }
    return true;
  }
