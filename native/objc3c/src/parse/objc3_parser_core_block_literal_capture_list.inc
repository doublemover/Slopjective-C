  bool ParseBlockLiteralExplicitCaptureList(Expr &block) {
    if (!Match(TokenKind::LBracket)) {
      return true;
    }

    block.block_has_explicit_capture_list = true;
    while (!At(TokenKind::Eof) && !At(TokenKind::RBracket)) {
      Expr::ExplicitBlockCaptureItem item;
      item.mode = "plain";
      if (At(TokenKind::Identifier) &&
          (Peek().text == "weak" || Peek().text == "unowned" ||
           Peek().text == "move" || Peek().text == "byref" ||
           Peek().text == "__block")) {
        item.mode = Advance().text;
        if (item.mode == "__block") {
          item.mode = "byref";
        }
      }
      if (!At(TokenKind::Identifier)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P313",
            "expected capture identifier in explicit block capture list"));
        return false;
      }
      item.name = Advance().text;
      block.block_explicit_capture_items_source_order.push_back(std::move(item));
      if (!Match(TokenKind::Comma)) {
        break;
      }
    }
    if (!Match(TokenKind::RBracket)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P314",
          "missing ']' after explicit block capture list"));
      return false;
    }
    return true;
  }
