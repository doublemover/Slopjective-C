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

  bool ParseTypedThrowsGenericSuffix(Objc3TypedThrowsPayload &payload) {
    if (!Match(TokenKind::Less)) {
      return true;
    }
    const Token &open = Previous();
    payload.has_generic_suffix = true;
    payload.generic_suffix_terminated = false;
    payload.generic_suffix_text = "<";
    int depth = 1;
    while (depth > 0 && !At(TokenKind::Eof)) {
      if (Match(TokenKind::Less)) {
        payload.generic_suffix_text += "<";
        ++depth;
        continue;
      }
      if (Match(TokenKind::Greater)) {
        payload.generic_suffix_text += ">";
        --depth;
        if (depth == 0) {
          payload.generic_suffix_terminated = true;
        }
        continue;
      }
      if (Match(TokenKind::GreaterGreater)) {
        if (depth >= 2) {
          payload.generic_suffix_text += ">>";
          depth -= 2;
          if (depth == 0) {
            payload.generic_suffix_terminated = true;
          }
          continue;
        }
        payload.generic_suffix_text += ">>";
        depth = -1;
        break;
      }
      payload.generic_suffix_text += Advance().text;
    }
    if (!payload.generic_suffix_terminated) {
      diagnostics_.push_back(BuildObjc3ReservedTypedThrowsDiagnostic(
          open, "malformed-unclosed-payload"));
      return false;
    }
    return true;
  }

  std::string BuildTypedThrowsPayloadSpelling(
      const Objc3TypedThrowsPayload &payload) const {
    std::string spelling;
    if (payload.id_spelling) {
      spelling = "id";
    } else if (payload.class_spelling) {
      spelling = "Class";
    } else if (payload.instancetype_spelling) {
      spelling = "instancetype";
    } else {
      spelling = payload.object_pointer_type_name;
    }
    spelling += payload.generic_suffix_text;
    for (unsigned i = 0; i < payload.pointer_declarator_depth; ++i) {
      spelling += "*";
    }
    return spelling;
  }

  bool ParseTypedThrowsPayloadTypeHead(Objc3TypedThrowsPayload &payload) {
    if (Match(TokenKind::KwId)) {
      const Token &type_token = Previous();
      payload.type = ValueType::ObjCId;
      payload.id_spelling = true;
      payload.line = type_token.line;
      payload.column = type_token.column;
      return true;
    }
    if (Match(TokenKind::KwClass)) {
      const Token &type_token = Previous();
      payload.type = ValueType::ObjCClass;
      payload.class_spelling = true;
      payload.line = type_token.line;
      payload.column = type_token.column;
      return true;
    }
    if (Match(TokenKind::KwInstancetype)) {
      const Token &type_token = Previous();
      payload.type = ValueType::ObjCInstancetype;
      payload.instancetype_spelling = true;
      payload.line = type_token.line;
      payload.column = type_token.column;
      return true;
    }
    if (At(TokenKind::Identifier)) {
      const Token type_token = Advance();
      payload.type = ValueType::ObjCObjectPtr;
      payload.object_pointer_type_spelling = true;
      payload.object_pointer_type_name = type_token.text;
      payload.line = type_token.line;
      payload.column = type_token.column;
      return true;
    }
    diagnostics_.push_back(BuildObjc3ReservedTypedThrowsDiagnostic(
        Peek(), "malformed-payload-type"));
    return false;
  }

  bool ParseTypedThrowsPayload(Objc3TypedThrowsPayload &payload,
                               const Token &throws_token) {
    const std::string payload_shape = ClassifyReservedTypedThrowsPayloadShape();
    if (payload_shape != "single-payload") {
      diagnostics_.push_back(
          BuildObjc3ReservedTypedThrowsDiagnostic(throws_token, payload_shape));
      return false;
    }

    if (!Match(TokenKind::LParen)) {
      diagnostics_.push_back(BuildObjc3ReservedTypedThrowsDiagnostic(
          throws_token, "absent-payload"));
      return false;
    }
    if (!ParseTypedThrowsPayloadTypeHead(payload)) {
      return false;
    }
    if (!ParseTypedThrowsGenericSuffix(payload)) {
      return false;
    }
    while (Match(TokenKind::Star)) {
      payload.has_pointer_declarator = true;
      ++payload.pointer_declarator_depth;
    }
    if (!Match(TokenKind::RParen)) {
      diagnostics_.push_back(BuildObjc3ReservedTypedThrowsDiagnostic(
          throws_token, "malformed-payload-type"));
      return false;
    }
    payload.declared = true;
    payload.canonical_spelling = BuildTypedThrowsPayloadSpelling(payload);
    return true;
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
      Objc3TypedThrowsPayload payload;
      if (!ParseTypedThrowsPayload(payload, throws_token)) {
        return false;
      }
      fn.throws_declared = true;
      fn.typed_throws_declared = true;
      fn.typed_throws_payload = payload;
      return true;
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
      Objc3TypedThrowsPayload payload;
      if (!ParseTypedThrowsPayload(payload, throws_token)) {
        return false;
      }
      method.throws_declared = true;
      method.typed_throws_declared = true;
      method.typed_throws_payload = payload;
      return true;
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
