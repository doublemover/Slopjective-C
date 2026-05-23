    case Expr::Kind::Number:
      return MakeScalarSemanticType(ValueType::I32);
    case Expr::Kind::BoolLiteral:
      return MakeScalarSemanticType(ValueType::Bool);
    case Expr::Kind::StringLiteral:
      return MakeScalarSemanticType(ValueType::TextHandle);
    case Expr::Kind::StringInterpolation: {
      expr->string_interpolation_payload_is_text.clear();
      expr->string_interpolation_payload_is_text.reserve(expr->args.size());
      for (const auto &payload : expr->args) {
        const SemanticTypeInfo payload_type = ValidateExpr(
            payload.get(), scopes, globals, functions, diagnostics,
            max_message_send_args, message_send_context);
        const bool payload_is_text =
            IsScalarSemanticType(payload_type) &&
            payload_type.type == ValueType::TextHandle;
        const bool payload_is_i32 =
            IsScalarSemanticType(payload_type) &&
            payload_type.type == ValueType::I32;
        expr->string_interpolation_payload_is_text.push_back(payload_is_text);
        if (!IsUnknownSemanticType(payload_type) && !payload_is_text &&
            !payload_is_i32) {
          diagnostics.push_back(MakeDiag(
              payload != nullptr ? payload->line : expr->line,
              payload != nullptr ? payload->column : expr->column, "O3S206",
              "type mismatch: string interpolation payloads support only Text and i32"));
        }
      }
      return MakeScalarSemanticType(ValueType::TextHandle);
    }
    case Expr::Kind::NilLiteral: {
      SemanticTypeInfo nil_type = MakeScalarSemanticType(ValueType::ObjCId);
      nil_type.canonical_type.nullability =
          Objc3SemanticCanonicalNullability::Nullable;
      nil_type.has_nullability_suffix = true;
      return nil_type;
    }
    case Expr::Kind::KeyPathLiteral:
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
                    objc3c::support::JoinStringVector(
                        expr->typed_keypath_components, ".") +
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
      if (const SemanticTypeInfo *local_type =
              ScopeLookupTypeOrNull(scopes, expr->ident)) {
        return *local_type;
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
