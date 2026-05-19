  std::string BuildObjc3DraftSyntaxSurfaceReplayKey(
      const Objc3DraftSyntaxSurfaceSummary &summary) const {
    std::ostringstream out;
    out << "draft-syntax-surfaces:v1"
        << ";blocks=" << summary.block_literal_sites
        << ";block_capture_lists=" << summary.block_explicit_capture_list_sites
        << ";block_byref_explicit=" << summary.block_explicit_capture_byref_sites
        << ";block_byref_captures=" << summary.block_byref_capture_sites
        << ";block_heap_candidates=" << summary.block_heap_escape_candidate_sites
        << ";try=" << summary.try_expression_sites
        << ";throw=" << summary.throw_statement_sites
        << ";do_catch=" << summary.do_catch_sites
        << ";error_catch_clauses=" << summary.error_catch_clause_sites
        << ";error_catch_bindings=" << summary.error_catch_binding_sites
        << ";error_catch_all=" << summary.error_catch_all_sites
        << ";error_bridge_payloads=" << summary.error_bridge_payload_sites
        << ";error_foreign_boundaries="
        << summary.error_foreign_boundary_annotation_sites
        << ";error_nested_cleanup_markers="
        << summary.error_nested_cleanup_marker_sites
        << ";throws_callables=" << summary.throws_callable_sites
        << ";async_callables=" << summary.async_callable_sites
        << ";await=" << summary.await_expression_sites
        << ";actors=" << summary.actor_interface_sites
        << ";actor_nonisolated=" << summary.actor_nonisolated_callable_sites
        << ";actor_isolation_markers=" << summary.actor_isolation_marker_sites
        << ";actor_sendable_markers=" << summary.actor_sendable_annotation_sites
        << ";task_runtime=" << summary.task_runtime_construct_sites
        << ";task_cancellation_checks=" << summary.task_cancellation_check_sites
        << ";task_suspension_points=" << summary.task_suspension_point_sites
        << ";macro_attrs=" << summary.macro_attribute_sites
        << ";macro_packages=" << summary.macro_package_sites
        << ";macro_provenance=" << summary.macro_provenance_sites
        << ";macro_cache_keys=" << summary.macro_cache_key_sites
        << ";macro_sandbox_policies=" << summary.macro_sandbox_policy_sites
        << ";property_behaviors=" << summary.property_behavior_sites
        << ";property_attrs=" << summary.property_attribute_sites
        << ";property_accessor_selectors="
        << summary.property_accessor_selector_sites
        << ";property_synthesis_metadata="
        << summary.property_synthesis_metadata_sites
        << ";property_reflection_inputs="
        << summary.property_reflection_input_sites
        << ";property_ownership_nullability="
        << summary.property_ownership_nullability_sites
        << ";interop_attrs=" << summary.interop_attribute_sites
        << ";interop_import_modules=" << summary.interop_import_module_sites
        << ";interop_swift_annotations="
        << summary.interop_swift_annotation_sites
        << ";interop_cxx_annotations=" << summary.interop_cxx_annotation_sites
        << ";interop_header_imports=" << summary.interop_header_import_sites
        << ";interop_export_headers=" << summary.interop_header_export_sites
        << ";interop_abi_align=" << summary.interop_abi_alignment_sites
        << ";interop_foreign_type=" << summary.interop_foreign_type_sites
        << ";interop_mixed_images=" << summary.interop_mixed_image_sites
        << ";interop_package_entries=" << summary.interop_package_entry_sites
        << ";interop_error_bridges=" << summary.interop_error_bridge_sites
        << ";total=" << summary.draft_syntax_surface_sites;
    return out.str();
  }
