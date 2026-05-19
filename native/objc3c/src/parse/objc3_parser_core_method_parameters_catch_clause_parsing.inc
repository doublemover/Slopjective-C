  bool ParseCatchClause(BlockStmt::CatchClause &clause) {
    const Token catch_token = Previous();
    clause.line = catch_token.line;
    clause.column = catch_token.column;
    if (Match(TokenKind::LParen)) {
      std::vector<std::string> pattern_tokens;
      while (!At(TokenKind::RParen) && !At(TokenKind::Eof)) {
        pattern_tokens.push_back(Advance().text);
      }
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109",
                                        "missing ')' after catch pattern"));
        return false;
      }
      if (pattern_tokens.empty()) {
        diagnostics_.push_back(MakeDiag(catch_token.line, catch_token.column,
                                        "O3P103",
                                        "empty catch pattern is not allowed"));
        return false;
      }
      clause.catch_all = false;
      clause.has_binding = true;
      clause.binding_name = pattern_tokens.back();
      if (pattern_tokens.size() > 1u) {
        std::ostringstream type_out;
        for (std::size_t index = 0; index + 1u < pattern_tokens.size(); ++index) {
          if (index != 0u) {
            type_out << ' ';
          }
          type_out << pattern_tokens[index];
        }
        clause.binding_type_spelling = type_out.str();
      }
    }
    clause.body = ParseControlBody();
    if (block_failed_) {
      block_failed_ = false;
      return false;
    }
    return true;
  }
