  void RebuildNSErrorBridgingProfile(FunctionDecl &fn) {
    fn.ns_error_bridging_profile = BuildNSErrorBridgingProfile(
        fn.ns_error_bridging_sites,
        fn.ns_error_parameter_sites,
        fn.ns_error_out_parameter_sites,
        fn.ns_error_bridge_path_sites,
        fn.failable_call_sites,
        fn.ns_error_bridging_normalized_sites,
        fn.ns_error_bridge_boundary_sites,
        fn.ns_error_bridging_contract_violation_sites,
        fn.deterministic_ns_error_bridging_lowering_handoff);
  }

  void RebuildNSErrorBridgingProfile(Objc3MethodDecl &method) {
    method.ns_error_bridging_profile = BuildNSErrorBridgingProfile(
        method.ns_error_bridging_sites,
        method.ns_error_parameter_sites,
        method.ns_error_out_parameter_sites,
        method.ns_error_bridge_path_sites,
        method.failable_call_sites,
        method.ns_error_bridging_normalized_sites,
        method.ns_error_bridge_boundary_sites,
        method.ns_error_bridging_contract_violation_sites,
        method.deterministic_ns_error_bridging_lowering_handoff);
  }
