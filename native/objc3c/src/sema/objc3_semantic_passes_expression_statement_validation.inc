      }
      if (AreObjCReferenceTypesAssignmentCompatible(then_type, else_type)) {
        if (then_type.type == else_type.type) {
          return then_type;
        }
        return MakeScalarSemanticType(ValueType::ObjCId);
      }
      if (!IsSameSemanticType(then_type, else_type)) {
        diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                       "type mismatch: conditional branches must be type-compatible"));
      }
      return IsSameSemanticType(then_type, else_type) ? then_type : MakeScalarSemanticType(ValueType::Unknown);
    }
    case Expr::Kind::BlockLiteral: {
      // block-runtime-semantic-rules freeze anchor: the current
      // block-literal semantic rule boundary only proves deterministic
      // parameter/capture metadata on the source surface and classifies the
      // value as function-shaped; runnable invocation and runtime object
      // semantics remain outside this boundary.
      // capture-legality, escape-classification, and invocation
      // typing anchor: lane-B now turns the parser-owned capture/source
      // annotations into live sema checks and a callable block signature for
      // source-only invocation typing while native block execution still fails
      // closed in later gates.
      const bool parameter_count_match = expr->block_parameter_names_lexicographic.size() == expr->block_parameter_count;
      const bool capture_count_match = expr->block_capture_names_lexicographic.size() == expr->block_capture_count;
      const bool parameters_deterministic =
          parameter_count_match && IsSortedUniqueStrings(expr->block_parameter_names_lexicographic);
      const bool captures_deterministic =
          capture_count_match && IsSortedUniqueStrings(expr->block_capture_names_lexicographic);

      if (!parameters_deterministic || !captures_deterministic) {
        diagnostics.push_back(MakeDiag(expr->line,
                                       expr->column,
                                       "O3S206",
                                       "type mismatch: block literal capture metadata must be deterministic"));
      }
      if (!expr->block_capture_set_deterministic) {
        diagnostics.push_back(MakeDiag(expr->line,
                                       expr->column,
                                       "O3S206",
                                       "type mismatch: block literal capture-set normalization failed"));
      }
      if (!expr->block_literal_is_normalized) {
        diagnostics.push_back(
            MakeDiag(expr->line, expr->column, "O3S206", "type mismatch: block literal semantic surface is not normalized"));
      }
      if (expr->block_capture_count > 0u && expr->block_capture_profile.empty()) {
        diagnostics.push_back(
            MakeDiag(expr->line, expr->column, "O3S206", "type mismatch: block literal capture profile is missing"));
      }
      ValidateBlockLiteralCaptureLegality(expr, scopes, globals, diagnostics);
      return MakeCallableSemanticTypeFromBlockLiteral(*expr);
    }
#include "sema/objc3_semantic_passes_expression_statement_validation_callable_checks.inc"
    case Expr::Kind::MessageSend: {
      return ValidateMessageSendExpr(expr, scopes, globals, functions,
                                     diagnostics, max_message_send_args,
                                     message_send_context);
    }
  }
  return MakeScalarSemanticType(ValueType::Unknown);
}

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
  const Objc3ProtocolQualifiedMessageRequirementResolution protocol_resolution =
      ResolveProtocolQualifiedMessageRequirement(
          receiver_type.canonical_type, selector, false, message_send_context);
  if (protocol_resolution.status ==
      Objc3ProtocolQualifiedMessageRequirementResolution::Status::
          MissingRequirementClosure) {
    const unsigned diag_line =
        expr->receiver != nullptr ? expr->receiver->line : expr->line;
    const unsigned diag_column =
        expr->receiver != nullptr ? expr->receiver->column : expr->column;
    diagnostics.push_back(MakeDiag(
        diag_line, diag_column, "O3S216",
        "selector resolution failed: protocol-qualified receiver '" +
            receiver_type.canonical_type.canonical_spelling +
            "' cannot resolve selector '" + selector +
            "' because protocol '" + protocol_resolution.failing_protocol_name +
            "' requirements are unavailable"));
    return MakeScalarSemanticType(ValueType::Unknown);
  }
  if (protocol_resolution.status ==
      Objc3ProtocolQualifiedMessageRequirementResolution::Status::
          MissingSelector) {
    const unsigned diag_line =
        expr->receiver != nullptr ? expr->receiver->line : expr->line;
    const unsigned diag_column =
        expr->receiver != nullptr ? expr->receiver->column : expr->column;
    diagnostics.push_back(MakeDiag(
        diag_line, diag_column, "O3S216",
        "selector resolution failed: protocol-qualified receiver '" +
            receiver_type.canonical_type.canonical_spelling +
            "' does not provide selector '" +
            FormatMethodSelectorForDiagnostic(selector, false) + "'"));
    return MakeScalarSemanticType(ValueType::Unknown);
  }
  if (protocol_resolution.status ==
      Objc3ProtocolQualifiedMessageRequirementResolution::Status::
          AmbiguousSelector) {
    const unsigned diag_line =
        expr->receiver != nullptr ? expr->receiver->line : expr->line;
    const unsigned diag_column =
        expr->receiver != nullptr ? expr->receiver->column : expr->column;
    diagnostics.push_back(MakeDiag(
        diag_line, diag_column, "O3S217",
        "selector resolution is ambiguous for protocol-qualified receiver '" +
            receiver_type.canonical_type.canonical_spelling +
            "' and selector '" +
            FormatMethodSelectorForDiagnostic(selector, false) +
            "': incompatible protocol requirements"));
    return MakeScalarSemanticType(ValueType::Unknown);
  }
  if (protocol_resolution.status ==
          Objc3ProtocolQualifiedMessageRequirementResolution::Status::
              Resolved &&
      protocol_resolution.property_requirement != nullptr) {
    const Objc3PropertyInfo &property =
        protocol_resolution.property_requirement->property;
    const std::size_t expected_arity =
        protocol_resolution.property_requirement_is_setter ? 1u : 0u;
    if (expr->args.size() != expected_arity) {
      diagnostics.push_back(MakeDiag(
          expr->line, expr->column, "O3S216",
          "selector resolution failed: property accessor selector '" +
              selector + "' resolves to arity " +
              std::to_string(expected_arity) +
              " for protocol-qualified receiver '" +
              receiver_type.canonical_type.canonical_spelling + "'"));
      return MakeScalarSemanticType(ValueType::Unknown);
    }
    if (protocol_resolution.property_requirement_is_setter) {
      const SemanticTypeInfo arg_type = ValidateExpr(
          expr->args[0].get(), scopes, globals, functions, diagnostics,
          max_message_send_args, message_send_context);
      const SemanticTypeInfo expected =
          MakeSemanticTypeFromPropertyInfo(property);
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
            expr->args[0]->line, expr->args[0]->column, "O3S206",
            "type mismatch: property accessor selector '" + selector +
                "' on protocol-qualified receiver '" +
                receiver_type.canonical_type.canonical_spelling +
                "' expects '" + SemanticTypeName(expected) +
                "', got '" + SemanticTypeName(arg_type) + "'"));
      }
      return MakeScalarSemanticType(ValueType::Void);
    }
    return MakeSemanticTypeFromPropertyInfo(property);
  }
  if (protocol_resolution.status ==
          Objc3ProtocolQualifiedMessageRequirementResolution::Status::
              Resolved &&
      protocol_resolution.requirement != nullptr) {
    const Objc3MethodInfo &method = protocol_resolution.requirement->method;
    if (expr->args.size() != method.arity) {
      diagnostics.push_back(MakeDiag(
          expr->line, expr->column, "O3S216",
          "selector resolution failed: selector '" + selector +
              "' resolves to arity " + std::to_string(method.arity) +
              " for protocol-qualified receiver '" +
              receiver_type.canonical_type.canonical_spelling + "'"));
      return MakeScalarSemanticType(ValueType::Unknown);
    }
    for (std::size_t i = 0; i < expr->args.size(); ++i) {
      const SemanticTypeInfo arg_type = ValidateExpr(
          expr->args[i].get(), scopes, globals, functions, diagnostics,
          max_message_send_args, message_send_context);
      const SemanticTypeInfo expected =
          MakeSemanticTypeFromMethodInfoParam(method, i);
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
            "type mismatch: selector '" + selector +
                "' on protocol-qualified receiver '" +
                receiver_type.canonical_type.canonical_spelling +
                "' expects '" + SemanticTypeName(expected) +
                "' for argument " + std::to_string(i) + ", got '" +
                SemanticTypeName(arg_type) + "'"));
      }
    }
    return MakeSemanticTypeFromMethodInfoReturn(method);
  }
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

static void ValidateStatements(const std::vector<std::unique_ptr<Stmt>> &statements,
                               std::vector<SemanticScope> &scopes,
                               const std::unordered_map<std::string, ValueType> &globals,
                               const std::unordered_map<std::string, FunctionInfo> &functions,
                               const SemanticTypeInfo &expected_return_type, const std::string &function_name,
                               std::vector<std::string> &diagnostics, int loop_depth, int switch_depth,
                               int defer_loop_base_depth, int defer_switch_base_depth,
                               bool inside_defer_body,
                               std::size_t max_message_send_args,
                               const Objc3MessageSendResolutionContext &message_send_context);

static void ValidateStatement(
    const Stmt *stmt, std::vector<SemanticScope> &scopes,
    const std::unordered_map<std::string, ValueType> &globals,
    const std::unordered_map<std::string, FunctionInfo> &functions,
    const SemanticTypeInfo &expected_return_type,
    const std::string &function_name, std::vector<std::string> &diagnostics,
    int loop_depth, int switch_depth, int defer_loop_base_depth,
    int defer_switch_base_depth, bool inside_defer_body,
    std::size_t max_message_send_args,
    const Objc3MessageSendResolutionContext &message_send_context);

static void ValidateStatementsFromIndex(
    const std::vector<std::unique_ptr<Stmt>> &statements, std::size_t start_index,
    std::vector<SemanticScope> &scopes,
    const std::unordered_map<std::string, ValueType> &globals,
    const std::unordered_map<std::string, FunctionInfo> &functions,
    const SemanticTypeInfo &expected_return_type, const std::string &function_name,
    std::vector<std::string> &diagnostics, int loop_depth, int switch_depth,
    int defer_loop_base_depth, int defer_switch_base_depth,
    bool inside_defer_body,
    std::size_t max_message_send_args,
    const Objc3MessageSendResolutionContext &message_send_context) {
  for (std::size_t index = start_index; index < statements.size(); ++index) {
    ValidateStatement(statements[index].get(), scopes, globals, functions,
                      expected_return_type, function_name, diagnostics,
                      loop_depth, switch_depth, defer_loop_base_depth,
                      defer_switch_base_depth, inside_defer_body,
                      max_message_send_args,
                      message_send_context);
  }
}

static void ValidateAssignmentCompatibility(const std::string &target_name, const std::string &op,
                                           const Expr *value_expr, unsigned line, unsigned column,
                                           bool found_target,
                                           const SemanticTypeInfo &target_type,
                                           const SemanticTypeInfo &value_type,
                                           std::vector<std::string> &diagnostics,
                                           const Objc3MessageSendResolutionContext &message_send_context) {
  if (op == "=") {
    const bool target_known_scalar = IsScalarSemanticType(target_type) &&
                                     (target_type.type == ValueType::I32 || target_type.type == ValueType::Bool);
    const bool value_known_scalar = IsScalarSemanticType(value_type) &&
                                    (value_type.type == ValueType::I32 || value_type.type == ValueType::Bool);
    const bool target_known_objc_ref = IsObjCReferenceSemanticType(target_type);
    const bool value_known_objc_ref = IsObjCReferenceSemanticType(value_type);
    const bool same_non_objc_semantic_type =
        IsSameSemanticType(target_type, value_type) &&
        !(target_known_objc_ref && value_known_objc_ref);
    const std::optional<bool> generic_variance_compatibility =
        AreGenericSpecializationsVarianceAssignmentCompatible(
            target_type, value_type, message_send_context.surface);
    const bool assign_matches =
        same_non_objc_semantic_type ||
        AreScalarI32AliasCompatible(target_type, value_type) ||
        (target_known_scalar && value_known_scalar && target_type.type == ValueType::I32 &&
         value_type.type == ValueType::Bool) ||
        (target_known_scalar && value_known_scalar && target_type.type == ValueType::Bool &&
         value_type.type == ValueType::I32 && IsBoolLikeI32Literal(value_expr)) ||
        (target_known_objc_ref && value_known_objc_ref &&
         AreObjCReferenceTypesAssignmentCompatible(target_type, value_type,
                                                  message_send_context.surface)) ||
        IsEscapingBlockRuntimeHandleCompatible(target_type, value_type);
    if (IsEscapingBlockRuntimeHandleCompatible(target_type, value_type)) {
      DiagnoseEscapingBlockRuntimeHandleCaptureLegality(line, column, value_type,
                                                        diagnostics);
    }
    if (found_target && IsUnsafeNullableToNonnullFlow(
                            target_type, value_type,
                            message_send_context.surface)) {
      diagnostics.push_back(MakeDiag(
          line, column, "O3S227",
          "nullability mismatch: assignment to nonnull Objective-C reference '" +
              target_name + "' may receive nullable value '" +
              SemanticTypeName(value_type) + "'"));
      return;
    }
    if (found_target && generic_variance_compatibility.has_value() &&
        !*generic_variance_compatibility) {
      diagnostics.push_back(MakeDiag(
          line, column, "O3S206",
          "type mismatch: generic variance policy rejects assignment to '" +
              target_name + "'"));
      return;
    }
    if (found_target && target_known_scalar && !IsUnknownSemanticType(value_type) &&
        !value_known_scalar && !assign_matches) {
      diagnostics.push_back(MakeDiag(line, column, "O3S206",
                                     "type mismatch: assignment to '" + target_name + "' expects '" +
                                         SemanticTypeName(target_type) + "', got '" +
                                         SemanticTypeName(value_type) + "'; " +
                                         FormatAtomicMemoryOrderMappingHint(op)));
      return;
    }
    if (found_target && target_known_scalar && value_known_scalar && !assign_matches) {
      diagnostics.push_back(MakeDiag(line, column, "O3S206",
                                     "type mismatch: assignment to '" + target_name + "' expects '" +
                                         SemanticTypeName(target_type) + "', got '" +
                                         SemanticTypeName(value_type) + "'; " +
                                         FormatAtomicMemoryOrderMappingHint(op)));
      return;
    }

    if (found_target && target_type.is_vector && !IsUnknownSemanticType(value_type) && !assign_matches) {
      diagnostics.push_back(MakeDiag(line, column, "O3S206",
                                     "type mismatch: assignment to '" + target_name + "' expects '" +
                                         SemanticTypeName(target_type) + "', got '" +
                                         SemanticTypeName(value_type) + "'; " +
                                         FormatAtomicMemoryOrderMappingHint(op)));
    }
    if (found_target && target_known_objc_ref && !IsUnknownSemanticType(value_type) &&
        ((!value_known_objc_ref) ||
         (value_known_objc_ref && !assign_matches))) {
      diagnostics.push_back(MakeDiag(line, column, "O3S206",
                                     "type mismatch: assignment to '" + target_name + "' expects '" +
                                         SemanticTypeName(target_type) + "', got '" +
                                         SemanticTypeName(value_type) + "'; " +
                                         FormatAtomicMemoryOrderMappingHint(op)));
    }
    return;
  }

  if (!IsCompoundAssignmentOperator(op)) {
    if (op == "++" || op == "--") {
      if (found_target && !IsUnknownSemanticType(target_type) &&
          !IsScalarI32CompatibleType(target_type)) {
        diagnostics.push_back(MakeDiag(line, column, "O3S206",
                                       "type mismatch: update operator '" + op + "' target '" + target_name +
                                            "' must be 'i32', got '" + SemanticTypeName(target_type) + "'; " +
                                           FormatAtomicMemoryOrderMappingHint(op)));
      }
      return;
    }
    diagnostics.push_back(MakeDiag(line, column, "O3S206",
                                   "type mismatch: unsupported assignment operator '" + op + "'; " +
                                       FormatAtomicMemoryOrderMappingHint(op)));
    return;
  }
  if (!found_target) {
    return;
  }
  if (!IsUnknownSemanticType(target_type) && !IsScalarI32CompatibleType(target_type)) {
    diagnostics.push_back(MakeDiag(line, column, "O3S206",
                                   "type mismatch: compound assignment '" + op + "' target '" + target_name +
                                       "' must be 'i32', got '" + SemanticTypeName(target_type) + "'; " +
                                       FormatAtomicMemoryOrderMappingHint(op)));
  }
  if (IsScalarI32CompatibleType(target_type) &&
      !IsUnknownSemanticType(value_type) && !IsScalarI32CompatibleType(value_type)) {
    diagnostics.push_back(MakeDiag(line, column, "O3S206",
                                   "type mismatch: compound assignment '" + op + "' value for '" + target_name +
                                       "' must be 'i32', got '" + SemanticTypeName(value_type) + "'; " +
                                       FormatAtomicMemoryOrderMappingHint(op)));
  }
}

static void CollectAtomicMemoryOrderMappingsInStatements(const std::vector<std::unique_ptr<Stmt>> &statements,
                                                         Objc3AtomicMemoryOrderMappingSummary &summary);

static void CollectAtomicMemoryOrderMappingsInForClause(const ForClause &clause,
                                                        Objc3AtomicMemoryOrderMappingSummary &summary) {
  if (clause.kind != ForClause::Kind::Assign) {
    return;
  }
  RecordAtomicMemoryOrderMapping(clause.op, summary);
}

static void CollectAtomicMemoryOrderMappingsInStatement(const Stmt *stmt,
                                                        Objc3AtomicMemoryOrderMappingSummary &summary) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Assign:
      if (stmt->assign_stmt != nullptr) {
        RecordAtomicMemoryOrderMapping(stmt->assign_stmt->op, summary);
      }
      return;
    case Stmt::Kind::If:
      if (stmt->if_stmt != nullptr) {
        CollectAtomicMemoryOrderMappingsInStatements(stmt->if_stmt->then_body, summary);
        CollectAtomicMemoryOrderMappingsInStatements(stmt->if_stmt->else_body, summary);
      }
      return;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt != nullptr) {
        CollectAtomicMemoryOrderMappingsInStatements(stmt->do_while_stmt->body, summary);
      }
      return;
    case Stmt::Kind::For:
      if (stmt->for_stmt != nullptr) {
        CollectAtomicMemoryOrderMappingsInForClause(stmt->for_stmt->init, summary);
        CollectAtomicMemoryOrderMappingsInForClause(stmt->for_stmt->step, summary);
        CollectAtomicMemoryOrderMappingsInStatements(stmt->for_stmt->body, summary);
      }
      return;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt != nullptr) {
        for (const auto &case_stmt : stmt->switch_stmt->cases) {
          CollectAtomicMemoryOrderMappingsInStatements(case_stmt.body, summary);
        }
      }
      return;
    case Stmt::Kind::While:
      if (stmt->while_stmt != nullptr) {
        CollectAtomicMemoryOrderMappingsInStatements(stmt->while_stmt->body, summary);
      }
      return;
    case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
      if (stmt->block_stmt != nullptr) {
        CollectAtomicMemoryOrderMappingsInStatements(stmt->block_stmt->body, summary);
      }
      return;
    case Stmt::Kind::Let:
    case Stmt::Kind::Return:
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
    case Stmt::Kind::Expr:
      return;
  }
}

static void CollectAtomicMemoryOrderMappingsInStatements(const std::vector<std::unique_ptr<Stmt>> &statements,
                                                         Objc3AtomicMemoryOrderMappingSummary &summary) {
  for (const auto &stmt : statements) {
    CollectAtomicMemoryOrderMappingsInStatement(stmt.get(), summary);
  }
}

static bool StatementDefinitelyExitsScope(const Stmt *stmt);

static bool StatementListDefinitelyExitsScope(
    const std::vector<std::unique_ptr<Stmt>> &statements) {
  if (statements.empty()) {
    return false;
  }
  return StatementDefinitelyExitsScope(statements.back().get());
}

static bool StatementDefinitelyExitsScope(const Stmt *stmt) {
  if (stmt == nullptr) {
    return false;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Return:
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
      return true;
    case Stmt::Kind::Block:
      return stmt->block_stmt != nullptr &&
             StatementListDefinitelyExitsScope(stmt->block_stmt->body);
    case Stmt::Kind::Defer:
      return false;
    case Stmt::Kind::If:
      return stmt->if_stmt != nullptr &&
             StatementListDefinitelyExitsScope(stmt->if_stmt->then_body) &&
             StatementListDefinitelyExitsScope(stmt->if_stmt->else_body);
    case Stmt::Kind::Assign:
    case Stmt::Kind::Let:
    case Stmt::Kind::DoWhile:
    case Stmt::Kind::For:
    case Stmt::Kind::Switch:
    case Stmt::Kind::While:
    case Stmt::Kind::Empty:
    case Stmt::Kind::Expr:
      return false;
  }
  return false;
}

struct MatchExhaustivenessInfo {
  bool exhaustive = false;
  bool bool_exhaustive = false;
  bool result_case_exhaustive = false;
};

static bool IsMatchCatchAllPattern(const SwitchCase &case_stmt) {
  return case_stmt.is_default ||
         case_stmt.match_pattern_kind == MatchPatternKind::Wildcard ||
         case_stmt.match_pattern_kind == MatchPatternKind::Binding;
}

static MatchExhaustivenessInfo ClassifyMatchExhaustiveness(
    const SwitchStmt &switch_stmt) {
  MatchExhaustivenessInfo info;
  bool saw_true = false;
  bool saw_false = false;
  bool saw_ok = false;
  bool saw_err = false;
  for (const auto &case_stmt : switch_stmt.cases) {
    if (IsMatchCatchAllPattern(case_stmt)) {
      info.exhaustive = true;
      return info;
    }
    if (case_stmt.match_pattern_kind == MatchPatternKind::LiteralBool) {
      if (case_stmt.value == 0) {
        saw_false = true;
      } else {
        saw_true = true;
      }
    } else if (case_stmt.match_pattern_kind == MatchPatternKind::ResultCase) {
      if (case_stmt.match_result_case_name == "Ok") {
        saw_ok = true;
      } else if (case_stmt.match_result_case_name == "Err") {
        saw_err = true;
      }
    }
  }
  info.bool_exhaustive = saw_true && saw_false;
  info.result_case_exhaustive = saw_ok && saw_err;
  info.exhaustive = info.bool_exhaustive || info.result_case_exhaustive;
  return info;
}

struct BranchNonnullRefinement {
  std::string symbol_name;
  bool refine_then = false;
  bool refine_else = false;
};

static BranchNonnullRefinement AnalyzeNonnullBranchRefinement(
    const Expr *condition) {
  BranchNonnullRefinement refinement;
  if (condition == nullptr || condition->kind != Expr::Kind::Binary) {
    return refinement;
  }
  if (condition->op != "==" && condition->op != "!=") {
    return refinement;
  }

  const Expr *identifier_expr = nullptr;
  const Expr *nil_expr = nullptr;
  if (condition->left != nullptr && condition->left->kind == Expr::Kind::Identifier &&
      condition->right != nullptr &&
      condition->right->kind == Expr::Kind::NilLiteral) {
    identifier_expr = condition->left.get();
    nil_expr = condition->right.get();
  } else if (condition->right != nullptr &&
             condition->right->kind == Expr::Kind::Identifier &&
             condition->left != nullptr &&
             condition->left->kind == Expr::Kind::NilLiteral) {
    identifier_expr = condition->right.get();
    nil_expr = condition->left.get();
  }
  if (identifier_expr == nullptr || nil_expr == nullptr) {
    return refinement;
  }

  refinement.symbol_name = identifier_expr->ident;
  refinement.refine_then = condition->op == "!=";
  refinement.refine_else = condition->op == "==";
  return refinement;
}

static void ApplyNonnullRefinementToScope(
    const BranchNonnullRefinement &refinement, bool refine_active,
    const std::vector<SemanticScope> &source_scopes, SemanticScope &target_scope) {
  if (!refine_active || refinement.symbol_name.empty()) {
    return;
  }
  const SemanticTypeInfo current_type =
      ScopeLookupType(source_scopes, refinement.symbol_name);
  if (IsNullableObjCReferenceSemanticType(current_type)) {
    target_scope[refinement.symbol_name] =
        MakeNonnullRefinedSemanticType(current_type);
  }
}

static void ValidateStatement(const Stmt *stmt, std::vector<SemanticScope> &scopes,
                              const std::unordered_map<std::string, ValueType> &globals,
                              const std::unordered_map<std::string, FunctionInfo> &functions,
                              const SemanticTypeInfo &expected_return_type, const std::string &function_name,
                              std::vector<std::string> &diagnostics, int loop_depth, int switch_depth,
                              int defer_loop_base_depth, int defer_switch_base_depth,
                              bool inside_defer_body,
                              std::size_t max_message_send_args,
                              const Objc3MessageSendResolutionContext &message_send_context) {
  if (stmt == nullptr) {
    return;
  }
  const auto resolve_assignment_target_type = [&](const std::string &target_name, SemanticTypeInfo &target_type) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
      auto found = it->find(target_name);
      if (found != it->end()) {
        target_type = found->second;
        return true;
      }
    }
    auto global_it = globals.find(target_name);
    if (global_it != globals.end()) {
      target_type = MakeSemanticTypeFromGlobal(global_it->second);
      return true;
    }
    return false;
  };
  const auto validate_for_clause = [&](const ForClause &clause) {
    switch (clause.kind) {
      case ForClause::Kind::None:
        return;
      case ForClause::Kind::Expr:
        (void)ValidateExpr(clause.value.get(), scopes, globals, functions, diagnostics,
                           max_message_send_args, message_send_context);
        return;
      case ForClause::Kind::Let: {
        if (scopes.empty()) {
          return;
        }
        const SemanticTypeInfo value_type = ValidateExpr(
            clause.value.get(), scopes, globals, functions, diagnostics,
            max_message_send_args, message_send_context);
        if (scopes.back().find(clause.name) != scopes.back().end()) {
          diagnostics.push_back(MakeDiag(clause.line, clause.column, "O3S201",
                                         "duplicate declaration '" + clause.name + "'"));
        } else {
          scopes.back().emplace(clause.name, value_type);
        }
        return;
      }
      case ForClause::Kind::Assign: {
        if (scopes.empty()) {
          return;
        }
        SemanticTypeInfo target_type = MakeScalarSemanticType(ValueType::Unknown);
        const bool found_target = resolve_assignment_target_type(clause.name, target_type);
        if (!found_target) {
          diagnostics.push_back(MakeDiag(clause.line, clause.column, "O3S214",
                                         "invalid assignment target '" + clause.name +
                                             "': target must be a mutable symbol"));
        }
        const SemanticTypeInfo value_type = ValidateExpr(
            clause.value.get(), scopes, globals, functions, diagnostics,
            max_message_send_args, message_send_context);
        ValidateAssignmentCompatibility(clause.name, clause.op, clause.value.get(), clause.line, clause.column,
                                        found_target, target_type, value_type,
                                        diagnostics, message_send_context);
        return;
      }
    }
  };

  switch (stmt->kind) {
    case Stmt::Kind::Let: {
      const LetStmt *let = stmt->let_stmt.get();
      if (let == nullptr || scopes.empty()) {
        return;
      }
      const SemanticTypeInfo value_type = ValidateExpr(
          let->value.get(), scopes, globals, functions, diagnostics,
          max_message_send_args, message_send_context);
      if (scopes.back().find(let->name) != scopes.back().end()) {
        diagnostics.push_back(MakeDiag(let->line, let->column, "O3S201",
                                       "duplicate declaration '" + let->name + "'"));
      } else {
        scopes.back().emplace(let->name, value_type);
      }
      return;
    }
    case Stmt::Kind::Assign: {
      const AssignStmt *assign = stmt->assign_stmt.get();
      if (assign == nullptr || scopes.empty()) {
        return;
      }
      SemanticTypeInfo target_type = MakeScalarSemanticType(ValueType::Unknown);
      const bool found_target = resolve_assignment_target_type(assign->name, target_type);
      if (!found_target) {
        diagnostics.push_back(MakeDiag(assign->line, assign->column, "O3S214",
                                       "invalid assignment target '" + assign->name +
                                           "': target must be a mutable symbol"));
      }
      const SemanticTypeInfo value_type = ValidateExpr(
          assign->value.get(), scopes, globals, functions, diagnostics,
          max_message_send_args, message_send_context);
      ValidateAssignmentCompatibility(assign->name, assign->op, assign->value.get(), assign->line, assign->column,
                                      found_target, target_type, value_type,
                                      diagnostics, message_send_context);
      return;
    }
    case Stmt::Kind::Return:
      if (stmt->return_stmt != nullptr) {
        const ReturnStmt *ret = stmt->return_stmt.get();
        if (inside_defer_body) {
          diagnostics.push_back(MakeDiag(
              ret->line,
              ret->column,
              "O3S222",
              "control-flow misuse: 'return' is not allowed inside defer body"));
          if (ret->value != nullptr) {
            (void)ValidateExpr(ret->value.get(), scopes, globals, functions,
                               diagnostics, max_message_send_args,
                               message_send_context);
          }
          return;
        }
        if (ret->value == nullptr) {
          if (!(IsScalarSemanticType(expected_return_type) && expected_return_type.type == ValueType::Void)) {
            diagnostics.push_back(MakeDiag(ret->line, ret->column, "O3S211",
                                           "type mismatch: function '" + function_name + "' must return '" +
                                               SemanticTypeName(expected_return_type) + "'"));
          }
          return;
        }

        if (IsScalarSemanticType(expected_return_type) && expected_return_type.type == ValueType::Void) {
          diagnostics.push_back(MakeDiag(ret->line, ret->column, "O3S211",
                                         "type mismatch: void function '" + function_name +
                                             "' must use 'return;'"));
          (void)ValidateExpr(ret->value.get(), scopes, globals, functions, diagnostics,
                             max_message_send_args, message_send_context);
          return;
        }

        const SemanticTypeInfo return_type = ValidateExpr(
            ret->value.get(), scopes, globals, functions, diagnostics,
            max_message_send_args, message_send_context);
        if (IsEscapingBlockRuntimeHandleCompatible(expected_return_type,
                                                   return_type)) {
          DiagnoseEscapingBlockRuntimeHandleCaptureLegality(
              ret->line, ret->column, return_type, diagnostics);
        }
        if (IsUnsafeNullableToNonnullFlow(expected_return_type, return_type)) {
          diagnostics.push_back(MakeDiag(
              ret->line, ret->column, "O3S227",
              "nullability mismatch: return expression in function '" +
                  function_name +
                  "' may flow nullable value into nonnull Objective-C reference return"));
          return;
        }
        const bool return_matches = IsSameSemanticType(return_type, expected_return_type) ||
            AreScalarI32AliasCompatible(expected_return_type, return_type) ||
            (IsScalarSemanticType(expected_return_type) && IsScalarSemanticType(return_type) &&
             expected_return_type.type == ValueType::I32 && return_type.type == ValueType::Bool) ||
            (IsScalarSemanticType(expected_return_type) && IsScalarSemanticType(return_type) &&
             expected_return_type.type == ValueType::Bool && return_type.type == ValueType::I32 &&
             IsBoolLikeI32Literal(ret->value.get())) ||
            AreObjCReferenceTypesAssignmentCompatible(expected_return_type, return_type) ||
            IsEscapingBlockRuntimeHandleCompatible(expected_return_type, return_type);
        if (!return_matches && !IsUnknownSemanticType(return_type) &&
            !(IsScalarSemanticType(return_type) && return_type.type == ValueType::Function)) {
          diagnostics.push_back(MakeDiag(ret->line, ret->column, "O3S211",
                                         "type mismatch: return expression in function '" + function_name +
                                             "' must be '" + SemanticTypeName(expected_return_type) +
                                             "', got '" + SemanticTypeName(return_type) + "'"));
        }
      }
      return;
    case Stmt::Kind::Expr:
      if (stmt->expr_stmt != nullptr) {
        (void)ValidateExpr(stmt->expr_stmt->value.get(), scopes, globals, functions, diagnostics,
                           max_message_send_args, message_send_context);
      }
      return;
    case Stmt::Kind::If: {
      const IfStmt *if_stmt = stmt->if_stmt.get();
      if (if_stmt == nullptr) {
        return;
      }
      if (if_stmt->optional_binding_surface_enabled) {
        std::vector<std::pair<std::string, SemanticTypeInfo>> refined_bindings;
        const std::size_t binding_count = std::min(
            if_stmt->optional_binding_clause_count, if_stmt->then_body.size());
        for (std::size_t index = 0; index < binding_count; ++index) {
          const Stmt *binding_stmt = if_stmt->then_body[index].get();
          if (binding_stmt == nullptr || binding_stmt->kind != Stmt::Kind::Let ||
              binding_stmt->let_stmt == nullptr) {
            diagnostics.push_back(MakeDiag(
                if_stmt->line, if_stmt->column, "O3S206",
                "type mismatch: optional binding lowering must start with synthetic let clauses"));
            continue;
          }
          const SemanticTypeInfo value_type = ValidateExpr(
              binding_stmt->let_stmt->value.get(), scopes, globals, functions,
              diagnostics, max_message_send_args, message_send_context);
          if (!IsUnknownSemanticType(value_type) &&
              !IsNullableObjCReferenceSemanticType(value_type)) {
            diagnostics.push_back(MakeDiag(
                binding_stmt->let_stmt->line, binding_stmt->let_stmt->column,
                "O3S206",
                "type mismatch: optional binding requires an optional ObjC-reference-compatible source"));
            continue;
          }
          refined_bindings.emplace_back(
              binding_stmt->let_stmt->name,
              MakeNonnullRefinedSemanticType(value_type));
        }
        scopes.push_back({});
        for (const auto &binding : refined_bindings) {
          scopes.back()[binding.first] = binding.second;
        }
        for (const auto &guard_condition : if_stmt->guard_condition_exprs) {
          const SemanticTypeInfo condition_type = ValidateExpr(
              guard_condition.get(), scopes, globals, functions, diagnostics,
              max_message_send_args, message_send_context);
          if (!IsUnknownSemanticType(condition_type) &&
              !IsScalarBoolCompatibleType(condition_type)) {
            diagnostics.push_back(MakeDiag(
                if_stmt->line, if_stmt->column, "O3S206",
                "type mismatch: guard condition must be bool-compatible"));
          }
        }
        ValidateStatementsFromIndex(if_stmt->then_body, binding_count, scopes, globals,
                                    functions, expected_return_type, function_name,
                                    diagnostics, loop_depth, switch_depth,
                                    defer_loop_base_depth, defer_switch_base_depth,
                                    inside_defer_body,
                                    max_message_send_args, message_send_context);
        scopes.pop_back();
        scopes.push_back({});
        ValidateStatements(if_stmt->else_body, scopes, globals, functions,
                           expected_return_type, function_name, diagnostics,
                           loop_depth, switch_depth, defer_loop_base_depth,
                           defer_switch_base_depth, inside_defer_body,
                           max_message_send_args,
                           message_send_context);
        scopes.pop_back();
        if (if_stmt->guard_binding_surface_enabled) {
          if (!StatementListDefinitelyExitsScope(if_stmt->else_body)) {
            diagnostics.push_back(MakeDiag(
                if_stmt->line, if_stmt->column, "O3S206",
                "type mismatch: guard binding else block must exit the current scope"));
          }
          if (!scopes.empty()) {
            for (const auto &binding : refined_bindings) {
              scopes.back()[binding.first] = binding.second;
            }
          }
        }
      } else if (if_stmt->guard_condition_list_surface_enabled) {
        for (const auto &guard_condition : if_stmt->guard_condition_exprs) {
          const SemanticTypeInfo condition_type = ValidateExpr(
              guard_condition.get(), scopes, globals, functions, diagnostics,
              max_message_send_args, message_send_context);
          if (!IsUnknownSemanticType(condition_type) &&
              !IsScalarBoolCompatibleType(condition_type)) {
            diagnostics.push_back(MakeDiag(
                if_stmt->line, if_stmt->column, "O3S206",
                "type mismatch: guard condition must be bool-compatible"));
          }
        }
        scopes.push_back({});
        ValidateStatements(if_stmt->else_body, scopes, globals, functions,
                           expected_return_type, function_name, diagnostics,
                           loop_depth, switch_depth, defer_loop_base_depth,
                           defer_switch_base_depth, inside_defer_body,
                           max_message_send_args,
                           message_send_context);
        scopes.pop_back();
        if (!StatementListDefinitelyExitsScope(if_stmt->else_body)) {
          diagnostics.push_back(MakeDiag(
              if_stmt->line, if_stmt->column, "O3S206",
              "type mismatch: guard condition else block must exit the current scope"));
        }
      } else {
        const SemanticTypeInfo condition_type = ValidateExpr(
            if_stmt->condition.get(), scopes, globals, functions, diagnostics,
            max_message_send_args, message_send_context);
        if (!IsUnknownSemanticType(condition_type) && !IsScalarBoolCompatibleType(condition_type)) {
          diagnostics.push_back(MakeDiag(if_stmt->line, if_stmt->column, "O3S206",
                                         "type mismatch: if condition must be bool-compatible"));
        }
        const BranchNonnullRefinement branch_refinement =
            AnalyzeNonnullBranchRefinement(if_stmt->condition.get());
        scopes.push_back({});
        ApplyNonnullRefinementToScope(branch_refinement, branch_refinement.refine_then,
                                      scopes, scopes.back());
        ValidateStatements(if_stmt->then_body, scopes, globals, functions, expected_return_type, function_name,
                           diagnostics, loop_depth, switch_depth,
                           defer_loop_base_depth, defer_switch_base_depth,
                           inside_defer_body,
                           max_message_send_args, message_send_context);
        scopes.pop_back();
        scopes.push_back({});
        ApplyNonnullRefinementToScope(branch_refinement, branch_refinement.refine_else,
                                      scopes, scopes.back());
        ValidateStatements(if_stmt->else_body, scopes, globals, functions, expected_return_type, function_name,
                           diagnostics, loop_depth, switch_depth,
                           defer_loop_base_depth, defer_switch_base_depth,
                           inside_defer_body,
                           max_message_send_args, message_send_context);
        scopes.pop_back();
        return;
      }
      return;
