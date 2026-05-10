  bool ParseFunctionCallableTail(
      FunctionDecl &fn,
      bool &has_return_annotation) {
    if (!ParseOptionalAsyncClause(fn)) {
      SynchronizeTopLevel();
      return false;
    }

    if (!ParseOptionalThrowsClause(fn)) {
      SynchronizeTopLevel();
      return false;
    }

    if (!ParseFunctionReturnAnnotation(fn, has_return_annotation)) {
      return false;
    }

    if (!ParseOptionalThrowsClause(fn)) {
      SynchronizeTopLevel();
      return false;
    }

    if (!ParseOptionalAsyncClause(fn)) {
      SynchronizeTopLevel();
      return false;
    }

    if (!ParseOptionalCallableBridgeAttributes(fn)) {
      SynchronizeTopLevel();
      return false;
    }

    PublishFunctionCallableTailProfiles(fn);

    return RejectUnexpectedFunctionDeclarationQualifier(
        "after function return annotation");
  }
