  std::string ClassifyReservedTypedThrowsPayloadShape() const {
    if (!At(TokenKind::LParen)) {
      return "absent-payload";
    }

    std::size_t cursor = index_ + 1u;
    int paren_depth = 1;
    int angle_depth = 0;
    bool saw_payload_token = false;
    bool saw_top_level_comma = false;
    while (cursor < tokens_.size()) {
      const Token &token = tokens_[cursor];
      if (token.kind == TokenKind::Eof) {
        return "malformed-unclosed-payload";
      }
      if (token.kind == TokenKind::LParen) {
        ++paren_depth;
        saw_payload_token = true;
        ++cursor;
        continue;
      }
      if (token.kind == TokenKind::RParen) {
        --paren_depth;
        if (paren_depth == 0) {
          break;
        }
        saw_payload_token = true;
        ++cursor;
        continue;
      }

      if (paren_depth == 1) {
        if (token.kind == TokenKind::Less) {
          ++angle_depth;
          saw_payload_token = true;
          ++cursor;
          continue;
        }
        if (token.kind == TokenKind::Greater) {
          if (angle_depth > 0) {
            --angle_depth;
          }
          saw_payload_token = true;
          ++cursor;
          continue;
        }
        if (token.kind == TokenKind::GreaterGreater) {
          if (angle_depth >= 2) {
            angle_depth -= 2;
          } else {
            angle_depth = 0;
          }
          saw_payload_token = true;
          ++cursor;
          continue;
        }
        if (token.kind == TokenKind::Comma && angle_depth == 0) {
          saw_top_level_comma = true;
        }
      }
      saw_payload_token = true;
      ++cursor;
    }

    if (cursor >= tokens_.size() || tokens_[cursor].kind != TokenKind::RParen) {
      return "malformed-unclosed-payload";
    }
    if (!saw_payload_token) {
      return "empty-payload";
    }
    if (saw_top_level_comma) {
      return "multi-payload";
    }
    return "single-payload";
  }

  bool ParseOptionalThrowsClause(FunctionDecl &fn) {
    if (!AtThrowsClauseKeyword()) {
      return true;
    }
    const Token throws_token = Advance();
    if (fn.throws_declared) {
      diagnostics_.push_back(MakeDiag(throws_token.line, throws_token.column, "O3P181",
                                      "duplicate 'throws' declaration modifier"));
      return false;
    }
    if (At(TokenKind::LParen)) {
      diagnostics_.push_back(
          BuildObjc3ReservedTypedThrowsDiagnostic(
              throws_token, ClassifyReservedTypedThrowsPayloadShape()));
      return false;
    }
    fn.throws_declared = true;
    return true;
  }

  bool ParseOptionalThrowsClause(Objc3MethodDecl &method) {
    if (!AtThrowsClauseKeyword()) {
      return true;
    }
    const Token throws_token = Advance();
    if (method.throws_declared) {
      diagnostics_.push_back(MakeDiag(throws_token.line, throws_token.column, "O3P181",
                                      "duplicate 'throws' declaration modifier"));
      return false;
    }
    if (At(TokenKind::LParen)) {
      diagnostics_.push_back(
          BuildObjc3ReservedTypedThrowsDiagnostic(
              throws_token, ClassifyReservedTypedThrowsPayloadShape()));
      return false;
    }
    method.throws_declared = true;
    return true;
  }

  bool ParseOptionalAsyncClause(FunctionDecl &fn) {
    if (!AtAsyncClauseKeyword()) {
      return true;
    }
    const Token async_token = Advance();
    if (fn.async_declared) {
      diagnostics_.push_back(MakeDiag(async_token.line, async_token.column, "O3P181",
                                      "duplicate 'async' declaration modifier"));
      return false;
    }
    fn.async_declared = true;
    return true;
  }

  bool ParseOptionalAsyncClause(Objc3MethodDecl &method) {
    if (!AtAsyncClauseKeyword()) {
      return true;
    }
    const Token async_token = Advance();
    if (method.async_declared) {
      diagnostics_.push_back(MakeDiag(async_token.line, async_token.column, "O3P181",
                                      "duplicate 'async' declaration modifier"));
      return false;
    }
    method.async_declared = true;
    return true;
  }
