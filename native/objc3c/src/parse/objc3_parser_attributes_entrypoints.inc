bool ParseObjc3OptionalContainerDispatchAttributes(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index,
    std::vector<std::string> &diagnostics,
    Objc3InterfaceDecl &decl) {
  Objc3AttributeParser parser(tokens, index, diagnostics);
  return parser.ParseOptionalContainerDispatchAttributes(decl);
}

bool ParseObjc3OptionalCallableBridgeAttributes(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index,
    std::vector<std::string> &diagnostics,
    FunctionDecl &decl) {
  Objc3AttributeParser parser(tokens, index, diagnostics);
  return parser.ParseOptionalCallableBridgeAttributes(decl);
}

bool ParseObjc3OptionalCallableBridgeAttributes(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index,
    std::vector<std::string> &diagnostics,
    Objc3MethodDecl &decl) {
  Objc3AttributeParser parser(tokens, index, diagnostics);
  return parser.ParseOptionalCallableBridgeAttributes(decl);
}

bool ParseObjc3LocalStorageAttribute(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index,
    std::vector<std::string> &diagnostics,
    LetStmt &stmt) {
  Objc3AttributeParser parser(tokens, index, diagnostics);
  return parser.ParseLocalStorageAttribute(stmt);
}

bool ParseObjc3LocalStorageSugar(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index,
    std::vector<std::string> &diagnostics,
    LetStmt &stmt) {
  Objc3AttributeParser parser(tokens, index, diagnostics);
  return parser.ParseLocalStorageSugar(stmt);
}
