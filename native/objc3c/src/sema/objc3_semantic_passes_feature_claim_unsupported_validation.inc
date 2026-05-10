static void DiagnoseEscapingBlockRuntimeHandleCaptureLegality(
    unsigned line, unsigned column, const SemanticTypeInfo &value,
    std::vector<std::string> &diagnostics) {
  (void)line;
  (void)column;
  (void)diagnostics;
  if (!value.callable_block_runtime_handle_candidate) {
    return;
  }
}

static void DiagnoseUnsupportedFeatureClaimsInExpr(
    const Expr *expr,
    std::vector<std::string> &diagnostics,
    Objc3UnsupportedFeatureClaimEnforcementStats &stats,
    const Objc3UnsupportedFeatureClaimContext &context);

static bool BlockLiteralUsesRunnableC003Subset(const Expr &expr) {
  // byref-cell/copy-helper/dispose-helper anchor: the runnable block
  // slice now admits non-escaping byref and ownership-sensitive captures at the
  // unsupported-feature gate as soon as the source model is normalized. The
  // gate intentionally remains parser/source-truth driven so later sema and
  // lowering passes can consume the real byref/helper lowering path without
  // depending on post-sema runtime counters. Heap-promotion remains deferred
  // to later lowering/runtime checks instead of this source admission step.
  return expr.block_literal_is_normalized &&
         expr.block_source_model_is_normalized &&
         expr.block_parameter_count <= 4u &&
         expr.block_source_storage_annotations_are_normalized;
}

static bool BlockLiteralUsesRunnableC002Subset(const Expr &expr) {
  // executable-block-object/invoke-thunk anchor: preserve the
  // original runnable-block gate name as a compatibility surface for the C002
  // readiness contract while C003 widens the admitted non-escaping subset.
  return BlockLiteralUsesRunnableC003Subset(expr);
}

static bool ExprContainsBlockLiteral(const Expr *expr);

static bool StmtContainsBlockLiteral(const Stmt *stmt) {
  if (stmt == nullptr) {
    return false;
  }

  switch (stmt->kind) {
    case Stmt::Kind::Let:
      return stmt->let_stmt != nullptr &&
             ExprContainsBlockLiteral(stmt->let_stmt->value.get());
    case Stmt::Kind::Assign:
      return stmt->assign_stmt != nullptr &&
             ExprContainsBlockLiteral(stmt->assign_stmt->value.get());
    case Stmt::Kind::Return:
      return stmt->return_stmt != nullptr &&
             ExprContainsBlockLiteral(stmt->return_stmt->value.get());
    case Stmt::Kind::If:
      if (stmt->if_stmt == nullptr) {
        return false;
      }
      if (ExprContainsBlockLiteral(stmt->if_stmt->condition.get())) {
        return true;
      }
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        if (StmtContainsBlockLiteral(then_stmt.get())) {
          return true;
        }
      }
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        if (StmtContainsBlockLiteral(else_stmt.get())) {
          return true;
        }
      }
      return false;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt == nullptr) {
        return false;
      }
      for (const auto &body_stmt : stmt->do_while_stmt->body) {
        if (StmtContainsBlockLiteral(body_stmt.get())) {
          return true;
        }
      }
      return ExprContainsBlockLiteral(stmt->do_while_stmt->condition.get());
    case Stmt::Kind::For:
      if (stmt->for_stmt == nullptr) {
        return false;
      }
      if (ExprContainsBlockLiteral(stmt->for_stmt->init.value.get()) ||
          ExprContainsBlockLiteral(stmt->for_stmt->condition.get()) ||
          ExprContainsBlockLiteral(stmt->for_stmt->step.value.get())) {
        return true;
      }
      for (const auto &body_stmt : stmt->for_stmt->body) {
        if (StmtContainsBlockLiteral(body_stmt.get())) {
          return true;
        }
      }
      return false;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt == nullptr) {
        return false;
      }
      if (ExprContainsBlockLiteral(stmt->switch_stmt->condition.get())) {
        return true;
      }
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &case_stmt : switch_case.body) {
          if (StmtContainsBlockLiteral(case_stmt.get())) {
            return true;
          }
        }
      }
      return false;
    case Stmt::Kind::While:
      if (stmt->while_stmt == nullptr) {
        return false;
      }
      if (ExprContainsBlockLiteral(stmt->while_stmt->condition.get())) {
        return true;
      }
      for (const auto &body_stmt : stmt->while_stmt->body) {
        if (StmtContainsBlockLiteral(body_stmt.get())) {
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
        if (StmtContainsBlockLiteral(body_stmt.get())) {
          return true;
        }
      }
      return false;
    case Stmt::Kind::Expr:
      return stmt->expr_stmt != nullptr &&
             ExprContainsBlockLiteral(stmt->expr_stmt->value.get());
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return false;
  }
  return false;
}

static bool ExprContainsBlockLiteral(const Expr *expr) {
  if (expr == nullptr) {
    return false;
  }
  if (expr->kind == Expr::Kind::BlockLiteral) {
    return true;
  }

  if (ExprContainsBlockLiteral(expr->left.get()) ||
      ExprContainsBlockLiteral(expr->right.get()) ||
      ExprContainsBlockLiteral(expr->third.get()) ||
      ExprContainsBlockLiteral(expr->receiver.get())) {
    return true;
  }
  for (const auto &arg : expr->args) {
    if (ExprContainsBlockLiteral(arg.get())) {
      return true;
    }
  }
  return false;
}

static bool FunctionBodyContainsBlockLiteral(const FunctionDecl &fn) {
  for (const auto &stmt : fn.body) {
    if (StmtContainsBlockLiteral(stmt.get())) {
      return true;
    }
  }
  return false;
}

static bool MethodBodyContainsBlockLiteral(const Objc3MethodDecl &method) {
  for (const auto &stmt : method.body) {
    if (StmtContainsBlockLiteral(stmt.get())) {
      return true;
    }
  }
  return false;
}

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

static void DiagnoseUnsupportedFeatureClaimsInExpr(
    const Expr *expr,
    std::vector<std::string> &diagnostics,
    Objc3UnsupportedFeatureClaimEnforcementStats &stats,
    const Objc3UnsupportedFeatureClaimContext &context) {
  if (expr == nullptr) {
    return;
  }

  switch (expr->kind) {
  // executable-block-source-closure anchor: once block literals
  // parse successfully they still fail closed here until the next runtime step and later
  // runtime issues make them runnable.
  case Expr::Kind::BlockLiteral:
    // block-source-model-completion anchor: source-only frontend
    // projection runs may admit block literals through sema so manifest-only
    // tooling can prove the completed source model, while native emit paths
    // continue to fail closed until later lowering/runtime work lands.
    // block-runtime-semantic-rules freeze anchor: current semantic
    // behavior is intentionally split between source-only admission and native
    // fail-closed rejection, with no runnable block object semantics implied.
    if (ShouldRejectBlockLiteralFeatureClaim(
            context,
            BlockLiteralUsesRunnableC002Subset(*expr))) {
      RecordUnsupportedFeatureClaimDiagnostic(
          stats.blocks_source_rejection_site_count,
          expr->line,
          expr->column,
          "unsupported feature claim: block literals are not yet runnable in Objective-C 3 native mode",
          diagnostics,
          stats);
    }
    return;
  case Expr::Kind::Call:
    for (const auto &arg : expr->args) {
      DiagnoseUnsupportedFeatureClaimsInExpr(arg.get(), diagnostics, stats, context);
    }
    return;
  case Expr::Kind::MessageSend:
    DiagnoseUnsupportedFeatureClaimsInExpr(
        expr->receiver.get(), diagnostics, stats, context);
    for (const auto &arg : expr->args) {
      DiagnoseUnsupportedFeatureClaimsInExpr(arg.get(), diagnostics, stats, context);
    }
    return;
  case Expr::Kind::Binary:
    DiagnoseUnsupportedFeatureClaimsInExpr(
        expr->left.get(), diagnostics, stats, context);
    DiagnoseUnsupportedFeatureClaimsInExpr(
        expr->right.get(), diagnostics, stats, context);
    return;
  case Expr::Kind::Conditional:
    DiagnoseUnsupportedFeatureClaimsInExpr(
        expr->left.get(), diagnostics, stats, context);
    DiagnoseUnsupportedFeatureClaimsInExpr(
        expr->right.get(), diagnostics, stats, context);
    DiagnoseUnsupportedFeatureClaimsInExpr(
        expr->third.get(), diagnostics, stats, context);
    return;
  case Expr::Kind::BoolLiteral:
  case Expr::Kind::Identifier:
  case Expr::Kind::NilLiteral:
  case Expr::Kind::Number:
  default:
    return;
  }
}
