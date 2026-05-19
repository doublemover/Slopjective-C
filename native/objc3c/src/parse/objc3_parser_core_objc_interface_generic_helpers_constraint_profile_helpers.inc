  void ParseObjcInterfaceGenericParameterVariance(
      Objc3GenericParamDecl &param) {
    if (At(TokenKind::Identifier) && IsGenericVarianceMarker(Peek().text)) {
      param.variance_spelling = Advance().text;
    }
  }

  bool ParseObjcInterfaceGenericParameterName(Objc3GenericParamDecl &param) {
    const Token &name_token = Peek();
    if (!Match(TokenKind::Identifier)) {
      return RejectInvalidObjcInterfaceGenericParameterIdentifier(name_token);
    }
    param.name = Previous().text;
    param.line = Previous().line;
    param.column = Previous().column;
    return true;
  }

  bool ParseObjcInterfaceGenericConstraint(
      Objc3GenericParamDecl &param,
      bool &clause_closed_by_constraint_suffix) {
    if (!Match(TokenKind::Colon)) {
      return true;
    }

    param.has_constraint = true;
    param.constraint_line = Previous().line;
    param.constraint_column = Previous().column;
    if (Match(TokenKind::KwId) || Match(TokenKind::KwClass) ||
        Match(TokenKind::KwInstancetype) || Match(TokenKind::Identifier)) {
      param.constraint_type_name = Previous().text;
    } else {
      const Token &constraint_token = Peek();
      return RejectInvalidObjcInterfaceGenericConstraintType(constraint_token);
    }
    if (At(TokenKind::Less)) {
      unsigned split_closing_greater_tokens = 0;
      param.has_constraint_generic_suffix =
          ParseRawGenericSuffixText(param.constraint_generic_suffix_text,
                                    param.constraint_generic_suffix_terminated,
                                    param.constraint_line,
                                    param.constraint_column, "O3P112",
                                    "unterminated Objective-C interface generic constraint suffix",
                                    &split_closing_greater_tokens);
      if (!param.has_constraint_generic_suffix) {
        return false;
      }
      clause_closed_by_constraint_suffix = split_closing_greater_tokens > 0u;
    }
    return true;
  }
