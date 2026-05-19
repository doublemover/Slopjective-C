  bool RejectMissingCleanupSugarOpenParen() {
    const Token &token = Peek();
    return RejectLocalStorageAttributeAt(
        token, "O3P317", "missing '(' after @cleanup");
  }

  bool RejectMissingCleanupSugarFunctionIdentifier() {
    const Token &token = Peek();
    return RejectLocalStorageAttributeAt(
        token, "O3P318", "@cleanup requires cleanup function identifier");
  }

  bool RejectMissingCleanupSugarCloseParen() {
    const Token &token = Peek();
    return RejectLocalStorageAttributeAt(
        token, "O3P319", "missing ')' after @cleanup payload");
  }

  bool RejectMissingResourceSugarOpenParen() {
    const Token &token = Peek();
    return RejectLocalStorageAttributeAt(
        token, "O3P320", "missing '(' after @resource");
  }

  bool RejectMissingResourceSugarCleanupIdentifier() {
    const Token &token = Peek();
    return RejectLocalStorageAttributeAt(
        token, "O3P321", "@resource requires cleanup function identifier");
  }

  bool RejectMissingResourceSugarCleanupSeparator() {
    const Token &token = Peek();
    return RejectLocalStorageAttributeAt(
        token, "O3P322",
        "missing ',' after @resource cleanup function");
  }

  bool RejectMissingResourceSugarInvalidClause() {
    const Token &token = Peek();
    return RejectLocalStorageAttributeAt(
        token, "O3P323", "@resource requires invalid clause");
  }

  bool RejectMissingResourceSugarInvalidSeparator() {
    const Token &token = Peek();
    return RejectLocalStorageAttributeAt(
        token, "O3P324", "missing ':' after invalid in @resource");
  }

  bool RejectEmptyResourceSugarInvalidExpression() {
    const Token &token = Peek();
    return RejectLocalStorageAttributeAt(
        token, "O3P325",
        "@resource invalid expression must not be empty");
  }

  bool RejectMissingResourceSugarCloseParen() {
    const Token &token = Peek();
    return RejectLocalStorageAttributeAt(
        token, "O3P326", "missing ')' after @resource payload");
  }
