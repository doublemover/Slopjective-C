  bool FinalizeFunctionReturnTypeParsing(FunctionDecl &fn) {
    fn.return_typecheck_family_symbol = BuildObjcTypecheckReturnFamilySymbol(fn);

    if (!ParseFunctionReturnTypeSuffix(fn)) {
      return false;
    }

    if (!FinalizeFunctionReturnValueOptionalType(fn)) {
      return false;
    }

    FinalizeFunctionReturnTypeProfiles(fn);

    return true;
  }
