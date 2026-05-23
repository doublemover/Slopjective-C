  std::unique_ptr<Expr> ParsePostfix() {
    auto expr = ParsePrimary();
    // Part 3 source-closure anchor: postfix parsing now admits calls,
    // message-send receivers, and optional-member access lowering sugar while
    // broader typed key-path execution still remains deferred.
    while (expr != nullptr) {
      if (Match(TokenKind::LParen)) {
        expr = ParsePostfixCallExpression(std::move(expr));
        if (expr == nullptr) {
          return nullptr;
        }
        continue;
      }

      if (Match(TokenKind::QuestionDot)) {
        expr = ParsePostfixOptionalMemberAccess(
            std::move(expr), Previous());
        if (expr == nullptr) {
          return nullptr;
        }
        continue;
      }

      if (Match(TokenKind::LBracket)) {
        const Token open = Previous();
        auto index = ParseExpression();
        if (index == nullptr) {
          return nullptr;
        }
        if (!Match(TokenKind::RBracket)) {
          const Token &token = Peek();
          diagnostics_.push_back(MakeDiag(
              token.line, token.column, "O3P215",
              "missing ']' after collection index expression"));
          return nullptr;
        }
        expr = BuildObjc3IndexAccessExpr(open, std::move(expr),
                                         std::move(index));
        continue;
      }

      break;
    }
    return expr;
  }
