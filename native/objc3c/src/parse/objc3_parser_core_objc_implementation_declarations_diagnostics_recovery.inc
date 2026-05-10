  void DiagnoseUnsupportedObjcImplementationDeclMember() {
    const Token &token = Peek();
    diagnostics_.push_back(
        MakeDiag(token.line, token.column, "O3P100",
                 "unsupported token inside @implementation declaration"));
  }

  bool DiagnoseMissingObjcImplementationDeclEnd() {
    const Token &token = Peek();
    diagnostics_.push_back(MakeDiag(
        token.line, token.column, "O3P111",
        "missing '@end' after @implementation"));
    SynchronizeTopLevel();
    return false;
  }
