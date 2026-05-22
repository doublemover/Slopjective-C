  struct StringInterpolationTokenParts {
    std::vector<std::string> segments;
    std::vector<std::string> payload_sources;
  };

  bool TryDecodeStringInterpolationToken(
      const Token &string_token,
      StringInterpolationTokenParts &parts) {
    parts.segments.clear();
    parts.payload_sources.clear();
    const std::string &token_text = string_token.text;
    if (token_text.size() < 2u || token_text.front() != '"' ||
        token_text.back() != '"') {
      diagnostics_.push_back(MakeDiag(
          string_token.line, string_token.column, "O3P104",
          "invalid canonical string literal token"));
      return false;
    }

    std::string segment;
    for (std::size_t index = 1u; index + 1u < token_text.size();) {
      const char current = token_text[index];
      if (current != '\\') {
        segment.push_back(current);
        ++index;
        continue;
      }
      if (index + 2u >= token_text.size()) {
        diagnostics_.push_back(MakeDiag(
            string_token.line, string_token.column, "O3L010",
            "invalid string escape at end of literal"));
        return false;
      }
      const char escaped = token_text[index + 1u];
      if (escaped == '(') {
        parts.segments.push_back(segment);
        segment.clear();
        index += 2u;
        const std::size_t payload_begin = index;
        int depth = 1;
        while (index + 1u < token_text.size()) {
          const char payload_current = token_text[index];
          if (payload_current == '(') {
            ++depth;
          } else if (payload_current == ')') {
            --depth;
            if (depth == 0) {
              break;
            }
          }
          ++index;
        }
        if (index + 1u >= token_text.size() || depth != 0) {
          diagnostics_.push_back(MakeDiag(
              string_token.line, string_token.column, "O3L011",
              "unterminated string interpolation; expected ')'"));
          return false;
        }
        const std::string payload =
            token_text.substr(payload_begin, index - payload_begin);
        if (payload.find("\\(") != std::string::npos) {
          diagnostics_.push_back(MakeDiag(
              string_token.line, string_token.column, "O3L011",
              "nested string interpolation is unsupported"));
          return false;
        }
        parts.payload_sources.push_back(payload);
        ++index;
        continue;
      }
      switch (escaped) {
        case 'n':
          segment.push_back('\n');
          break;
        case 'r':
          segment.push_back('\r');
          break;
        case 't':
          segment.push_back('\t');
          break;
        case '"':
          segment.push_back('"');
          break;
        case '\\':
          segment.push_back('\\');
          break;
        default:
          diagnostics_.push_back(MakeDiag(
              string_token.line, string_token.column, "O3L010",
              std::string("invalid string escape '\\") + escaped + "'"));
          return false;
      }
      index += 2u;
    }
    parts.segments.push_back(segment);
    for (const std::string &decoded_segment : parts.segments) {
      int unused_unit_count = 0;
      if (!TryCountObjc3Utf8Scalars(decoded_segment, unused_unit_count)) {
        diagnostics_.push_back(MakeDiag(
            string_token.line, string_token.column, "O3L012",
            "malformed UTF-8 in string interpolation segment"));
        return false;
      }
    }
    return parts.segments.size() == parts.payload_sources.size() + 1u;
  }

  std::unique_ptr<Expr> ParseStringInterpolationPayload(
      const std::string &payload_source,
      const Token &string_token) {
    Objc3Lexer lexer(payload_source);
    std::vector<std::string> payload_diagnostics;
    const std::vector<Token> payload_tokens = lexer.Run(payload_diagnostics);
    for (const std::string &diagnostic : payload_diagnostics) {
      diagnostics_.push_back(diagnostic);
    }
    if (!payload_diagnostics.empty()) {
      return nullptr;
    }
    Objc3Parser payload_parser(payload_tokens);
    auto payload = payload_parser.ParseExpression();
    for (const std::string &diagnostic : payload_parser.TakeDiagnostics()) {
      diagnostics_.push_back(diagnostic);
    }
    if (payload == nullptr) {
      diagnostics_.push_back(MakeDiag(
          string_token.line, string_token.column, "O3P103",
          "invalid string interpolation payload expression"));
      return nullptr;
    }
    if (!payload_parser.At(TokenKind::Eof)) {
      const Token &token = payload_parser.Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P104",
          "string interpolation payload must contain one expression"));
      return nullptr;
    }
    return payload;
  }

  std::unique_ptr<Expr> ParsePrimaryLiteralOrIdentifier(bool &matched_primary) {
    matched_primary = false;
    if (Match(TokenKind::Number)) {
      matched_primary = true;
      int value = 0;
      const Token &number_token = Previous();
      if (!ParseIntegerLiteralValue(number_token.text, value)) {
        diagnostics_.push_back(MakeDiag(
            number_token.line, number_token.column, "O3P103",
            "invalid numeric literal '" + number_token.text + "'"));
        return nullptr;
      }
      return BuildObjc3NumericLiteralExpr(value, number_token);
    }
    if (Match(TokenKind::KwTrue) || Match(TokenKind::KwFalse)) {
      matched_primary = true;
      const Token &bool_token = Previous();
      return BuildObjc3BoolLiteralExpr(
          bool_token.kind == TokenKind::KwTrue, bool_token);
    }
    if (Match(TokenKind::KwNil)) {
      matched_primary = true;
      return BuildObjc3NilLiteralExpr(Previous());
    }
    if (Match(TokenKind::String)) {
      matched_primary = true;
      const Token &string_token = Previous();
      if (string_token.text.find("\\(") != std::string::npos) {
        StringInterpolationTokenParts parts;
        if (!TryDecodeStringInterpolationToken(string_token, parts)) {
          return nullptr;
        }
        std::vector<std::unique_ptr<Expr>> payloads;
        payloads.reserve(parts.payload_sources.size());
        for (const std::string &payload_source : parts.payload_sources) {
          auto payload =
              ParseStringInterpolationPayload(payload_source, string_token);
          if (payload == nullptr) {
            return nullptr;
          }
          payloads.push_back(std::move(payload));
        }
        return BuildObjc3StringInterpolationExpr(
            std::move(parts.segments), std::move(payloads), string_token);
      }
      std::string value;
      int unit_count = 0;
      if (!TryDecodeObjc3StringTokenText(string_token.text, value) ||
          !TryCountObjc3Utf8Scalars(value, unit_count)) {
        diagnostics_.push_back(MakeDiag(
            string_token.line, string_token.column, "O3P104",
            "invalid canonical string literal token"));
        return nullptr;
      }
      return BuildObjc3StringLiteralExpr(
          value, static_cast<int>(value.size()), unit_count, string_token);
    }
    if (Match(TokenKind::Caret)) {
      matched_primary = true;
      return ParseBlockLiteralExpression();
    }
    if (Match(TokenKind::KwAtKeypath)) {
      matched_primary = true;
      const Token keypath_token = Previous();
      return ParseKeyPathLiteralExpression(keypath_token);
    }
    if (Match(TokenKind::Identifier)) {
      matched_primary = true;
      const Token &identifier = Previous();
      return BuildObjc3IdentifierExpr(identifier.text, identifier);
    }
    return nullptr;
  }
