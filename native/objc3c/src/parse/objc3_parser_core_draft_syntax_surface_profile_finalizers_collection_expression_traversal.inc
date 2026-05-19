  void CountDraftSyntaxExpr(const Expr *expr,
                            Objc3DraftSyntaxSurfaceSummary &summary) const {
    if (expr == nullptr) {
      return;
    }
    if (expr->kind == Expr::Kind::BlockLiteral) {
      ++summary.block_literal_sites;
      if (expr->block_has_explicit_capture_list) {
        ++summary.block_explicit_capture_list_sites;
      }
      summary.block_explicit_capture_byref_sites +=
          expr->block_explicit_capture_byref_count;
      summary.block_byref_capture_sites += expr->block_byref_capture_count;
      if (expr->block_escape_shape_promotes_to_heap_candidate) {
        ++summary.block_heap_escape_candidate_sites;
      }
      CountDraftSyntaxStatements(expr->block_body, summary);
    }
    if (expr->try_expression_enabled) {
      ++summary.try_expression_sites;
    }
    if (expr->throw_statement_enabled) {
      ++summary.throw_statement_sites;
    }
    if (expr->await_expression_enabled) {
      ++summary.await_expression_sites;
    }
    CountDraftSyntaxExpr(expr->receiver.get(), summary);
    CountDraftSyntaxExpr(expr->left.get(), summary);
    CountDraftSyntaxExpr(expr->right.get(), summary);
    CountDraftSyntaxExpr(expr->third.get(), summary);
    for (const auto &arg : expr->args) {
      CountDraftSyntaxExpr(arg.get(), summary);
    }
  }
