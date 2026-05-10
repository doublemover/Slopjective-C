
static bool ExprContainsRunnableBlockLiteral(const Expr *expr) {
  if (expr == nullptr) {
    return false;
  }
  if (expr->kind == Expr::Kind::BlockLiteral) {
    return BlockLiteralUsesRunnableC002Subset(*expr);
  }
  if (ExprContainsRunnableBlockLiteral(expr->left.get()) ||
      ExprContainsRunnableBlockLiteral(expr->right.get()) ||
      ExprContainsRunnableBlockLiteral(expr->third.get()) ||
      ExprContainsRunnableBlockLiteral(expr->receiver.get())) {
    return true;
  }
  for (const auto &arg : expr->args) {
    if (ExprContainsRunnableBlockLiteral(arg.get())) {
      return true;
    }
  }
  return false;
}

static bool StmtContainsRunnableBlockLiteral(const Stmt *stmt) {
  if (stmt == nullptr) {
    return false;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    return stmt->let_stmt != nullptr &&
           ExprContainsRunnableBlockLiteral(stmt->let_stmt->value.get());
  case Stmt::Kind::Assign:
    return stmt->assign_stmt != nullptr &&
           ExprContainsRunnableBlockLiteral(stmt->assign_stmt->value.get());
  case Stmt::Kind::Return:
    return stmt->return_stmt != nullptr &&
           ExprContainsRunnableBlockLiteral(stmt->return_stmt->value.get());
  case Stmt::Kind::Expr:
    return stmt->expr_stmt != nullptr &&
           ExprContainsRunnableBlockLiteral(stmt->expr_stmt->value.get());
  case Stmt::Kind::If:
    if (stmt->if_stmt == nullptr) {
      return false;
    }
    if (ExprContainsRunnableBlockLiteral(stmt->if_stmt->condition.get())) {
      return true;
    }
    for (const auto &then_stmt : stmt->if_stmt->then_body) {
      if (StmtContainsRunnableBlockLiteral(then_stmt.get())) {
        return true;
      }
    }
    for (const auto &else_stmt : stmt->if_stmt->else_body) {
      if (StmtContainsRunnableBlockLiteral(else_stmt.get())) {
        return true;
      }
    }
    return false;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt == nullptr) {
      return false;
    }
    if (ExprContainsRunnableBlockLiteral(
            stmt->do_while_stmt->condition.get())) {
      return true;
    }
    for (const auto &body_stmt : stmt->do_while_stmt->body) {
      if (StmtContainsRunnableBlockLiteral(body_stmt.get())) {
        return true;
      }
    }
    return false;
  case Stmt::Kind::For:
    if (stmt->for_stmt == nullptr) {
      return false;
    }
    if (ExprContainsRunnableBlockLiteral(stmt->for_stmt->condition.get()) ||
        ExprContainsRunnableBlockLiteral(stmt->for_stmt->init.value.get()) ||
        ExprContainsRunnableBlockLiteral(stmt->for_stmt->step.value.get())) {
      return true;
    }
    for (const auto &body_stmt : stmt->for_stmt->body) {
      if (StmtContainsRunnableBlockLiteral(body_stmt.get())) {
        return true;
      }
    }
    return false;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt == nullptr) {
      return false;
    }
    if (ExprContainsRunnableBlockLiteral(stmt->switch_stmt->condition.get())) {
      return true;
    }
    for (const auto &switch_case : stmt->switch_stmt->cases) {
      for (const auto &body_stmt : switch_case.body) {
        if (StmtContainsRunnableBlockLiteral(body_stmt.get())) {
          return true;
        }
      }
    }
    return false;
  case Stmt::Kind::While:
    if (stmt->while_stmt == nullptr) {
      return false;
    }
    if (ExprContainsRunnableBlockLiteral(stmt->while_stmt->condition.get())) {
      return true;
    }
    for (const auto &body_stmt : stmt->while_stmt->body) {
      if (StmtContainsRunnableBlockLiteral(body_stmt.get())) {
        return true;
      }
    }
    return false;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt == nullptr) {
      return false;
    }
    for (const auto &body_stmt : stmt->block_stmt->body) {
      if (StmtContainsRunnableBlockLiteral(body_stmt.get())) {
        return true;
      }
    }
    return false;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return false;
  }
  return false;
}

static bool FunctionBodyContainsRunnableBlockLiteral(const FunctionDecl &fn) {
  for (const auto &stmt : fn.body) {
    if (StmtContainsRunnableBlockLiteral(stmt.get())) {
      return true;
    }
  }
  return false;
}

static bool MethodBodyContainsRunnableBlockLiteral(const Objc3MethodDecl &method) {
  for (const auto &stmt : method.body) {
    if (StmtContainsRunnableBlockLiteral(stmt.get())) {
      return true;
    }
  }
  return false;
}

static void DiagnoseUnsupportedFunctionFeatureClaims(
    const FunctionDecl &fn,
    std::vector<std::string> &diagnostics,
    Objc3UnsupportedFeatureClaimEnforcementStats &stats,
    const Objc3UnsupportedFeatureClaimContext &context) {
  // ARC mode-handling core implementation anchor: executable
  // function ownership qualifiers become runnable only under explicit
  // -fobjc-arc mode while the non-ARC path remains fail-closed.
  // runtime-backed-object-ownership freeze anchor: the current
  // runnable ownership slice is still limited to object/property/accessor
  // ownership profiles plus the legacy ownership summary lanes below. ARC
  // ownership qualifiers on executable functions/methods remain fail-closed
  // until the later runtime-backed ownership implementation work lands.
  // retainable-object semantic-rule freeze anchor: runtime-backed
  // property/member ownership metadata is truthful, but executable
  // retain/release legality, `@autoreleasepool`, and destruction ordering
  // remain fail-closed until the later storage legality/runtime work.
  // ownership-lowering baseline freeze anchor: lane-C preserves the
  // legacy ownership lowering summaries those later runtime issues must
  // replace, but no executable retain/release/autorelease/weak runtime hooks
  // are emitted out of function-level ownership qualifiers yet.
  // ARC cleanup/weak/lifetime lowering anchor: sema still only
  // publishes the canonical ownership and block-interaction packets; scope
  // cleanup scheduling, weak current-property helper calls, and block-capture
  // lifetime cleanup remain the responsibility of lane-C lowering.
  // ARC/block autorelease-return lowering anchor: sema still only
  // publishes return-autorelease and block-escape intent; terminal branch
  // cleanup ordering and escaping-block/autorelease-return composition remain
  // the responsibility of lane-C lowering.
  // runnable-arc-runtime gate anchor: lane-E freezes the supported
  // ARC slice against parser-only or metadata-only claims; sema remains the
  // truthful source of the A/B-lane legality and ownership packets it consumes.
  // runnable-arc-closeout anchor: lane-E closes the current ARC
  // tranche by consuming this preserved semantic state through integrated smoke
  // and runbook proof rather than widening semantic acceptance here.
  if (fn.throws_declared && ShouldRejectThrowsFeatureClaim(context)) {
    RecordUnsupportedFeatureClaimDiagnostic(
        stats.throws_source_rejection_site_count,
        fn.line,
        fn.column,
        "unsupported feature claim: 'throws' is not yet runnable in Objective-C 3 native mode",
        diagnostics,
        stats);
  }
  const bool allow_source_only_block_owned_bindings =
      context.allow_source_only_block_literals &&
      FunctionBodyContainsBlockLiteral(fn);
  const bool allow_runnable_block_owned_bindings =
      FunctionBodyContainsRunnableBlockLiteral(fn);
  for (const auto &param : fn.params) {
    if (!param.has_ownership_qualifier) {
      continue;
    }
    if (!ShouldRejectOwnershipQualifierFeatureClaim(
            context,
            allow_source_only_block_owned_bindings,
            allow_runnable_block_owned_bindings)) {
      // byref/copy-dispose/object-ownership anchor: source-only block
      // semantic modeling may admit ownership-qualified executable bindings so
      // lane-B can classify owned vs weak/unowned captures without widening the
      // native runnable ARC surface.
      continue;
    }
    RecordUnsupportedFeatureClaimDiagnostic(
        stats.arc_source_rejection_site_count,
        OwnershipQualifierLine(param.ownership_qualifier_tokens, param.line),
        OwnershipQualifierColumn(param.ownership_qualifier_tokens, param.column),
        "unsupported feature claim: ARC ownership qualifiers are not yet runnable in Objective-C 3 native mode",
        diagnostics,
        stats);
  }
  if (fn.has_return_ownership_qualifier &&
      ShouldRejectOwnershipQualifierFeatureClaim(
          context,
          allow_source_only_block_owned_bindings,
          allow_runnable_block_owned_bindings)) {
    RecordUnsupportedFeatureClaimDiagnostic(
        stats.arc_source_rejection_site_count,
        OwnershipQualifierLine(
            fn.return_ownership_qualifier_tokens, fn.line),
        OwnershipQualifierColumn(
            fn.return_ownership_qualifier_tokens, fn.column),
        "unsupported feature claim: ARC ownership qualifiers are not yet runnable in Objective-C 3 native mode",
        diagnostics,
        stats);
  }
  for (const auto &stmt : fn.body) {
    DiagnoseUnsupportedFeatureClaimsInStmt(stmt.get(), diagnostics, stats, context);
  }
}

static void DiagnoseUnsupportedMethodFeatureClaims(
    const Objc3MethodDecl &method,
    std::vector<std::string> &diagnostics,
    Objc3UnsupportedFeatureClaimEnforcementStats &stats,
    const Objc3UnsupportedFeatureClaimContext &context) {
  // ARC mode-handling core implementation anchor: executable method
  // ownership qualifiers become runnable only under explicit -fobjc-arc mode
  // while the non-ARC path remains fail-closed.
  // runtime-backed-object-ownership freeze anchor: method-level ARC
  // ownership qualifiers still sit outside the truthful runnable object-model
  // claim even though property/member ownership profiles are already preserved
  // through the object runtime metadata path.
  // retainable-object semantic-rule freeze anchor: method-level
  // retain/release legality and destruction-order behavior remain deferred even
  // though runtime-backed property/member ownership metadata is already live.
  // ownership-lowering baseline freeze anchor: method-level
  // ownership qualifiers still terminate in semantic unsupported-feature
  // diagnostics while the preserved lowering summaries remain the only
  // ownership hook surface available to IR closeout.
  if (method.throws_declared && ShouldRejectThrowsFeatureClaim(context)) {
    RecordUnsupportedFeatureClaimDiagnostic(
        stats.throws_source_rejection_site_count,
        method.line,
        method.column,
        "unsupported feature claim: 'throws' is not yet runnable in Objective-C 3 native mode",
        diagnostics,
        stats);
  }
  const bool allow_source_only_block_owned_bindings =
      context.allow_source_only_block_literals &&
      MethodBodyContainsBlockLiteral(method);
  const bool allow_runnable_block_owned_bindings =
      MethodBodyContainsRunnableBlockLiteral(method);
  for (const auto &param : method.params) {
    if (!param.has_ownership_qualifier) {
      continue;
    }
    if (!ShouldRejectOwnershipQualifierFeatureClaim(
            context,
            allow_source_only_block_owned_bindings,
            allow_runnable_block_owned_bindings)) {
      // byref/copy-dispose/object-ownership anchor: source-only block
      // semantic modeling may admit ownership-qualified executable bindings so
      // lane-B can classify owned vs weak/unowned captures without widening the
      // native runnable ARC surface.
      continue;
    }
    RecordUnsupportedFeatureClaimDiagnostic(
        stats.arc_source_rejection_site_count,
        OwnershipQualifierLine(param.ownership_qualifier_tokens, param.line),
        OwnershipQualifierColumn(param.ownership_qualifier_tokens, param.column),
        "unsupported feature claim: ARC ownership qualifiers are not yet runnable in Objective-C 3 native mode",
        diagnostics,
        stats);
  }
  if (method.has_return_ownership_qualifier &&
      ShouldRejectOwnershipQualifierFeatureClaim(
          context,
          allow_source_only_block_owned_bindings,
          allow_runnable_block_owned_bindings)) {
    RecordUnsupportedFeatureClaimDiagnostic(
        stats.arc_source_rejection_site_count,
        OwnershipQualifierLine(
            method.return_ownership_qualifier_tokens, method.line),
        OwnershipQualifierColumn(
            method.return_ownership_qualifier_tokens, method.column),
        "unsupported feature claim: ARC ownership qualifiers are not yet runnable in Objective-C 3 native mode",
        diagnostics,
        stats);
  }
  for (const auto &stmt : method.body) {
    DiagnoseUnsupportedFeatureClaimsInStmt(stmt.get(), diagnostics, stats, context);
  }
}

static Objc3UnsupportedFeatureClaimEnforcementStats
DiagnoseUnsupportedFeatureClaimSources(
    const Objc3Program &ast,
    bool allow_source_only_block_literals,
    bool allow_source_only_defer_statements,
    bool allow_source_only_error_runtime_surface,
    bool arc_mode_enabled,
    std::vector<std::string> &diagnostics) {
  // unsupported-feature enforcement anchor: keep the
  // runnable-core positive path at zero-site readiness and reject accepted
  // unsupported advanced sources deterministically with O3S221.
  Objc3UnsupportedFeatureClaimEnforcementStats stats;
  const Objc3UnsupportedFeatureClaimContext context =
      BuildUnsupportedFeatureClaimContext(ast,
                                          allow_source_only_block_literals,
                                          allow_source_only_defer_statements,
                                          allow_source_only_error_runtime_surface,
                                          arc_mode_enabled);
  for (const auto &fn : ast.functions) {
    DiagnoseUnsupportedFunctionFeatureClaims(fn, diagnostics, stats, context);
  }
  for (const auto &protocol_decl : ast.protocols) {
    for (const auto &method : protocol_decl.methods) {
      DiagnoseUnsupportedMethodFeatureClaims(method, diagnostics, stats, context);
    }
  }
  for (const auto &interface_decl : ast.interfaces) {
    for (const auto &method : interface_decl.methods) {
      DiagnoseUnsupportedMethodFeatureClaims(method, diagnostics, stats, context);
    }
  }
  for (const auto &implementation_decl : ast.implementations) {
    for (const auto &method : implementation_decl.methods) {
      DiagnoseUnsupportedMethodFeatureClaims(method, diagnostics, stats, context);
    }
  }
  if (stats.throws_source_rejection_site_count > 0u) {
    ++stats.live_unsupported_feature_family_count;
  }
  if (stats.blocks_source_rejection_site_count > 0u) {
    ++stats.live_unsupported_feature_family_count;
  }
  if (stats.arc_source_rejection_site_count > 0u) {
    ++stats.live_unsupported_feature_family_count;
  }
  const std::size_t categorized_unsupported_site_count =
      stats.throws_source_rejection_site_count +
      stats.blocks_source_rejection_site_count +
      stats.arc_source_rejection_site_count;
  if (stats.live_unsupported_feature_site_count >
      categorized_unsupported_site_count) {
    ++stats.live_unsupported_feature_family_count;
  }
  return stats;
}

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
    case Expr::Kind::Call:
    case Expr::Kind::MessageSend:
      DiagnoseOwnershipResourceMoveExpr(expr->receiver.get(), scopes, diagnostics);
      for (const auto &arg : expr->args) {
        DiagnoseOwnershipResourceMoveExpr(arg.get(), scopes, diagnostics);
      }
      return;
    case Expr::Kind::Number:
    case Expr::Kind::BoolLiteral:
    case Expr::Kind::NilLiteral:
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
