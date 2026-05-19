  void DiagnoseUnsupportedObjcActorInterfaceDeclMember() {
    const Token &token = Peek();
    diagnostics_.push_back(MakeDiag(
        token.line, token.column, "O3P100",
        "unsupported token inside actor class declaration"));
  }

  bool DiagnoseMissingObjcActorInterfaceDeclEnd() {
    const Token &token = Peek();
    diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P111",
                                    "missing '@end' after actor class"));
    SynchronizeTopLevel();
    return false;
  }
