    return false;
  }
  std::size_t cursor = object_name.size();
  while (cursor < trimmed.size() &&
         std::isspace(static_cast<unsigned char>(trimmed[cursor])) != 0) {
    ++cursor;
  }
  if (cursor >= trimmed.size() || trimmed[cursor] != '<') {
    return true;
  }
  int depth = 0;
  for (std::size_t index = cursor; index < trimmed.size(); ++index) {
    const char c = trimmed[index];
    if (c == '<') {
      ++depth;
    } else if (c == '>') {
      --depth;
      if (depth == 0) {
        generic_suffix_text = trimmed.substr(cursor, index - cursor + 1u);
        return true;
      }
      if (depth < 0) {
        return false;
      }
    }
  }
  return false;
}

#include "sema/objc3_semantic_passes_generic_protocol_message_validation_generic_argument_protocol_relations.inc"

#include "sema/objc3_semantic_passes_generic_protocol_specialization_validation.inc"

static void ValidateProtocolCompositionIdentifierBindings(
    const Objc3Program &ast,
    const std::unordered_map<std::string, Objc3ProtocolSemanticDefinition>
        &definitions,
    std::vector<std::string> &diagnostics) {
  auto validate_suffix =
      [&](bool has_generic_suffix, bool supports_generic_suffix,
          const std::string &suffix_text, unsigned line, unsigned column,
          const std::string &context) {
        if (!has_generic_suffix || !supports_generic_suffix) {
          return;
        }
        const ProtocolCompositionParseResult parsed =
            ParseProtocolCompositionSuffixText(suffix_text);
        if (!parsed.IsValid()) {
          return;
        }
        for (const auto &protocol_name : parsed.names_lexicographic) {
          if (definitions.find(protocol_name) == definitions.end()) {
            diagnostics.push_back(MakeDiag(
                line, column, "O3S206",
                "type mismatch: protocol-qualified type for " + context +
                    " references unknown protocol '" + protocol_name + "'"));
          }
        }
      };

  auto validate_property = [&](const Objc3PropertyDecl &property,
                               const std::string &context) {
    validate_suffix(property.has_generic_suffix,
                    SupportsGenericPropertyTypeSuffix(property),
                    property.generic_suffix_text, property.generic_line,
                    property.generic_column, context);
  };

  auto validate_method = [&](const Objc3MethodDecl &method,
                             const std::string &context) {
    validate_suffix(method.has_return_generic_suffix,
                    SupportsGenericReturnTypeSuffix(method),
                    method.return_generic_suffix_text,
                    method.return_generic_line, method.return_generic_column,
                    context + " return type");
    for (const auto &param : method.params) {
      validate_suffix(param.has_generic_suffix,
                      SupportsGenericParamTypeSuffix(param),
                      param.generic_suffix_text, param.generic_line,
                      param.generic_column,
                      context + " parameter '" + param.name + "'");
    }
  };

  for (const auto &fn : ast.functions) {
    validate_suffix(fn.has_return_generic_suffix,
                    SupportsGenericReturnTypeSuffix(fn),
                    fn.return_generic_suffix_text, fn.return_generic_line,
                    fn.return_generic_column,
                    "function '" + fn.name + "' return type");
    for (const auto &param : fn.params) {
      validate_suffix(param.has_generic_suffix,
                      SupportsGenericParamTypeSuffix(param),
                      param.generic_suffix_text, param.generic_line,
                      param.generic_column,
                      "function '" + fn.name + "' parameter '" +
                          param.name + "'");
    }
  }

  for (const auto &protocol_decl : ast.protocols) {
    for (const auto &property : protocol_decl.properties) {
      validate_property(property, "protocol '" + protocol_decl.name +
                                      "' property '" + property.name + "'");
    }
    for (const auto &method : protocol_decl.methods) {
      validate_method(method, "protocol '" + protocol_decl.name +
                                  "' selector '" + MethodSelectorName(method) +
                                  "'");
    }
  }

  for (const auto &interface_decl : ast.interfaces) {
    const std::string container_name = FormatObjcContainerName(
        interface_decl.name, interface_decl.has_category,
        interface_decl.category_name);
    const std::string container_label =
        interface_decl.has_category ? "category interface" : "interface";
    for (const auto &property : interface_decl.properties) {
      validate_property(property, container_label + " '" + container_name +
                                      "' property '" + property.name + "'");
    }
    for (const auto &method : interface_decl.methods) {
      validate_method(method, container_label + " '" + container_name +
                                  "' selector '" + MethodSelectorName(method) +
                                  "'");
    }
  }

  for (const auto &implementation_decl : ast.implementations) {
    const std::string container_name = FormatObjcContainerName(
        implementation_decl.name, implementation_decl.has_category,
        implementation_decl.category_name);
    const std::string container_label = implementation_decl.has_category
                                            ? "category implementation"
                                            : "implementation";
    for (const auto &property : implementation_decl.properties) {
      validate_property(property, container_label + " '" + container_name +
                                      "' property '" + property.name + "'");
    }
    for (const auto &method : implementation_decl.methods) {
      validate_method(method, container_label + " '" + container_name +
                                  "' selector '" + MethodSelectorName(method) +
                                  "'");
    }
  }
}

// Legacy extraction anchors retained for contract tests:
// static ValueType ValidateMessageSendExpr(
// if (receiver_type != ValueType::Unknown && !IsMessageCompatibleType(receiver_type)) {
static SemanticTypeInfo ValidateMessageSendExpr(const Expr *expr,
                                                const std::vector<SemanticScope> &scopes,
                                                const std::unordered_map<std::string, ValueType> &globals,
                                                const std::unordered_map<std::string, FunctionInfo> &functions,
                                                std::vector<std::string> &diagnostics,
                                                std::size_t max_message_send_args,
                                                const Objc3MessageSendResolutionContext &message_send_context);

#include "sema/objc3_semantic_passes_generic_protocol_message_validation_block_captures.inc"

static SemanticTypeInfo ValidateExpr(const Expr *expr, const std::vector<SemanticScope> &scopes,
                                     const std::unordered_map<std::string, ValueType> &globals,
                                     const std::unordered_map<std::string, FunctionInfo> &functions,
                                     std::vector<std::string> &diagnostics,
                                     std::size_t max_message_send_args,
                                     const Objc3MessageSendResolutionContext &message_send_context) {
  if (expr == nullptr) {
    return MakeScalarSemanticType(ValueType::Unknown);
  }
  if (expr->await_expression_enabled &&
      !message_send_context.inside_async_context) {
    diagnostics.push_back(MakeDiag(
        expr->line, expr->column, "O3S223",
        "async semantics failed: 'await' is only valid inside an async function or method"));
  }
  switch (expr->kind) {
    case Expr::Kind::Number:
      return MakeScalarSemanticType(ValueType::I32);
    case Expr::Kind::BoolLiteral:
      return MakeScalarSemanticType(ValueType::Bool);
    case Expr::Kind::NilLiteral: {
      SemanticTypeInfo nil_type = MakeScalarSemanticType(ValueType::ObjCId);
      nil_type.canonical_type.nullability =
          Objc3SemanticCanonicalNullability::Nullable;
      nil_type.has_nullability_suffix = true;
      return nil_type;
    }
    case Expr::Kind::Identifier: {
      if (expr->typed_keypath_literal_enabled) {
        if (!expr->typed_keypath_literal_is_normalized ||
            expr->typed_keypath_components.empty()) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: typed key-path literal normalization failed"));
          return MakeScalarSemanticType(ValueType::Unknown);
        }
        std::string validated_owner_name;
        bool is_class_root = false;
        if (!TryResolveTypedKeyPathValidatedOwner(
                *expr, scopes, globals, functions, message_send_context,
                validated_owner_name, is_class_root)) {
          diagnostics.push_back(MakeDiag(
              expr->line, expr->column, "O3S206",
              "type mismatch: typed key-path root '" +
                  expr->typed_keypath_root_name +
                  "' must resolve to 'self', a known class type, or an ObjC-reference-compatible identifier"));
          return MakeScalarSemanticType(ValueType::Unknown);
        }
        if (is_class_root) {
          if (expr->typed_keypath_components.size() != 1u) {
            diagnostics.push_back(MakeDiag(
                expr->line, expr->column, "O3S206",
                "type mismatch: typed key-path member chain '" +
                    JoinStringVector(expr->typed_keypath_components, ".") +
                    "' is unsupported until executable typed key-path lowering is enabled"));
            return MakeScalarSemanticType(ValueType::Unknown);
          }
          const std::string &component = expr->typed_keypath_components.front();
          if (FindTypedKeyPathPropertyOnOwner(
                  *message_send_context.surface, validated_owner_name,
                  component) == nullptr) {
            diagnostics.push_back(MakeDiag(
                expr->line, expr->column, "O3S206",
                "type mismatch: typed key-path component '" + component +
                    "' is not a readable property on root '" +
                    validated_owner_name + "'"));
            return MakeScalarSemanticType(ValueType::Unknown);
          }
        }
        return MakeScalarSemanticType(ValueType::ObjCId);
      }
      if (expr->ident == "super" && !message_send_context.inside_method) {
        diagnostics.push_back(MakeDiag(
            expr->line, expr->column, "O3S216",
            "selector resolution failed: 'super' is only valid inside an implementation method"));
        return MakeScalarSemanticType(ValueType::Unknown);
      }
      const SemanticTypeInfo local_type = ScopeLookupType(scopes, expr->ident);
      if (!IsUnknownSemanticType(local_type)) {
        return local_type;
      }
      auto global_it = globals.find(expr->ident);
      if (global_it != globals.end()) {
        return MakeSemanticTypeFromGlobal(global_it->second);
      }
      if (functions.find(expr->ident) != functions.end()) {
        diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                       "type mismatch: function '" + expr->ident +
                                           "' cannot be used as a value"));
        return MakeScalarSemanticType(ValueType::Function);
      }
      diagnostics.push_back(
          MakeDiag(expr->line, expr->column, "O3S202", "undefined identifier '" + expr->ident + "'"));
      return MakeScalarSemanticType(ValueType::Unknown);
    }
    case Expr::Kind::Binary: {
      const SemanticTypeInfo lhs = ValidateExpr(
          expr->left.get(), scopes, globals, functions, diagnostics,
          max_message_send_args, message_send_context);
      const SemanticTypeInfo rhs = ValidateExpr(
          expr->right.get(), scopes, globals, functions, diagnostics,
          max_message_send_args, message_send_context);

      if (expr->op == "??") {
        const bool lhs_reference_like =
            IsObjCReferenceSemanticType(lhs) || IsCallableSemanticType(lhs);
        const bool rhs_reference_like =
            IsObjCReferenceSemanticType(rhs) || IsCallableSemanticType(rhs);
        if (!IsUnknownSemanticType(lhs) && !lhs_reference_like) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: nil-coalescing lhs must be ObjC-reference-compatible"));
        }
        if (!IsUnknownSemanticType(rhs) && !rhs_reference_like) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: nil-coalescing rhs must be ObjC-reference-compatible"));
        }
        if (IsUnknownSemanticType(lhs)) {
          return rhs;
        }
        if (IsUnknownSemanticType(rhs)) {
          return MakeNonnullRefinedSemanticType(lhs);
        }
        if (AreObjCReferenceTypesAssignmentCompatible(lhs, rhs)) {
          if (lhs.type == rhs.type) {
            return rhs.has_nullability_suffix ? lhs : MakeNonnullRefinedSemanticType(lhs);
          }
          return MakeNonnullRefinedSemanticType(MakeScalarSemanticType(ValueType::ObjCId));
        }
        if (IsCallableSemanticType(lhs) && IsCallableSemanticType(rhs) &&
            lhs.callable_param_types == rhs.callable_param_types &&
            lhs.callable_return_type == rhs.callable_return_type) {
          return lhs;
        }
        diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                       "type mismatch: nil-coalescing operands must be type-compatible"));
        return MakeScalarSemanticType(ValueType::Unknown);
      }

      if (expr->op == "+" || expr->op == "-" || expr->op == "*" || expr->op == "/" || expr->op == "%") {
        if (!IsUnknownSemanticType(lhs) && !IsScalarI32CompatibleType(lhs)) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for arithmetic lhs, got '" +
                                             SemanticTypeName(lhs) + "'"));
        }
        if (!IsUnknownSemanticType(rhs) && !IsScalarI32CompatibleType(rhs)) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for arithmetic rhs, got '" +
                                             SemanticTypeName(rhs) + "'"));
        }
        return MakeScalarSemanticType(ValueType::I32);
      }

      if (expr->op == "&" || expr->op == "|" || expr->op == "^" || expr->op == "<<" || expr->op == ">>") {
        if (!IsUnknownSemanticType(lhs) && !IsScalarI32CompatibleType(lhs)) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for bitwise lhs, got '" +
                                             SemanticTypeName(lhs) + "'"));
        }
        if (!IsUnknownSemanticType(rhs) && !IsScalarI32CompatibleType(rhs)) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for bitwise rhs, got '" +
                                             SemanticTypeName(rhs) + "'"));
        }
        return MakeScalarSemanticType(ValueType::I32);
      }

      if (expr->op == "==" || expr->op == "!=") {
        if (lhs.is_vector || rhs.is_vector) {
          if (!IsUnknownSemanticType(lhs) && !IsUnknownSemanticType(rhs) && !IsSameSemanticType(lhs, rhs)) {
            diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                           "type mismatch: equality compares '" + SemanticTypeName(lhs) +
                                               "' with '" + SemanticTypeName(rhs) + "'"));
          }
          return MakeScalarSemanticType(ValueType::Bool);
        }

        const bool bool_to_i32_literal =
            (lhs.type == ValueType::Bool && rhs.type == ValueType::I32 && IsBoolLikeI32Literal(expr->right.get())) ||
            (rhs.type == ValueType::Bool && lhs.type == ValueType::I32 && IsBoolLikeI32Literal(expr->left.get()));
        const bool i32_alias_coercion = AreScalarI32AliasCompatible(lhs, rhs);
        if (!IsUnknownSemanticType(lhs) && !IsUnknownSemanticType(rhs) && lhs.type != rhs.type &&
            !bool_to_i32_literal && !i32_alias_coercion) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: equality compares '" + SemanticTypeName(lhs) +
                                             "' with '" + SemanticTypeName(rhs) + "'"));
        }
        return MakeScalarSemanticType(ValueType::Bool);
      }

      if (expr->op == "<" || expr->op == "<=" || expr->op == ">" || expr->op == ">=") {
        if (!IsUnknownSemanticType(lhs) && !IsScalarI32CompatibleType(lhs)) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for relational lhs, got '" +
                                             SemanticTypeName(lhs) + "'"));
        }
        if (!IsUnknownSemanticType(rhs) && !IsScalarI32CompatibleType(rhs)) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for relational rhs, got '" +
                                             SemanticTypeName(rhs) + "'"));
        }
        return MakeScalarSemanticType(ValueType::Bool);
      }

      if (expr->op == "&&" || expr->op == "||") {
        if (!IsUnknownSemanticType(lhs) && (lhs.is_vector || (lhs.type != ValueType::Bool && lhs.type != ValueType::I32))) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected bool for logical lhs, got '" +
                                             SemanticTypeName(lhs) + "'"));
        }
        if (!IsUnknownSemanticType(rhs) && (rhs.is_vector || (rhs.type != ValueType::Bool && rhs.type != ValueType::I32))) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected bool for logical rhs, got '" +
                                             SemanticTypeName(rhs) + "'"));
        }
        return MakeScalarSemanticType(ValueType::Bool);
      }

      return MakeScalarSemanticType(ValueType::Unknown);
    }
    case Expr::Kind::Conditional: {
      if (expr->left == nullptr || expr->right == nullptr || expr->third == nullptr) {
        return MakeScalarSemanticType(ValueType::Unknown);
      }

      const SemanticTypeInfo condition_type = ValidateExpr(
          expr->left.get(), scopes, globals, functions, diagnostics,
          max_message_send_args, message_send_context);
      if (!IsUnknownSemanticType(condition_type) && !IsScalarBoolCompatibleType(condition_type)) {
        diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                       "type mismatch: conditional condition must be bool-compatible"));
      }

      const SemanticTypeInfo then_type = ValidateExpr(
          expr->right.get(), scopes, globals, functions, diagnostics,
          max_message_send_args, message_send_context);
      const SemanticTypeInfo else_type = ValidateExpr(
          expr->third.get(), scopes, globals, functions, diagnostics,
          max_message_send_args, message_send_context);

      if (IsUnknownSemanticType(then_type)) {
        return else_type;
      }
      if (IsUnknownSemanticType(else_type)) {
        return then_type;
      }
      const bool then_scalar = IsScalarSemanticType(then_type) &&
                               (then_type.type == ValueType::I32 || then_type.type == ValueType::Bool);
      const bool else_scalar = IsScalarSemanticType(else_type) &&
                               (else_type.type == ValueType::I32 || else_type.type == ValueType::Bool);
      if (then_scalar && else_scalar) {
        if (then_type.type == else_type.type) {
          return then_type;
        }
        return MakeScalarSemanticType(ValueType::I32);
      }
      if (AreScalarI32AliasCompatible(then_type, else_type)) {
        if (then_type.type == else_type.type) {
          return then_type;
        }
        return MakeScalarSemanticType(ValueType::I32);
