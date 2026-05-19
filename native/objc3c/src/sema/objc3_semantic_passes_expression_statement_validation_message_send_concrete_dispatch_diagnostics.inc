  const Objc3ResolvedMessageSendMethod resolution =
      ResolveConcreteMessageSendMethod(*expr, message_send_context);
  if (resolution.status ==
      Objc3ResolvedMessageSendMethod::Status::MissingConcreteSelector) {
    const Expr::DispatchSurfaceKind dispatch_kind =
        ResolveNormalizedMessageSendDispatchSurfaceKind(*expr,
                                                       message_send_context);
    const bool builtin_runtime_alloc_new =
        expr->args.empty() &&
        dispatch_kind == Expr::DispatchSurfaceKind::Class &&
        (selector == "alloc" || selector == "new");
    const bool builtin_runtime_init =
        expr->args.empty() &&
        dispatch_kind == Expr::DispatchSurfaceKind::Instance &&
        selector == "init";
    if (builtin_runtime_alloc_new || builtin_runtime_init) {
      // canonical runnable-object-sample anchor: concrete class
      // receivers admit runtime-owned builtin alloc/new support, and concrete
      // instance-self receivers admit builtin init support, so canonical
      // runnable object samples can compile without placeholder source bodies
      // for those selectors.
      return MakeScalarSemanticType(ValueType::ObjCId);
    }
    const unsigned diag_line =
        expr->receiver != nullptr ? expr->receiver->line : expr->line;
    const unsigned diag_column =
        expr->receiver != nullptr ? expr->receiver->column : expr->column;
    diagnostics.push_back(MakeDiag(
        diag_line, diag_column, "O3S216",
        "selector resolution failed: concrete dispatch receiver '" +
            resolution.owner_name + "' does not provide selector '" + selector +
            "'"));
    return MakeScalarSemanticType(ValueType::Unknown);
  }
  if (resolution.status ==
      Objc3ResolvedMessageSendMethod::Status::AmbiguousConcreteSelector) {
    const unsigned diag_line =
        expr->receiver != nullptr ? expr->receiver->line : expr->line;
    const unsigned diag_column =
        expr->receiver != nullptr ? expr->receiver->column : expr->column;
    const std::string detail =
        resolution.cycle_detected
            ? "super-chain cycle detected during selector resolution"
            : (resolution.missing_base
                   ? "super-chain lookup requires a missing base interface"
                   : "incompatible declarations produce no unique concrete target");
    diagnostics.push_back(MakeDiag(
        diag_line, diag_column, "O3S217",
        "selector resolution is ambiguous for concrete dispatch receiver '" +
            resolution.owner_name + "' and selector '" + selector + "': " +
            detail));
    return MakeScalarSemanticType(ValueType::Unknown);
  }
  if (resolution.status == Objc3ResolvedMessageSendMethod::Status::Resolved &&
      resolution.method != nullptr) {
    if (expr->args.size() != resolution.method->arity) {
      diagnostics.push_back(MakeDiag(
          expr->line, expr->column, "O3S216",
          "selector resolution failed: selector '" + selector +
              "' resolves to arity " +
              std::to_string(resolution.method->arity) + " for receiver '" +
              resolution.owner_name + "'"));
      return MakeScalarSemanticType(ValueType::Unknown);
    }
    for (std::size_t i = 0; i < expr->args.size(); ++i) {
      const SemanticTypeInfo arg_type = ValidateExpr(
          expr->args[i].get(), scopes, globals, functions, diagnostics,
          max_message_send_args, message_send_context);
      const SemanticTypeInfo expected =
          MakeSemanticTypeFromMethodInfoParam(*resolution.method, i);
      const bool bool_coercion =
          !expected.is_vector && expected.type == ValueType::Bool &&
          !arg_type.is_vector && arg_type.type == ValueType::I32;
      const bool i32_alias_coercion =
          AreScalarI32AliasCompatible(expected, arg_type);
      const bool objc_reference_coercion =
          AreObjCReferenceTypesAssignmentCompatible(expected, arg_type);
      if (!IsUnknownSemanticType(arg_type) && !IsUnknownSemanticType(expected) &&
          !IsSameSemanticType(arg_type, expected) && !bool_coercion &&
          !i32_alias_coercion && !objc_reference_coercion) {
        diagnostics.push_back(MakeDiag(
            expr->args[i]->line, expr->args[i]->column, "O3S206",
            "type mismatch: selector '" + selector + "' on receiver '" +
                resolution.owner_name + "' expects '" +
                SemanticTypeName(expected) + "' for argument " +
                std::to_string(i) + ", got '" + SemanticTypeName(arg_type) +
                "'"));
      }
    }
    return MakeSemanticTypeFromMethodInfoReturn(*resolution.method);
  }
  return MakeScalarSemanticType(ValueType::ObjCId);
