  bool ConsumeCleanupLocalStorageSugarAttribute() {
    return Match(TokenKind::KwAtCleanup);
  }

  bool ConsumeResourceLocalStorageSugarAttribute() {
    return Match(TokenKind::KwAtResource);
  }
