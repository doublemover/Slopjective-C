    existing.ns_error_bridging_profile_is_normalized =
        existing.ns_error_bridging_profile_is_normalized ||
        fn.ns_error_bridging_profile_is_normalized;
    existing.deterministic_ns_error_bridging_lowering_handoff =
        existing.deterministic_ns_error_bridging_lowering_handoff ||
        fn.deterministic_ns_error_bridging_lowering_handoff;
    existing.ns_error_bridging_sites =
        std::max(existing.ns_error_bridging_sites, fn.ns_error_bridging_sites);
    existing.ns_error_parameter_sites = std::max(existing.ns_error_parameter_sites,
                                                 fn.ns_error_parameter_sites);
    existing.ns_error_out_parameter_sites = std::max(
        existing.ns_error_out_parameter_sites, fn.ns_error_out_parameter_sites);
    existing.ns_error_bridge_path_sites = std::max(
        existing.ns_error_bridge_path_sites, fn.ns_error_bridge_path_sites);
    existing.failable_call_sites =
        std::max(existing.failable_call_sites, fn.failable_call_sites);
    existing.ns_error_bridging_normalized_sites = std::max(
        existing.ns_error_bridging_normalized_sites,
        fn.ns_error_bridging_normalized_sites);
    existing.ns_error_bridge_boundary_sites =
        std::max(existing.ns_error_bridge_boundary_sites,
                 fn.ns_error_bridge_boundary_sites);
    existing.ns_error_bridging_contract_violation_sites = std::max(
        existing.ns_error_bridging_contract_violation_sites,
        fn.ns_error_bridging_contract_violation_sites);
    existing.is_pure_annotation = existing.is_pure_annotation || fn.is_pure;
