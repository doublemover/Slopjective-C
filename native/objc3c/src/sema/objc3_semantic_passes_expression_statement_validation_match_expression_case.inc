    case Expr::Kind::MatchExpression: {
      const SemanticTypeInfo scrutinee_type = ValidateExpr(
          expr->match_expression_scrutinee.get(), scopes, globals, functions,
          diagnostics, max_message_send_args, message_send_context,
          allow_throwing_call);
      const MatchExpressionExhaustivenessInfo exhaustiveness =
          ClassifyMatchExpressionExhaustiveness(*expr);
      expr->match_expression_exhaustive = exhaustiveness.exhaustive;
      expr->match_expression_result_typing_supported = false;
      expr->match_expression_lowering_eligible = false;
      expr->match_expression_result_type_mismatch = false;
      expr->match_expression_guard_effect_fail_closed = false;
      expr->match_expression_result_type_spelling.clear();

      if (!exhaustiveness.exhaustive) {
        diagnostics.push_back(MakeDiag(
            expr->line, expr->column, "O3S206",
            "type mismatch: match expression must be exhaustive for the admitted pattern surface"));
      }

      SemanticTypeInfo result_type = MakeScalarSemanticType(ValueType::Unknown);
      bool saw_result_type = false;
      bool result_type_mismatch = false;
      bool guard_effect_fail_closed = false;
      bool result_case_pattern_seen = false;

      for (const auto &arm : expr->match_expression_arms) {
        std::vector<SemanticScope> arm_scopes = scopes;
        arm_scopes.push_back({});
        if ((arm.pattern_kind == Expr::MatchExpressionPatternKind::Binding ||
             arm.pattern_kind == Expr::MatchExpressionPatternKind::ResultCase) &&
            !arm.binding_name.empty()) {
          arm_scopes.back()[arm.binding_name] = scrutinee_type;
        }
        if (arm.pattern_kind ==
            Expr::MatchExpressionPatternKind::ResultCase) {
          result_case_pattern_seen = true;
        }
        if (arm.has_guard) {
          if (MatchGuardHasUnsupportedSideEffect(
                  arm.guard_condition.get())) {
            guard_effect_fail_closed = true;
            diagnostics.push_back(MakeDiag(
                arm.guard_line, arm.guard_column, "O3S206",
                "type mismatch: match expression guard must be side-effect-free and may not call functions, send messages, throw, await, or nest match expressions"));
          }
          const SemanticTypeInfo guard_type = ValidateExpr(
              arm.guard_condition.get(), arm_scopes, globals, functions,
              diagnostics, max_message_send_args, message_send_context,
              allow_throwing_call);
          if (!IsUnknownSemanticType(guard_type) &&
              (guard_type.is_vector || guard_type.type != ValueType::Bool)) {
            diagnostics.push_back(MakeDiag(
                arm.guard_line, arm.guard_column, "O3S206",
                "type mismatch: match expression guard condition must be bool"));
          }
        }
        const SemanticTypeInfo arm_type = ValidateExpr(
            arm.value.get(), arm_scopes, globals, functions, diagnostics,
            max_message_send_args, message_send_context, allow_throwing_call);

        if (IsUnknownSemanticType(arm_type)) {
          continue;
        }
        if (!saw_result_type || IsUnknownSemanticType(result_type)) {
          result_type = arm_type;
          saw_result_type = true;
          continue;
        }
        const bool scalar_pair =
            IsScalarSemanticType(result_type) &&
            IsScalarSemanticType(arm_type) &&
            ((result_type.type == ValueType::I32 ||
              result_type.type == ValueType::Bool) &&
             (arm_type.type == ValueType::I32 ||
              arm_type.type == ValueType::Bool));
        if (scalar_pair) {
          if (result_type.type != arm_type.type) {
            result_type = MakeScalarSemanticType(ValueType::I32);
          }
          continue;
        }
        if (AreScalarI32AliasCompatible(result_type, arm_type)) {
          if (result_type.type != arm_type.type) {
            result_type = MakeScalarSemanticType(ValueType::I32);
          }
          continue;
        }
        if (AreObjCReferenceTypesAssignmentCompatible(result_type, arm_type)) {
          if (result_type.type != arm_type.type) {
            result_type = MakeScalarSemanticType(ValueType::ObjCId);
          }
          continue;
        }
        if (!IsSameSemanticType(result_type, arm_type)) {
          result_type_mismatch = true;
          diagnostics.push_back(MakeDiag(
              arm.line, arm.column, "O3S206",
              "type mismatch: match expression arms must produce one compatible result type"));
        }
      }

      expr->match_expression_result_type_mismatch = result_type_mismatch;
      expr->match_expression_guard_effect_fail_closed =
          guard_effect_fail_closed;
      if (saw_result_type && !result_type_mismatch) {
        expr->match_expression_result_typing_supported = true;
        expr->match_expression_result_type_spelling =
            SemanticTypeName(result_type);
      }
      const bool scalar_lowering_type =
          IsScalarSemanticType(result_type) &&
          (result_type.type == ValueType::I32 ||
           result_type.type == ValueType::Bool ||
           result_type.type == ValueType::ObjCId ||
           result_type.type == ValueType::TextHandle);
      expr->match_expression_lowering_eligible =
          exhaustiveness.exhaustive && saw_result_type &&
          !result_type_mismatch && !guard_effect_fail_closed &&
          scalar_lowering_type && !result_case_pattern_seen;
      return saw_result_type ? result_type
                             : MakeScalarSemanticType(ValueType::Unknown);
    }
