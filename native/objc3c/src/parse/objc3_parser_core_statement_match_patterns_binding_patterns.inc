  bool ParseBindingMatchPattern(SwitchCase &case_stmt, bool &handled) {
    handled = false;
    if (!(Match(TokenKind::KwLet) || Match(TokenKind::KwVar))) {
      return true;
    }

    handled = true;
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
