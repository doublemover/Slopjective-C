bool At(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t index,
    TokenKind kind) {
  return AtTokenKind(tokens, index, kind);
}

bool Match(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index,
    TokenKind kind) {
  return MatchTokenKind(tokens, index, kind);
}

void Advance(const std::vector<Objc3LexToken> &tokens, std::size_t &index) {
  (void)AdvanceToken(tokens, index);
}
