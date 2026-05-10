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

#include "sema/objc3_semantic_passes_expression_statement_validation_message_send.inc"

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

#include "sema/objc3_semantic_passes_expression_statement_validation_assignment.inc"

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

#include "sema/objc3_semantic_passes_expression_statement_validation_statement_flow.inc"

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
