namespace {

static void PopulateObjc3SemaDispatchRuntimeArcValidationFlowRuntimeLinkReadiness(
    Objc3SemaDispatchRuntimeArcParityValidationReadinessRecord &record,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_runtime_link_host_link_handoff) {
  const auto &runtime_link_summary = surface.runtime_link_host_link_summary;
  record.runtime_link_host_link_ready =
      deterministic_runtime_link_host_link_handoff &&
      runtime_link_summary.message_send_sites ==
          surface.runtime_link_host_link_message_send_sites_total &&
      runtime_link_summary.runtime_link_required_sites ==
          surface.runtime_link_host_link_required_sites_total &&
      runtime_link_summary.runtime_link_elided_sites ==
          surface.runtime_link_host_link_elided_sites_total &&
      runtime_link_summary.runtime_dispatch_arg_slots ==
          surface.runtime_link_host_link_runtime_dispatch_arg_slots_total &&
      runtime_link_summary.runtime_dispatch_declaration_parameter_count ==
          surface
              .runtime_link_host_link_runtime_dispatch_declaration_parameter_count_total &&
      runtime_link_summary.contract_violation_sites ==
          surface.runtime_link_host_link_contract_violation_sites_total &&
      runtime_link_summary.runtime_dispatch_symbol ==
          surface.runtime_link_host_link_runtime_dispatch_symbol &&
      runtime_link_summary.default_runtime_dispatch_symbol_binding ==
          surface
              .runtime_link_host_link_default_runtime_dispatch_symbol_binding &&
      runtime_link_summary.runtime_link_required_sites +
              runtime_link_summary.runtime_link_elided_sites ==
          runtime_link_summary.message_send_sites &&
      runtime_link_summary.contract_violation_sites <=
          runtime_link_summary.message_send_sites &&
      (runtime_link_summary.message_send_sites == 0 ||
       runtime_link_summary.runtime_dispatch_declaration_parameter_count ==
           runtime_link_summary.runtime_dispatch_arg_slots + 2u) &&
      (runtime_link_summary.default_runtime_dispatch_symbol_binding ==
       (runtime_link_summary.runtime_dispatch_symbol ==
        kObjc3RuntimeLinkHostLinkDefaultDispatchSymbol)) &&
      runtime_link_summary.deterministic;
}

}  // namespace
