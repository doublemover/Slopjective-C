  std::vector<ParsedOptionalBindingClause> ParseOptionalBindingClauses() {
    std::vector<ParsedOptionalBindingClause> clauses;
    bool binding_is_mutable = false;
    if (Match(TokenKind::KwLet)) {
      binding_is_mutable = false;
    } else if (Match(TokenKind::KwVar)) {
      binding_is_mutable = true;
    } else {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P103", "expected 'let' or 'var' in optional binding"));
      return {};
    }
    while (true) {
      ParsedOptionalBindingClause clause =
          ParseOptionalBindingClauseWithMutability(binding_is_mutable);
      if (clause.value == nullptr || clause.name.empty()) {
        return {};
      }
      clauses.push_back(std::move(clause));
      if (!Match(TokenKind::Comma)) {
        break;
      }
      if (At(TokenKind::KwLet)) {
        Advance();
        binding_is_mutable = false;
      } else if (At(TokenKind::KwVar)) {
        Advance();
        binding_is_mutable = true;
      }
    }
    return clauses;
  }
