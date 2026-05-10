  template <typename TCallableDecl>
  void CountDraftSyntaxCallable(const TCallableDecl &decl,
                                Objc3DraftSyntaxSurfaceSummary &summary) const {
    CountDraftSyntaxCallableErrorCleanupDiagnostics(decl, summary);
    CountDraftSyntaxCallableConcurrencyTaskDiagnostics(decl, summary);
    CountDraftSyntaxCallableMacroDiagnostics(decl, summary);
    CountDraftSyntaxCallableInteropDiagnostics(decl, summary);
    CountDraftSyntaxStatements(decl.body, summary);
  }
