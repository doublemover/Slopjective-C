  Expr::MatchExpressionPatternKind MatchExpressionPatternKindForCase(
      const SwitchCase &case_stmt) const {
    if (case_stmt.is_default) {
      return Expr::MatchExpressionPatternKind::Wildcard;
    }
    switch (case_stmt.match_pattern_kind) {
      case MatchPatternKind::LiteralInteger:
        return Expr::MatchExpressionPatternKind::LiteralInteger;
      case MatchPatternKind::LiteralBool:
        return Expr::MatchExpressionPatternKind::LiteralBool;
      case MatchPatternKind::LiteralNil:
        return Expr::MatchExpressionPatternKind::LiteralNil;
      case MatchPatternKind::Wildcard:
        return Expr::MatchExpressionPatternKind::Wildcard;
      case MatchPatternKind::Binding:
        return Expr::MatchExpressionPatternKind::Binding;
      case MatchPatternKind::ResultCase:
        return Expr::MatchExpressionPatternKind::ResultCase;
      case MatchPatternKind::None:
        return Expr::MatchExpressionPatternKind::None;
    }
    return Expr::MatchExpressionPatternKind::None;
  }

  Expr::MatchExpressionArm BuildMatchExpressionArmFromCase(
      const SwitchCase &case_stmt) const {
    Expr::MatchExpressionArm arm;
    arm.is_default = case_stmt.is_default;
    arm.pattern_kind = MatchExpressionPatternKindForCase(case_stmt);
    arm.literal_value = case_stmt.value;
    arm.binding_mutable = case_stmt.match_binding_mutable;
    arm.binding_name = case_stmt.match_binding_name;
    arm.result_case_name = case_stmt.match_result_case_name;
    arm.has_guard = case_stmt.has_match_guard;
    arm.line = case_stmt.line;
    arm.column = case_stmt.column;
    arm.pattern_line = case_stmt.value_line;
    arm.pattern_column = case_stmt.value_column;
    arm.guard_line = case_stmt.match_guard_line;
    arm.guard_column = case_stmt.match_guard_column;
    return arm;
  }

  bool ConsumeMatchExpressionFatArrow(const char *context) {
    if (Match(TokenKind::Equal)) {
      if (Match(TokenKind::Greater)) {
        return true;
      }
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P107",
          std::string("missing '>' after '=' in match expression ") +
              context));
      return false;
    }
    const Token &token = Peek();
    diagnostics_.push_back(MakeDiag(
        token.line, token.column, "O3P107",
        std::string("missing '=>' after match expression ") + context));
    return false;
  }

  bool ParseMatchExpressionGuardIfPresent(Expr::MatchExpressionArm &arm) {
    if (!(At(TokenKind::Identifier) && Peek().text == "where")) {
      return true;
    }
    const Token where_token = Advance();
    if (At(TokenKind::Equal) || At(TokenKind::Semicolon) ||
        At(TokenKind::RBrace) || At(TokenKind::Eof)) {
      diagnostics_.push_back(MakeDiag(
          where_token.line, where_token.column,
          kObjc3ParserDiagnosticReservedGuardedMatchPatternCode,
          "match expression guarded pattern requires a condition after 'where'"));
      return false;
    }
    arm.has_guard = true;
    arm.guard_line = where_token.line;
    arm.guard_column = where_token.column;
    arm.guard_condition = ParseExpression();
    if (arm.guard_condition == nullptr) {
      diagnostics_.push_back(MakeDiag(
          where_token.line, where_token.column,
          kObjc3ParserDiagnosticReservedGuardedMatchPatternCode,
          "invalid match expression guard condition"));
      return false;
    }
    return true;
  }

  bool ParseMatchExpressionCaseArm(
      std::vector<Expr::MatchExpressionArm> &arms) {
    SwitchCase case_stmt;
    case_stmt.line = Previous().line;
    case_stmt.column = Previous().column;
    case_stmt.is_default = false;
    if (!ParseMatchPattern(case_stmt)) {
      return false;
    }
    Expr::MatchExpressionArm arm =
        BuildMatchExpressionArmFromCase(case_stmt);
    if (!ParseMatchExpressionGuardIfPresent(arm)) {
      return false;
    }
    if (!ConsumeMatchExpressionFatArrow("case arm")) {
      return false;
    }
    arm.value = ParseExpression();
    if (arm.value == nullptr) {
      return false;
    }
    if (!Match(TokenKind::Semicolon)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          BuildObjc3MissingSemicolonDiagnostic(token, "match expression arm"));
      return false;
    }
    arms.push_back(std::move(arm));
    return true;
  }

  bool ParseMatchExpressionDefaultArm(
      std::vector<Expr::MatchExpressionArm> &arms) {
    SwitchCase default_stmt;
    default_stmt.line = Previous().line;
    default_stmt.column = Previous().column;
    default_stmt.is_default = true;
    default_stmt.match_pattern_enabled = true;
    default_stmt.match_pattern_kind = MatchPatternKind::Wildcard;
    Expr::MatchExpressionArm arm =
        BuildMatchExpressionArmFromCase(default_stmt);
    if (!ConsumeMatchExpressionFatArrow("default arm")) {
      return false;
    }
    arm.value = ParseExpression();
    if (arm.value == nullptr) {
      return false;
    }
    if (!Match(TokenKind::Semicolon)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          BuildObjc3MissingSemicolonDiagnostic(token, "match expression arm"));
      return false;
    }
    arms.push_back(std::move(arm));
    return true;
  }

  std::unique_ptr<Expr> ParseMatchExpression() {
    const Token match_token = Advance();
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P106",
          "missing '(' after match expression"));
      return nullptr;
    }
    auto scrutinee = ParseExpressionWithBlockLiteralSourceUse(
        BlockLiteralSourceUseKind::ExpressionSite);
    if (scrutinee == nullptr) {
      return nullptr;
    }
    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P109",
          "missing ')' after match expression scrutinee"));
      return nullptr;
    }
    if (!Match(TokenKind::LBrace)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P110",
          "missing '{' for match expression body"));
      return nullptr;
    }

    std::vector<Expr::MatchExpressionArm> arms;
    while (!At(TokenKind::RBrace) && !At(TokenKind::Eof)) {
      if (Match(TokenKind::KwCase)) {
        if (!ParseMatchExpressionCaseArm(arms)) {
          return nullptr;
        }
        continue;
      }
      if (Match(TokenKind::KwDefault)) {
        if (!ParseMatchExpressionDefaultArm(arms)) {
          return nullptr;
        }
        continue;
      }
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P103",
          "match expression body expects 'case' or 'default' arm"));
      return nullptr;
    }
    if (!Match(TokenKind::RBrace)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P110",
          "missing '}' after match expression body"));
      return nullptr;
    }
    if (arms.empty()) {
      diagnostics_.push_back(MakeDiag(
          match_token.line, match_token.column, "O3P103",
          "match expression requires at least one arm"));
      return nullptr;
    }
    return BuildObjc3MatchExpression(
        match_token, std::move(scrutinee), std::move(arms));
  }
