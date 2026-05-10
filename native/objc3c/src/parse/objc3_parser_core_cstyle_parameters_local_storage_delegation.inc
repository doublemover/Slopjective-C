  bool ParseLocalStorageAttribute(LetStmt &stmt) {
    return objc3c::parse::ParseObjc3LocalStorageAttribute(
        tokens_, index_, diagnostics_, stmt);
  }

  bool ParseLocalStorageSugar(LetStmt &stmt) {
    return objc3c::parse::ParseObjc3LocalStorageSugar(
        tokens_, index_, diagnostics_, stmt);
  }
