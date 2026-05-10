  bool IsGenericVarianceMarker(const std::string &text) const {
    return text == "__covariant" || text == "__contravariant" ||
           text == "__invariant";
  }

  bool LooksLikeObjcInterfaceGenericParameterClause() const {
    if (!At(TokenKind::Less)) {
      return false;
    }
    int depth = 0;
    for (std::size_t offset = 0; index_ + offset < tokens_.size(); ++offset) {
      const Token &token = tokens_[index_ + offset];
      if (token.kind == TokenKind::Less) {
        ++depth;
        continue;
      }
      if (token.kind == TokenKind::Greater) {
        --depth;
        if (depth == 0) {
          return false;
        }
        continue;
      }
      if (depth == 1 &&
          (token.kind == TokenKind::Colon ||
           (token.kind == TokenKind::Identifier &&
            IsGenericVarianceMarker(token.text)))) {
        return true;
      }
      if (token.kind == TokenKind::Eof) {
        return false;
      }
    }
    return false;
  }

  bool ParseRawGenericSuffixText(std::string &suffix_text,
                                 bool &suffix_terminated,
                                 unsigned &line,
                                 unsigned &column,
                                 const std::string &diagnostic_code,
                                 const std::string &diagnostic_message,
                                 unsigned *split_closing_greater_tokens = nullptr) {
    if (!Match(TokenKind::Less)) {
      return false;
    }
    const Token &open = Previous();
    line = open.line;
    column = open.column;
    suffix_text = "<";
    suffix_terminated = false;
    int depth = 1;
    while (depth > 0 && !At(TokenKind::Eof)) {
      if (Match(TokenKind::Less)) {
        suffix_text += "<";
        ++depth;
        continue;
      }
      if (Match(TokenKind::Greater)) {
        suffix_text += ">";
        --depth;
        if (depth == 0) {
          suffix_terminated = true;
        }
        continue;
      }
      if (Match(TokenKind::GreaterGreater)) {
        if (depth >= 2) {
          suffix_text += ">>";
          depth -= 2;
          if (depth == 0) {
            suffix_terminated = true;
          }
          continue;
        }
        if (split_closing_greater_tokens != nullptr) {
          suffix_text += ">";
          --depth;
          *split_closing_greater_tokens += 1;
          suffix_terminated = true;
          continue;
        }
        suffix_text += ">>";
        depth = -1;
        break;
      }
      suffix_text += Advance().text;
    }
    if (!suffix_terminated) {
      diagnostics_.push_back(
          MakeDiag(open.line, open.column, diagnostic_code, diagnostic_message));
      return false;
    }
    return true;
  }

  bool ParseObjcInterfaceGenericParameterClause(Objc3InterfaceDecl &decl) {
    if (!LooksLikeObjcInterfaceGenericParameterClause()) {
      return true;
    }
    Match(TokenKind::Less);
    bool clause_closed_by_constraint_suffix = false;
    while (!At(TokenKind::Greater) && !At(TokenKind::Eof) &&
           !clause_closed_by_constraint_suffix) {
      Objc3GenericParamDecl param;
      if (At(TokenKind::Identifier) &&
          IsGenericVarianceMarker(Peek().text)) {
        param.variance_spelling = Advance().text;
      }

      const Token &name_token = Peek();
      if (!Match(TokenKind::Identifier)) {
        diagnostics_.push_back(MakeDiag(
            name_token.line, name_token.column, "O3P101",
            "invalid Objective-C interface generic parameter identifier"));
        return false;
      }
      param.name = Previous().text;
      param.line = Previous().line;
      param.column = Previous().column;

      if (Match(TokenKind::Colon)) {
        param.has_constraint = true;
        param.constraint_line = Previous().line;
        param.constraint_column = Previous().column;
        if (Match(TokenKind::KwId) || Match(TokenKind::KwClass) ||
            Match(TokenKind::KwInstancetype) || Match(TokenKind::Identifier)) {
          param.constraint_type_name = Previous().text;
        } else {
          const Token &constraint_token = Peek();
          diagnostics_.push_back(MakeDiag(
              constraint_token.line, constraint_token.column, "O3P101",
              "invalid Objective-C interface generic constraint type"));
          return false;
        }
        if (At(TokenKind::Less)) {
          unsigned split_closing_greater_tokens = 0;
          param.has_constraint_generic_suffix =
              ParseRawGenericSuffixText(param.constraint_generic_suffix_text,
                                        param.constraint_generic_suffix_terminated,
                                        param.constraint_line,
                                        param.constraint_column, "O3P112",
                                        "unterminated Objective-C interface generic constraint suffix",
                                        &split_closing_greater_tokens);
          if (!param.has_constraint_generic_suffix) {
            return false;
          }
          clause_closed_by_constraint_suffix =
              split_closing_greater_tokens > 0u;
        }
      }

      decl.generic_params.push_back(std::move(param));
      if (clause_closed_by_constraint_suffix) {
        break;
      }
      if (Match(TokenKind::Comma)) {
        continue;
      }
      break;
    }

    if (!clause_closed_by_constraint_suffix && !Match(TokenKind::Greater)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P112",
          "missing '>' after Objective-C interface generic parameter list"));
      return false;
    }
    return true;
  }
