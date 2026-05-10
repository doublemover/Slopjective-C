  bool ParseLocalStorageSugar(LetStmt &stmt) {
    if (ConsumeCleanupLocalStorageSugarAttribute()) {
      return ParseCleanupLocalStorageSugarPayload(stmt);
    }
    if (ConsumeResourceLocalStorageSugarAttribute()) {
      return ParseResourceLocalStorageSugarPayload(stmt);
    }
    return false;
  }
