  result.message_send_selector_lowering_summary =
      result.integration_surface.message_send_selector_lowering_summary;
  result.deterministic_message_send_selector_lowering_handoff =
      result.type_metadata_handoff.message_send_selector_lowering_summary.deterministic &&
      result.integration_surface.message_send_selector_lowering_summary.deterministic &&
      IsEquivalentMessageSendSelectorLoweringSummary(
          result.integration_surface.message_send_selector_lowering_summary,
          result.type_metadata_handoff.message_send_selector_lowering_summary) &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.unary_form_sites +
              result.type_metadata_handoff.message_send_selector_lowering_summary.keyword_form_sites ==
          result.type_metadata_handoff.message_send_selector_lowering_summary.message_send_sites &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_symbol_sites <=
          result.type_metadata_handoff.message_send_selector_lowering_summary.message_send_sites &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_argument_piece_entries <=
          result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_piece_entries &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_normalized_sites <=
          result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_symbol_sites &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_form_mismatch_sites <=
          result.type_metadata_handoff.message_send_selector_lowering_summary.message_send_sites &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_arity_mismatch_sites <=
          result.type_metadata_handoff.message_send_selector_lowering_summary.message_send_sites &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_symbol_mismatch_sites <=
          result.type_metadata_handoff.message_send_selector_lowering_summary.message_send_sites &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_missing_symbol_sites <=
          result.type_metadata_handoff.message_send_selector_lowering_summary.message_send_sites &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_contract_violation_sites <=
          result.type_metadata_handoff.message_send_selector_lowering_summary.message_send_sites;
  result.dispatch_abi_marshalling_summary = result.integration_surface.dispatch_abi_marshalling_summary;
  result.deterministic_dispatch_abi_marshalling_handoff =
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.deterministic &&
      result.integration_surface.dispatch_abi_marshalling_summary.deterministic &&
      IsEquivalentDispatchAbiMarshallingSummary(
          result.integration_surface.dispatch_abi_marshalling_summary,
          result.type_metadata_handoff.dispatch_abi_marshalling_summary) &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.receiver_slots ==
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.message_send_sites &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.selector_symbol_slots +
              result.type_metadata_handoff.dispatch_abi_marshalling_summary.missing_selector_symbol_sites ==
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.message_send_sites &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.keyword_argument_slots +
              result.type_metadata_handoff.dispatch_abi_marshalling_summary.unary_argument_slots ==
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.argument_slots &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.keyword_argument_slots <=
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.argument_slots &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.unary_argument_slots <=
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.argument_slots &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.selector_symbol_slots <=
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.message_send_sites &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.missing_selector_symbol_sites <=
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.message_send_sites &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.arity_mismatch_sites <=
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.message_send_sites &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.contract_violation_sites <=
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.message_send_sites;
  result.nil_receiver_semantics_foldability_summary =
      result.integration_surface.nil_receiver_semantics_foldability_summary;
  result.deterministic_nil_receiver_semantics_foldability_handoff =
      result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.deterministic &&
      result.integration_surface.nil_receiver_semantics_foldability_summary.deterministic &&
      IsEquivalentNilReceiverSemanticsFoldabilitySummary(
          result.integration_surface.nil_receiver_semantics_foldability_summary,
          result.type_metadata_handoff.nil_receiver_semantics_foldability_summary) &&
      result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.receiver_nil_literal_sites ==
          result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites &&
      result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.nil_receiver_foldable_sites <=
          result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites &&
      result.type_metadata_handoff.nil_receiver_semantics_foldability_summary
              .nil_receiver_runtime_dispatch_required_sites +
              result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.nil_receiver_foldable_sites ==
          result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.message_send_sites &&
      result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites +
              result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.non_nil_receiver_sites ==
          result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.message_send_sites &&
      result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.contract_violation_sites <=
          result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.message_send_sites;
  result.super_dispatch_method_family_summary =
      result.integration_surface.super_dispatch_method_family_summary;
  result.deterministic_super_dispatch_method_family_handoff =
      result.type_metadata_handoff.super_dispatch_method_family_summary.deterministic &&
      result.integration_surface.super_dispatch_method_family_summary.deterministic &&
      IsEquivalentSuperDispatchMethodFamilySummary(
          result.integration_surface.super_dispatch_method_family_summary,
          result.type_metadata_handoff.super_dispatch_method_family_summary) &&
      result.type_metadata_handoff.super_dispatch_method_family_summary.receiver_super_identifier_sites ==
          result.type_metadata_handoff.super_dispatch_method_family_summary.super_dispatch_enabled_sites &&
      result.type_metadata_handoff.super_dispatch_method_family_summary.super_dispatch_requires_class_context_sites ==
          result.type_metadata_handoff.super_dispatch_method_family_summary.super_dispatch_enabled_sites &&
      result.type_metadata_handoff.super_dispatch_method_family_summary.method_family_alloc_sites +
              result.type_metadata_handoff.super_dispatch_method_family_summary.method_family_init_sites +
              result.type_metadata_handoff.super_dispatch_method_family_summary.method_family_copy_sites +
              result.type_metadata_handoff.super_dispatch_method_family_summary.method_family_mutable_copy_sites +
              result.type_metadata_handoff.super_dispatch_method_family_summary.method_family_new_sites +
              result.type_metadata_handoff.super_dispatch_method_family_summary.method_family_none_sites ==
          result.type_metadata_handoff.super_dispatch_method_family_summary.message_send_sites &&
      result.type_metadata_handoff.super_dispatch_method_family_summary.method_family_returns_related_result_sites <=
          result.type_metadata_handoff.super_dispatch_method_family_summary.method_family_init_sites &&
      result.type_metadata_handoff.super_dispatch_method_family_summary.method_family_returns_retained_result_sites <=
          result.type_metadata_handoff.super_dispatch_method_family_summary.message_send_sites &&
      result.type_metadata_handoff.super_dispatch_method_family_summary.contract_violation_sites <=
          result.type_metadata_handoff.super_dispatch_method_family_summary.message_send_sites;
  result.runtime_link_host_link_summary = result.integration_surface.runtime_link_host_link_summary;
  result.deterministic_runtime_link_host_link_handoff =
      result.type_metadata_handoff.runtime_link_host_link_summary.deterministic &&
      result.integration_surface.runtime_link_host_link_summary.deterministic &&
      IsEquivalentRuntimeLinkHostLinkSummary(
          result.integration_surface.runtime_link_host_link_summary,
          result.type_metadata_handoff.runtime_link_host_link_summary) &&
      result.type_metadata_handoff.runtime_link_host_link_summary.runtime_link_required_sites +
              result.type_metadata_handoff.runtime_link_host_link_summary.runtime_link_elided_sites ==
          result.type_metadata_handoff.runtime_link_host_link_summary.message_send_sites &&
      result.type_metadata_handoff.runtime_link_host_link_summary.contract_violation_sites <=
          result.type_metadata_handoff.runtime_link_host_link_summary.message_send_sites &&
      (result.type_metadata_handoff.runtime_link_host_link_summary.message_send_sites == 0 ||
       result.type_metadata_handoff.runtime_link_host_link_summary.runtime_dispatch_declaration_parameter_count ==
           result.type_metadata_handoff.runtime_link_host_link_summary.runtime_dispatch_arg_slots + 2u) &&
      (result.type_metadata_handoff.runtime_link_host_link_summary.default_runtime_dispatch_symbol_binding ==
       (result.type_metadata_handoff.runtime_link_host_link_summary.runtime_dispatch_symbol ==
        kObjc3RuntimeLinkHostLinkDefaultDispatchSymbol));
