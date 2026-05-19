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
