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
