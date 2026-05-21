  bool RejectInvalidFunctionGenericParameterIdentifier(const Token &token) {
    diagnostics_.push_back(MakeDiag(
        token.line, token.column, "O3P101",
        "invalid function generic parameter identifier"));
    return false;
  }

  bool RejectInvalidFunctionGenericConstraintType(const Token &token) {
    diagnostics_.push_back(MakeDiag(
        token.line, token.column, "O3P101",
        "invalid function generic constraint type"));
    return false;
  }

  bool RejectMissingFunctionGenericParameterListCloser(const Token &token) {
    diagnostics_.push_back(MakeDiag(
        token.line, token.column, "O3P112",
        "missing '>' after function generic parameter list"));
    return false;
  }

  bool ParseFunctionGenericParameterName(Objc3GenericParamDecl &param) {
    const Token &name_token = Peek();
    if (!Match(TokenKind::Identifier)) {
      return RejectInvalidFunctionGenericParameterIdentifier(name_token);
    }
    param.name = Previous().text;
    param.line = Previous().line;
    param.column = Previous().column;
    return true;
  }

  bool ParseFunctionGenericConstraint(
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
      return RejectInvalidFunctionGenericConstraintType(Peek());
    }
    if (At(TokenKind::Less)) {
      unsigned split_closing_greater_tokens = 0;
      param.has_constraint_generic_suffix =
          ParseRawGenericSuffixText(param.constraint_generic_suffix_text,
                                    param.constraint_generic_suffix_terminated,
                                    param.constraint_line,
                                    param.constraint_column, "O3P112",
                                    "unterminated function generic constraint suffix",
                                    &split_closing_greater_tokens);
      if (!param.has_constraint_generic_suffix) {
        return false;
      }
      clause_closed_by_constraint_suffix = split_closing_greater_tokens > 0u;
    }
    return true;
  }

  bool ParseFunctionGenericParameterClause(FunctionDecl &fn) {
    if (!Match(TokenKind::Less)) {
      return true;
    }

    bool clause_closed_by_constraint_suffix = false;
    while (!At(TokenKind::Greater) && !At(TokenKind::Eof) &&
           !clause_closed_by_constraint_suffix) {
      Objc3GenericParamDecl param;
      ParseObjcInterfaceGenericParameterVariance(param);
      if (!ParseFunctionGenericParameterName(param)) {
        return false;
      }
      if (!ParseFunctionGenericConstraint(
              param, clause_closed_by_constraint_suffix)) {
        return false;
      }
      fn.generic_params.push_back(std::move(param));
      if (clause_closed_by_constraint_suffix) {
        break;
      }
      if (Match(TokenKind::Comma)) {
        continue;
      }
      break;
    }

    if (clause_closed_by_constraint_suffix) {
      return true;
    }
    if (!Match(TokenKind::Greater)) {
      return RejectMissingFunctionGenericParameterListCloser(Peek());
    }
    return true;
  }

