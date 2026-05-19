  void PublishNSErrorBridgingSiteCounters(
      Objc3MethodDecl &method,
      const Objc3NSErrorBridgingProfile &profile) {
    method.ns_error_bridging_sites = profile.ns_error_bridging_sites;
    method.ns_error_parameter_sites = profile.ns_error_parameter_sites;
    method.ns_error_out_parameter_sites = profile.ns_error_out_parameter_sites;
    method.ns_error_bridge_path_sites = profile.ns_error_bridge_path_sites;
    method.failable_call_sites = profile.failable_call_sites;
    method.ns_error_bridging_normalized_sites = profile.normalized_sites;
    method.ns_error_bridge_boundary_sites = profile.bridge_boundary_sites;
    method.ns_error_bridging_contract_violation_sites =
        profile.contract_violation_sites;
    method.deterministic_ns_error_bridging_lowering_handoff =
        profile.deterministic_ns_error_bridging_lowering_handoff;
  }
