struct Objc3ErrorHandlingTryOperandSurface {
  bool throwing_callable = false;
  bool bridged_callable = false;
  std::string typed_throws_payload;
};

static void ResolveMessageSendErrorSurface(const Objc3Program &program,
                                           const std::string &selector,
                                           Objc3ErrorHandlingTryOperandSurface &surface) {
  const auto inspect_method = [&](const Objc3MethodDecl &method) {
    if (method.selector != selector) {
      return;
    }
    if (method.typed_throws_declared &&
        !method.typed_throws_payload.canonical_spelling.empty()) {
      if (surface.typed_throws_payload.empty()) {
        surface.typed_throws_payload =
            method.typed_throws_payload.canonical_spelling;
      } else if (Objc3TypedThrowsPayloadIdentity(
                     surface.typed_throws_payload) !=
                 Objc3TypedThrowsPayloadIdentity(
                     method.typed_throws_payload.canonical_spelling)) {
        surface.bridged_callable = true;
        surface.typed_throws_payload.clear();
      }
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
      if (fn->typed_throws_declared &&
          !fn->typed_throws_payload.canonical_spelling.empty()) {
        surface.typed_throws_payload =
            fn->typed_throws_payload.canonical_spelling;
      }
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
