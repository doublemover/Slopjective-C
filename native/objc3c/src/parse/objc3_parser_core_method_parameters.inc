  void ParseParameterTypeSuffix(FuncParam &param) {
    bool parsed_generic_suffix = false;
    while (true) {
      if (At(TokenKind::Less) && !parsed_generic_suffix) {
        Match(TokenKind::Less);
        const Token &open = Previous();
        param.has_generic_suffix = true;
        param.generic_suffix_terminated = false;
        param.generic_line = open.line;
        param.generic_column = open.column;
        param.generic_suffix_text = "<";
        int depth = 1;
        while (depth > 0 && !At(TokenKind::Eof)) {
          if (Match(TokenKind::Less)) {
            param.generic_suffix_text += "<";
            ++depth;
            continue;
          }
          if (Match(TokenKind::Greater)) {
            param.generic_suffix_text += ">";
            --depth;
            if (depth == 0) {
              param.generic_suffix_terminated = true;
            }
            continue;
          }
          if (Match(TokenKind::GreaterGreater)) {
            if (depth >= 2) {
              param.generic_suffix_text += ">>";
              depth -= 2;
              if (depth == 0) {
                param.generic_suffix_terminated = true;
              }
              continue;
            }
            param.generic_suffix_text += ">>";
            depth = -1;
            break;
          }
          param.generic_suffix_text += Advance().text;
        }
        if (!param.generic_suffix_terminated) {
          diagnostics_.push_back(MakeDiag(open.line, open.column, "O3P108",
                                          "unterminated generic parameter type suffix"));
          return;
        }
        parsed_generic_suffix = true;
        continue;
      }

      if (Match(TokenKind::Star)) {
        param.has_pointer_declarator = true;
        param.pointer_declarator_depth += 1;
        param.pointer_declarator_tokens.push_back(
            MakeSemaTokenMetadata(Objc3SemaTokenKind::PointerDeclarator, Previous()));
        continue;
      }

      if (At(TokenKind::Question) || At(TokenKind::Bang)) {
        param.nullability_suffix_tokens.push_back(
            MakeSemaTokenMetadata(Objc3SemaTokenKind::NullabilitySuffix, Advance()));
        continue;
      }

      if (At(TokenKind::Identifier) && IsOwnershipQualifierSpelling(Peek().text)) {
        const Token qualifier = Advance();
        param.has_ownership_qualifier = true;
        param.ownership_qualifier_spelling = qualifier.text;
        param.ownership_qualifier_tokens.push_back(
            MakeSemaTokenMetadata(Objc3SemaTokenKind::OwnershipQualifier, qualifier));
        continue;
      }

      break;
    }
  }

  std::vector<std::unique_ptr<Stmt>> ParseBlock() {
    std::vector<std::unique_ptr<Stmt>> body;
    if (!Match(TokenKind::LBrace)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P110", "missing '{' to start block"));
      block_failed_ = true;
      return {};
    }

    while (!At(TokenKind::RBrace) && !At(TokenKind::Eof)) {
      auto stmt = ParseStatement();
      if (stmt != nullptr) {
        body.push_back(std::move(stmt));
      } else {
        SynchronizeStatement();
      }
    }

    if (!Match(TokenKind::RBrace)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P111", "missing '}' to end block"));
      block_failed_ = true;
      return {};
    }

    return body;
  }

  std::vector<std::unique_ptr<Stmt>> ParseControlBody() {
    if (At(TokenKind::LBrace)) {
      return ParseBlock();
    }
    std::vector<std::unique_ptr<Stmt>> body;
    auto stmt = ParseStatement();
    if (stmt == nullptr) {
      block_failed_ = true;
      return {};
    }
    body.push_back(std::move(stmt));
    return body;
  }

  std::string BuildTryExpressionProfile(const Expr &expr) {
    return objc3c::parse::BuildObjc3TryExpressionProfile(expr);
  }

  std::string BuildThrowStatementProfile(const Expr &expr) {
    return objc3c::parse::BuildObjc3ThrowStatementProfile(expr);
  }

  std::string BuildDoCatchProfile(const BlockStmt &block) {
    return objc3c::parse::BuildObjc3DoCatchProfile(block);
  }

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

  void SynchronizeTopLevel() {
    objc3c::parse::SynchronizeObjc3ParserTopLevel(tokens_, index_);
  }

  void SynchronizeFunctionTail() {
    objc3c::parse::SynchronizeObjc3ParserFunctionTail(tokens_, index_);
  }

  void SynchronizeStatement() {
    objc3c::parse::SynchronizeObjc3ParserStatement(tokens_, index_);
  }

