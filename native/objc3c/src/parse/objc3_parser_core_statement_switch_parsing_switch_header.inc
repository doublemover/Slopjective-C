      const auto skip_malformed_switch_statement = [&]() {
        while (!At(TokenKind::LBrace) && !At(TokenKind::Semicolon) &&
               !At(TokenKind::RBrace) && !At(TokenKind::Eof)) {
          Advance();
        }
        if (!At(TokenKind::LBrace)) {
          return;
        }
        int brace_depth = 0;
        do {
          if (Match(TokenKind::LBrace)) {
            ++brace_depth;
          } else if (Match(TokenKind::RBrace)) {
            --brace_depth;
          } else {
            Advance();
          }
        } while (brace_depth > 0 && !At(TokenKind::Eof));
      };

      if (!Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P106", "missing '(' after switch"));
        skip_malformed_switch_statement();
        return nullptr;
      }
      stmt->switch_stmt->condition = ParseExpressionWithBlockLiteralSourceUse(
          BlockLiteralSourceUseKind::ExpressionSite);
      if (stmt->switch_stmt->condition == nullptr) {
        skip_malformed_switch_statement();
        return nullptr;
      }
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after switch condition"));
        skip_malformed_switch_statement();
        return nullptr;
      }
      if (!Match(TokenKind::LBrace)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P110", "missing '{' for switch body"));
        skip_malformed_switch_statement();
        return nullptr;
      }
