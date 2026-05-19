bool IsNSErrorBridgingProfileNormalized(
    std::size_t ns_error_bridging_sites,
    std::size_t ns_error_parameter_sites,
    std::size_t ns_error_out_parameter_sites,
    std::size_t ns_error_bridge_path_sites,
    std::size_t failable_call_sites,
    std::size_t normalized_sites,
    std::size_t bridge_boundary_sites,
    std::size_t contract_violation_sites) {
  if (ns_error_out_parameter_sites > ns_error_parameter_sites) {
    return false;
  }
  if (ns_error_bridge_path_sites > ns_error_out_parameter_sites ||
      ns_error_bridge_path_sites > failable_call_sites) {
    return false;
  }
  if (normalized_sites + bridge_boundary_sites != ns_error_bridging_sites) {
    return false;
  }
  if (ns_error_parameter_sites > ns_error_bridging_sites ||
      ns_error_out_parameter_sites > ns_error_bridging_sites ||
      ns_error_bridge_path_sites > ns_error_bridging_sites ||
      failable_call_sites > ns_error_bridging_sites ||
      normalized_sites > ns_error_bridging_sites ||
      bridge_boundary_sites > ns_error_bridging_sites) {
    return false;
  }
  return contract_violation_sites == 0;
}
