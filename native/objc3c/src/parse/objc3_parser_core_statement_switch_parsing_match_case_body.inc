          if (!Match(TokenKind::Colon)) {
            const Token &token = Peek();
            diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P107", "missing ':' after match pattern"));
            return nullptr;
          }
          if (!At(TokenKind::LBrace)) {
            const Token &token = Peek();
            diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P110", "match case requires braced body"));
            return nullptr;
          }
          case_stmt.body = ParseBlock();
          if (block_failed_) {
            block_failed_ = false;
            return nullptr;
          }
