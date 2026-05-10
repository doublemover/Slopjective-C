  bool ParseFunctionReturnGenericSuffix(
      FunctionDecl &fn,
      bool parsed_generic_suffix,
      bool &suffix_failed) {
    suffix_failed = false;
    if (!At(TokenKind::Less) || parsed_generic_suffix) {
      return false;
    }

    Match(TokenKind::Less);
    const Token &open = Previous();
    fn.has_return_generic_suffix = true;
    fn.return_generic_suffix_terminated = false;
    fn.return_generic_line = open.line;
    fn.return_generic_column = open.column;
    fn.return_generic_suffix_text = "<";
    int depth = 1;
    while (depth > 0 && !At(TokenKind::Eof)) {
      if (Match(TokenKind::Less)) {
        fn.return_generic_suffix_text += "<";
        ++depth;
        continue;
      }
      if (Match(TokenKind::Greater)) {
        fn.return_generic_suffix_text += ">";
        --depth;
        if (depth == 0) {
          fn.return_generic_suffix_terminated = true;
        }
        continue;
      }
      if (Match(TokenKind::GreaterGreater)) {
        if (depth >= 2) {
          fn.return_generic_suffix_text += ">>";
          depth -= 2;
          if (depth == 0) {
            fn.return_generic_suffix_terminated = true;
          }
          continue;
        }
        fn.return_generic_suffix_text += ">>";
        depth = -1;
        break;
      }
      fn.return_generic_suffix_text += Advance().text;
    }
    if (!fn.return_generic_suffix_terminated) {
      diagnostics_.push_back(
          MakeDiag(fn.return_generic_line, fn.return_generic_column, "O3P114",
                   "unterminated generic function return type suffix"));
      suffix_failed = true;
    }
    return true;
  }
