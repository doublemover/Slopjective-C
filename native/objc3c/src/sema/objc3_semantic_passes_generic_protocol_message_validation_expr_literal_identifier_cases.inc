    case Expr::Kind::Number:
      return MakeScalarSemanticType(ValueType::I32);
    case Expr::Kind::BoolLiteral:
      return MakeScalarSemanticType(ValueType::Bool);
    case Expr::Kind::StringLiteral:
      return MakeScalarSemanticType(ValueType::TextHandle);
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
