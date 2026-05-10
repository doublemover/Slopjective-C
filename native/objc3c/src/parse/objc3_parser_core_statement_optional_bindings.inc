  struct ParsedOptionalBindingClause {
    std::string name;
    bool is_mutable = false;
    std::unique_ptr<Expr> value;
    unsigned line = 1;
    unsigned column = 1;
  };

  struct ParsedGuardConditionClause {
    enum class Kind {
      OptionalBinding,
      BooleanExpr,
    };

    Kind kind = Kind::BooleanExpr;
    ParsedOptionalBindingClause binding;
    std::unique_ptr<Expr> condition;
    unsigned line = 1;
    unsigned column = 1;
  };

  ParsedOptionalBindingClause ParseOptionalBindingClauseWithMutability(
      bool binding_is_mutable) {
    ParsedOptionalBindingClause clause;
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P103", "expected identifier in optional binding"));
      return clause;
    }
    const Token name_token = Advance();
    if (!Match(TokenKind::Equal)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P103", "missing '=' in optional binding"));
      return clause;
    }
    clause.name = name_token.text;
    clause.is_mutable = binding_is_mutable;
    clause.line = name_token.line;
    clause.column = name_token.column;
    clause.value = ParseExpressionWithBlockLiteralSourceUse(
        BlockLiteralSourceUseKind::ExpressionSite);
    return clause;
  }

  ParsedOptionalBindingClause ParseOptionalBindingClause() {
    bool binding_is_mutable = false;
    if (Match(TokenKind::KwLet)) {
      binding_is_mutable = false;
    } else if (Match(TokenKind::KwVar)) {
      binding_is_mutable = true;
    } else {
      ParsedOptionalBindingClause clause;
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P103", "expected 'let' or 'var' in optional binding"));
      return clause;
    }
    return ParseOptionalBindingClauseWithMutability(binding_is_mutable);
  }

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

  std::vector<ParsedGuardConditionClause> ParseGuardConditionClauses() {
    std::vector<ParsedGuardConditionClause> clauses;
    while (true) {
      ParsedGuardConditionClause clause;
      clause.line = Peek().line;
      clause.column = Peek().column;
      if (At(TokenKind::KwLet) || At(TokenKind::KwVar)) {
        clause.kind = ParsedGuardConditionClause::Kind::OptionalBinding;
        clause.binding = ParseOptionalBindingClause();
        if (clause.binding.value == nullptr || clause.binding.name.empty()) {
          return {};
        }
      } else {
        clause.kind = ParsedGuardConditionClause::Kind::BooleanExpr;
        clause.condition = ParseExpressionWithBlockLiteralSourceUse(
            BlockLiteralSourceUseKind::ExpressionSite);
        if (clause.condition == nullptr) {
          return {};
        }
      }
      clauses.push_back(std::move(clause));
      if (!Match(TokenKind::Comma)) {
        break;
      }
    }
    return clauses;
  }
