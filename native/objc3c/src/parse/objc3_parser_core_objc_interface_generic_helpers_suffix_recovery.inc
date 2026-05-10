  bool ParseRawGenericSuffixText(std::string &suffix_text,
                                 bool &suffix_terminated,
                                 unsigned &line,
                                 unsigned &column,
                                 const std::string &diagnostic_code,
                                 const std::string &diagnostic_message,
                                 unsigned *split_closing_greater_tokens = nullptr) {
    if (!Match(TokenKind::Less)) {
      return false;
    }
    const Token &open = Previous();
    line = open.line;
    column = open.column;
    suffix_text = "<";
    suffix_terminated = false;
    int depth = 1;
    while (depth > 0 && !At(TokenKind::Eof)) {
      if (Match(TokenKind::Less)) {
        suffix_text += "<";
        ++depth;
        continue;
      }
      if (Match(TokenKind::Greater)) {
        suffix_text += ">";
        --depth;
        if (depth == 0) {
          suffix_terminated = true;
        }
        continue;
      }
      if (Match(TokenKind::GreaterGreater)) {
        if (depth >= 2) {
          suffix_text += ">>";
          depth -= 2;
          if (depth == 0) {
            suffix_terminated = true;
          }
          continue;
        }
        if (split_closing_greater_tokens != nullptr) {
          suffix_text += ">";
          --depth;
          *split_closing_greater_tokens += 1;
          suffix_terminated = true;
          continue;
        }
        suffix_text += ">>";
        depth = -1;
        break;
      }
      suffix_text += Advance().text;
    }
    if (!suffix_terminated) {
      return RejectUnterminatedObjcInterfaceGenericSuffix(
          open, diagnostic_code, diagnostic_message);
    }
    return true;
  }
