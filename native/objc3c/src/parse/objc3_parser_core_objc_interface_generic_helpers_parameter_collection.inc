  bool ParseObjcInterfaceGenericParameterClause(Objc3InterfaceDecl &decl) {
    if (!LooksLikeObjcInterfaceGenericParameterClause()) {
      return true;
    }
    Match(TokenKind::Less);
    bool clause_closed_by_constraint_suffix = false;
    while (!At(TokenKind::Greater) && !At(TokenKind::Eof) &&
           !clause_closed_by_constraint_suffix) {
      Objc3GenericParamDecl param;
      ParseObjcInterfaceGenericParameterVariance(param);

      if (!ParseObjcInterfaceGenericParameterName(param)) {
        return false;
      }

      if (!ParseObjcInterfaceGenericConstraint(
              param, clause_closed_by_constraint_suffix)) {
        return false;
      }

      PublishObjcInterfaceGenericParameter(decl, std::move(param));
      if (clause_closed_by_constraint_suffix) {
        break;
      }
      if (Match(TokenKind::Comma)) {
        continue;
      }
      break;
    }

    return FinishObjcInterfaceGenericParameterClause(
        clause_closed_by_constraint_suffix);
  }
