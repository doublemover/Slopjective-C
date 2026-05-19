    bool saw_local_storage_annotation = false;
    LetStmt local_storage_annotation;
    if (AtIdentifierText("__attribute__")) {
      Advance();
      if (!ParseLocalStorageAttribute(local_storage_annotation)) {
        return nullptr;
      }
      saw_local_storage_annotation = true;
    } else if (At(TokenKind::KwAtCleanup) || At(TokenKind::KwAtResource)) {
      if (!ParseLocalStorageSugar(local_storage_annotation)) {
        return nullptr;
      }
      saw_local_storage_annotation = true;
    }
