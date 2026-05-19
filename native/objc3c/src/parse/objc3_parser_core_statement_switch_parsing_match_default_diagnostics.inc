          if (At(TokenKind::Equal) && index_ + 1 < tokens_.size() &&
              tokens_[index_ + 1].kind == TokenKind::Greater) {
            const Token token = Peek();
            diagnostics_.push_back(
                MakeDiag(token.line, token.column, "O3P156", "reserved expression-form 'match' arm"));
            return nullptr;
          }
