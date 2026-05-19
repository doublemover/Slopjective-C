  template <typename TCallableDecl>
  void CountDraftSyntaxCallableConcurrencyTaskDiagnostics(
      const TCallableDecl &decl,
      Objc3DraftSyntaxSurfaceSummary &summary) const {
    if (decl.async_declared) {
      ++summary.async_callable_sites;
    }
    if (decl.objc_nonisolated_declared) {
      ++summary.actor_nonisolated_callable_sites;
    }
    summary.actor_isolation_marker_sites += decl.actor_isolation_decl_sites;
    if (decl.objc_nonisolated_declared) {
      ++summary.actor_isolation_marker_sites;
    }
    summary.actor_sendable_annotation_sites += decl.sendable_annotation_sites;
    summary.task_runtime_construct_sites += decl.runtime_hook_sites;
    summary.task_cancellation_check_sites += decl.cancellation_check_sites;
    summary.task_suspension_point_sites += decl.suspension_point_sites;
  }
