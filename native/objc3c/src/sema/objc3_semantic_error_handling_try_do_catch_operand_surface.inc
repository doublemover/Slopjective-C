struct Objc3ErrorHandlingTryOperandSurface {
  bool throwing_callable = false;
  bool bridged_callable = false;
  std::string typed_throws_payload;
};

static const Objc3MethodInfo *FindTypedThrowsSurfaceMethodWithKind(
    const std::unordered_map<std::string, Objc3MethodInfo> &methods,
    const std::string &selector,
    bool is_class_method) {
  const auto method_it = methods.find(selector);
  if (method_it == methods.end() ||
      method_it->second.is_class_method != is_class_method) {
    return nullptr;
  }
  return &method_it->second;
}

static const Objc3MethodInfo *ResolveTypedThrowsSurfaceMethodInSuperChain(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &interface_name,
    const std::string &selector,
    bool is_class_method) {
  const auto interface_it = surface.interfaces.find(interface_name);
  if (interface_it == surface.interfaces.end()) {
    return nullptr;
  }

  std::string next_super = interface_it->second.super_name;
  std::unordered_set<std::string> visited;
  while (!next_super.empty()) {
    if (!visited.insert(next_super).second) {
      return nullptr;
    }
    const auto super_it = surface.interfaces.find(next_super);
    if (super_it == surface.interfaces.end()) {
      return nullptr;
    }
    const Objc3MethodInfo *method = FindTypedThrowsSurfaceMethodWithKind(
        super_it->second.methods, selector, is_class_method);
    if (method != nullptr) {
      return method;
    }
    next_super = super_it->second.super_name;
  }
  return nullptr;
}

static const Objc3MethodInfo *ResolveTypedThrowsConcreteOwnerMethod(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name,
    const std::string &selector,
    bool is_class_method) {
  const auto merge_it = surface.category_merge_surfaces.find(owner_name);
  if (merge_it != surface.category_merge_surfaces.end()) {
    const Objc3MethodInfo *merged = FindTypedThrowsSurfaceMethodWithKind(
        merge_it->second.merged_methods, selector, is_class_method);
    if (merged != nullptr) {
      return merged;
    }
  }

  const auto implementation_it = surface.implementations.find(owner_name);
  if (implementation_it != surface.implementations.end()) {
    const Objc3MethodInfo *implementation_method =
        FindTypedThrowsSurfaceMethodWithKind(implementation_it->second.methods,
                                             selector, is_class_method);
    if (implementation_method != nullptr) {
      return implementation_method;
    }
  }

  const auto interface_it = surface.interfaces.find(owner_name);
  if (interface_it != surface.interfaces.end()) {
    const Objc3MethodInfo *interface_method =
        FindTypedThrowsSurfaceMethodWithKind(interface_it->second.methods,
                                             selector, is_class_method);
    if (interface_method != nullptr) {
      return interface_method;
    }
  }

  return ResolveTypedThrowsSurfaceMethodInSuperChain(
      surface, owner_name, selector, is_class_method);
}

static const Objc3MethodInfo *ResolveConcreteMessageSendErrorSurfaceMethod(
    const Expr &operand,
    const Objc3ErrorHandlingSemanticWalkContext &context) {
  if (context.surface == nullptr || operand.receiver == nullptr ||
      operand.receiver->kind != Expr::Kind::Identifier) {
    return nullptr;
  }

  const std::string &receiver_name = operand.receiver->ident;
  if (context.inside_method && receiver_name == "super") {
    if (context.current_implementation_name.empty()) {
      return nullptr;
    }
    return ResolveTypedThrowsSurfaceMethodInSuperChain(
        *context.surface, context.current_implementation_name,
        operand.selector, context.is_class_method);
  }

  std::string owner_name;
  bool is_class_method = true;
  if (context.inside_method && receiver_name == "self") {
    owner_name = context.current_implementation_name;
    is_class_method = context.is_class_method;
  } else if (context.surface->interfaces.find(receiver_name) !=
                 context.surface->interfaces.end() ||
             context.surface->implementations.find(receiver_name) !=
                 context.surface->implementations.end()) {
    owner_name = receiver_name;
    is_class_method = true;
  } else {
    return nullptr;
  }

  if (owner_name.empty()) {
    return nullptr;
  }
  return ResolveTypedThrowsConcreteOwnerMethod(
      *context.surface, owner_name, operand.selector, is_class_method);
}

static void ApplyMethodInfoErrorSurface(
    const Objc3MethodInfo &method,
    Objc3ErrorHandlingTryOperandSurface &surface) {
  surface.throwing_callable =
      surface.throwing_callable || method.throws_declared;
  if (method.typed_throws_declared &&
      !method.typed_throws_error_type_spelling.empty()) {
    surface.typed_throws_payload = method.typed_throws_error_type_spelling;
  }
  surface.bridged_callable =
      surface.bridged_callable || method.ns_error_bridge_path_sites > 0;
}

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
ResolveTryOperandSurface(const Expr *operand,
                         const Objc3Program &program,
                         const Objc3ErrorHandlingSemanticWalkContext &context) {
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
    if (const Objc3MethodInfo *method =
            ResolveConcreteMessageSendErrorSurfaceMethod(*operand, context)) {
      ApplyMethodInfoErrorSurface(*method, surface);
      return surface;
    }
    ResolveMessageSendErrorSurface(program, operand->selector, surface);
  }
  return surface;
}
