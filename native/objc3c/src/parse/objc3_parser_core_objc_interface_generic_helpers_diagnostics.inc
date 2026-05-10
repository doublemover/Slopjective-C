  bool RejectInvalidObjcInterfaceGenericParameterIdentifier(const Token &token) {
    diagnostics_.push_back(MakeDiag(
        token.line, token.column, "O3P101",
        "invalid Objective-C interface generic parameter identifier"));
    return false;
  }

  bool RejectInvalidObjcInterfaceGenericConstraintType(const Token &token) {
    diagnostics_.push_back(MakeDiag(
        token.line, token.column, "O3P101",
        "invalid Objective-C interface generic constraint type"));
    return false;
  }

  bool RejectMissingObjcInterfaceGenericParameterListCloser(const Token &token) {
    diagnostics_.push_back(MakeDiag(
        token.line, token.column, "O3P112",
        "missing '>' after Objective-C interface generic parameter list"));
    return false;
  }

  bool RejectUnterminatedObjcInterfaceGenericSuffix(
      const Token &open, const std::string &diagnostic_code,
      const std::string &diagnostic_message) {
    diagnostics_.push_back(MakeDiag(open.line, open.column, diagnostic_code,
                                    diagnostic_message));
    return false;
  }
