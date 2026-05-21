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
  if (expr->kind == Expr::Kind::KeyPathLiteral &&
      expr->typed_keypath_literal_enabled) {
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
    case Stmt::Kind::ForIn:
      if (stmt->for_in_stmt != nullptr) {
        CollectTypeSystemTypeSemanticExprSites(
            stmt->for_in_stmt->collection.get(), scopes, globals, functions,
            max_message_send_args, message_send_context, summary);
        scopes.push_back({});
        CollectTypeSystemTypeSemanticStmtSitesFromList(
            stmt->for_in_stmt->body, scopes, globals, functions,
            max_message_send_args, message_send_context, summary);
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
    case Stmt::Kind::CollectionMutation:
      if (stmt->collection_mutation_stmt != nullptr) {
        CollectTypeSystemTypeSemanticExprSites(
            stmt->collection_mutation_stmt->key_or_index.get(), scopes,
            globals, functions, max_message_send_args, message_send_context,
            summary);
        CollectTypeSystemTypeSemanticExprSites(
            stmt->collection_mutation_stmt->value.get(), scopes, globals,
            functions, max_message_send_args, message_send_context, summary);
      }
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}
