  bool ParseObjcMethodMarker(Objc3MethodDecl &method) {
    if (Match(TokenKind::Minus)) {
      method.is_class_method = false;
    } else if (Match(TokenKind::Plus)) {
      method.is_class_method = true;
    } else {
      return false;
    }

    const Token method_marker = Previous();
    method.line = method_marker.line;
    method.column = method_marker.column;
    return true;
  }

  bool RejectReservedObjcMethodGenericClause() {
    if (!At(TokenKind::Less)) {
      return true;
    }

    const Token generic_clause = Advance();
    int depth = 1;
    while (depth > 0 && !At(TokenKind::Eof)) {
      if (Match(TokenKind::Less)) {
        ++depth;
        continue;
      }
      if (Match(TokenKind::Greater)) {
        --depth;
        continue;
      }
      (void)Advance();
    }

    diagnostics_.push_back(MakeDiag(
        generic_clause.line, generic_clause.column, "O3P114",
        "generic Objective-C method declarations ('- <T> ...' or '+ <T> ...') "
        "are reserved for a future Objective-C 3 revision; selector identity "
        "does not admit method type parameters yet"));
    return false;
  }

  bool ParseObjcMethodReturnHeader(Objc3MethodDecl &method) {
    if (!RejectReservedObjcMethodGenericClause()) {
      return false;
    }

    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P106", "missing '(' after Objective-C method marker"));
      return false;
    }

    FunctionDecl synthetic_fn;
    if (!ParseFunctionReturnType(synthetic_fn)) {
      return false;
    }
    CopyMethodReturnTypeFromFunctionDecl(synthetic_fn, method);

    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P109", "missing ')' after Objective-C method return type"));
      return false;
    }

    return true;
  }
