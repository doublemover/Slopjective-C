void WalkObjc3ProfileSymbolsInBody(
    const std::vector<std::unique_ptr<Stmt>> &body,
    const Objc3ProfileSymbolWalker &walker) {
  for (const auto &stmt : body) {
    WalkObjc3ProfileSymbolsInStmt(stmt.get(), walker);
  }
}
