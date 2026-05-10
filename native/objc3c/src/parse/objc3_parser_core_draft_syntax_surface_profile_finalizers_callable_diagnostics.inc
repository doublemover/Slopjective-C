  template <typename TCallableDecl>
  void CountDraftSyntaxCallable(const TCallableDecl &decl,
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
    if (decl.objc_macro_declared) {
      ++summary.macro_attribute_sites;
    }
    if (decl.objc_macro_package_declared) {
      ++summary.macro_package_sites;
    }
    if (decl.objc_macro_provenance_declared) {
      ++summary.macro_provenance_sites;
    }
    if (decl.objc_macro_cache_key_declared) {
      ++summary.macro_cache_key_sites;
    }
    if (decl.objc_macro_sandbox_declared) {
      ++summary.macro_sandbox_policy_sites;
    }
    if (decl.objc_foreign_declared || decl.objc_import_module_declared ||
        decl.objc_swift_name_declared || decl.objc_swift_private_declared ||
        decl.objc_cxx_name_declared || decl.objc_header_name_declared ||
        decl.objc_export_header_declared || decl.objc_abi_align_declared ||
        decl.objc_foreign_type_declared || decl.objc_mixed_image_declared ||
        decl.objc_package_entry_declared ||
        decl.objc_nserror_declared || decl.objc_status_code_declared) {
      ++summary.interop_attribute_sites;
    }
    if (decl.objc_export_header_declared) {
      ++summary.interop_header_export_sites;
    }
    if (decl.objc_abi_align_declared) {
      ++summary.interop_abi_alignment_sites;
    }
    if (decl.objc_foreign_type_declared) {
      ++summary.interop_foreign_type_sites;
    }
    if (decl.objc_mixed_image_declared) {
      ++summary.interop_mixed_image_sites;
    }
    if (decl.objc_package_entry_declared) {
      ++summary.interop_package_entry_sites;
    }
    if (decl.objc_import_module_declared) {
      ++summary.interop_import_module_sites;
    }
    if (decl.objc_swift_name_declared) {
      ++summary.interop_swift_annotation_sites;
    }
    if (decl.objc_swift_private_declared) {
      ++summary.interop_swift_annotation_sites;
    }
    if (decl.objc_cxx_name_declared) {
      ++summary.interop_cxx_annotation_sites;
    }
    if (decl.objc_header_name_declared) {
      ++summary.interop_header_import_sites;
    }
    summary.interop_error_bridge_sites +=
        decl.objc_nserror_attribute_sites + decl.objc_status_code_attribute_sites;
    CountDraftSyntaxStatements(decl.body, summary);
  }
