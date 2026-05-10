      if (!Match(TokenKind::KwElse)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P100", "missing 'else' after guard condition list"));
        return nullptr;
      }
      stmt->if_stmt->else_body = ParseControlBody();
      if (block_failed_) {
        block_failed_ = false;
        return nullptr;
      }
