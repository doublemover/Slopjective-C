Objc3ConcurrencyActorIsolationSendableSemanticModelSummary
BuildConcurrencyActorIsolationSendableSemanticModelSummary(
    const Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary
        &source_summary,
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3ConcurrencyActorIsolationSendableSemanticModelSummary summary;
  const Objc3ActorIsolationSendabilitySummary &actor_summary =
      surface.actor_isolation_sendability_summary;

  summary.actor_interface_sites = source_summary.actor_interface_sites;
  summary.actor_method_sites = source_summary.actor_method_sites;
  summary.actor_property_sites = source_summary.actor_property_sites;
  summary.objc_nonisolated_annotation_sites =
      source_summary.objc_nonisolated_annotation_sites;
  summary.actor_member_executor_annotation_sites =
      source_summary.actor_member_executor_annotation_sites;
  summary.actor_async_method_sites = source_summary.actor_async_method_sites;
  summary.actor_member_metadata_sites =
      source_summary.actor_member_metadata_sites;
  summary.actor_isolation_sendability_sites =
      actor_summary.actor_isolation_sendability_sites;
  summary.actor_isolation_decl_sites = actor_summary.actor_isolation_decl_sites;
  summary.actor_hop_sites = actor_summary.actor_hop_sites;
  summary.sendable_annotation_sites = actor_summary.sendable_annotation_sites;
  summary.non_sendable_crossing_sites =
      actor_summary.non_sendable_crossing_sites;
  summary.isolation_boundary_sites = actor_summary.isolation_boundary_sites;
  summary.normalized_sites = actor_summary.normalized_sites;
  summary.gate_blocked_sites = actor_summary.gate_blocked_sites;
  summary.contract_violation_sites = actor_summary.contract_violation_sites;

  summary.source_dependency_required = true;
  summary.actor_member_source_supported =
      source_summary.actor_declaration_source_supported &&
      source_summary.actor_member_source_supported &&
      source_summary.isolation_annotation_source_supported &&
      source_summary.actor_metadata_surface_supported &&
      source_summary.deterministic_handoff &&
      summary.actor_member_metadata_sites ==
          summary.actor_method_sites + summary.actor_property_sites;
  summary.actor_isolation_sendability_profile_normalized =
      actor_summary.deterministic &&
      summary.normalized_sites + summary.gate_blocked_sites ==
          summary.actor_isolation_sendability_sites &&
      summary.gate_blocked_sites <= summary.non_sendable_crossing_sites &&
      summary.contract_violation_sites == 0u;
  summary.strict_concurrency_selection_supported = true;
  summary.actor_runtime_deferred = true;
  summary.executor_runtime_deferred = true;
  summary.cross_actor_enforcement_deferred = true;
  summary.deterministic =
      summary.actor_member_source_supported &&
      summary.actor_isolation_sendability_profile_normalized;
  summary.ready_for_semantic_expansion = summary.deterministic;
  if (!summary.deterministic) {
    summary.failure_reason =
        "actor-member source closure and actor/sendability profile must remain deterministic";
  }

  std::ostringstream out;
  out << summary.contract_id
      << ";source-dependency=" << summary.frontend_dependency_contract_id
      << ";actor-interfaces=" << summary.actor_interface_sites
      << ";actor-methods=" << summary.actor_method_sites
      << ";actor-properties=" << summary.actor_property_sites
      << ";nonisolated-sites=" << summary.objc_nonisolated_annotation_sites
      << ";actor-member-executor-sites="
      << summary.actor_member_executor_annotation_sites
      << ";actor-async-method-sites=" << summary.actor_async_method_sites
      << ";actor-metadata-sites=" << summary.actor_member_metadata_sites
      << ";actor-sendability-sites="
      << summary.actor_isolation_sendability_sites
      << ";actor-decl-sites=" << summary.actor_isolation_decl_sites
      << ";actor-hop-sites=" << summary.actor_hop_sites
      << ";sendable-sites=" << summary.sendable_annotation_sites
      << ";non-sendable-sites=" << summary.non_sendable_crossing_sites
      << ";isolation-boundary-sites=" << summary.isolation_boundary_sites
      << ";normalized-sites=" << summary.normalized_sites
      << ";gate-blocked-sites=" << summary.gate_blocked_sites
      << ";contract-violation-sites=" << summary.contract_violation_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}
