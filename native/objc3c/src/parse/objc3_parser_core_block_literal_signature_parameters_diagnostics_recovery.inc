  bool ReportExpectedBlockLiteralParameterIdentifier() {
    const Token &token = Peek();
    diagnostics_.push_back(MakeDiag(
        token.line, token.column, "O3P166",
        "expected parameter identifier in block literal"));
    return false;
  }

  bool ReportMissingBlockLiteralParameterListClosing() {
    const Token &token = Peek();
    diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109",
                                    "missing ')' after block literal parameter list"));
    return false;
  }
