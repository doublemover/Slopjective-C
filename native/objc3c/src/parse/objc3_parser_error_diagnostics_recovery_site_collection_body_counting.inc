Objc3ErrorDiagnosticsRecoverySiteCounts
CountErrorDiagnosticsRecoverySitesInBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3ErrorDiagnosticsRecoverySiteCounts counts;
  Objc3ProfileSymbolWalker walker{
      &CollectErrorDiagnosticsRecoveryProfileSymbol, &counts};
  WalkObjc3ProfileSymbolsInBody(body, walker);
  return counts;
}
