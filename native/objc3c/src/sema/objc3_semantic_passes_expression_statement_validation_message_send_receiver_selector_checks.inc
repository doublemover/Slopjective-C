  // super/direct/dynamic legality expansion anchor: lane-B rejects
  // illegal `super` sites before concrete resolution, keeps direct dispatch
  // reserved/non-goal, and leaves admitted dynamic sites on the runtime path
  // with their normalized method-family metadata intact.
  if (expr->dispatch_surface_is_normalized &&
      expr->dispatch_surface_kind == Expr::DispatchSurfaceKind::Direct) {
    diagnostics.push_back(MakeDiag(
        expr->line, expr->column, "O3S216",
        "selector resolution failed: direct dispatch remains reserved for selector '" +
            selector + "'"));
    return MakeScalarSemanticType(ValueType::Unknown);
  }
  if (expr->dispatch_surface_is_normalized &&
      expr->dispatch_surface_kind == Expr::DispatchSurfaceKind::Super) {
    if (!message_send_context.inside_method ||
        message_send_context.current_implementation_name.empty()) {
      diagnostics.push_back(MakeDiag(
          expr->line, expr->column, "O3S216",
          "selector resolution failed: 'super' dispatch requires an enclosing implementation method"));
      return MakeScalarSemanticType(ValueType::Unknown);
    }
    if (message_send_context.current_super_name.empty()) {
      diagnostics.push_back(MakeDiag(
          expr->line, expr->column, "O3S216",
          "selector resolution failed: implementation '" +
              message_send_context.current_implementation_name +
              "' does not provide a superclass for 'super' dispatch"));
      return MakeScalarSemanticType(ValueType::Unknown);
    }
  }
  const SemanticTypeInfo receiver_type = ValidateExpr(
      expr->receiver.get(), scopes, globals, functions, diagnostics,
      max_message_send_args, message_send_context);
  if (expr->optional_send_enabled && !IsUnknownSemanticType(receiver_type) &&
      !IsObjCReferenceSemanticType(receiver_type)) {
    diagnostics.push_back(MakeDiag(
        expr->line, expr->column, "O3S206",
        "type mismatch: optional send receiver for selector '" + selector +
            "' must be ObjC-reference-compatible"));
  }
  if (!expr->optional_send_enabled && IsNullableObjCReferenceSemanticType(receiver_type)) {
    diagnostics.push_back(MakeDiag(
        expr->line, expr->column, "O3S206",
        "type mismatch: ordinary send receiver for selector '" + selector +
            "' must be proven nonnull or use optional send syntax"));
  }
  if (!IsUnknownSemanticType(receiver_type) && !IsMessageCompatibleType(receiver_type)) {
    const unsigned diag_line = expr->receiver != nullptr ? expr->receiver->line : expr->line;
    const unsigned diag_column = expr->receiver != nullptr ? expr->receiver->column : expr->column;
    diagnostics.push_back(MakeDiag(diag_line, diag_column, "O3S207",
                                   "type mismatch: message receiver for selector '" + selector +
                                       "' must be ObjC-reference-compatible or scalar bridge-compatible, got '" +
                                       SemanticTypeName(receiver_type) + "'"));
  }

  if (expr->args.size() > max_message_send_args) {
    diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S208",
                                   "arity mismatch: message '" + selector + "' has " +
                                       std::to_string(expr->args.size()) +
                                       " argument(s); native frontend supports at most " +
                                       std::to_string(max_message_send_args)));
  }

  for (std::size_t i = 0; i < expr->args.size(); ++i) {
    const auto &arg = expr->args[i];
    const SemanticTypeInfo arg_type = ValidateExpr(
        arg.get(), scopes, globals, functions, diagnostics,
        max_message_send_args, message_send_context);
    if (!IsUnknownSemanticType(arg_type) && !IsMessageCompatibleType(arg_type)) {
      diagnostics.push_back(MakeDiag(arg->line, arg->column, "O3S209",
                                     "type mismatch: message argument " + std::to_string(i) +
                                         " for selector '" + selector +
                                         "' must be ObjC-reference-compatible or scalar bridge-compatible, got '" +
                                          SemanticTypeName(arg_type) + "'"));
    }
  }
