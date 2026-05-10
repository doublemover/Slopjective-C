  std::size_t CountObjc3DraftSyntaxSurfaceSites(
      const Objc3DraftSyntaxSurfaceSummary &summary) const {
    return summary.block_literal_sites + summary.block_explicit_capture_list_sites +
           summary.block_explicit_capture_byref_sites +
           summary.block_byref_capture_sites +
           summary.block_heap_escape_candidate_sites + summary.try_expression_sites +
           summary.throw_statement_sites + summary.do_catch_sites +
           summary.error_catch_clause_sites + summary.error_catch_binding_sites +
           summary.error_catch_all_sites + summary.error_bridge_payload_sites +
           summary.error_foreign_boundary_annotation_sites +
           summary.error_nested_cleanup_marker_sites +
           summary.throws_callable_sites + summary.async_callable_sites +
           summary.await_expression_sites + summary.actor_interface_sites +
           summary.actor_nonisolated_callable_sites +
           summary.actor_isolation_marker_sites +
           summary.actor_sendable_annotation_sites +
           summary.task_runtime_construct_sites +
           summary.task_cancellation_check_sites +
           summary.task_suspension_point_sites +
           summary.macro_attribute_sites + summary.macro_package_sites +
           summary.macro_provenance_sites + summary.macro_cache_key_sites +
           summary.macro_sandbox_policy_sites + summary.property_behavior_sites +
           summary.property_attribute_sites +
           summary.property_accessor_selector_sites +
           summary.property_synthesis_metadata_sites +
           summary.property_reflection_input_sites +
           summary.property_ownership_nullability_sites +
           summary.interop_attribute_sites + summary.interop_import_module_sites +
           summary.interop_swift_annotation_sites +
           summary.interop_cxx_annotation_sites +
           summary.interop_header_import_sites +
           summary.interop_header_export_sites +
           summary.interop_abi_alignment_sites + summary.interop_foreign_type_sites +
           summary.interop_mixed_image_sites + summary.interop_package_entry_sites +
           summary.interop_error_bridge_sites;
  }

  bool IsObjc3DraftSyntaxSurfaceSummaryNormalized(
      const Objc3DraftSyntaxSurfaceSummary &summary) const {
    return !summary.replay_key.empty() &&
           summary.draft_syntax_surface_sites >= summary.block_literal_sites &&
           summary.block_explicit_capture_byref_sites <=
               summary.block_byref_capture_sites &&
           summary.actor_nonisolated_callable_sites <=
               summary.actor_isolation_marker_sites + summary.actor_interface_sites &&
           summary.task_cancellation_check_sites <=
               summary.task_runtime_construct_sites + summary.task_suspension_point_sites &&
           summary.draft_syntax_surface_sites >= summary.interop_attribute_sites;
  }
