static SemanticTypeInfo ValidateMessageSendExpr(const Expr *expr,
                                                const std::vector<SemanticScope> &scopes,
                                                const std::unordered_map<std::string, ValueType> &globals,
                                                const std::unordered_map<std::string, FunctionInfo> &functions,
                                                std::vector<std::string> &diagnostics,
                                                std::size_t max_message_send_args,
                                                const Objc3MessageSendResolutionContext &message_send_context) {
  const std::string selector = expr->selector.empty() ? "<unknown>" : expr->selector;
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
#include "sema/objc3_semantic_passes_expression_statement_validation_message_send_protocol_qualified.inc"
  if (!receiver_type.object_pointer_type_name.empty() &&
      message_send_context.surface != nullptr) {
    const Objc3ResolvedMessageSendMethod typed_receiver_resolution =
        ResolveConcreteOwnerMethod(*message_send_context.surface,
                                   receiver_type.object_pointer_type_name,
                                   selector, false);
    if (typed_receiver_resolution.status ==
        Objc3ResolvedMessageSendMethod::Status::MissingConcreteSelector) {
      const Objc3ResolvedMessageSendPropertyAccessor property_resolution =
          ResolveConcreteOwnerPropertyAccessor(*message_send_context.surface,
                                               receiver_type.object_pointer_type_name,
                                               selector, false);
      if (property_resolution.property != nullptr) {
        const std::size_t expected_arity =
            property_resolution.is_setter ? 1u : 0u;
        if (expr->args.size() != expected_arity) {
          diagnostics.push_back(MakeDiag(
              expr->line, expr->column, "O3S216",
              "selector resolution failed: property accessor selector '" +
                  selector + "' resolves to arity " +
                  std::to_string(expected_arity) + " for typed receiver '" +
                  receiver_type.object_pointer_type_name + "'"));
          return MakeScalarSemanticType(ValueType::Unknown);
        }
        if (property_resolution.is_setter) {
          const SemanticTypeInfo arg_type = ValidateExpr(
              expr->args[0].get(), scopes, globals, functions, diagnostics,
              max_message_send_args, message_send_context);
          const SemanticTypeInfo expected = SubstituteGenericReceiverType(
              MakeSemanticTypeFromPropertyInfo(*property_resolution.property),
              receiver_type.canonical_type, *message_send_context.surface);
          const bool bool_coercion =
              !expected.is_vector && expected.type == ValueType::Bool &&
              !arg_type.is_vector && arg_type.type == ValueType::I32;
          const bool i32_alias_coercion =
              AreScalarI32AliasCompatible(expected, arg_type);
          const bool objc_reference_coercion =
              AreObjCReferenceTypesAssignmentCompatible(expected, arg_type);
          if (!IsUnknownSemanticType(arg_type) &&
              !IsUnknownSemanticType(expected) &&
              !IsSameSemanticType(arg_type, expected) && !bool_coercion &&
              !i32_alias_coercion && !objc_reference_coercion) {
            diagnostics.push_back(MakeDiag(
                expr->args[0]->line, expr->args[0]->column, "O3S206",
                "type mismatch: property accessor selector '" + selector +
                    "' on typed receiver '" +
                    receiver_type.object_pointer_type_name + "' expects '" +
                    SemanticTypeName(expected) + "', got '" +
                    SemanticTypeName(arg_type) + "'"));
          }
          return MakeScalarSemanticType(ValueType::Void);
        }
        return SubstituteGenericReceiverType(
            MakeSemanticTypeFromPropertyInfo(*property_resolution.property),
            receiver_type.canonical_type, *message_send_context.surface);
      }
      const unsigned diag_line =
          expr->receiver != nullptr ? expr->receiver->line : expr->line;
      const unsigned diag_column =
          expr->receiver != nullptr ? expr->receiver->column : expr->column;
      diagnostics.push_back(MakeDiag(
          diag_line, diag_column, "O3S216",
          "selector resolution failed: typed receiver '" +
              receiver_type.object_pointer_type_name +
              "' does not provide selector '" + selector + "'"));
      return MakeScalarSemanticType(ValueType::Unknown);
    }
    if (typed_receiver_resolution.status ==
        Objc3ResolvedMessageSendMethod::Status::AmbiguousConcreteSelector) {
      const unsigned diag_line =
          expr->receiver != nullptr ? expr->receiver->line : expr->line;
      const unsigned diag_column =
          expr->receiver != nullptr ? expr->receiver->column : expr->column;
      const std::string detail =
          typed_receiver_resolution.cycle_detected
              ? "super-chain cycle detected during selector resolution"
              : (typed_receiver_resolution.missing_base
                     ? "super-chain lookup requires a missing base interface"
                     : "incompatible declarations produce no unique concrete target");
      diagnostics.push_back(MakeDiag(
          diag_line, diag_column, "O3S217",
          "selector resolution is ambiguous for typed receiver '" +
              receiver_type.object_pointer_type_name + "' and selector '" +
              selector + "': " + detail));
      return MakeScalarSemanticType(ValueType::Unknown);
    }
    if (typed_receiver_resolution.status ==
            Objc3ResolvedMessageSendMethod::Status::Resolved &&
        typed_receiver_resolution.method != nullptr) {
      const Objc3MethodInfo &method = *typed_receiver_resolution.method;
      if (expr->args.size() != method.arity) {
        diagnostics.push_back(MakeDiag(
            expr->line, expr->column, "O3S216",
            "selector resolution failed: selector '" + selector +
                "' resolves to arity " + std::to_string(method.arity) +
                " for typed receiver '" +
                receiver_type.object_pointer_type_name + "'"));
        return MakeScalarSemanticType(ValueType::Unknown);
      }
      for (std::size_t i = 0; i < expr->args.size(); ++i) {
        const SemanticTypeInfo arg_type = ValidateExpr(
            expr->args[i].get(), scopes, globals, functions, diagnostics,
            max_message_send_args, message_send_context);
        const SemanticTypeInfo expected = SubstituteGenericReceiverType(
            MakeSemanticTypeFromMethodInfoParam(method, i),
            receiver_type.canonical_type, *message_send_context.surface);
        const bool bool_coercion =
            !expected.is_vector && expected.type == ValueType::Bool &&
            !arg_type.is_vector && arg_type.type == ValueType::I32;
        const bool i32_alias_coercion =
            AreScalarI32AliasCompatible(expected, arg_type);
        const bool objc_reference_coercion =
            AreObjCReferenceTypesAssignmentCompatible(expected, arg_type);
        if (!IsUnknownSemanticType(arg_type) &&
            !IsUnknownSemanticType(expected) &&
            !IsSameSemanticType(arg_type, expected) && !bool_coercion &&
            !i32_alias_coercion && !objc_reference_coercion) {
          diagnostics.push_back(MakeDiag(
              expr->args[i]->line, expr->args[i]->column, "O3S206",
              "type mismatch: selector '" + selector +
                  "' on typed receiver '" +
                  receiver_type.object_pointer_type_name + "' expects '" +
                  SemanticTypeName(expected) + "' for argument " +
                  std::to_string(i) + ", got '" + SemanticTypeName(arg_type) +
                  "'"));
        }
      }
      return SubstituteGenericReceiverType(
          MakeSemanticTypeFromMethodInfoReturn(method),
          receiver_type.canonical_type, *message_send_context.surface);
    }
  }
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
}
