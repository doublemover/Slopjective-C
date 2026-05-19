  summary.macro_execution_deferred = true;
  summary.property_behavior_runtime_deferred = true;
  summary.deterministic =
      summary.derive_macro_source_supported &&
      summary.macro_package_provenance_surface_reused &&
      summary.property_behavior_source_supported &&
      summary.synthesized_visibility_surface_reused;
  summary.ready_for_core_implementation = summary.deterministic;
  if (!summary.deterministic) {
    summary.failure_reason =
        "metaprogramming metaprogramming source and property-behavior semantic surfaces must remain deterministic";
  }

  std::ostringstream out;
  out << summary.contract_id
      << ";source=" << source_summary.contract_id
      << ";macro-source=" << macro_summary.contract_id
      << ";property-source=" << property_summary.contract_id
      << ";derive-markers=" << summary.derive_marker_sites
      << ";macro-sites=" << summary.macro_marker_sites << ":"
      << summary.macro_package_sites << ":" << summary.macro_provenance_sites
      << ":" << summary.expansion_visible_macro_sites
      << ";property-sites=" << summary.property_behavior_sites << ":"
      << summary.interface_property_behavior_sites << ":"
      << summary.implementation_property_behavior_sites << ":"
      << summary.protocol_property_behavior_sites
      << ";synthesized-visible=" << summary.synthesized_binding_visible_sites
      << ":" << summary.synthesized_getter_visible_sites << ":"
      << summary.synthesized_setter_visible_sites
      << ";property-behavior-contract-violations="
      << summary.property_behavior_contract_violation_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}
