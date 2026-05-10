Objc3DispatchDispatchControlLoweringContract
BuildDispatchDispatchControlLoweringSnapshot(
    const Objc3DispatchDispatchIntentSemanticModelSummary &semantic_summary,
    const Objc3DispatchDispatchIntentLegalitySummary &legality_summary,
    const Objc3DispatchDispatchIntentCompatibilitySummary
        &compatibility_summary) {
  Objc3DispatchDispatchControlLoweringContract contract;
  const std::size_t dispatch_intent_callable_capacity =
      compatibility_summary.callable_dispatch_intent_sites;
  const std::size_t dispatch_intent_container_capacity =
      compatibility_summary.container_dispatch_intent_sites;
  const std::size_t dispatch_intent_override_capacity =
      semantic_summary.effective_direct_member_sites +
      dispatch_intent_container_capacity;
  const std::size_t dispatch_intent_guard_capacity =
      dispatch_intent_callable_capacity + dispatch_intent_container_capacity;
  const std::size_t raw_guard_blocked_sites =
      legality_summary.illegal_final_superclass_sites +
      legality_summary.illegal_sealed_superclass_sites +
      legality_summary.illegal_final_override_sites +
      legality_summary.illegal_direct_override_sites +
      compatibility_summary.illegal_direct_dynamic_conflict_sites +
      compatibility_summary.illegal_final_dynamic_conflict_sites +
      compatibility_summary.illegal_non_method_callable_sites +
      compatibility_summary.illegal_protocol_method_sites +
      compatibility_summary.illegal_category_method_sites +
      compatibility_summary.illegal_category_container_sites;
  contract.direct_call_candidate_sites =
      semantic_summary.effective_direct_member_sites;
  contract.direct_members_defaulted_sites =
      semantic_summary.direct_members_defaulted_method_sites;
  contract.dynamic_opt_out_sites =
      semantic_summary.direct_members_dynamic_opt_out_sites;
  contract.final_container_sites = semantic_summary.final_container_sites;
  contract.sealed_container_sites = semantic_summary.sealed_container_sites;
  contract.override_legality_sites =
      std::min(legality_summary.override_sites,
               dispatch_intent_override_capacity);
  contract.metadata_preserved_callable_sites =
      dispatch_intent_callable_capacity;
  contract.metadata_preserved_container_sites =
      dispatch_intent_container_capacity;
  contract.guard_blocked_sites =
      std::min(raw_guard_blocked_sites, dispatch_intent_guard_capacity);
  contract.contract_violation_sites = 0;
  contract.deterministic =
      semantic_summary.deterministic &&
      semantic_summary.ready_for_core_implementation &&
      legality_summary.deterministic &&
      legality_summary.ready_for_lowering_and_runtime &&
      compatibility_summary.deterministic &&
      compatibility_summary.ready_for_lowering_and_runtime;
  return contract;
}
