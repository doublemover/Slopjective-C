Objc3AsyncContinuationSiteCounts CountAsyncContinuationSitesInBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3AsyncContinuationSiteCounts counts;
  Objc3ProfileSymbolWalker walker{
      &CollectAsyncContinuationProfileSymbol, &counts};
  WalkObjc3ProfileSymbolsInBody(body, walker);
  return counts;
}
