  bool ParseObjcCategoryClause(std::string &category_name, bool &has_category) {
    if (!Match(TokenKind::LParen)) {
      return true;
    }
    // category-merge source-order anchor: parser preserves category
    // attachment spelling and declaration order verbatim so sema can build one
    // deterministic realized-class merge surface without reparsing tokens.
    has_category = true;
    if (Match(TokenKind::Identifier)) {
      category_name = Previous().text;
    }
    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P109", "missing ')' after Objective-C category name"));
      return false;
    }
    return true;
  }
