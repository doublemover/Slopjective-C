  bool ParseObjcMethodSelectorHead(Objc3MethodDecl::SelectorPiece &head_piece) {
    const Token &selector_head = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(
          MakeDiag(selector_head.line, selector_head.column, "O3P101", "invalid Objective-C selector identifier"));
      return false;
    }

    const Token head_token = Previous();
    head_piece.keyword = head_token.text;
    head_piece.line = head_token.line;
    head_piece.column = head_token.column;
    return true;
  }

  bool ParseObjcMethodSelectorParameterTail(
      Objc3MethodDecl &method, Objc3MethodDecl::SelectorPiece &head_piece) {
    if (!Match(TokenKind::Colon)) {
      if (At(TokenKind::Less)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P114",
            "Objective-C method generic parameter clauses must immediately "
            "follow the method marker before the return type"));
        return false;
      }
      method.selector_pieces.push_back(std::move(head_piece));
      return true;
    }

    head_piece.has_parameter = true;
    FuncParam first_param;
    if (!ParseObjcMethodParameterClause(first_param)) {
      return false;
    }
    head_piece.parameter_name = first_param.name;
    method.params.push_back(std::move(first_param));
    method.selector_pieces.push_back(std::move(head_piece));

    while (At(TokenKind::Identifier) && (index_ + 1 < tokens_.size()) &&
           tokens_[index_ + 1].kind == TokenKind::Colon) {
      const Token keyword = Advance();
      (void)Match(TokenKind::Colon);

      Objc3MethodDecl::SelectorPiece keyword_piece;
      keyword_piece.keyword = keyword.text;
      keyword_piece.has_parameter = true;
      keyword_piece.line = keyword.line;
      keyword_piece.column = keyword.column;

      FuncParam keyword_param;
      if (!ParseObjcMethodParameterClause(keyword_param)) {
        return false;
      }
      keyword_piece.parameter_name = keyword_param.name;
      method.params.push_back(std::move(keyword_param));
      method.selector_pieces.push_back(std::move(keyword_piece));
    }

    return true;
  }

  bool ParseObjcMethodSelector(Objc3MethodDecl &method) {
    Objc3MethodDecl::SelectorPiece head_piece;
    if (!ParseObjcMethodSelectorHead(head_piece)) {
      return false;
    }

    if (!ParseObjcMethodSelectorParameterTail(method, head_piece)) {
      return false;
    }

    std::vector<objc3c::support::selectors::SelectorPieceSpelling>
        selector_pieces;
    selector_pieces.reserve(method.selector_pieces.size());
    for (const Objc3MethodDecl::SelectorPiece &piece :
         method.selector_pieces) {
      selector_pieces.push_back({piece.keyword, piece.has_parameter});
    }
    method.selector =
        objc3c::support::selectors::BuildNormalizedSelectorSpelling(
            selector_pieces);
    method.selector_is_normalized = true;
    return true;
  }
