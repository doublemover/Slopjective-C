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

  void CountDraftSyntaxForClause(const ForClause &clause,
                                 Objc3DraftSyntaxSurfaceSummary &summary) const {
    CountDraftSyntaxExpr(clause.value.get(), summary);
  }

  void CountDraftSyntaxBlock(const BlockStmt *block,
                             Objc3DraftSyntaxSurfaceSummary &summary) const {
    if (block == nullptr) {
      return;
    }
    if (block->is_do_catch_scope) {
      ++summary.do_catch_sites;
    }
    CountDraftSyntaxStatements(block->body, summary);
    for (const auto &clause : block->catch_clauses) {
      ++summary.error_catch_clause_sites;
      if (clause.has_binding) {
        ++summary.error_catch_binding_sites;
      }
      if (clause.catch_all) {
        ++summary.error_catch_all_sites;
      }
      CountDraftSyntaxStatements(clause.body, summary);
    }
  }

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
