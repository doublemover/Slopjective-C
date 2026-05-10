  bool ParseResultCaseMatchPattern(SwitchCase &case_stmt, bool &handled) {
    handled = false;
    if (!Match(TokenKind::Dot)) {
      return true;
    }

    handled = true;
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
