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
