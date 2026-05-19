  void CountDraftSyntaxForClause(const ForClause &clause,
                                 Objc3DraftSyntaxSurfaceSummary &summary) const {
    CountDraftSyntaxExpr(clause.value.get(), summary);
  }
