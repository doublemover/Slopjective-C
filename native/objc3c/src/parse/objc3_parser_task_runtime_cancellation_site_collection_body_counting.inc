Objc3TaskRuntimeCancellationSiteCounts CountTaskRuntimeCancellationSitesInBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3TaskRuntimeCancellationSiteCounts counts;
  Objc3ProfileSymbolWalker walker{
      &CollectTaskRuntimeCancellationProfileSymbol, &counts};
  WalkObjc3ProfileSymbolsInBody(body, walker);
  return counts;
}
