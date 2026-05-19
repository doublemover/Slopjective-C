  bool ParseParameterGenericTypeSuffix(
      FuncParam &param,
      bool &parsed_generic_suffix,
      bool &suffix_failed) {
    if (!At(TokenKind::Less) || parsed_generic_suffix) {
      return false;
    }

    Match(TokenKind::Less);
    const Token &open = Previous();
    param.has_generic_suffix = true;
    param.generic_suffix_terminated = false;
    param.generic_line = open.line;
    param.generic_column = open.column;
    param.generic_suffix_text = "<";
    int depth = 1;
    while (depth > 0 && !At(TokenKind::Eof)) {
      if (Match(TokenKind::Less)) {
        param.generic_suffix_text += "<";
        ++depth;
        continue;
      }
      if (Match(TokenKind::Greater)) {
        param.generic_suffix_text += ">";
        --depth;
        if (depth == 0) {
          param.generic_suffix_terminated = true;
        }
        continue;
      }
      if (Match(TokenKind::GreaterGreater)) {
        if (depth >= 2) {
          param.generic_suffix_text += ">>";
          depth -= 2;
          if (depth == 0) {
            param.generic_suffix_terminated = true;
          }
          continue;
        }
        param.generic_suffix_text += ">>";
        depth = -1;
        break;
      }
      param.generic_suffix_text += Advance().text;
    }
    if (!param.generic_suffix_terminated) {
      diagnostics_.push_back(MakeDiag(open.line, open.column, "O3P108",
                                      "unterminated generic parameter type suffix"));
      suffix_failed = true;
      return true;
    }
    parsed_generic_suffix = true;
    return true;
  }
