namespace {

static void PopulateObjc3SemaDispatchRuntimeArcValidationFlowDispatchReadiness(
    Objc3SemaDispatchRuntimeArcParityValidationReadinessRecord &record,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_dispatch_abi_marshalling_handoff,
    bool deterministic_nil_receiver_semantics_foldability_handoff,
    bool deterministic_super_dispatch_method_family_handoff) {
  const auto &dispatch_summary = surface.dispatch_abi_marshalling_summary;
  record.dispatch_abi_marshalling_ready =
      deterministic_dispatch_abi_marshalling_handoff &&
      dispatch_summary.message_send_sites ==
          surface.dispatch_abi_marshalling_sites_total &&
      dispatch_summary.receiver_slots ==
          surface.dispatch_abi_marshalling_receiver_slots_total &&
      dispatch_summary.selector_symbol_slots ==
          surface.dispatch_abi_marshalling_selector_symbol_slots_total &&
      dispatch_summary.argument_slots ==
          surface.dispatch_abi_marshalling_argument_slots_total &&
      dispatch_summary.keyword_argument_slots ==
          surface.dispatch_abi_marshalling_keyword_argument_slots_total &&
      dispatch_summary.unary_argument_slots ==
          surface.dispatch_abi_marshalling_unary_argument_slots_total &&
      dispatch_summary.arity_mismatch_sites ==
          surface.dispatch_abi_marshalling_arity_mismatch_sites_total &&
      dispatch_summary.missing_selector_symbol_sites ==
          surface.dispatch_abi_marshalling_missing_selector_symbol_sites_total &&
      dispatch_summary.contract_violation_sites ==
          surface.dispatch_abi_marshalling_contract_violation_sites_total &&
      dispatch_summary.receiver_slots == dispatch_summary.message_send_sites &&
      dispatch_summary.selector_symbol_slots +
              dispatch_summary.missing_selector_symbol_sites ==
          dispatch_summary.message_send_sites &&
      dispatch_summary.keyword_argument_slots +
              dispatch_summary.unary_argument_slots ==
          dispatch_summary.argument_slots &&
      dispatch_summary.keyword_argument_slots <= dispatch_summary.argument_slots &&
      dispatch_summary.unary_argument_slots <= dispatch_summary.argument_slots &&
      dispatch_summary.selector_symbol_slots <=
          dispatch_summary.message_send_sites &&
      dispatch_summary.missing_selector_symbol_sites <=
          dispatch_summary.message_send_sites &&
      dispatch_summary.arity_mismatch_sites <=
          dispatch_summary.message_send_sites &&
      dispatch_summary.contract_violation_sites <=
          dispatch_summary.message_send_sites &&
      dispatch_summary.deterministic;
  const auto &nil_receiver_summary =
      surface.nil_receiver_semantics_foldability_summary;
  record.nil_receiver_semantics_foldability_ready =
      deterministic_nil_receiver_semantics_foldability_handoff &&
      nil_receiver_summary.message_send_sites ==
          surface.nil_receiver_semantics_foldability_sites_total &&
      nil_receiver_summary.receiver_nil_literal_sites ==
          surface
              .nil_receiver_semantics_foldability_receiver_nil_literal_sites_total &&
      nil_receiver_summary.nil_receiver_semantics_enabled_sites ==
          surface.nil_receiver_semantics_foldability_enabled_sites_total &&
      nil_receiver_summary.nil_receiver_foldable_sites ==
          surface.nil_receiver_semantics_foldability_foldable_sites_total &&
      nil_receiver_summary.nil_receiver_runtime_dispatch_required_sites ==
          surface
              .nil_receiver_semantics_foldability_runtime_dispatch_required_sites_total &&
      nil_receiver_summary.non_nil_receiver_sites ==
          surface.nil_receiver_semantics_foldability_non_nil_receiver_sites_total &&
      nil_receiver_summary.contract_violation_sites ==
          surface.nil_receiver_semantics_foldability_contract_violation_sites_total &&
      nil_receiver_summary.receiver_nil_literal_sites ==
          nil_receiver_summary.nil_receiver_semantics_enabled_sites &&
      nil_receiver_summary.nil_receiver_foldable_sites <=
          nil_receiver_summary.nil_receiver_semantics_enabled_sites &&
      nil_receiver_summary.nil_receiver_runtime_dispatch_required_sites +
              nil_receiver_summary.nil_receiver_foldable_sites ==
          nil_receiver_summary.message_send_sites &&
      nil_receiver_summary.nil_receiver_semantics_enabled_sites +
              nil_receiver_summary.non_nil_receiver_sites ==
          nil_receiver_summary.message_send_sites &&
      nil_receiver_summary.contract_violation_sites <=
          nil_receiver_summary.message_send_sites &&
      nil_receiver_summary.deterministic;
  const auto &super_dispatch_summary =
      surface.super_dispatch_method_family_summary;
  record.super_dispatch_method_family_ready =
      deterministic_super_dispatch_method_family_handoff &&
      super_dispatch_summary.message_send_sites ==
          surface.super_dispatch_method_family_sites_total &&
      super_dispatch_summary.receiver_super_identifier_sites ==
          surface
              .super_dispatch_method_family_receiver_super_identifier_sites_total &&
      super_dispatch_summary.super_dispatch_enabled_sites ==
          surface.super_dispatch_method_family_enabled_sites_total &&
      super_dispatch_summary.super_dispatch_requires_class_context_sites ==
          surface
              .super_dispatch_method_family_requires_class_context_sites_total &&
      super_dispatch_summary.method_family_alloc_sites ==
          surface.super_dispatch_method_family_alloc_sites_total &&
      super_dispatch_summary.method_family_init_sites ==
          surface.super_dispatch_method_family_init_sites_total &&
      super_dispatch_summary.method_family_copy_sites ==
          surface.super_dispatch_method_family_copy_sites_total &&
      super_dispatch_summary.method_family_mutable_copy_sites ==
          surface.super_dispatch_method_family_mutable_copy_sites_total &&
      super_dispatch_summary.method_family_new_sites ==
          surface.super_dispatch_method_family_new_sites_total &&
      super_dispatch_summary.method_family_none_sites ==
          surface.super_dispatch_method_family_none_sites_total &&
      super_dispatch_summary.method_family_returns_retained_result_sites ==
          surface
              .super_dispatch_method_family_returns_retained_result_sites_total &&
      super_dispatch_summary.method_family_returns_related_result_sites ==
          surface
              .super_dispatch_method_family_returns_related_result_sites_total &&
      super_dispatch_summary.contract_violation_sites ==
          surface.super_dispatch_method_family_contract_violation_sites_total &&
      super_dispatch_summary.receiver_super_identifier_sites ==
          super_dispatch_summary.super_dispatch_enabled_sites &&
      super_dispatch_summary.super_dispatch_requires_class_context_sites ==
          super_dispatch_summary.super_dispatch_enabled_sites &&
      super_dispatch_summary.method_family_alloc_sites +
              super_dispatch_summary.method_family_init_sites +
              super_dispatch_summary.method_family_copy_sites +
              super_dispatch_summary.method_family_mutable_copy_sites +
              super_dispatch_summary.method_family_new_sites +
              super_dispatch_summary.method_family_none_sites ==
          super_dispatch_summary.message_send_sites &&
      super_dispatch_summary.method_family_returns_related_result_sites <=
          super_dispatch_summary.method_family_init_sites &&
      super_dispatch_summary.method_family_returns_retained_result_sites <=
          super_dispatch_summary.message_send_sites &&
      super_dispatch_summary.contract_violation_sites <=
          super_dispatch_summary.message_send_sites &&
      super_dispatch_summary.deterministic;
}

}  // namespace
