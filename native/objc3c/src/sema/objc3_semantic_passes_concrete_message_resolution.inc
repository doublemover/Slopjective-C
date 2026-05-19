static Objc3ResolvedMessageSendMethod ResolveConcreteOwnerMethod(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name,
    const std::string &selector,
    bool is_class_method) {
  Objc3ResolvedMessageSendMethod resolution;
  resolution.owner_name = owner_name;
  resolution.is_class_method = is_class_method;

  const Objc3MethodInfo *merged_category_method =
      ResolveMethodInMergedCategorySurface(surface, owner_name, selector,
                                          is_class_method);
  if (merged_category_method != nullptr) {
    resolution.status = Objc3ResolvedMessageSendMethod::Status::Resolved;
    resolution.method = merged_category_method;
    return resolution;
  }

  const Objc3MethodInfo *interface_method =
      ResolveMethodInOwnerInterface(surface, owner_name, selector, is_class_method);
  const Objc3MethodInfo *implementation_method = ResolveMethodInOwnerImplementation(
      surface, owner_name, selector, is_class_method);

  if (interface_method == nullptr && implementation_method == nullptr) {
    bool missing_base = false;
    bool cycle_detected = false;
    const Objc3MethodInfo *super_method = FindSurfaceMethodInSuperChain(
        surface, owner_name, selector, is_class_method, missing_base,
        cycle_detected);
    if (cycle_detected || missing_base) {
      resolution.status =
          Objc3ResolvedMessageSendMethod::Status::AmbiguousConcreteSelector;
      resolution.missing_base = missing_base;
      resolution.cycle_detected = cycle_detected;
      return resolution;
    }
    if (super_method != nullptr) {
      resolution.status = Objc3ResolvedMessageSendMethod::Status::Resolved;
      resolution.method = super_method;
      return resolution;
    }
    resolution.status =
        Objc3ResolvedMessageSendMethod::Status::MissingConcreteSelector;
    return resolution;
  }
  if (interface_method != nullptr && implementation_method != nullptr) {
    if (!IsCompatibleMethodSignature(*interface_method, *implementation_method)) {
      resolution.status =
          Objc3ResolvedMessageSendMethod::Status::AmbiguousConcreteSelector;
      return resolution;
    }
    resolution.status = Objc3ResolvedMessageSendMethod::Status::Resolved;
    resolution.method = implementation_method;
    return resolution;
  }
  resolution.status = Objc3ResolvedMessageSendMethod::Status::Resolved;
  resolution.method =
      implementation_method != nullptr ? implementation_method : interface_method;
  return resolution;
}

static Expr::DispatchSurfaceKind ResolveNormalizedMessageSendDispatchSurfaceKind(
    const Expr &expr,
    const Objc3MessageSendResolutionContext &context) {
  Expr::DispatchSurfaceKind dispatch_kind = expr.dispatch_surface_kind;
  if (expr.dispatch_surface_is_normalized) {
    return dispatch_kind;
  }
  if (expr.receiver == nullptr) {
    return dispatch_kind;
  }
  if (expr.receiver->kind == Expr::Kind::NilLiteral) {
    return Expr::DispatchSurfaceKind::Instance;
  }
  if (expr.receiver->kind != Expr::Kind::Identifier) {
    return Expr::DispatchSurfaceKind::Dynamic;
  }
  if (context.inside_method && expr.receiver->ident == "super") {
    return Expr::DispatchSurfaceKind::Super;
  }
  if ((context.surface != nullptr &&
       context.surface->interfaces.find(expr.receiver->ident) !=
           context.surface->interfaces.end()) ||
      (context.surface != nullptr &&
       context.surface->implementations.find(expr.receiver->ident) !=
           context.surface->implementations.end())) {
    return Expr::DispatchSurfaceKind::Class;
  }
  if (context.inside_method && expr.receiver->ident == "self") {
    return context.is_class_method ? Expr::DispatchSurfaceKind::Class
                                   : Expr::DispatchSurfaceKind::Instance;
  }
  return Expr::DispatchSurfaceKind::Instance;
}

static Objc3ResolvedMessageSendMethod ResolveConcreteMessageSendMethod(
    const Expr &expr,
    const Objc3MessageSendResolutionContext &context) {
  Objc3ResolvedMessageSendMethod resolution;
  if (context.surface == nullptr || expr.receiver == nullptr) {
    return resolution;
  }

  const Expr::DispatchSurfaceKind dispatch_kind =
      ResolveNormalizedMessageSendDispatchSurfaceKind(expr, context);

  switch (dispatch_kind) {
    case Expr::DispatchSurfaceKind::Super: {
      resolution.owner_name = context.current_super_name;
      resolution.is_class_method = context.is_class_method;
      if (context.current_implementation_name.empty() ||
          context.current_super_name.empty()) {
        resolution.status =
            Objc3ResolvedMessageSendMethod::Status::MissingConcreteSelector;
        return resolution;
      }
      bool missing_base = false;
      bool cycle_detected = false;
      const Objc3MethodInfo *super_method = FindSurfaceMethodInSuperChain(
          *context.surface, context.current_implementation_name, expr.selector,
          context.is_class_method, missing_base, cycle_detected);
      if (cycle_detected || missing_base) {
        resolution.status =
            Objc3ResolvedMessageSendMethod::Status::AmbiguousConcreteSelector;
        resolution.missing_base = missing_base;
        resolution.cycle_detected = cycle_detected;
        return resolution;
      }
      if (super_method == nullptr) {
        resolution.status =
            Objc3ResolvedMessageSendMethod::Status::MissingConcreteSelector;
        return resolution;
      }
      resolution.status = Objc3ResolvedMessageSendMethod::Status::Resolved;
      resolution.method = super_method;
      return resolution;
    }
    case Expr::DispatchSurfaceKind::Class: {
      std::string owner_name;
      if (expr.receiver->kind == Expr::Kind::Identifier &&
          expr.receiver->ident == "self" && context.inside_method &&
          !context.current_implementation_name.empty()) {
        owner_name = context.current_implementation_name;
      } else if (expr.receiver->kind == Expr::Kind::Identifier) {
        owner_name = expr.receiver->ident;
      }
      if (owner_name.empty()) {
        return resolution;
      }
      return ResolveConcreteOwnerMethod(*context.surface, owner_name,
                                        expr.selector, true);
    }
    case Expr::DispatchSurfaceKind::Instance: {
      if (!(expr.receiver->kind == Expr::Kind::Identifier &&
            expr.receiver->ident == "self" && context.inside_method &&
            !context.is_class_method &&
            !context.current_implementation_name.empty())) {
        return resolution;
      }
      return ResolveConcreteOwnerMethod(*context.surface,
                                        context.current_implementation_name,
                                        expr.selector, false);
    }
    case Expr::DispatchSurfaceKind::Direct:
    case Expr::DispatchSurfaceKind::Dynamic:
    case Expr::DispatchSurfaceKind::Unclassified:
      return resolution;
  }
  return resolution;
}
