  void PublishCleanupLocalStorageSugarProfile(LetStmt &stmt) {
    stmt.cleanup_attribute_declared = true;
    stmt.cleanup_sugar_declared = true;
    stmt.cleanup_profile_is_normalized = true;
    stmt.cleanup_profile =
        BuildCleanupAttributeProfile(true, stmt.cleanup_function_symbol);
  }

  void PublishResourceLocalStorageSugarProfile(LetStmt &stmt) {
    stmt.resource_attribute_declared = true;
    stmt.resource_sugar_declared = true;
    stmt.resource_profile_is_normalized = true;
    stmt.resource_profile = BuildResourceAttributeProfile(
        true, stmt.resource_close_symbol, stmt.resource_invalid_expression);
  }
