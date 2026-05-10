namespace {

static void PopulateObjc3SemaDispatchRuntimeArcValidationFlowArcOwnershipReadiness(
    Objc3SemaDispatchRuntimeArcParityValidationReadinessRecord &record,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_retain_release_operation_handoff,
    bool deterministic_weak_unowned_semantics_handoff) {
  const auto &retain_release_summary = surface.retain_release_operation_summary;
  record.retain_release_operation_ready =
      deterministic_retain_release_operation_handoff &&
      retain_release_summary.ownership_qualified_sites ==
          surface.retain_release_operation_ownership_qualified_sites_total &&
      retain_release_summary.retain_insertion_sites ==
          surface.retain_release_operation_retain_insertion_sites_total &&
      retain_release_summary.release_insertion_sites ==
          surface.retain_release_operation_release_insertion_sites_total &&
      retain_release_summary.autorelease_insertion_sites ==
          surface.retain_release_operation_autorelease_insertion_sites_total &&
      retain_release_summary.contract_violation_sites ==
          surface.retain_release_operation_contract_violation_sites_total &&
      retain_release_summary.retain_insertion_sites <=
          retain_release_summary.ownership_qualified_sites +
              retain_release_summary.contract_violation_sites &&
      retain_release_summary.release_insertion_sites <=
          retain_release_summary.ownership_qualified_sites +
              retain_release_summary.contract_violation_sites &&
      retain_release_summary.autorelease_insertion_sites <=
          retain_release_summary.ownership_qualified_sites +
              retain_release_summary.contract_violation_sites &&
      retain_release_summary.deterministic;
  const auto &weak_unowned_summary = surface.weak_unowned_semantics_summary;
  record.weak_unowned_semantics_ready =
      deterministic_weak_unowned_semantics_handoff &&
      weak_unowned_summary.ownership_candidate_sites ==
          surface.weak_unowned_semantics_ownership_candidate_sites_total &&
      weak_unowned_summary.weak_reference_sites ==
          surface.weak_unowned_semantics_weak_reference_sites_total &&
      weak_unowned_summary.unowned_reference_sites ==
          surface.weak_unowned_semantics_unowned_reference_sites_total &&
      weak_unowned_summary.unowned_safe_reference_sites ==
          surface.weak_unowned_semantics_unowned_safe_reference_sites_total &&
      weak_unowned_summary.weak_unowned_conflict_sites ==
          surface.weak_unowned_semantics_conflict_sites_total &&
      weak_unowned_summary.contract_violation_sites ==
          surface.weak_unowned_semantics_contract_violation_sites_total &&
      weak_unowned_summary.unowned_safe_reference_sites <=
          weak_unowned_summary.unowned_reference_sites &&
      weak_unowned_summary.weak_unowned_conflict_sites <=
          weak_unowned_summary.ownership_candidate_sites &&
      weak_unowned_summary.contract_violation_sites <=
          weak_unowned_summary.ownership_candidate_sites +
              weak_unowned_summary.weak_unowned_conflict_sites &&
      weak_unowned_summary.deterministic;
}

}  // namespace
