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

      break;
    }
    return expr;
  }
