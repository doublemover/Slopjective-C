  void CountDraftSyntaxStmt(const Stmt *stmt,
                            Objc3DraftSyntaxSurfaceSummary &summary) const {
    if (stmt == nullptr) {
      return;
    }
    if (stmt->kind == Stmt::Kind::Defer) {
      ++summary.error_nested_cleanup_marker_sites;
    }
    CountDraftSyntaxBlock(stmt->block_stmt.get(), summary);
    if (stmt->let_stmt != nullptr) {
      CountDraftSyntaxExpr(stmt->let_stmt->value.get(), summary);
    }
    if (stmt->assign_stmt != nullptr) {
      CountDraftSyntaxExpr(stmt->assign_stmt->value.get(), summary);
    }
    if (stmt->return_stmt != nullptr) {
      CountDraftSyntaxExpr(stmt->return_stmt->value.get(), summary);
    }
    if (stmt->expr_stmt != nullptr) {
      CountDraftSyntaxExpr(stmt->expr_stmt->value.get(), summary);
    }
    if (stmt->if_stmt != nullptr) {
      CountDraftSyntaxExpr(stmt->if_stmt->condition.get(), summary);
      CountDraftSyntaxStatements(stmt->if_stmt->then_body, summary);
      CountDraftSyntaxStatements(stmt->if_stmt->else_body, summary);
      for (const auto &condition : stmt->if_stmt->guard_condition_exprs) {
        CountDraftSyntaxExpr(condition.get(), summary);
      }
    }
    if (stmt->do_while_stmt != nullptr) {
      CountDraftSyntaxStatements(stmt->do_while_stmt->body, summary);
      CountDraftSyntaxExpr(stmt->do_while_stmt->condition.get(), summary);
    }
    if (stmt->for_stmt != nullptr) {
      CountDraftSyntaxForClause(stmt->for_stmt->init, summary);
      CountDraftSyntaxExpr(stmt->for_stmt->condition.get(), summary);
      CountDraftSyntaxForClause(stmt->for_stmt->step, summary);
      CountDraftSyntaxStatements(stmt->for_stmt->body, summary);
    }
    if (stmt->switch_stmt != nullptr) {
      CountDraftSyntaxExpr(stmt->switch_stmt->condition.get(), summary);
      for (const auto &case_stmt : stmt->switch_stmt->cases) {
        CountDraftSyntaxStatements(case_stmt.body, summary);
      }
    }
    if (stmt->while_stmt != nullptr) {
      CountDraftSyntaxExpr(stmt->while_stmt->condition.get(), summary);
      CountDraftSyntaxStatements(stmt->while_stmt->body, summary);
    }
  }

  void CountDraftSyntaxStatements(
      const std::vector<std::unique_ptr<Stmt>> &statements,
      Objc3DraftSyntaxSurfaceSummary &summary) const {
    for (const auto &stmt : statements) {
      CountDraftSyntaxStmt(stmt.get(), summary);
    }
  }
