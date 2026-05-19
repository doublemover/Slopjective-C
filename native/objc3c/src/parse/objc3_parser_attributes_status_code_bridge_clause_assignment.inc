  template <typename TCallableDecl>
  bool ApplyStatusCodeBridgeAttributeClause(
      TCallableDecl &decl,
      const Token &clause,
      const std::string &value_text,
      bool &saw_success,
      bool &saw_error_type,
      bool &saw_mapping) {
    if (clause.text == "success") {
      if (saw_success) {
        diagnostics_.push_back(MakeDiag(
            clause.line, clause.column, "O3P275",
            "duplicate objc_status_code success clause"));
        return false;
      }
      decl.objc_status_code_success_literal = value_text;
      saw_success = true;
      return true;
    }

    if (clause.text == "error_type") {
      if (saw_error_type) {
        diagnostics_.push_back(MakeDiag(
            clause.line, clause.column, "O3P276",
            "duplicate objc_status_code error_type clause"));
        return false;
      }
      decl.objc_status_code_error_type_spelling = value_text;
      saw_error_type = true;
      return true;
    }

    if (clause.text == "mapping") {
      if (saw_mapping) {
        diagnostics_.push_back(MakeDiag(
            clause.line, clause.column, "O3P277",
            "duplicate objc_status_code mapping clause"));
        return false;
      }
      decl.objc_status_code_mapping_symbol = value_text;
      saw_mapping = true;
      return true;
    }

    diagnostics_.push_back(MakeDiag(
        clause.line, clause.column, "O3P278",
        "unsupported objc_status_code clause '" + clause.text + "'"));
    return false;
  }
