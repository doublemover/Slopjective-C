  BlockLiteralSourceUseKind CurrentBlockLiteralSourceUseKind() const {
    if (block_literal_source_use_stack_.empty()) {
      return BlockLiteralSourceUseKind::ExpressionSite;
    }
    return block_literal_source_use_stack_.back();
  }

  std::unique_ptr<Expr> ParseExpressionWithBlockLiteralSourceUse(
      BlockLiteralSourceUseKind kind) {
    ScopedBlockLiteralSourceUse scope(block_literal_source_use_stack_, kind);
    return ParseConditional();
  }

  std::unique_ptr<Expr> ParseExpression() { return ParseConditional(); }

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

  bool ParseMatchPattern(SwitchCase &case_stmt) {
    case_stmt.match_pattern_enabled = true;
    if (At(TokenKind::Identifier) && Peek().text == "_") {
      const Token wildcard = Advance();
      case_stmt.match_pattern_kind = MatchPatternKind::Wildcard;
      case_stmt.value_line = wildcard.line;
      case_stmt.value_column = wildcard.column;
      return true;
    }

    if (Match(TokenKind::Number)) {
      case_stmt.match_pattern_kind = MatchPatternKind::LiteralInteger;
      case_stmt.value_line = Previous().line;
      case_stmt.value_column = Previous().column;
      case_stmt.value = std::atoi(Previous().text.c_str());
      return true;
    }
    if (Match(TokenKind::KwTrue) || Match(TokenKind::KwFalse)) {
      case_stmt.match_pattern_kind = MatchPatternKind::LiteralBool;
      case_stmt.value_line = Previous().line;
      case_stmt.value_column = Previous().column;
      case_stmt.value = Previous().kind == TokenKind::KwTrue ? 1 : 0;
      return true;
    }
    if (Match(TokenKind::KwNil)) {
      case_stmt.match_pattern_kind = MatchPatternKind::LiteralNil;
      case_stmt.value_line = Previous().line;
      case_stmt.value_column = Previous().column;
      case_stmt.value = 0;
      return true;
    }
    if (Match(TokenKind::KwLet) || Match(TokenKind::KwVar)) {
      const Token binding_token = Previous();
      const bool is_mutable = binding_token.kind == TokenKind::KwVar;
      if (!At(TokenKind::Identifier)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P103", "expected identifier in match binding pattern"));
        return false;
      }
      const Token name_token = Advance();
      case_stmt.match_pattern_kind = MatchPatternKind::Binding;
      case_stmt.match_binding_mutable = is_mutable;
      case_stmt.match_binding_name = name_token.text;
      case_stmt.value_line = name_token.line;
      case_stmt.value_column = name_token.column;
      return true;
    }
    if (At(TokenKind::Identifier) && Peek().text == "is") {
      const Token token = Advance();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P158", "unsupported type-test match pattern"));
      return false;
    }
    if (Match(TokenKind::Dot)) {
      const Token &result_case = Peek();
      if (!Match(TokenKind::Identifier)) {
        diagnostics_.push_back(
            MakeDiag(result_case.line, result_case.column, "O3P103", "invalid result-case match pattern"));
        return false;
      }
      if (Previous().text != "Ok" && Previous().text != "Err") {
        diagnostics_.push_back(
            MakeDiag(Previous().line, Previous().column, "O3P103", "invalid result-case match pattern"));
        return false;
      }
      const Token result_name = Previous();
      if (!Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P106", "missing '(' after result-case pattern"));
        return false;
      }
      if (!(Match(TokenKind::KwLet) || Match(TokenKind::KwVar))) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P103", "expected 'let' or 'var' in result-case pattern"));
        return false;
      }
      const bool is_mutable = Previous().kind == TokenKind::KwVar;
      if (!At(TokenKind::Identifier)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P103", "expected identifier in result-case pattern"));
        return false;
      }
      const Token name_token = Advance();
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P109", "missing ')' after result-case pattern"));
        return false;
      }
      case_stmt.match_pattern_kind = MatchPatternKind::ResultCase;
      case_stmt.match_result_case_name = result_name.text;
      case_stmt.match_binding_mutable = is_mutable;
      case_stmt.match_binding_name = name_token.text;
      case_stmt.value_line = result_name.line;
      case_stmt.value_column = result_name.column;
      return true;
    }

    const Token &token = Peek();
    diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P103", "invalid match pattern"));
    return false;
  }

