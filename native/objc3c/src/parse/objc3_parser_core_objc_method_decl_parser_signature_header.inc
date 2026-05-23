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

  bool ParseObjcMethodGenericParameterClause(Objc3MethodDecl &method) {
    if (!At(TokenKind::Less)) {
      return true;
    }

    FunctionDecl synthetic_fn;
    if (!ParseFunctionGenericParameterClause(synthetic_fn)) {
      return false;
    }
    method.generic_params = std::move(synthetic_fn.generic_params);
    return true;
  }

  bool ParseObjcMethodReturnHeader(Objc3MethodDecl &method) {
    if (!ParseObjcMethodGenericParameterClause(method)) {
      return false;
    }
    if (method.generic_callable_reification_marker_declared &&
        method.generic_params.empty()) {
      diagnostics_.push_back(MakeDiag(
          method.generic_callable_reification_marker_line,
          method.generic_callable_reification_marker_column, "O3P114",
          "declaration-scoped generic reification marker '@reify_generics' "
          "requires a generic Objective-C method declaration"));
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
