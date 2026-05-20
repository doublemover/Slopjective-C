Objc3NilReceiverSemanticsFoldabilityContract
BuildNilReceiverSemanticsFoldabilityContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3NilReceiverSemanticsFoldabilityContract contract;
  contract.message_send_sites =
      sema_parity_surface.nil_receiver_semantics_foldability_sites_total;
  contract.receiver_nil_literal_sites =
      sema_parity_surface
          .nil_receiver_semantics_foldability_receiver_nil_literal_sites_total;
  contract.nil_receiver_semantics_enabled_sites =
      sema_parity_surface
          .nil_receiver_semantics_foldability_enabled_sites_total;
  contract.nil_receiver_foldable_sites =
      sema_parity_surface
          .nil_receiver_semantics_foldability_foldable_sites_total;
  contract.nil_receiver_runtime_dispatch_required_sites =
      sema_parity_surface
          .nil_receiver_semantics_foldability_runtime_dispatch_required_sites_total;
  contract.non_nil_receiver_sites =
      sema_parity_surface
          .nil_receiver_semantics_foldability_non_nil_receiver_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface
          .nil_receiver_semantics_foldability_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.nil_receiver_semantics_foldability_summary
          .deterministic &&
      sema_parity_surface
          .deterministic_nil_receiver_semantics_foldability_handoff;
  return contract;
}

Objc3SuperDispatchMethodFamilyContract BuildSuperDispatchMethodFamilyContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3SuperDispatchMethodFamilyContract contract;
  contract.message_send_sites =
      sema_parity_surface.super_dispatch_method_family_sites_total;
  contract.receiver_super_identifier_sites =
      sema_parity_surface
          .super_dispatch_method_family_receiver_super_identifier_sites_total;
  contract.super_dispatch_enabled_sites =
      sema_parity_surface.super_dispatch_method_family_enabled_sites_total;
  contract.super_dispatch_requires_class_context_sites =
      sema_parity_surface
          .super_dispatch_method_family_requires_class_context_sites_total;
  contract.method_family_alloc_sites =
      sema_parity_surface.super_dispatch_method_family_alloc_sites_total;
  contract.method_family_init_sites =
      sema_parity_surface.super_dispatch_method_family_init_sites_total;
  contract.method_family_copy_sites =
      sema_parity_surface.super_dispatch_method_family_copy_sites_total;
  contract.method_family_mutable_copy_sites =
      sema_parity_surface.super_dispatch_method_family_mutable_copy_sites_total;
  contract.method_family_new_sites =
      sema_parity_surface.super_dispatch_method_family_new_sites_total;
  contract.method_family_none_sites =
      sema_parity_surface.super_dispatch_method_family_none_sites_total;
  contract.method_family_returns_retained_result_sites =
      sema_parity_surface
          .super_dispatch_method_family_returns_retained_result_sites_total;
  contract.method_family_returns_related_result_sites =
      sema_parity_surface
          .super_dispatch_method_family_returns_related_result_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface
          .super_dispatch_method_family_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.super_dispatch_method_family_summary.deterministic &&
      sema_parity_surface.deterministic_super_dispatch_method_family_handoff;
  return contract;
}

Objc3RuntimeLinkHostLinkContract BuildRuntimeLinkHostLinkContract(
    const Objc3DispatchAbiMarshallingContract &dispatch_abi_marshalling_contract,
    const Objc3NilReceiverSemanticsFoldabilityContract
        &nil_receiver_semantics_foldability_contract,
    const Objc3FrontendOptions &options) {
  Objc3RuntimeLinkHostLinkContract contract;
  contract.message_send_sites =
      dispatch_abi_marshalling_contract.message_send_sites;
  contract.runtime_link_required_sites =
      nil_receiver_semantics_foldability_contract
          .nil_receiver_runtime_dispatch_required_sites;
  if (contract.runtime_link_required_sites <= contract.message_send_sites) {
    contract.runtime_link_elided_sites =
        contract.message_send_sites - contract.runtime_link_required_sites;
  } else {
    contract.runtime_link_elided_sites = 0;
    contract.contract_violation_sites = 1;
  }
  contract.runtime_dispatch_arg_slots =
      options.lowering.max_message_send_args;
  contract.runtime_dispatch_declaration_parameter_count =
      contract.runtime_dispatch_arg_slots + 2u;
  contract.runtime_dispatch_symbol = options.lowering.runtime_dispatch_symbol;
  contract.default_runtime_dispatch_symbol_binding =
      contract.runtime_dispatch_symbol == kObjc3ArtifactRuntimeDispatchSymbol;
  contract.deterministic =
      dispatch_abi_marshalling_contract.deterministic &&
      nil_receiver_semantics_foldability_contract.deterministic;
  return contract;
}

Objc3RuntimeDispatchLoweringAbiContract BuildRuntimeDispatchLoweringAbiContract(
    const Objc3DispatchAbiMarshallingContract &dispatch_abi_marshalling_contract,
    const Objc3RuntimeLinkHostLinkContract &runtime_link_host_link_contract,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api_summary) {
  Objc3RuntimeDispatchLoweringAbiContract contract;
  contract.message_send_sites =
      dispatch_abi_marshalling_contract.message_send_sites;
  contract.fixed_argument_slot_count =
      runtime_link_host_link_contract.runtime_dispatch_arg_slots;
  contract.runtime_dispatch_parameter_count =
      runtime_link_host_link_contract
          .runtime_dispatch_declaration_parameter_count;
  contract.canonical_runtime_dispatch_symbol =
      runtime_bootstrap_api_summary.dispatch_entrypoint_symbol;
  contract.default_lowering_target_symbol =
      contract.canonical_runtime_dispatch_symbol;
  contract.default_lowering_target_model =
      kObjc3ArtifactRuntimeDispatchLiveCutoverDefaultTargetModel;
  contract.strict_dispatch_error_model =
      kObjc3ArtifactRuntimeDispatchLiveCutoverStrictDispatchModel;
  contract.deferred_cases_model =
      kObjc3ArtifactRuntimeDispatchLiveCutoverDeferredCasesModel;
  contract.selector_lookup_symbol =
      runtime_bootstrap_api_summary.selector_lookup_symbol;
  contract.selector_handle_type =
      runtime_bootstrap_api_summary.selector_handle_type;
  contract.fail_closed =
      IsReadyObjc3RuntimeBootstrapApiSummary(runtime_bootstrap_api_summary);
  contract.deterministic =
      dispatch_abi_marshalling_contract.deterministic &&
      runtime_link_host_link_contract.deterministic &&
      !runtime_bootstrap_api_summary.replay_key.empty();
  return contract;
}
