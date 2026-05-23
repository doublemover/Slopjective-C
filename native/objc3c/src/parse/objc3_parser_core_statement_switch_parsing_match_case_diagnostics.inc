          if (At(TokenKind::Equal) && index_ + 1 < tokens_.size() &&
              tokens_[index_ + 1].kind == TokenKind::Greater) {
            const Token token = Peek();
            diagnostics_.push_back(
                MakeDiag(token.line, token.column, "O3P156", "reserved expression-form 'match' arm"));
            return nullptr;
          }
          if (At(TokenKind::Identifier) && Peek().text == "where") {
            const Token token = Advance();
            if (At(TokenKind::Colon) || At(TokenKind::LBrace) ||
                At(TokenKind::RBrace) || At(TokenKind::Eof)) {
              diagnostics_.push_back(
                  MakeDiag(token.line, token.column, "O3P157",
                           "guarded match pattern requires a condition after 'where'"));
              return nullptr;
            }
            case_stmt.has_match_guard = true;
            case_stmt.match_guard_line = token.line;
            case_stmt.match_guard_column = token.column;
            case_stmt.match_guard_condition = ParseExpression();
            if (case_stmt.match_guard_condition == nullptr) {
              diagnostics_.push_back(
                  MakeDiag(token.line, token.column, "O3P157",
                           "invalid guarded match pattern condition"));
              return nullptr;
            }
            if (At(TokenKind::Equal) && index_ + 1 < tokens_.size() &&
                tokens_[index_ + 1].kind == TokenKind::Greater) {
              const Token arrow_token = Peek();
              diagnostics_.push_back(
                  MakeDiag(arrow_token.line, arrow_token.column, "O3P156",
                           "reserved expression-form 'match' arm"));
              return nullptr;
            }
          }
