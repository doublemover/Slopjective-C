std::string BuildNSErrorBridgingProfile(
    std::size_t ns_error_bridging_sites,
    std::size_t ns_error_parameter_sites,
    std::size_t ns_error_out_parameter_sites,
    std::size_t ns_error_bridge_path_sites,
    std::size_t failable_call_sites,
    std::size_t normalized_sites,
    std::size_t bridge_boundary_sites,
    std::size_t contract_violation_sites,
    bool deterministic_ns_error_bridging_lowering_handoff) {
  std::ostringstream out;
  out << "ns-error-bridging:ns_error_bridging_sites="
      << ns_error_bridging_sites
      << ";ns_error_parameter_sites=" << ns_error_parameter_sites
      << ";ns_error_out_parameter_sites=" << ns_error_out_parameter_sites
      << ";ns_error_bridge_path_sites=" << ns_error_bridge_path_sites
      << ";failable_call_sites=" << failable_call_sites
      << ";normalized_sites=" << normalized_sites
      << ";bridge_boundary_sites=" << bridge_boundary_sites
      << ";contract_violation_sites=" << contract_violation_sites
      << ";deterministic_ns_error_bridging_lowering_handoff="
      << (deterministic_ns_error_bridging_lowering_handoff ? "true" : "false");
  return out.str();
}
