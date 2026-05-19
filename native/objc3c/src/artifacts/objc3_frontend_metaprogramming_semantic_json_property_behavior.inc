std::string
BuildMetaprogrammingPropertyBehaviorLegalityCompatibilitySummaryJson(
    const Objc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary
        &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"semantic_dependency_contract_id\":\""
      << EscapeJsonString(summary.semantic_dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"property_behavior_sites\":" << summary.property_behavior_sites
      << ",\"supported_behavior_sites\":" << summary.supported_behavior_sites
      << ",\"unsupported_behavior_sites\":" << summary.unsupported_behavior_sites
      << ",\"observed_behavior_sites\":" << summary.observed_behavior_sites
      << ",\"projected_behavior_sites\":" << summary.projected_behavior_sites
      << ",\"observed_on_protocol_sites\":"
      << summary.observed_on_protocol_sites
      << ",\"observed_readonly_conflict_sites\":"
      << summary.observed_readonly_conflict_sites
      << ",\"projected_writable_conflict_sites\":"
      << summary.projected_writable_conflict_sites
      << ",\"non_object_behavior_sites\":" << summary.non_object_behavior_sites
      << ",\"semantic_dependency_required\":"
      << (summary.semantic_dependency_required ? "true" : "false")
      << ",\"supported_behavior_inventory_landed\":"
      << (summary.supported_behavior_inventory_landed ? "true" : "false")
      << ",\"unsupported_behavior_fail_closed\":"
      << (summary.unsupported_behavior_fail_closed ? "true" : "false")
      << ",\"owner_topology_fail_closed\":"
      << (summary.owner_topology_fail_closed ? "true" : "false")
      << ",\"interaction_legality_fail_closed\":"
      << (summary.interaction_legality_fail_closed ? "true" : "false")
      << ",\"storage_legality_fail_closed\":"
      << (summary.storage_legality_fail_closed ? "true" : "false")
      << ",\"runtime_materialization_deferred\":"
      << (summary.runtime_materialization_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}
