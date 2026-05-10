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
