  std::string BuildTryExpressionProfile(const Expr &expr) {
    return objc3c::parse::BuildObjc3TryExpressionProfile(expr);
  }

  std::string BuildThrowStatementProfile(const Expr &expr) {
    return objc3c::parse::BuildObjc3ThrowStatementProfile(expr);
  }

  std::string BuildDoCatchProfile(const BlockStmt &block) {
    return objc3c::parse::BuildObjc3DoCatchProfile(block);
  }
