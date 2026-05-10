void CollectInlineAsmIntrinsicSitesFromSymbol(
    const std::string &symbol,
    Objc3InlineAsmIntrinsicSiteCounts &counts) {
  if (IsInlineAsmCallSymbol(symbol)) {
    counts.inline_asm_sites += 1u;
  }
  if (IsIntrinsicCallSymbol(symbol)) {
    counts.intrinsic_sites += 1u;
    counts.governed_intrinsic_sites += 1u;
    if (IsPrivilegedIntrinsicCallSymbol(symbol)) {
      counts.privileged_intrinsic_sites += 1u;
    }
  }
}

void CollectInlineAsmIntrinsicProfileSymbol(
    const std::string &symbol,
    void *context) {
  auto *counts = static_cast<Objc3InlineAsmIntrinsicSiteCounts *>(context);
  CollectInlineAsmIntrinsicSitesFromSymbol(symbol, *counts);
}
