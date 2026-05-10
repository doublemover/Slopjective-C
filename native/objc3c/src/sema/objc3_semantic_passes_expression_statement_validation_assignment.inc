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
