  void ResetFunctionReturnTypeState(FunctionDecl &fn) {
    ResetFunctionReturnTypeSpellingState(fn);
    ResetFunctionReturnGenericProfileState(fn);
    ResetFunctionReturnModuleProfileState(fn);
    ResetFunctionReturnPointerOwnershipState(fn);
  }
