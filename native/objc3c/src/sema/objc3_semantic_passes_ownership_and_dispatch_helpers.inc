      auto body_scopes = scopes;
      body_scopes.push_back({});
      DiagnoseOwnershipResourceMoveStatements(do_while_stmt->body, body_scopes,
                                          diagnostics);
      MergeOwnershipResourceMoveState(scopes, body_scopes);
      DiagnoseOwnershipResourceMoveExpr(do_while_stmt->condition.get(), scopes,
                                    diagnostics);
      return;
    }
    case Stmt::Kind::For: {
      const ForStmt *for_stmt = stmt->for_stmt.get();
      if (for_stmt == nullptr) {
        return;
      }
      auto loop_scopes = scopes;
      loop_scopes.push_back({});
      DiagnoseOwnershipResourceMoveForClause(for_stmt->init, loop_scopes,
                                         diagnostics);
      DiagnoseOwnershipResourceMoveExpr(for_stmt->condition.get(), loop_scopes,
                                    diagnostics);
      DiagnoseOwnershipResourceMoveForClause(for_stmt->step, loop_scopes,
                                         diagnostics);
      loop_scopes.push_back({});
      DiagnoseOwnershipResourceMoveStatements(for_stmt->body, loop_scopes,
                                          diagnostics);
      MergeOwnershipResourceMoveState(scopes, loop_scopes);
      return;
    }
    case Stmt::Kind::Switch: {
      const SwitchStmt *switch_stmt = stmt->switch_stmt.get();
      if (switch_stmt == nullptr) {
        return;
      }
      DiagnoseOwnershipResourceMoveExpr(switch_stmt->condition.get(), scopes,
                                    diagnostics);
      for (const auto &case_stmt : switch_stmt->cases) {
        auto case_scopes = scopes;
        case_scopes.push_back({});
        DiagnoseOwnershipResourceMoveStatements(case_stmt.body, case_scopes,
                                            diagnostics);
        MergeOwnershipResourceMoveState(scopes, case_scopes);
      }
      return;
    }
    case Stmt::Kind::While: {
      const WhileStmt *while_stmt = stmt->while_stmt.get();
      if (while_stmt == nullptr) {
        return;
      }
      DiagnoseOwnershipResourceMoveExpr(while_stmt->condition.get(), scopes,
                                    diagnostics);
      auto body_scopes = scopes;
      body_scopes.push_back({});
      DiagnoseOwnershipResourceMoveStatements(while_stmt->body, body_scopes,
                                          diagnostics);
      MergeOwnershipResourceMoveState(scopes, body_scopes);
      return;
    }
    case Stmt::Kind::Block:
      if (stmt->block_stmt != nullptr) {
        auto block_scopes = scopes;
        block_scopes.push_back({});
        DiagnoseOwnershipResourceMoveStatements(stmt->block_stmt->body, block_scopes,
                                            diagnostics);
        MergeOwnershipResourceMoveState(scopes, block_scopes);
      }
      return;
    case Stmt::Kind::Defer:
      if (stmt->block_stmt != nullptr) {
        auto defer_scopes = scopes;
        defer_scopes.push_back({});
        DiagnoseOwnershipResourceMoveStatements(stmt->block_stmt->body, defer_scopes,
                                            diagnostics);
      }
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

static void DiagnoseOwnershipResourceMoveStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<OwnershipResourceMoveScope> &scopes,
    std::vector<std::string> &diagnostics) {
  for (const auto &stmt : statements) {
    DiagnoseOwnershipResourceMoveStatement(stmt.get(), scopes, diagnostics);
  }
}

static void DiagnoseOwnershipResourceMoveSemantics(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    const std::vector<SemanticScope> &semantic_scopes,
    std::vector<std::string> &diagnostics) {
  std::vector<OwnershipResourceMoveScope> scopes;
  scopes.reserve(semantic_scopes.size());
  for (const auto &semantic_scope : semantic_scopes) {
    OwnershipResourceMoveScope scope;
    for (const auto &[name, _] : semantic_scope) {
      scope[name] = {};
    }
    scopes.push_back(std::move(scope));
  }
  DiagnoseOwnershipResourceMoveStatements(statements, scopes, diagnostics);
}

struct OwnershipResourceMoveSiteProfile {
  std::size_t cleanup_owned_local_sites = 0;
  std::size_t resource_move_capture_sites = 0;
};

static void CollectOwnershipResourceMoveExprSites(const Expr *expr,
                                              OwnershipResourceMoveSiteProfile &profile);

static void CollectOwnershipResourceMoveStmtSites(const Stmt *stmt,
                                              OwnershipResourceMoveSiteProfile &profile) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Let:
      if (stmt->let_stmt != nullptr) {
        if (IsOwnershipCleanupOwnedLocal(*stmt->let_stmt)) {
          ++profile.cleanup_owned_local_sites;
        }
        CollectOwnershipResourceMoveExprSites(stmt->let_stmt->value.get(), profile);
      }
      return;
    case Stmt::Kind::Assign:
      if (stmt->assign_stmt != nullptr) {
        CollectOwnershipResourceMoveExprSites(stmt->assign_stmt->value.get(), profile);
      }
      return;
    case Stmt::Kind::Return:
      if (stmt->return_stmt != nullptr) {
        CollectOwnershipResourceMoveExprSites(stmt->return_stmt->value.get(), profile);
      }
      return;
    case Stmt::Kind::If:
      if (stmt->if_stmt != nullptr) {
        CollectOwnershipResourceMoveExprSites(stmt->if_stmt->condition.get(), profile);
        for (const auto &body_stmt : stmt->if_stmt->then_body) {
          CollectOwnershipResourceMoveStmtSites(body_stmt.get(), profile);
        }
        for (const auto &body_stmt : stmt->if_stmt->else_body) {
          CollectOwnershipResourceMoveStmtSites(body_stmt.get(), profile);
        }
      }
      return;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt != nullptr) {
        for (const auto &body_stmt : stmt->do_while_stmt->body) {
          CollectOwnershipResourceMoveStmtSites(body_stmt.get(), profile);
        }
        CollectOwnershipResourceMoveExprSites(stmt->do_while_stmt->condition.get(), profile);
      }
      return;
    case Stmt::Kind::For:
      if (stmt->for_stmt != nullptr) {
        CollectOwnershipResourceMoveExprSites(stmt->for_stmt->init.value.get(), profile);
        CollectOwnershipResourceMoveExprSites(stmt->for_stmt->condition.get(), profile);
        CollectOwnershipResourceMoveExprSites(stmt->for_stmt->step.value.get(), profile);
        for (const auto &body_stmt : stmt->for_stmt->body) {
          CollectOwnershipResourceMoveStmtSites(body_stmt.get(), profile);
        }
      }
      return;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt != nullptr) {
        CollectOwnershipResourceMoveExprSites(stmt->switch_stmt->condition.get(), profile);
        for (const auto &case_stmt : stmt->switch_stmt->cases) {
          for (const auto &body_stmt : case_stmt.body) {
            CollectOwnershipResourceMoveStmtSites(body_stmt.get(), profile);
          }
        }
      }
      return;
    case Stmt::Kind::While:
      if (stmt->while_stmt != nullptr) {
        CollectOwnershipResourceMoveExprSites(stmt->while_stmt->condition.get(), profile);
        for (const auto &body_stmt : stmt->while_stmt->body) {
          CollectOwnershipResourceMoveStmtSites(body_stmt.get(), profile);
        }
      }
      return;
    case Stmt::Kind::Block:
    case Stmt::Kind::Defer:
      if (stmt->block_stmt != nullptr) {
        for (const auto &body_stmt : stmt->block_stmt->body) {
          CollectOwnershipResourceMoveStmtSites(body_stmt.get(), profile);
        }
      }
      return;
    case Stmt::Kind::Expr:
      if (stmt->expr_stmt != nullptr) {
        CollectOwnershipResourceMoveExprSites(stmt->expr_stmt->value.get(), profile);
      }
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

static void CollectOwnershipResourceMoveExprSites(const Expr *expr,
                                              OwnershipResourceMoveSiteProfile &profile) {
  if (expr == nullptr) {
    return;
  }
  if (expr->kind == Expr::Kind::BlockLiteral) {
    for (const auto &item : expr->block_explicit_capture_items_source_order) {
      if (item.mode == "move") {
        ++profile.resource_move_capture_sites;
      }
    }
  }
  CollectOwnershipResourceMoveExprSites(expr->receiver.get(), profile);
  CollectOwnershipResourceMoveExprSites(expr->left.get(), profile);
  CollectOwnershipResourceMoveExprSites(expr->right.get(), profile);
  CollectOwnershipResourceMoveExprSites(expr->third.get(), profile);
  for (const auto &arg : expr->args) {
    CollectOwnershipResourceMoveExprSites(arg.get(), profile);
  }
}

#include "sema/objc3_semantic_passes_borrowed_ownership_diagnostics.inc"

#include "sema/objc3_semantic_passes_ownership_site_profiles.inc"

#include "sema/objc3_semantic_passes_typed_keypath_dispatch_helpers.inc"

#include "sema/objc3_semantic_passes_protocol_metaprogramming_helpers.inc"
