  void DiagnoseUnsupportedObjcInterfaceDeclMember() {
    const Token &token = Peek();
    diagnostics_.push_back(MakeDiag(
        token.line, token.column, "O3P100",
        "unsupported token inside @interface declaration"));
  }

  bool DiagnoseMissingObjcInterfaceDeclEnd() {
    const Token &token = Peek();
    diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P111",
                                    "missing '@end' after @interface"));
    SynchronizeTopLevel();
    return false;
  }
