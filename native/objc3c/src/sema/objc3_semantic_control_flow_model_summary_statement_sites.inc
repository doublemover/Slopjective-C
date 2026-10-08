static void CollectControlFlowControlFlowSemanticStmtSitesFromList(
    const std::vector<std::unique_ptr<Stmt>> &statements, int loop_depth,
    int switch_depth, int defer_loop_base_depth, int defer_switch_base_depth,
    bool inside_defer_body,
    Objc3ControlFlowControlFlowSemanticModelSummary &summary);

static void CollectControlFlowControlFlowSemanticExprSites(
    const Expr *expr, Objc3ControlFlowControlFlowSemanticModelSummary &summary);

static void CollectControlFlowControlFlowSemanticExprListSites(
    const std::vector<std::unique_ptr<Expr>> &expressions,
    Objc3ControlFlowControlFlowSemanticModelSummary &summary) {
  for (const auto &expr : expressions) {
    CollectControlFlowControlFlowSemanticExprSites(expr.get(), summary);
  }
}

static void CollectControlFlowControlFlowSemanticExprSites(
    const Expr *expr, Objc3ControlFlowControlFlowSemanticModelSummary &summary) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
  case Expr::Kind::Binary:
    CollectControlFlowControlFlowSemanticExprSites(expr->left.get(), summary);
    CollectControlFlowControlFlowSemanticExprSites(expr->right.get(), summary);
    return;
  case Expr::Kind::Conditional:
    CollectControlFlowControlFlowSemanticExprSites(expr->left.get(), summary);
    CollectControlFlowControlFlowSemanticExprSites(expr->right.get(), summary);
    CollectControlFlowControlFlowSemanticExprSites(expr->third.get(), summary);
    return;
  case Expr::Kind::CollectionLiteral:
    CollectControlFlowControlFlowSemanticExprListSites(expr->collection_keys,
                                                       summary);
    CollectControlFlowControlFlowSemanticExprListSites(expr->collection_values,
                                                       summary);
    return;
  case Expr::Kind::IndexAccess:
    CollectControlFlowControlFlowSemanticExprSites(expr->left.get(), summary);
    CollectControlFlowControlFlowSemanticExprSites(expr->right.get(), summary);
    return;
  case Expr::Kind::Call:
  case Expr::Kind::Try:
  case Expr::Kind::Throw:
  case Expr::Kind::StringInterpolation:
    CollectControlFlowControlFlowSemanticExprListSites(expr->args, summary);
    return;
  case Expr::Kind::MessageSend:
    CollectControlFlowControlFlowSemanticExprSites(expr->receiver.get(),
                                                   summary);
    CollectControlFlowControlFlowSemanticExprListSites(expr->args, summary);
    return;
  case Expr::Kind::BlockLiteral:
    CollectControlFlowControlFlowSemanticStmtSitesFromList(
        expr->block_body, 0, 0, 0, 0, false, summary);
    return;
  case Expr::Kind::MatchExpression: {
    ++summary.match_expression_semantic_sites;
    const MatchExpressionExhaustivenessInfo exhaustiveness =
        ClassifyMatchExpressionExhaustiveness(*expr);
    if (exhaustiveness.exhaustive) {
      ++summary.match_expression_exhaustive_sites;
    } else {
      ++summary.match_expression_non_exhaustive_diagnostic_sites;
    }
    if (expr->match_expression_result_typing_supported) {
      ++summary.match_expression_result_type_sites;
    }
    if (expr->match_expression_lowering_eligible) {
      ++summary.match_expression_lowering_eligible_sites;
    }
    if (expr->match_expression_guard_effect_fail_closed) {
      ++summary.match_expression_guard_effect_fail_closed_sites;
    }
    if (expr->match_expression_result_type_mismatch) {
      ++summary.match_expression_result_type_mismatch_sites;
    }
    bool saw_guard = false;
    bool saw_binding = false;
    bool saw_result_case = false;
    CollectControlFlowControlFlowSemanticExprSites(
        expr->match_expression_scrutinee.get(), summary);
    for (const auto &arm : expr->match_expression_arms) {
      if (arm.has_guard) {
        saw_guard = true;
      }
      if (arm.pattern_kind == Expr::MatchExpressionPatternKind::Binding &&
          !arm.binding_name.empty()) {
        saw_binding = true;
      }
      if (arm.pattern_kind == Expr::MatchExpressionPatternKind::ResultCase &&
          !arm.binding_name.empty()) {
        saw_result_case = true;
      }
      CollectControlFlowControlFlowSemanticExprSites(
          arm.guard_condition.get(), summary);
      CollectControlFlowControlFlowSemanticExprSites(arm.value.get(), summary);
    }
    if (saw_guard) {
      ++summary.match_expression_guard_condition_sites;
    }
    if (saw_binding) {
      ++summary.match_expression_binding_scope_sites;
    }
    if (saw_result_case) {
      ++summary.match_expression_result_case_scope_sites;
    }
    return;
  }
  case Expr::Kind::Number:
  case Expr::Kind::BoolLiteral:
  case Expr::Kind::NilLiteral:
  case Expr::Kind::StringLiteral:
  case Expr::Kind::Identifier:
  case Expr::Kind::KeyPathLiteral:
    return;
  }
}

static void CollectControlFlowControlFlowSemanticStmtSites(
    const Stmt *stmt, int loop_depth, int switch_depth,
    int defer_loop_base_depth, int defer_switch_base_depth,
    bool inside_defer_body,
    Objc3ControlFlowControlFlowSemanticModelSummary &summary) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectControlFlowControlFlowSemanticExprSites(
          stmt->let_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      CollectControlFlowControlFlowSemanticExprSites(
          stmt->assign_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectControlFlowControlFlowSemanticExprSites(
          stmt->return_stmt->value.get(), summary);
    }
    if (inside_defer_body) {
      ++summary.defer_nonlocal_exit_diagnostic_sites;
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt != nullptr) {
      const IfStmt *if_stmt = stmt->if_stmt.get();
      CollectControlFlowControlFlowSemanticExprSites(
          if_stmt->condition.get(), summary);
      CollectControlFlowControlFlowSemanticExprListSites(
          if_stmt->guard_condition_exprs, summary);
      if (if_stmt->optional_binding_surface_enabled &&
          if_stmt->guard_binding_surface_enabled) {
        ++summary.guard_binding_semantic_sites;
        summary.guard_binding_clause_semantic_sites +=
            if_stmt->optional_binding_clause_count;
        summary.guard_refinement_sites += if_stmt->optional_binding_clause_count;
        ++summary.guard_exit_enforcement_sites;
        summary.guard_condition_clause_semantic_sites +=
            if_stmt->guard_boolean_condition_clause_count;
      } else if (if_stmt->guard_condition_list_surface_enabled) {
        ++summary.guard_condition_statement_sites;
        summary.guard_condition_clause_semantic_sites +=
            if_stmt->guard_boolean_condition_clause_count;
        ++summary.guard_exit_enforcement_sites;
      }
      CollectControlFlowControlFlowSemanticStmtSitesFromList(
          if_stmt->then_body, loop_depth, switch_depth, defer_loop_base_depth,
          defer_switch_base_depth, inside_defer_body, summary);
      CollectControlFlowControlFlowSemanticStmtSitesFromList(
          if_stmt->else_body, loop_depth, switch_depth, defer_loop_base_depth,
          defer_switch_base_depth, inside_defer_body, summary);
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt != nullptr) {
      CollectControlFlowControlFlowSemanticExprSites(
          stmt->do_while_stmt->condition.get(), summary);
      CollectControlFlowControlFlowSemanticStmtSitesFromList(
          stmt->do_while_stmt->body, loop_depth + 1, switch_depth,
          defer_loop_base_depth, defer_switch_base_depth, inside_defer_body,
          summary);
    }
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt != nullptr) {
      CollectControlFlowControlFlowSemanticExprSites(
          stmt->for_stmt->init.value.get(), summary);
      CollectControlFlowControlFlowSemanticExprSites(
          stmt->for_stmt->condition.get(), summary);
      CollectControlFlowControlFlowSemanticExprSites(
          stmt->for_stmt->step.value.get(), summary);
      CollectControlFlowControlFlowSemanticStmtSitesFromList(
          stmt->for_stmt->body, loop_depth + 1, switch_depth,
          defer_loop_base_depth, defer_switch_base_depth, inside_defer_body,
          summary);
    }
    return;
  case Stmt::Kind::ForIn:
    if (stmt->for_in_stmt != nullptr) {
      CollectControlFlowControlFlowSemanticExprSites(
          stmt->for_in_stmt->collection.get(), summary);
      CollectControlFlowControlFlowSemanticStmtSitesFromList(
          stmt->for_in_stmt->body, loop_depth + 1, switch_depth,
          defer_loop_base_depth, defer_switch_base_depth, inside_defer_body,
          summary);
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt != nullptr) {
      const SwitchStmt *switch_stmt = stmt->switch_stmt.get();
      CollectControlFlowControlFlowSemanticExprSites(
          switch_stmt->condition.get(), summary);
      if (switch_stmt->match_surface_enabled) {
        ++summary.match_statement_semantic_sites;
        const MatchExhaustivenessInfo exhaustiveness =
            ClassifyMatchExhaustiveness(*switch_stmt);
        if (exhaustiveness.exhaustive) {
          ++summary.match_exhaustive_statement_sites;
        } else {
          ++summary.match_non_exhaustive_diagnostic_sites;
        }
        if (exhaustiveness.bool_exhaustive) {
          ++summary.match_bool_exhaustive_sites;
        }
        if (exhaustiveness.result_case_exhaustive) {
          ++summary.match_result_case_exhaustive_sites;
        }
        for (const auto &case_stmt : switch_stmt->cases) {
          if (case_stmt.is_default) {
            ++summary.match_default_pattern_sites;
          } else {
            if (case_stmt.has_match_guard) {
              ++summary.match_guard_condition_sites;
              CollectControlFlowControlFlowSemanticExprSites(
                  case_stmt.match_guard_condition.get(), summary);
            }
            switch (case_stmt.match_pattern_kind) {
            case MatchPatternKind::Wildcard:
              ++summary.match_wildcard_pattern_sites;
              break;
            case MatchPatternKind::LiteralInteger:
            case MatchPatternKind::LiteralBool:
            case MatchPatternKind::LiteralNil:
              ++summary.match_literal_pattern_sites;
              break;
            case MatchPatternKind::Binding:
              if (!case_stmt.match_binding_name.empty()) {
                ++summary.match_binding_scope_sites;
              }
              break;
            case MatchPatternKind::ResultCase:
              if (!case_stmt.match_binding_name.empty()) {
                ++summary.match_result_case_scope_sites;
              }
              break;
            case MatchPatternKind::None:
              break;
            }
          }
          CollectControlFlowControlFlowSemanticStmtSitesFromList(
              case_stmt.body, loop_depth, switch_depth + 1,
              defer_loop_base_depth, defer_switch_base_depth,
              inside_defer_body, summary);
        }
        return;
      }
      for (const auto &case_stmt : switch_stmt->cases) {
        CollectControlFlowControlFlowSemanticStmtSitesFromList(
            case_stmt.body, loop_depth, switch_depth + 1,
            defer_loop_base_depth, defer_switch_base_depth,
            inside_defer_body, summary);
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt != nullptr) {
      CollectControlFlowControlFlowSemanticExprSites(
          stmt->while_stmt->condition.get(), summary);
      CollectControlFlowControlFlowSemanticStmtSitesFromList(
          stmt->while_stmt->body, loop_depth + 1, switch_depth,
          defer_loop_base_depth, defer_switch_base_depth, inside_defer_body,
          summary);
    }
    return;
  case Stmt::Kind::Block:
    if (stmt->block_stmt != nullptr) {
      CollectControlFlowControlFlowSemanticStmtSitesFromList(
          stmt->block_stmt->body, loop_depth, switch_depth,
          defer_loop_base_depth, defer_switch_base_depth, inside_defer_body,
          summary);
    }
    return;
  case Stmt::Kind::Defer:
    ++summary.defer_statement_semantic_sites;
    ++summary.defer_scope_cleanup_order_sites;
    if (stmt->block_stmt != nullptr) {
      CollectControlFlowControlFlowSemanticStmtSitesFromList(
          stmt->block_stmt->body, loop_depth, switch_depth, loop_depth,
          switch_depth, true, summary);
    }
    return;
  case Stmt::Kind::Break:
    ++summary.break_statement_sites;
    if (inside_defer_body && loop_depth <= defer_loop_base_depth &&
        switch_depth <= defer_switch_base_depth) {
      ++summary.defer_nonlocal_exit_diagnostic_sites;
    } else if (loop_depth <= 0 && switch_depth <= 0) {
      ++summary.break_restriction_diagnostic_sites;
    }
    return;
  case Stmt::Kind::Continue:
    ++summary.continue_statement_sites;
    if (inside_defer_body && loop_depth <= defer_loop_base_depth) {
      ++summary.defer_nonlocal_exit_diagnostic_sites;
    } else if (loop_depth <= 0) {
      ++summary.continue_restriction_diagnostic_sites;
    }
    return;
  case Stmt::Kind::CollectionMutation:
    if (stmt->collection_mutation_stmt != nullptr) {
      CollectControlFlowControlFlowSemanticExprSites(
          stmt->collection_mutation_stmt->key_or_index.get(), summary);
      CollectControlFlowControlFlowSemanticExprSites(
          stmt->collection_mutation_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectControlFlowControlFlowSemanticExprSites(
          stmt->expr_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::Empty:
    return;
  }
}

static void CollectControlFlowControlFlowSemanticStmtSitesFromList(
    const std::vector<std::unique_ptr<Stmt>> &statements, int loop_depth,
    int switch_depth, int defer_loop_base_depth, int defer_switch_base_depth,
    bool inside_defer_body,
    Objc3ControlFlowControlFlowSemanticModelSummary &summary) {
  for (const auto &stmt : statements) {
    CollectControlFlowControlFlowSemanticStmtSites(stmt.get(), loop_depth,
                                             switch_depth,
                                             defer_loop_base_depth,
                                             defer_switch_base_depth,
                                             inside_defer_body, summary);
  }
}
