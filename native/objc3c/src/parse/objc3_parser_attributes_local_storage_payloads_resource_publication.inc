  void PublishLocalResourceAttributeProfile(LetStmt &stmt) {
    stmt.resource_attribute_declared = true;
    stmt.resource_profile_is_normalized = true;
    stmt.resource_profile = BuildResourceAttributeProfile(
        true, stmt.resource_close_symbol, stmt.resource_invalid_expression);
  }
