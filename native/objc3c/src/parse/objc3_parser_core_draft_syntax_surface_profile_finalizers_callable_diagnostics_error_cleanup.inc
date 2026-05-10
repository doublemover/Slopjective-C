  template <typename TCallableDecl>
  void CountDraftSyntaxCallableErrorCleanupDiagnostics(
      const TCallableDecl &decl,
      Objc3DraftSyntaxSurfaceSummary &summary) const {
    if (decl.throws_declared) {
      ++summary.throws_callable_sites;
    }
    summary.error_bridge_payload_sites += decl.objc_nserror_attribute_sites;
    summary.error_bridge_payload_sites += decl.status_code_success_clause_sites;
    summary.error_bridge_payload_sites += decl.status_code_error_type_clause_sites;
    summary.error_bridge_payload_sites += decl.status_code_mapping_clause_sites;
    summary.error_foreign_boundary_annotation_sites +=
        decl.objc_nserror_attribute_sites + decl.objc_status_code_attribute_sites;
    summary.error_nested_cleanup_marker_sites +=
        decl.cleanup_action_sites + decl.cleanup_scope_sites +
        decl.cleanup_resume_sites;
  }
