void WalkObjc3ProfileSymbolsInForClause(
    const ForClause &clause,
    const Objc3ProfileSymbolWalker &walker) {
  WalkObjc3ProfileSymbolsInExpr(clause.value.get(), walker);
}
