  void CopyMethodReturnTypeFromFunctionDecl(const FunctionDecl &source, Objc3MethodDecl &target) {
    CopyObjc3MethodReturnTypeFromFunctionDecl(source, target);
  }

  void CopyPropertyTypeFromParam(const FuncParam &source, Objc3PropertyDecl &target) {
    CopyObjc3PropertyTypeFromParam(source, target);
  }
