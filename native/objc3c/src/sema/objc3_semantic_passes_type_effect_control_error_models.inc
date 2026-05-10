    }
    case Stmt::Kind::DoWhile: {
      const DoWhileStmt *do_while_stmt = stmt->do_while_stmt.get();
      if (do_while_stmt == nullptr) {
        return;
      }
      scopes.push_back({});
      ValidateStatements(do_while_stmt->body, scopes, globals, functions, expected_return_type, function_name,
                         diagnostics, loop_depth + 1, switch_depth,
                         defer_loop_base_depth, defer_switch_base_depth,
                         inside_defer_body,
                         max_message_send_args, message_send_context);
      scopes.pop_back();

      const SemanticTypeInfo condition_type = ValidateExpr(
          do_while_stmt->condition.get(), scopes, globals, functions, diagnostics,
          max_message_send_args, message_send_context);
      if (!IsUnknownSemanticType(condition_type) && !IsScalarBoolCompatibleType(condition_type)) {
        diagnostics.push_back(MakeDiag(do_while_stmt->line, do_while_stmt->column, "O3S206",
                                       "type mismatch: do-while condition must be bool-compatible"));
      }
      return;
    }
    case Stmt::Kind::For: {
      const ForStmt *for_stmt = stmt->for_stmt.get();
      if (for_stmt == nullptr) {
        return;
      }
      scopes.push_back({});
      validate_for_clause(for_stmt->init);
      if (for_stmt->condition != nullptr) {
        const SemanticTypeInfo condition_type = ValidateExpr(
            for_stmt->condition.get(), scopes, globals, functions, diagnostics,
            max_message_send_args, message_send_context);
        if (!IsUnknownSemanticType(condition_type) && !IsScalarBoolCompatibleType(condition_type)) {
          diagnostics.push_back(MakeDiag(for_stmt->line, for_stmt->column, "O3S206",
                                         "type mismatch: for condition must be bool-compatible"));
        }
      }
      validate_for_clause(for_stmt->step);
      scopes.push_back({});
      ValidateStatements(for_stmt->body, scopes, globals, functions, expected_return_type, function_name, diagnostics,
                         loop_depth + 1, switch_depth, defer_loop_base_depth,
                         defer_switch_base_depth, inside_defer_body,
                         max_message_send_args, message_send_context);
      scopes.pop_back();
      scopes.pop_back();
      return;
    }
    case Stmt::Kind::Switch: {
      const SwitchStmt *switch_stmt = stmt->switch_stmt.get();
      if (switch_stmt == nullptr) {
        return;
      }
      const SemanticTypeInfo condition_type = ValidateExpr(
          switch_stmt->condition.get(), scopes, globals, functions, diagnostics,
          max_message_send_args, message_send_context);
      if (switch_stmt->match_surface_enabled) {
        const MatchExhaustivenessInfo exhaustiveness =
            ClassifyMatchExhaustiveness(*switch_stmt);
        if (!exhaustiveness.exhaustive) {
          diagnostics.push_back(MakeDiag(
              switch_stmt->line, switch_stmt->column, "O3S206",
              "type mismatch: match statement must be exhaustive for the admitted pattern surface"));
        }
        for (const auto &case_stmt : switch_stmt->cases) {
          scopes.push_back({});
          if ((case_stmt.match_pattern_kind == MatchPatternKind::Binding ||
               case_stmt.match_pattern_kind == MatchPatternKind::ResultCase) &&
              !case_stmt.match_binding_name.empty()) {
            scopes.back()[case_stmt.match_binding_name] = condition_type;
          }
          ValidateStatements(case_stmt.body, scopes, globals, functions,
                             expected_return_type, function_name, diagnostics,
                             loop_depth, switch_depth + 1,
                             defer_loop_base_depth, defer_switch_base_depth,
                             inside_defer_body,
                             max_message_send_args, message_send_context);
          scopes.pop_back();
        }
        return;
      }
      if (!IsUnknownSemanticType(condition_type) && !IsScalarBoolCompatibleType(condition_type)) {
        diagnostics.push_back(MakeDiag(switch_stmt->line, switch_stmt->column, "O3S206",
                                       "type mismatch: switch condition must be i32-compatible"));
      }

      std::unordered_set<int> seen_case_values;
      bool seen_default = false;
      for (const auto &case_stmt : switch_stmt->cases) {
        if (case_stmt.is_default) {
          if (seen_default) {
            diagnostics.push_back(MakeDiag(case_stmt.line, case_stmt.column, "O3S206",
                                           "type mismatch: duplicate default label in switch"));
          }
          seen_default = true;
        } else {
          if (!seen_case_values.insert(case_stmt.value).second) {
            diagnostics.push_back(MakeDiag(case_stmt.value_line, case_stmt.value_column, "O3S206",
                                           "type mismatch: duplicate case label '" +
                                               std::to_string(case_stmt.value) + "' in switch"));
          }
        }
        scopes.push_back({});
        ValidateStatements(case_stmt.body, scopes, globals, functions, expected_return_type, function_name,
                           diagnostics, loop_depth, switch_depth + 1,
                           defer_loop_base_depth, defer_switch_base_depth,
                           inside_defer_body,
                           max_message_send_args, message_send_context);
        scopes.pop_back();
      }
      return;
    }
    case Stmt::Kind::While: {
      const WhileStmt *while_stmt = stmt->while_stmt.get();
      if (while_stmt == nullptr) {
        return;
      }
      const SemanticTypeInfo condition_type = ValidateExpr(
          while_stmt->condition.get(), scopes, globals, functions, diagnostics,
          max_message_send_args, message_send_context);
      if (!IsUnknownSemanticType(condition_type) && !IsScalarBoolCompatibleType(condition_type)) {
        diagnostics.push_back(MakeDiag(while_stmt->line, while_stmt->column, "O3S206",
                                       "type mismatch: while condition must be bool-compatible"));
      }
      scopes.push_back({});
      ValidateStatements(while_stmt->body, scopes, globals, functions, expected_return_type, function_name,
                         diagnostics, loop_depth + 1, switch_depth,
                         defer_loop_base_depth, defer_switch_base_depth,
                         inside_defer_body,
                         max_message_send_args, message_send_context);
      scopes.pop_back();
      return;
    }
    case Stmt::Kind::Block:
    {
      const BlockStmt *block_stmt = stmt->block_stmt.get();
      if (block_stmt == nullptr) {
        return;
      }
      scopes.push_back({});
      ValidateStatements(block_stmt->body, scopes, globals, functions, expected_return_type, function_name,
                         diagnostics, loop_depth, switch_depth,
                         defer_loop_base_depth, defer_switch_base_depth,
                         inside_defer_body,
                         max_message_send_args, message_send_context);
      scopes.pop_back();
      return;
    }
    case Stmt::Kind::Defer: {
      const BlockStmt *block_stmt = stmt->block_stmt.get();
      if (block_stmt == nullptr) {
        return;
      }
      scopes.push_back({});
      ValidateStatements(block_stmt->body, scopes, globals, functions,
                         expected_return_type, function_name, diagnostics,
                         loop_depth, switch_depth, loop_depth, switch_depth,
                         true, max_message_send_args, message_send_context);
      scopes.pop_back();
      return;
    }
    case Stmt::Kind::Break:
      if (inside_defer_body && loop_depth <= defer_loop_base_depth &&
          switch_depth <= defer_switch_base_depth) {
        diagnostics.push_back(MakeDiag(
            stmt->line,
            stmt->column,
            "O3S222",
            "control-flow misuse: 'break' may not exit a defer body"));
        return;
      }
      if (loop_depth <= 0 && switch_depth <= 0) {
        diagnostics.push_back(MakeDiag(stmt->line, stmt->column, "O3S212", "loop-control misuse: 'break' outside loop"));
      }
      return;
    case Stmt::Kind::Continue:
      if (inside_defer_body && loop_depth <= defer_loop_base_depth) {
        diagnostics.push_back(MakeDiag(
            stmt->line,
            stmt->column,
            "O3S222",
            "control-flow misuse: 'continue' may not exit a defer body"));
        return;
      }
      if (loop_depth <= 0) {
        diagnostics.push_back(
            MakeDiag(stmt->line, stmt->column, "O3S213", "loop-control misuse: 'continue' outside loop"));
      }
      return;
    case Stmt::Kind::Empty:
      return;
  }
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
                               const Objc3MessageSendResolutionContext &message_send_context) {
  for (const auto &stmt : statements) {
    ValidateStatement(stmt.get(), scopes, globals, functions, expected_return_type, function_name, diagnostics,
                      loop_depth, switch_depth, defer_loop_base_depth,
                      defer_switch_base_depth, inside_defer_body,
                      max_message_send_args,
                      message_send_context);
  }
}

static void CollectTypeSystemTypeSemanticExprSites(
    const Expr *expr, const std::vector<SemanticScope> &scopes,
    const std::unordered_map<std::string, ValueType> &globals,
    const std::unordered_map<std::string, FunctionInfo> &functions,
    std::size_t max_message_send_args,
    const Objc3MessageSendResolutionContext &message_send_context,
    Objc3TypeSystemTypeSemanticModelSummary &summary) {
  if (expr == nullptr) {
    return;
  }

  if (expr->kind == Expr::Kind::Binary && expr->op == "??") {
    ++summary.nil_coalescing_sites;
    ++summary.optional_propagation_sites;
  }
  if (expr->kind == Expr::Kind::MessageSend && expr->optional_send_enabled) {
    ++summary.optional_send_sites;
  }
  if (expr->kind == Expr::Kind::Identifier && expr->typed_keypath_literal_enabled) {
    ++summary.typed_keypath_literal_sites;
    if (expr->typed_keypath_root_is_self) {
      ++summary.typed_keypath_self_root_sites;
    }
    std::string validated_owner_name;
    bool is_class_root = false;
    const bool root_is_legal = TryResolveTypedKeyPathValidatedOwner(
        *expr, scopes, globals, functions, message_send_context,
        validated_owner_name, is_class_root);
    if (is_class_root) {
      ++summary.typed_keypath_class_root_sites;
    }
    const bool has_valid_shape =
        expr->typed_keypath_literal_is_normalized &&
        !expr->typed_keypath_root_name.empty() &&
        !expr->typed_keypath_components.empty() &&
        std::all_of(expr->typed_keypath_components.begin(),
                    expr->typed_keypath_components.end(),
                    [](const std::string &component) { return !component.empty(); });
    if (!has_valid_shape) {
      ++summary.typed_keypath_contract_violation_sites;
    }
    if (!root_is_legal) {
      ++summary.typed_keypath_root_legality_violation_sites;
      ++summary.typed_keypath_contract_violation_sites;
    } else if (is_class_root &&
               (expr->typed_keypath_components.size() != 1u ||
                FindTypedKeyPathPropertyOnOwner(
                    *message_send_context.surface, validated_owner_name,
                    expr->typed_keypath_components.front()) == nullptr)) {
      ++summary.typed_keypath_member_path_contract_violation_sites;
      ++summary.typed_keypath_contract_violation_sites;
    }
  }

  CollectTypeSystemTypeSemanticExprSites(expr->receiver.get(), scopes, globals, functions,
                                    max_message_send_args, message_send_context,
                                    summary);
  CollectTypeSystemTypeSemanticExprSites(expr->left.get(), scopes, globals, functions,
                                    max_message_send_args, message_send_context,
                                    summary);
  CollectTypeSystemTypeSemanticExprSites(expr->right.get(), scopes, globals, functions,
                                    max_message_send_args, message_send_context,
                                    summary);
  CollectTypeSystemTypeSemanticExprSites(expr->third.get(), scopes, globals, functions,
                                    max_message_send_args, message_send_context,
                                    summary);
  for (const auto &arg : expr->args) {
    CollectTypeSystemTypeSemanticExprSites(arg.get(), scopes, globals, functions,
                                      max_message_send_args,
                                      message_send_context, summary);
  }
}

static void CollectTypeSystemTypeSemanticStmtSites(
    const Stmt *stmt, std::vector<SemanticScope> &scopes,
    const std::unordered_map<std::string, ValueType> &globals,
    const std::unordered_map<std::string, FunctionInfo> &functions,
    std::size_t max_message_send_args,
    const Objc3MessageSendResolutionContext &message_send_context,
    Objc3TypeSystemTypeSemanticModelSummary &summary);

static void CollectTypeSystemTypeSemanticStmtSitesFromList(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<SemanticScope> &scopes,
    const std::unordered_map<std::string, ValueType> &globals,
    const std::unordered_map<std::string, FunctionInfo> &functions,
    std::size_t max_message_send_args,
    const Objc3MessageSendResolutionContext &message_send_context,
    Objc3TypeSystemTypeSemanticModelSummary &summary) {
  for (const auto &stmt : statements) {
    CollectTypeSystemTypeSemanticStmtSites(stmt.get(), scopes, globals, functions,
                                      max_message_send_args,
                                      message_send_context, summary);
  }
}

static void CollectTypeSystemTypeSemanticStmtSites(
    const Stmt *stmt, std::vector<SemanticScope> &scopes,
    const std::unordered_map<std::string, ValueType> &globals,
    const std::unordered_map<std::string, FunctionInfo> &functions,
    std::size_t max_message_send_args,
    const Objc3MessageSendResolutionContext &message_send_context,
    Objc3TypeSystemTypeSemanticModelSummary &summary) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
    case Stmt::Kind::Let: {
      if (stmt->let_stmt == nullptr || scopes.empty()) {
        return;
      }
      CollectTypeSystemTypeSemanticExprSites(stmt->let_stmt->value.get(), scopes, globals,
                                        functions, max_message_send_args,
                                        message_send_context, summary);
      return;
    }
    case Stmt::Kind::Assign:
      if (stmt->assign_stmt != nullptr) {
        CollectTypeSystemTypeSemanticExprSites(stmt->assign_stmt->value.get(), scopes,
                                          globals, functions,
                                          max_message_send_args,
                                          message_send_context, summary);
      }
      return;
    case Stmt::Kind::Return:
      if (stmt->return_stmt != nullptr) {
        CollectTypeSystemTypeSemanticExprSites(stmt->return_stmt->value.get(), scopes,
                                          globals, functions,
                                          max_message_send_args,
                                          message_send_context, summary);
      }
      return;
    case Stmt::Kind::Expr:
      if (stmt->expr_stmt != nullptr) {
        CollectTypeSystemTypeSemanticExprSites(stmt->expr_stmt->value.get(), scopes,
                                          globals, functions,
                                          max_message_send_args,
                                          message_send_context, summary);
      }
      return;
    case Stmt::Kind::If: {
      const IfStmt *if_stmt = stmt->if_stmt.get();
      if (if_stmt == nullptr) {
        return;
      }
      if (if_stmt->optional_binding_surface_enabled) {
        ++summary.optional_binding_sites;
        summary.optional_binding_clause_sites +=
            if_stmt->optional_binding_clause_count;
        summary.optional_flow_refinement_sites +=
            if_stmt->optional_binding_clause_count;
        if (if_stmt->guard_binding_surface_enabled) {
          ++summary.guard_binding_sites;
          ++summary.guard_binding_exit_enforcement_sites;
          if (!StatementListDefinitelyExitsScope(if_stmt->else_body)) {
            ++summary.optional_flow_contract_violation_sites;
          }
        }
        const std::size_t binding_count = std::min(
            if_stmt->optional_binding_clause_count, if_stmt->then_body.size());
        for (std::size_t index = 0; index < binding_count; ++index) {
          const Stmt *binding_stmt = if_stmt->then_body[index].get();
          if (binding_stmt == nullptr || binding_stmt->kind != Stmt::Kind::Let ||
              binding_stmt->let_stmt == nullptr) {
            ++summary.optional_binding_contract_violation_sites;
            continue;
          }
          CollectTypeSystemTypeSemanticExprSites(binding_stmt->let_stmt->value.get(),
                                            scopes, globals, functions,
                                            max_message_send_args,
                                            message_send_context, summary);
        }
      } else {
        CollectTypeSystemTypeSemanticExprSites(if_stmt->condition.get(), scopes, globals,
                                          functions, max_message_send_args,
                                          message_send_context, summary);
        const BranchNonnullRefinement refinement =
            AnalyzeNonnullBranchRefinement(if_stmt->condition.get());
        if (!refinement.symbol_name.empty()) {
          ++summary.optional_flow_refinement_sites;
        }
      }
      scopes.push_back({});
      CollectTypeSystemTypeSemanticStmtSitesFromList(if_stmt->then_body, scopes, globals,
                                                functions,
                                                max_message_send_args,
                                                message_send_context, summary);
      scopes.pop_back();
      scopes.push_back({});
      CollectTypeSystemTypeSemanticStmtSitesFromList(if_stmt->else_body, scopes, globals,
                                                functions,
                                                max_message_send_args,
                                                message_send_context, summary);
      scopes.pop_back();
      return;
    }
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt != nullptr) {
        scopes.push_back({});
        CollectTypeSystemTypeSemanticStmtSitesFromList(stmt->do_while_stmt->body, scopes,
                                                  globals, functions,
                                                  max_message_send_args,
                                                  message_send_context, summary);
        scopes.pop_back();
        CollectTypeSystemTypeSemanticExprSites(stmt->do_while_stmt->condition.get(),
                                          scopes, globals, functions,
                                          max_message_send_args,
                                          message_send_context, summary);
      }
      return;
    case Stmt::Kind::For:
      if (stmt->for_stmt != nullptr) {
        scopes.push_back({});
        switch (stmt->for_stmt->init.kind) {
          case ForClause::Kind::Expr:
          case ForClause::Kind::Let:
          case ForClause::Kind::Assign:
            CollectTypeSystemTypeSemanticExprSites(stmt->for_stmt->init.value.get(), scopes,
                                              globals, functions,
                                              max_message_send_args,
                                              message_send_context, summary);
            break;
          case ForClause::Kind::None:
            break;
        }
        CollectTypeSystemTypeSemanticExprSites(stmt->for_stmt->condition.get(), scopes,
                                          globals, functions,
                                          max_message_send_args,
                                          message_send_context, summary);
        switch (stmt->for_stmt->step.kind) {
          case ForClause::Kind::Expr:
          case ForClause::Kind::Let:
          case ForClause::Kind::Assign:
            CollectTypeSystemTypeSemanticExprSites(stmt->for_stmt->step.value.get(), scopes,
                                              globals, functions,
                                              max_message_send_args,
                                              message_send_context, summary);
            break;
          case ForClause::Kind::None:
            break;
        }
        scopes.push_back({});
        CollectTypeSystemTypeSemanticStmtSitesFromList(stmt->for_stmt->body, scopes, globals,
                                                  functions,
                                                  max_message_send_args,
                                                  message_send_context, summary);
        scopes.pop_back();
        scopes.pop_back();
      }
      return;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt != nullptr) {
        CollectTypeSystemTypeSemanticExprSites(stmt->switch_stmt->condition.get(), scopes,
                                          globals, functions,
                                          max_message_send_args,
                                          message_send_context, summary);
        for (const auto &case_stmt : stmt->switch_stmt->cases) {
          scopes.push_back({});
          CollectTypeSystemTypeSemanticStmtSitesFromList(case_stmt.body, scopes, globals,
                                                    functions,
                                                    max_message_send_args,
                                                    message_send_context, summary);
          scopes.pop_back();
        }
      }
      return;
    case Stmt::Kind::While:
      if (stmt->while_stmt != nullptr) {
        CollectTypeSystemTypeSemanticExprSites(stmt->while_stmt->condition.get(), scopes,
                                          globals, functions,
                                          max_message_send_args,
                                          message_send_context, summary);
        scopes.push_back({});
        CollectTypeSystemTypeSemanticStmtSitesFromList(stmt->while_stmt->body, scopes,
                                                  globals, functions,
                                                  max_message_send_args,
                                                  message_send_context, summary);
        scopes.pop_back();
      }
      return;
    case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
      if (stmt->block_stmt != nullptr) {
        scopes.push_back({});
        CollectTypeSystemTypeSemanticStmtSitesFromList(stmt->block_stmt->body, scopes,
                                                  globals, functions,
                                                  max_message_send_args,
                                                  message_send_context, summary);
        scopes.pop_back();
      }
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

static void AccumulateCanonicalTypeSemanticModelSummary(
    const Objc3SemanticCanonicalType &type,
    Objc3TypeSystemTypeSemanticModelSummary &summary) {
  ++summary.canonical_type_entries;
  if (type.kind == Objc3SemanticCanonicalTypeKind::Object ||
      type.kind == Objc3SemanticCanonicalTypeKind::ClassObject ||
      type.kind == Objc3SemanticCanonicalTypeKind::ProtocolObject ||
      type.kind == Objc3SemanticCanonicalTypeKind::Instancetype ||
      type.kind == Objc3SemanticCanonicalTypeKind::ObjectPointer ||
      type.kind == Objc3SemanticCanonicalTypeKind::ForeignObject) {
    ++summary.canonical_object_type_entries;
  }
  if (type.has_protocol_composition) {
    ++summary.canonical_protocol_qualified_entries;
  }
  summary.canonical_generic_argument_entries +=
      type.generic_arguments_lexicographic.size();
  switch (type.nullability) {
    case Objc3SemanticCanonicalNullability::Nullable:
      ++summary.canonical_nullable_entries;
      break;
    case Objc3SemanticCanonicalNullability::Nonnull:
      ++summary.canonical_nonnull_entries;
      break;
    case Objc3SemanticCanonicalNullability::ImplicitlyUnwrapped:
      ++summary.canonical_implicitly_unwrapped_entries;
      break;
    case Objc3SemanticCanonicalNullability::NullResettable:
      ++summary.canonical_null_resettable_entries;
      break;
    case Objc3SemanticCanonicalNullability::Inherited:
    case Objc3SemanticCanonicalNullability::Unspecified:
      ++summary.canonical_unspecified_nullability_entries;
      break;
  }
  if (type.has_invalid_type_suffix || !type.deterministic) {
    ++summary.canonical_invalid_type_entries;
  }
}

static void AccumulateCanonicalTypeSemanticModelSummaryFromSurface(
    const Objc3SemanticIntegrationSurface &surface,
    Objc3TypeSystemTypeSemanticModelSummary &summary) {
  for (const auto &function_entry : surface.functions) {
    const FunctionInfo &function = function_entry.second;
    for (const auto &param_type : function.param_canonical_types) {
      AccumulateCanonicalTypeSemanticModelSummary(param_type, summary);
    }
    AccumulateCanonicalTypeSemanticModelSummary(function.return_canonical_type,
                                               summary);
  }
  const auto accumulate_container =
      [&summary](const auto &container_map) {
        for (const auto &container_entry : container_map) {
          for (const auto &property_entry : container_entry.second.properties) {
            AccumulateCanonicalTypeSemanticModelSummary(
                property_entry.second.canonical_type, summary);
          }
          for (const auto &method_entry : container_entry.second.methods) {
            const Objc3MethodInfo &method = method_entry.second;
            for (const auto &param_type : method.param_canonical_types) {
              AccumulateCanonicalTypeSemanticModelSummary(param_type, summary);
            }
            AccumulateCanonicalTypeSemanticModelSummary(
                method.return_canonical_type, summary);
          }
        }
      };
  accumulate_container(surface.interfaces);
  accumulate_container(surface.implementations);
  accumulate_container(surface.category_interfaces);
  accumulate_container(surface.category_implementations);
}

Objc3TypeSystemTypeSemanticModelSummary BuildTypeSystemTypeSemanticModelSummary(
    const Objc3Program &ast, const Objc3SemanticIntegrationSurface &surface,
    std::size_t max_message_send_args) {
  Objc3TypeSystemTypeSemanticModelSummary summary;
  AccumulateCanonicalTypeSemanticModelSummaryFromSurface(surface, summary);
  summary.object_pointer_semantic_sites =
      surface.type_annotation_surface_summary.object_pointer_type_sites;
  summary.protocol_composition_semantic_sites =
      surface.protocol_qualified_object_type_summary.protocol_composition_sites;
  summary.generic_suffix_semantic_sites =
      surface.type_annotation_surface_summary.generic_suffix_sites;
  summary.generic_erasure_semantic_sites =
      surface.generic_metadata_abi_summary.generic_metadata_abi_sites;
  summary.nullability_suffix_semantic_sites =
      surface.type_annotation_surface_summary.nullability_suffix_sites;
  summary.nullability_semantic_sites =
      surface.nullability_flow_warning_precision_summary.nullability_flow_sites;
  summary.invalid_generic_suffix_semantic_sites =
      surface.type_annotation_surface_summary.invalid_generic_suffix_sites;
  summary.invalid_nullability_suffix_semantic_sites =
      surface.type_annotation_surface_summary.invalid_nullability_suffix_sites;
  summary.invalid_protocol_composition_semantic_sites =
      surface.protocol_qualified_object_type_summary.contract_violation_sites;
  std::unordered_map<std::string, ValueType> body_globals = surface.globals;
  for (const auto &interface_decl : ast.interfaces) {
    body_globals.try_emplace(interface_decl.name, ValueType::ObjCClass);
  }
  for (const auto &implementation_decl : ast.implementations) {
    body_globals.try_emplace(implementation_decl.name, ValueType::ObjCClass);
  }
  for (const auto &function : ast.functions) {
    std::vector<SemanticScope> scopes;
    scopes.push_back({});
    for (const auto &param : function.params) {
      scopes.back().emplace(param.name, MakeSemanticTypeFromParam(param));
    }
    Objc3MessageSendResolutionContext context;
    context.surface = &surface;
    CollectTypeSystemTypeSemanticStmtSitesFromList(function.body, scopes, body_globals,
                                              surface.functions,
                                              max_message_send_args, context,
                                              summary);
  }
  for (const auto &implementation : ast.implementations) {
    std::string super_name;
    auto interface_it = surface.interfaces.find(implementation.name);
    if (interface_it != surface.interfaces.end()) {
      super_name = interface_it->second.super_name;
    }
    for (const auto &method : implementation.methods) {
      std::vector<SemanticScope> scopes;
      scopes.push_back({});
      scopes.back().emplace("self",
                            MakeScalarSemanticType(ValueType::ObjCInstancetype));
      if (!super_name.empty()) {
        scopes.back().emplace("super",
                              MakeScalarSemanticType(ValueType::ObjCInstancetype));
      }
      for (const auto &param : method.params) {
        scopes.back().emplace(param.name, MakeSemanticTypeFromParam(param));
      }
      Objc3MessageSendResolutionContext context;
      context.inside_method = true;
      context.surface = &surface;
      context.current_implementation_name = implementation.name;
      context.current_super_name = super_name;
      CollectTypeSystemTypeSemanticStmtSitesFromList(method.body, scopes, body_globals,
                                                surface.functions,
                                                max_message_send_args, context,
                                                summary);
    }
  }
  summary.deterministic =
      summary.optional_binding_contract_violation_sites <=
          summary.optional_binding_clause_sites &&
      summary.optional_send_contract_violation_sites <= summary.optional_send_sites &&
      summary.optional_flow_contract_violation_sites <=
          (summary.optional_flow_refinement_sites +
           summary.guard_binding_exit_enforcement_sites) &&
      summary.typed_keypath_root_legality_violation_sites <=
          summary.typed_keypath_literal_sites &&
      summary.typed_keypath_member_path_contract_violation_sites <=
          summary.typed_keypath_literal_sites &&
      summary.typed_keypath_contract_violation_sites <=
          summary.typed_keypath_literal_sites &&
      summary.typed_keypath_self_root_sites <= summary.typed_keypath_literal_sites &&
      summary.typed_keypath_class_root_sites <= summary.typed_keypath_literal_sites &&
      summary.protocol_composition_semantic_sites <=
          summary.object_pointer_semantic_sites &&
      summary.generic_suffix_semantic_sites <=
          summary.object_pointer_semantic_sites &&
      summary.nullability_suffix_semantic_sites <=
          summary.object_pointer_semantic_sites &&
      summary.invalid_generic_suffix_semantic_sites <=
          summary.generic_suffix_semantic_sites &&
      summary.invalid_nullability_suffix_semantic_sites <=
          summary.nullability_suffix_semantic_sites &&
      summary.invalid_protocol_composition_semantic_sites <=
          summary.protocol_composition_semantic_sites &&
      summary.canonical_invalid_type_entries <= summary.canonical_type_entries &&
      summary.canonical_object_type_entries <= summary.canonical_type_entries &&
      summary.canonical_protocol_qualified_entries <=
          summary.canonical_type_entries &&
      summary.canonical_nullable_entries +
              summary.canonical_nonnull_entries +
              summary.canonical_implicitly_unwrapped_entries +
              summary.canonical_null_resettable_entries +
              summary.canonical_unspecified_nullability_entries ==
          summary.canonical_type_entries &&
      summary.optional_propagation_sites == summary.nil_coalescing_sites &&
      summary.guard_binding_exit_enforcement_sites <=
          summary.guard_binding_sites;
  summary.ready_for_lowering_and_runtime = summary.deterministic;
  std::ostringstream out;
  out << summary.contract_id << ";bindings=" << summary.optional_binding_sites
      << ";clauses=" << summary.optional_binding_clause_sites
      << ";guards=" << summary.guard_binding_sites
      << ";optional-sends=" << summary.optional_send_sites
      << ";coalescing=" << summary.nil_coalescing_sites
      << ";propagation=" << summary.optional_propagation_sites
      << ";refinement=" << summary.optional_flow_refinement_sites
      << ";guard-exit=" << summary.guard_binding_exit_enforcement_sites
      << ";keypaths=" << summary.typed_keypath_literal_sites
      << ";keypath-self=" << summary.typed_keypath_self_root_sites
      << ";keypath-class=" << summary.typed_keypath_class_root_sites
      << ";object-pointers=" << summary.object_pointer_semantic_sites
      << ";protocol-compositions="
      << summary.protocol_composition_semantic_sites
      << ";generic-suffixes=" << summary.generic_suffix_semantic_sites
      << ";generic-erasure=" << summary.generic_erasure_semantic_sites
      << ";nullability-suffixes="
      << summary.nullability_suffix_semantic_sites
      << ";nullability=" << summary.nullability_semantic_sites
      << ";canonical-types=" << summary.canonical_type_entries
      << ";canonical-object-types="
      << summary.canonical_object_type_entries
      << ";canonical-protocol-qualified="
      << summary.canonical_protocol_qualified_entries
      << ";canonical-generics="
      << summary.canonical_generic_argument_entries
      << ";canonical-nullable=" << summary.canonical_nullable_entries
      << ";canonical-nonnull=" << summary.canonical_nonnull_entries
      << ";canonical-iuo="
      << summary.canonical_implicitly_unwrapped_entries
      << ";canonical-null-resettable="
      << summary.canonical_null_resettable_entries
      << ";canonical-unspecified-nullability="
      << summary.canonical_unspecified_nullability_entries
      << ";canonical-invalid=" << summary.canonical_invalid_type_entries
      << ";invalid-generic-suffixes="
      << summary.invalid_generic_suffix_semantic_sites
      << ";invalid-nullability-suffixes="
      << summary.invalid_nullability_suffix_semantic_sites
      << ";invalid-protocol-compositions="
      << summary.invalid_protocol_composition_semantic_sites
      << ";binding-violations=" << summary.optional_binding_contract_violation_sites
      << ";optional-send-violations=" << summary.optional_send_contract_violation_sites
      << ";flow-violations=" << summary.optional_flow_contract_violation_sites
      << ";keypath-root-violations="
      << summary.typed_keypath_root_legality_violation_sites
      << ";keypath-member-violations="
      << summary.typed_keypath_member_path_contract_violation_sites
      << ";keypath-violations=" << summary.typed_keypath_contract_violation_sites;
  summary.replay_key = out.str();
  return summary;
}

Objc3ErrorHandlingErrorSemanticModelSummary BuildErrorHandlingErrorSemanticModelSummary(
    const Objc3FrontendErrorHandlingErrorSourceClosureSummary &source_summary,
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3ErrorHandlingErrorSemanticModelSummary summary;
  summary.function_throws_declaration_sites =
      source_summary.function_throws_declaration_sites;
  summary.method_throws_declaration_sites =
      source_summary.method_throws_declaration_sites;
  summary.throws_declaration_sites =
      summary.function_throws_declaration_sites +
      summary.method_throws_declaration_sites;
  summary.result_like_sites = surface.result_like_lowering_summary.result_like_sites;
  summary.result_success_sites =
      surface.result_like_lowering_summary.result_success_sites;
  summary.result_failure_sites =
      surface.result_like_lowering_summary.result_failure_sites;
  summary.result_branch_sites =
      surface.result_like_lowering_summary.result_branch_sites;
  summary.result_payload_sites =
      surface.result_like_lowering_summary.result_payload_sites;
  summary.ns_error_bridging_sites =
      surface.ns_error_bridging_summary.ns_error_bridging_sites;
  summary.ns_error_out_parameter_sites =
      surface.ns_error_bridging_summary.ns_error_out_parameter_sites;
  summary.ns_error_bridge_path_sites =
      surface.ns_error_bridging_summary.ns_error_bridge_path_sites;
  summary.objc_nserror_attribute_sites =
      source_summary.objc_nserror_attribute_sites;
