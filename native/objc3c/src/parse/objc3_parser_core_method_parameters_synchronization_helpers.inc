  void SynchronizeTopLevel() {
    objc3c::parse::SynchronizeObjc3ParserTopLevel(tokens_, index_);
  }

  void SynchronizeFunctionTail() {
    objc3c::parse::SynchronizeObjc3ParserFunctionTail(tokens_, index_);
  }

  void SynchronizeStatement() {
    objc3c::parse::SynchronizeObjc3ParserStatement(tokens_, index_);
  }
