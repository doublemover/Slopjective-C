  summary.objc_status_code_attribute_sites =
      source_summary.objc_status_code_attribute_sites;
  summary.status_code_success_clause_sites =
      source_summary.status_code_success_clause_sites;
  summary.status_code_error_type_clause_sites =
      source_summary.status_code_error_type_clause_sites;
  summary.status_code_mapping_clause_sites =
      source_summary.status_code_mapping_clause_sites;
  summary.placeholder_throws_propagation_sites =
      surface.throws_propagation_summary.throws_propagation_sites;
  summary.placeholder_unwind_cleanup_sites =
      surface.unwind_cleanup_summary.unwind_cleanup_sites;

  summary.source_dependency_required = true;
  summary.throws_declaration_semantics_landed =
      source_summary.throws_declaration_source_supported &&
      summary.throws_declaration_sites ==
          source_summary.function_throws_declaration_sites +
              source_summary.method_throws_declaration_sites;
  summary.result_carrier_profile_semantics_landed =
      source_summary.result_carrier_source_supported &&
      surface.result_like_lowering_summary.deterministic &&
      summary.result_like_sites == source_summary.result_like_sites &&
      summary.result_success_sites == source_summary.result_success_sites &&
      summary.result_failure_sites == source_summary.result_failure_sites &&
      summary.result_branch_sites == source_summary.result_branch_sites &&
      summary.result_payload_sites == source_summary.result_payload_sites;
  summary.ns_error_bridging_profile_semantics_landed =
      source_summary.ns_error_bridging_source_supported &&
      surface.ns_error_bridging_summary.deterministic &&
      summary.ns_error_bridging_sites >= source_summary.ns_error_bridging_sites &&
      summary.ns_error_out_parameter_sites >=
          source_summary.ns_error_out_parameter_sites &&
      summary.ns_error_bridge_path_sites >=
          source_summary.ns_error_bridge_path_sites &&
      summary.ns_error_out_parameter_sites <= summary.ns_error_bridging_sites &&
      summary.ns_error_bridge_path_sites <= summary.ns_error_bridging_sites;
  summary.bridge_marker_semantics_landed =
      source_summary.error_bridge_marker_source_supported &&
      summary.objc_nserror_attribute_sites ==
          source_summary.objc_nserror_attribute_sites &&
      summary.objc_status_code_attribute_sites ==
          source_summary.objc_status_code_attribute_sites &&
      summary.status_code_success_clause_sites ==
          source_summary.status_code_success_clause_sites &&
      summary.status_code_error_type_clause_sites ==
          source_summary.status_code_error_type_clause_sites &&
      summary.status_code_mapping_clause_sites ==
          source_summary.status_code_mapping_clause_sites;
  summary.parser_fail_closed_boundary_required = true;
  summary.parser_fail_closed_boundary_preserved =
      source_summary.try_keyword_reserved &&
      source_summary.throw_keyword_reserved &&
      source_summary.catch_keyword_reserved && source_summary.try_fail_closed &&
      source_summary.throw_fail_closed && source_summary.do_catch_fail_closed;
  summary.propagation_runtime_deferred = true;
  summary.status_to_error_runtime_deferred = true;
  summary.native_error_abi_deferred = true;
  summary.placeholder_throws_summary_carried =
      surface.throws_propagation_summary.deterministic &&
      surface.unwind_cleanup_summary.deterministic;
  summary.deterministic =
      source_summary.deterministic_handoff &&
      surface.result_like_lowering_summary.deterministic &&
      surface.ns_error_bridging_summary.deterministic &&
      surface.throws_propagation_summary.deterministic &&
      surface.unwind_cleanup_summary.deterministic &&
      summary.throws_declaration_semantics_landed &&
      summary.result_carrier_profile_semantics_landed &&
      summary.ns_error_bridging_profile_semantics_landed &&
      summary.bridge_marker_semantics_landed &&
      summary.parser_fail_closed_boundary_preserved;
  summary.ready_for_lowering_and_runtime = false;

  std::ostringstream out;
  out << summary.contract_id
      << ";source-dependency=" << summary.frontend_dependency_contract_id
      << ";throws=" << summary.throws_declaration_sites
      << ";result-like=" << summary.result_like_sites
      << ";nserror=" << summary.ns_error_bridging_sites
      << ";bridge-markers=" << summary.objc_nserror_attribute_sites << ":"
      << summary.objc_status_code_attribute_sites
      << ";placeholder-throws=" << summary.placeholder_throws_propagation_sites
      << ";placeholder-unwind=" << summary.placeholder_unwind_cleanup_sites
      << ";parser-fail-closed="
      << (summary.parser_fail_closed_boundary_preserved ? "true" : "false")
      << ";propagation-runtime-deferred="
      << (summary.propagation_runtime_deferred ? "true" : "false")
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}
