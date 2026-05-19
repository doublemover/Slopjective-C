Objc3UnwindCleanupSiteCounts CountUnwindCleanupSitesInBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3UnwindCleanupSiteCounts counts;
  Objc3ProfileSymbolWalker walker{
      &CollectUnwindCleanupProfileSymbol, &counts};
  WalkObjc3ProfileSymbolsInBody(body, walker);
  return counts;
}
