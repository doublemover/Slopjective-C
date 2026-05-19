struct Objc3ErrorHandlingTryOperandSurface {
  bool throwing_callable = false;
  bool bridged_callable = false;
};

static void ResolveMessageSendErrorSurface(const Objc3Program &program,
                                           const std::string &selector,
                                           Objc3ErrorHandlingTryOperandSurface &surface) {
  const auto inspect_method = [&](const Objc3MethodDecl &method) {
    if (method.selector != selector) {
      return;
    }
    surface.throwing_callable =
        surface.throwing_callable || method.throws_declared;
    surface.bridged_callable =
        surface.bridged_callable ||
        CallableHasSemanticallyValidNSErrorStatusBridge(method, program);
  };

  for (const auto &protocol_decl : program.protocols) {
    for (const auto &method : protocol_decl.methods) {
      inspect_method(method);
    }
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      inspect_method(method);
    }
  }
  for (const auto &implementation_decl : program.implementations) {
    for (const auto &method : implementation_decl.methods) {
      inspect_method(method);
    }
  }
}

static Objc3ErrorHandlingTryOperandSurface
ResolveTryOperandSurface(const Expr *operand, const Objc3Program &program) {
  Objc3ErrorHandlingTryOperandSurface surface;
  if (operand == nullptr) {
    return surface;
  }

  if (operand->kind == Expr::Kind::Call) {
    if (const FunctionDecl *fn =
            FindFunctionDeclByName(program, operand->ident)) {
      surface.throwing_callable = fn->throws_declared;
      surface.bridged_callable =
          CallableHasSemanticallyValidNSErrorStatusBridge(*fn, program);
    }
    return surface;
  }

  if (operand->kind == Expr::Kind::MessageSend) {
    ResolveMessageSendErrorSurface(program, operand->selector, surface);
  }
  return surface;
}
