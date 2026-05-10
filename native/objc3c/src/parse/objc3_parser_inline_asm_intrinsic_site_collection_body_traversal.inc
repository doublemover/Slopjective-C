Objc3InlineAsmIntrinsicSiteCounts CountInlineAsmIntrinsicSitesInBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3InlineAsmIntrinsicSiteCounts counts;
  Objc3ProfileSymbolWalker walker{
      &CollectInlineAsmIntrinsicProfileSymbol, &counts};
  WalkObjc3ProfileSymbolsInBody(body, walker);
  return counts;
}
