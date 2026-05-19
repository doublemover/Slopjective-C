  bool ParseFunctionReturnAnnotation(
      FunctionDecl &fn,
      bool &has_return_annotation) {
    if (!Match(TokenKind::Minus)) {
      return true;
    }

    const Token arrow_start = Previous();
    if (!Match(TokenKind::Greater)) {
      diagnostics_.push_back(MakeDiag(
          arrow_start.line,
          arrow_start.column,
          "O3P114",
          "missing '>' in function return annotation"));
      SynchronizeFunctionTail();
      return false;
    }

    if (!ParseFunctionReturnType(fn)) {
      SynchronizeFunctionTail();
      return false;
    }
    has_return_annotation = true;
    return true;
  }
