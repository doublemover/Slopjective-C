  bool ParseObjcMethodTrailingAttributes(Objc3MethodDecl &method) {
    if (!ParseOptionalAsyncClause(method)) {
      return false;
    }

    if (!ParseOptionalThrowsClause(method)) {
      return false;
    }

    if (!ParseOptionalCallableBridgeAttributes(method)) {
      return false;
    }

    method.returns_borrowed_profile = BuildReturnsBorrowedProfile(
        method.objc_returns_borrowed_declared,
        method.objc_returns_borrowed_owner_index,
        method.return_borrowed_pointer_qualified);
    return true;
  }
