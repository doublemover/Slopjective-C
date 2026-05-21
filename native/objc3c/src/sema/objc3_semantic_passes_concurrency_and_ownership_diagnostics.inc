#include "sema/objc3_semantic_passes_concurrency_and_ownership_diagnostics_unsupported_feature_claims.inc"

#include "sema/objc3_semantic_passes_concurrency_task_callable_legality.inc"

#include "sema/objc3_semantic_passes_concurrency_actor_diagnostics.inc"

static bool IsOwnershipCleanupOwnedLocal(const LetStmt &stmt) {
  return stmt.cleanup_attribute_declared || stmt.cleanup_sugar_declared ||
         stmt.resource_attribute_declared || stmt.resource_sugar_declared;
}

static void MergeOwnershipResourceMoveState(
    std::vector<OwnershipResourceMoveScope> &target_scopes,
    const std::vector<OwnershipResourceMoveScope> &branch_scopes) {
  const std::size_t scope_count =
      std::min(target_scopes.size(), branch_scopes.size());
  for (std::size_t scope_index = 0; scope_index < scope_count; ++scope_index) {
    for (auto &[name, target_state] : target_scopes[scope_index]) {
      auto branch_it = branch_scopes[scope_index].find(name);
      if (branch_it == branch_scopes[scope_index].end()) {
        continue;
      }
      if (branch_it->second.moved) {
        target_state.moved = true;
        if (target_state.move_line == 1 && target_state.move_column == 1) {
          target_state.move_line = branch_it->second.move_line;
          target_state.move_column = branch_it->second.move_column;
        }
      }
    }
  }
}

static void DiagnoseOwnershipResourceMoveExpr(
    const Expr *expr, std::vector<OwnershipResourceMoveScope> &scopes,
    std::vector<std::string> &diagnostics);

static void DiagnoseOwnershipResourceMoveStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<OwnershipResourceMoveScope> &scopes,
    std::vector<std::string> &diagnostics);

static void DiagnoseOwnershipResourceMoveExpr(
    const Expr *expr, std::vector<OwnershipResourceMoveScope> &scopes,
    std::vector<std::string> &diagnostics) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
    case Expr::Kind::Identifier: {
      const auto *binding = LookupOwnershipResourceMoveBinding(scopes, expr->ident);
      if (binding != nullptr && binding->cleanup_owned && binding->moved) {
        diagnostics.push_back(MakeDiag(
            expr->line, expr->column, "O3S296",
            "resource move semantics failed: cleanup-owned local '" +
                expr->ident +
                "' is used after move capture transferred cleanup ownership"));
      }
      return;
    }
    case Expr::Kind::BlockLiteral: {
      std::unordered_set<std::string> moved_here;
      for (const auto &item : expr->block_explicit_capture_items_source_order) {
        auto *binding = LookupOwnershipResourceMoveBinding(scopes, item.name);
        if (item.mode == "move") {
          if (binding == nullptr) {
            continue;
          }
          if (!binding->cleanup_owned) {
            diagnostics.push_back(MakeDiag(
                expr->line, expr->column, "O3S295",
                "resource move semantics failed: move capture requires a cleanup or resource-backed local '" +
                    item.name + "'"));
            continue;
          }
          if (binding->moved) {
            diagnostics.push_back(MakeDiag(
                expr->line, expr->column, "O3S297",
                "resource move semantics failed: cleanup ownership for '" +
                    item.name +
                    "' was already transferred by an earlier move capture"));
            continue;
          }
          moved_here.insert(item.name);
          continue;
        }
        if (binding != nullptr && binding->cleanup_owned && binding->moved) {
          diagnostics.push_back(MakeDiag(
              expr->line, expr->column, "O3S296",
              "resource move semantics failed: cleanup-owned local '" +
                  item.name +
                  "' is used after move capture transferred cleanup ownership"));
        }
      }
      for (const auto &capture_name : expr->block_capture_names_lexicographic) {
        if (moved_here.count(capture_name) != 0u) {
          continue;
        }
        const auto *binding = LookupOwnershipResourceMoveBinding(scopes, capture_name);
        if (binding != nullptr && binding->cleanup_owned && binding->moved) {
          diagnostics.push_back(MakeDiag(
              expr->line, expr->column, "O3S296",
              "resource move semantics failed: cleanup-owned local '" +
                  capture_name +
                  "' is used after move capture transferred cleanup ownership"));
        }
      }
      for (const auto &capture_name : moved_here) {
        if (auto *binding = LookupOwnershipResourceMoveBinding(scopes, capture_name)) {
          binding->moved = true;
          binding->move_line = expr->line;
          binding->move_column = expr->column;
        }
      }
      return;
    }
    case Expr::Kind::Binary:
      DiagnoseOwnershipResourceMoveExpr(expr->left.get(), scopes, diagnostics);
      DiagnoseOwnershipResourceMoveExpr(expr->right.get(), scopes, diagnostics);
      return;
    case Expr::Kind::Conditional:
      DiagnoseOwnershipResourceMoveExpr(expr->left.get(), scopes, diagnostics);
      DiagnoseOwnershipResourceMoveExpr(expr->right.get(), scopes, diagnostics);
      DiagnoseOwnershipResourceMoveExpr(expr->third.get(), scopes, diagnostics);
      return;
    case Expr::Kind::CollectionLiteral:
      for (const auto &key : expr->collection_keys) {
        DiagnoseOwnershipResourceMoveExpr(key.get(), scopes, diagnostics);
      }
      for (const auto &value : expr->collection_values) {
        DiagnoseOwnershipResourceMoveExpr(value.get(), scopes, diagnostics);
      }
      return;
    case Expr::Kind::IndexAccess:
      DiagnoseOwnershipResourceMoveExpr(expr->left.get(), scopes, diagnostics);
      DiagnoseOwnershipResourceMoveExpr(expr->right.get(), scopes, diagnostics);
      return;
    case Expr::Kind::Call:
    case Expr::Kind::MessageSend:
      DiagnoseOwnershipResourceMoveExpr(expr->receiver.get(), scopes, diagnostics);
      for (const auto &arg : expr->args) {
        DiagnoseOwnershipResourceMoveExpr(arg.get(), scopes, diagnostics);
      }
      return;
    case Expr::Kind::Try:
    case Expr::Kind::Throw:
      for (const auto &arg : expr->args) {
        DiagnoseOwnershipResourceMoveExpr(arg.get(), scopes, diagnostics);
      }
      return;
    case Expr::Kind::KeyPathLiteral:
    case Expr::Kind::Number:
    case Expr::Kind::BoolLiteral:
    case Expr::Kind::NilLiteral:
    case Expr::Kind::StringLiteral:
      return;
  }
}

static void DiagnoseOwnershipResourceMoveForClause(
    const ForClause &clause, std::vector<OwnershipResourceMoveScope> &scopes,
    std::vector<std::string> &diagnostics) {
  switch (clause.kind) {
    case ForClause::Kind::None:
      return;
    case ForClause::Kind::Expr:
      DiagnoseOwnershipResourceMoveExpr(clause.value.get(), scopes, diagnostics);
      return;
    case ForClause::Kind::Let:
      DiagnoseOwnershipResourceMoveExpr(clause.value.get(), scopes, diagnostics);
      if (!scopes.empty()) {
        scopes.back()[clause.name] = {};
      }
      return;
    case ForClause::Kind::Assign: {
      auto *binding = LookupOwnershipResourceMoveBinding(scopes, clause.name);
      if (binding != nullptr && binding->cleanup_owned && binding->moved) {
        diagnostics.push_back(MakeDiag(
            clause.line, clause.column, "O3S296",
            "resource move semantics failed: cleanup-owned local '" +
                clause.name +
                "' is used after move capture transferred cleanup ownership"));
      }
      DiagnoseOwnershipResourceMoveExpr(clause.value.get(), scopes, diagnostics);
      return;
    }
  }
}

static void DiagnoseOwnershipResourceMoveStatement(
    const Stmt *stmt, std::vector<OwnershipResourceMoveScope> &scopes,
    std::vector<std::string> &diagnostics) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Let: {
      const LetStmt *let = stmt->let_stmt.get();
      if (let == nullptr || scopes.empty()) {
        return;
      }
      DiagnoseOwnershipResourceMoveExpr(let->value.get(), scopes, diagnostics);
      OwnershipResourceMoveBindingState state;
      state.cleanup_owned = IsOwnershipCleanupOwnedLocal(*let);
      scopes.back()[let->name] = state;
      return;
    }
    case Stmt::Kind::Assign: {
      const AssignStmt *assign = stmt->assign_stmt.get();
      if (assign == nullptr) {
        return;
      }
      auto *binding = LookupOwnershipResourceMoveBinding(scopes, assign->name);
      if (binding != nullptr && binding->cleanup_owned && binding->moved) {
        diagnostics.push_back(MakeDiag(
            assign->line, assign->column, "O3S296",
            "resource move semantics failed: cleanup-owned local '" +
                assign->name +
                "' is used after move capture transferred cleanup ownership"));
      }
      DiagnoseOwnershipResourceMoveExpr(assign->value.get(), scopes, diagnostics);
      return;
    }
    case Stmt::Kind::Return:
      if (stmt->return_stmt != nullptr) {
        DiagnoseOwnershipResourceMoveExpr(stmt->return_stmt->value.get(), scopes,
                                      diagnostics);
      }
      return;
    case Stmt::Kind::Expr:
      if (stmt->expr_stmt != nullptr) {
        DiagnoseOwnershipResourceMoveExpr(stmt->expr_stmt->value.get(), scopes,
                                      diagnostics);
      }
      return;
    case Stmt::Kind::If: {
      const IfStmt *if_stmt = stmt->if_stmt.get();
      if (if_stmt == nullptr) {
        return;
      }
      DiagnoseOwnershipResourceMoveExpr(if_stmt->condition.get(), scopes,
                                    diagnostics);
      auto then_scopes = scopes;
      then_scopes.push_back({});
      DiagnoseOwnershipResourceMoveStatements(if_stmt->then_body, then_scopes,
                                          diagnostics);
      auto else_scopes = scopes;
      else_scopes.push_back({});
      DiagnoseOwnershipResourceMoveStatements(if_stmt->else_body, else_scopes,
                                          diagnostics);
      MergeOwnershipResourceMoveState(scopes, then_scopes);
      MergeOwnershipResourceMoveState(scopes, else_scopes);
      return;
    }
    case Stmt::Kind::DoWhile: {
      const DoWhileStmt *do_while_stmt = stmt->do_while_stmt.get();
      if (do_while_stmt == nullptr) {
        return;
      }
