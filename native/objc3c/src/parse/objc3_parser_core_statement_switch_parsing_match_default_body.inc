          if (!Match(TokenKind::Colon)) {
            const Token &token = Peek();
            diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P107", "missing ':' after default label"));
            return nullptr;
          }
          if (!At(TokenKind::LBrace)) {
            const Token &token = Peek();
            diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P110", "match default requires braced body"));
            return nullptr;
          }
          default_stmt.body = ParseBlock();
          if (block_failed_) {
            block_failed_ = false;
            return nullptr;
          }
