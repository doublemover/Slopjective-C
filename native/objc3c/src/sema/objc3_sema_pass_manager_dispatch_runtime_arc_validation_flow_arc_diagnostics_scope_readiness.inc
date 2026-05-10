namespace {

static void
PopulateObjc3SemaDispatchRuntimeArcValidationFlowArcDiagnosticsScopeReadiness(
    Objc3SemaDispatchRuntimeArcParityValidationReadinessRecord &record,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_arc_diagnostics_fixit_handoff,
    bool deterministic_autoreleasepool_scope_handoff) {
  const auto &arc_diagnostics_summary = surface.arc_diagnostics_fixit_summary;
  record.arc_diagnostics_fixit_ready =
      deterministic_arc_diagnostics_fixit_handoff &&
      arc_diagnostics_summary.ownership_arc_diagnostic_candidate_sites ==
          surface.ownership_arc_diagnostic_candidate_sites_total &&
      arc_diagnostics_summary.ownership_arc_fixit_available_sites ==
          surface.ownership_arc_fixit_available_sites_total &&
      arc_diagnostics_summary.ownership_arc_profiled_sites ==
          surface.ownership_arc_profiled_sites_total &&
      arc_diagnostics_summary.ownership_arc_weak_unowned_conflict_diagnostic_sites ==
          surface.ownership_arc_weak_unowned_conflict_diagnostic_sites_total &&
      arc_diagnostics_summary.ownership_arc_empty_fixit_hint_sites ==
          surface.ownership_arc_empty_fixit_hint_sites_total &&
      arc_diagnostics_summary.contract_violation_sites ==
          surface.ownership_arc_contract_violation_sites_total &&
      arc_diagnostics_summary.ownership_arc_fixit_available_sites <=
          arc_diagnostics_summary.ownership_arc_diagnostic_candidate_sites +
              arc_diagnostics_summary.contract_violation_sites &&
      arc_diagnostics_summary.ownership_arc_profiled_sites <=
          arc_diagnostics_summary.ownership_arc_diagnostic_candidate_sites +
              arc_diagnostics_summary.contract_violation_sites &&
      arc_diagnostics_summary
              .ownership_arc_weak_unowned_conflict_diagnostic_sites <=
          arc_diagnostics_summary.ownership_arc_diagnostic_candidate_sites +
              arc_diagnostics_summary.contract_violation_sites &&
      arc_diagnostics_summary.ownership_arc_empty_fixit_hint_sites <=
          arc_diagnostics_summary.ownership_arc_fixit_available_sites +
              arc_diagnostics_summary.contract_violation_sites &&
      arc_diagnostics_summary.deterministic;
  const auto &autoreleasepool_summary = surface.autoreleasepool_scope_summary;
  record.autoreleasepool_scope_ready =
      deterministic_autoreleasepool_scope_handoff &&
      autoreleasepool_summary.scope_sites ==
          surface.autoreleasepool_scope_sites_total &&
      autoreleasepool_summary.scope_symbolized_sites ==
          surface.autoreleasepool_scope_symbolized_sites_total &&
      autoreleasepool_summary.contract_violation_sites ==
          surface.autoreleasepool_scope_contract_violation_sites_total &&
      autoreleasepool_summary.max_scope_depth ==
          surface.autoreleasepool_scope_max_depth_total &&
      autoreleasepool_summary.scope_symbolized_sites <=
          autoreleasepool_summary.scope_sites &&
      autoreleasepool_summary.contract_violation_sites <=
          autoreleasepool_summary.scope_sites &&
      (autoreleasepool_summary.scope_sites > 0u ||
       autoreleasepool_summary.max_scope_depth == 0u) &&
      autoreleasepool_summary.max_scope_depth <=
          static_cast<unsigned>(autoreleasepool_summary.scope_sites) &&
      autoreleasepool_summary.deterministic;
}

}  // namespace
