std::string BuildToolingDiagnosticTaxonomyPortabilityContractSummaryJson(
    const Objc3ToolingDiagnosticTaxonomyPortabilityContractSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"semantic_model\":\""
      << EscapeJsonString(summary.semantic_model)
      << "\",\"portability_model\":\""
      << EscapeJsonString(summary.portability_model)
      << "\",\"portability_dependency_surface_ids\":"
      << BuildStringArrayJson(summary.portability_dependency_surface_ids)
      << ",\"diagnostic_namespace\":\""
      << EscapeJsonString(summary.diagnostic_namespace)
      << "\",\"portability_dependency_count\":"
      << summary.portability_dependency_count
      << ",\"diagnostics_total\":" << summary.diagnostics_total
      << ",\"diagnostics_after_pass_final\":"
      << summary.diagnostics_after_pass_final
      << ",\"diagnostics_emitted_total\":"
      << summary.diagnostics_emitted_total
      << ",\"ownership_arc_diagnostic_candidate_sites\":"
      << summary.ownership_arc_diagnostic_candidate_sites
      << ",\"ownership_arc_fixit_available_sites\":"
      << summary.ownership_arc_fixit_available_sites
      << ",\"ownership_arc_profiled_sites\":"
      << summary.ownership_arc_profiled_sites
      << ",\"ownership_arc_weak_unowned_conflict_diagnostic_sites\":"
      << summary.ownership_arc_weak_unowned_conflict_diagnostic_sites
      << ",\"ownership_arc_empty_fixit_hint_sites\":"
      << summary.ownership_arc_empty_fixit_hint_sites
      << ",\"ownership_arc_contract_violation_sites\":"
      << summary.ownership_arc_contract_violation_sites
      << ",\"migration_canonicalization_candidate_sites\":"
      << summary.migration_canonicalization_candidate_sites
      << ",\"migration_surface_ready\":"
      << (summary.migration_surface_ready ? "true" : "false")
      << ",\"portability_dependencies_frozen\":"
      << (summary.portability_dependencies_frozen ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"recovery_replay_key\":\""
      << EscapeJsonString(summary.recovery_replay_key)
      << "\",\"arc_diagnostics_fixit_replay_key\":\""
      << EscapeJsonString(summary.arc_diagnostics_fixit_replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}
