  void PublishNSErrorBridgingSiteCounters(
      FunctionDecl &fn,
      const Objc3NSErrorBridgingProfile &profile) {
    fn.ns_error_bridging_sites = profile.ns_error_bridging_sites;
    fn.ns_error_parameter_sites = profile.ns_error_parameter_sites;
    fn.ns_error_out_parameter_sites = profile.ns_error_out_parameter_sites;
    fn.ns_error_bridge_path_sites = profile.ns_error_bridge_path_sites;
    fn.failable_call_sites = profile.failable_call_sites;
    fn.ns_error_bridging_normalized_sites = profile.normalized_sites;
    fn.ns_error_bridge_boundary_sites = profile.bridge_boundary_sites;
    fn.ns_error_bridging_contract_violation_sites =
        profile.contract_violation_sites;
    fn.deterministic_ns_error_bridging_lowering_handoff =
        profile.deterministic_ns_error_bridging_lowering_handoff;
  }
