static void DiagnoseUnsupportedFeatureClaimsInStmt(
    const Stmt *stmt,
    std::vector<std::string> &diagnostics,
    Objc3UnsupportedFeatureClaimEnforcementStats &stats,
    const Objc3UnsupportedFeatureClaimContext &context) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      DiagnoseUnsupportedFeatureClaimsInExpr(
          stmt->let_stmt->value.get(), diagnostics, stats, context);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      DiagnoseUnsupportedFeatureClaimsInExpr(
          stmt->assign_stmt->value.get(), diagnostics, stats, context);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      DiagnoseUnsupportedFeatureClaimsInExpr(
          stmt->return_stmt->value.get(), diagnostics, stats, context);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt == nullptr) {
      return;
    }
    DiagnoseUnsupportedFeatureClaimsInExpr(
        stmt->if_stmt->condition.get(), diagnostics, stats, context);
    for (const auto &then_stmt : stmt->if_stmt->then_body) {
      DiagnoseUnsupportedFeatureClaimsInStmt(
          then_stmt.get(), diagnostics, stats, context);
    }
    for (const auto &else_stmt : stmt->if_stmt->else_body) {
      DiagnoseUnsupportedFeatureClaimsInStmt(
          else_stmt.get(), diagnostics, stats, context);
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->do_while_stmt->body) {
      DiagnoseUnsupportedFeatureClaimsInStmt(
          body_stmt.get(), diagnostics, stats, context);
    }
    DiagnoseUnsupportedFeatureClaimsInExpr(
        stmt->do_while_stmt->condition.get(), diagnostics, stats, context);
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt == nullptr) {
      return;
    }
    DiagnoseUnsupportedFeatureClaimsInExpr(
        stmt->for_stmt->init.value.get(), diagnostics, stats, context);
    DiagnoseUnsupportedFeatureClaimsInExpr(
        stmt->for_stmt->condition.get(), diagnostics, stats, context);
    DiagnoseUnsupportedFeatureClaimsInExpr(
        stmt->for_stmt->step.value.get(), diagnostics, stats, context);
    for (const auto &body_stmt : stmt->for_stmt->body) {
      DiagnoseUnsupportedFeatureClaimsInStmt(
          body_stmt.get(), diagnostics, stats, context);
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt == nullptr) {
      return;
    }
    DiagnoseUnsupportedFeatureClaimsInExpr(
        stmt->switch_stmt->condition.get(), diagnostics, stats, context);
    for (const auto &switch_case : stmt->switch_stmt->cases) {
      for (const auto &case_stmt : switch_case.body) {
        DiagnoseUnsupportedFeatureClaimsInStmt(
            case_stmt.get(), diagnostics, stats, context);
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt == nullptr) {
      return;
    }
    DiagnoseUnsupportedFeatureClaimsInExpr(
        stmt->while_stmt->condition.get(), diagnostics, stats, context);
    for (const auto &body_stmt : stmt->while_stmt->body) {
      DiagnoseUnsupportedFeatureClaimsInStmt(
          body_stmt.get(), diagnostics, stats, context);
    }
    return;
  case Stmt::Kind::Block:
    if (stmt->block_stmt == nullptr) {
      return;
    }
    if (stmt->block_stmt->is_autoreleasepool_scope) {
      // autoreleasepool/destruction-order semantic expansion anchor:
      // lane-B identified the ownership-sensitive destruction-order surface so
      // later runtime work would not need to recover it from source.
      // runtime-memory-management implementation anchor: native mode
      // now accepts `@autoreleasepool` blocks and relies on emitted runtime
      // push/pop helpers plus live refcount/weak-table support instead of the
      // earlier fail-closed diagnostic path.
      // ownership-runtime-gate freeze anchor: lane-E now freezes
      // this accepted autoreleasepool slice as part of the supported ownership
      // runtime baseline and leaves ARC/block/public-ABI widening as explicit
      // non-goals for later milestones.
      // ownership-smoke closeout anchor: the runnable closeout matrix
      // consumes this admitted ownership-sensitive autoreleasepool slice
      // unchanged and may not widen it during the earlier tranche closeout.
    }
    for (const auto &body_stmt : stmt->block_stmt->body) {
      DiagnoseUnsupportedFeatureClaimsInStmt(
          body_stmt.get(), diagnostics, stats, context);
    }
    return;
  case Stmt::Kind::Defer:
    if (stmt->block_stmt == nullptr) {
      return;
    }
    if (ShouldRejectDeferFeatureClaim(context)) {
      diagnostics.push_back(MakeDiag(
          stmt->line,
          stmt->column,
          "O3S221",
          "unsupported feature claim: defer statements are not yet runnable in Objective-C 3 native mode"));
      ++stats.live_unsupported_feature_site_count;
      ++stats.live_unsupported_feature_diagnostic_count;
      return;
    }
    for (const auto &body_stmt : stmt->block_stmt->body) {
      DiagnoseUnsupportedFeatureClaimsInStmt(
          body_stmt.get(), diagnostics, stats, context);
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      DiagnoseUnsupportedFeatureClaimsInExpr(
          stmt->expr_stmt->value.get(), diagnostics, stats, context);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}
