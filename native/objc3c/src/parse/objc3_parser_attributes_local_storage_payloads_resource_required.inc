  bool ValidateLocalResourceAttributeRequiredClauses(
      const Token &attribute_name,
      const LocalResourceAttributeClauseState &clauses) {
    if (!clauses.saw_close || !clauses.saw_invalid) {
      return RejectLocalStorageAttributeAt(
          attribute_name, "O3P312",
          "objc_resource requires close and invalid clauses");
    }
    return true;
  }
