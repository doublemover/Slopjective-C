  return contract.total_marshaled_slots == expected_total;
}

std::string Objc3DispatchAbiMarshallingReplayKey(
    const Objc3DispatchAbiMarshallingContract &contract) {
  return std::string("message_send_sites=") + std::to_string(contract.message_send_sites) +
         ";receiver_slots_marshaled=" + std::to_string(contract.receiver_slots_marshaled) +
         ";selector_slots_marshaled=" + std::to_string(contract.selector_slots_marshaled) +
         ";argument_value_slots_marshaled=" + std::to_string(contract.argument_value_slots_marshaled) +
         ";argument_padding_slots_marshaled=" + std::to_string(contract.argument_padding_slots_marshaled) +
         ";argument_total_slots_marshaled=" + std::to_string(contract.argument_total_slots_marshaled) +
         ";total_marshaled_slots=" + std::to_string(contract.total_marshaled_slots) +
         ";runtime_dispatch_arg_slots=" + std::to_string(contract.runtime_dispatch_arg_slots) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3DispatchAbiMarshallingLaneContract;
}

bool IsValidObjc3NilReceiverSemanticsFoldabilityContract(
    const Objc3NilReceiverSemanticsFoldabilityContract &contract) {
  if (contract.receiver_nil_literal_sites != contract.nil_receiver_semantics_enabled_sites) {
    return false;
  }
  if (contract.nil_receiver_foldable_sites > contract.nil_receiver_semantics_enabled_sites) {
    return false;
  }
  if (contract.nil_receiver_runtime_dispatch_required_sites + contract.nil_receiver_foldable_sites !=
      contract.message_send_sites) {
    return false;
  }
  if (contract.nil_receiver_semantics_enabled_sites + contract.non_nil_receiver_sites !=
      contract.message_send_sites) {
    return false;
  }
  return contract.contract_violation_sites <= contract.message_send_sites;
}

std::string Objc3NilReceiverSemanticsFoldabilityReplayKey(
    const Objc3NilReceiverSemanticsFoldabilityContract &contract) {
  return std::string("message_send_sites=") + std::to_string(contract.message_send_sites) +
         ";receiver_nil_literal_sites=" + std::to_string(contract.receiver_nil_literal_sites) +
         ";nil_receiver_semantics_enabled_sites=" + std::to_string(contract.nil_receiver_semantics_enabled_sites) +
         ";nil_receiver_foldable_sites=" + std::to_string(contract.nil_receiver_foldable_sites) +
         ";nil_receiver_runtime_dispatch_required_sites=" +
         std::to_string(contract.nil_receiver_runtime_dispatch_required_sites) +
         ";non_nil_receiver_sites=" + std::to_string(contract.non_nil_receiver_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3NilReceiverSemanticsFoldabilityLaneContract;
}

bool IsValidObjc3SuperDispatchMethodFamilyContract(
    const Objc3SuperDispatchMethodFamilyContract &contract) {
  if (contract.receiver_super_identifier_sites != contract.super_dispatch_enabled_sites) {
    return false;
  }
  if (contract.super_dispatch_requires_class_context_sites != contract.super_dispatch_enabled_sites) {
    return false;
  }
  if (contract.method_family_init_sites + contract.method_family_copy_sites +
              contract.method_family_mutable_copy_sites + contract.method_family_new_sites +
              contract.method_family_none_sites !=
          contract.message_send_sites) {
    return false;
  }
  if (contract.method_family_returns_related_result_sites > contract.method_family_init_sites) {
    return false;
  }
  if (contract.method_family_returns_retained_result_sites > contract.message_send_sites) {
    return false;
  }
  return contract.contract_violation_sites <= contract.message_send_sites;
}

std::string Objc3SuperDispatchMethodFamilyReplayKey(
    const Objc3SuperDispatchMethodFamilyContract &contract) {
  return std::string("message_send_sites=") + std::to_string(contract.message_send_sites) +
         ";receiver_super_identifier_sites=" + std::to_string(contract.receiver_super_identifier_sites) +
         ";super_dispatch_enabled_sites=" + std::to_string(contract.super_dispatch_enabled_sites) +
         ";super_dispatch_requires_class_context_sites=" +
         std::to_string(contract.super_dispatch_requires_class_context_sites) +
         ";method_family_init_sites=" + std::to_string(contract.method_family_init_sites) +
         ";method_family_copy_sites=" + std::to_string(contract.method_family_copy_sites) +
         ";method_family_mutable_copy_sites=" + std::to_string(contract.method_family_mutable_copy_sites) +
         ";method_family_new_sites=" + std::to_string(contract.method_family_new_sites) +
         ";method_family_none_sites=" + std::to_string(contract.method_family_none_sites) +
         ";method_family_returns_retained_result_sites=" +
         std::to_string(contract.method_family_returns_retained_result_sites) +
         ";method_family_returns_related_result_sites=" +
         std::to_string(contract.method_family_returns_related_result_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3SuperDispatchMethodFamilyLaneContract;
}

bool IsValidObjc3RuntimeLinkHostLinkContract(
    const Objc3RuntimeLinkHostLinkContract &contract) {
  if (!IsValidRuntimeDispatchSymbol(contract.runtime_dispatch_symbol)) {
    return false;
  }
  if (contract.runtime_dispatch_arg_slots > kObjc3RuntimeDispatchMaxArgs) {
    return false;
  }
  if (contract.runtime_link_required_sites > contract.message_send_sites) {
    return false;
  }
  if (contract.runtime_link_required_sites + contract.runtime_link_elided_sites !=
      contract.message_send_sites) {
    return false;
  }
  if (contract.runtime_dispatch_declaration_parameter_count !=
      contract.runtime_dispatch_arg_slots + 2u) {
    return false;
  }
  if (contract.default_runtime_dispatch_symbol_binding !=
      (contract.runtime_dispatch_symbol == kObjc3RuntimeDispatchSymbol)) {
    return false;
  }
  return contract.contract_violation_sites <= contract.message_send_sites;
}

std::string Objc3RuntimeLinkHostLinkReplayKey(
    const Objc3RuntimeLinkHostLinkContract &contract) {
  return std::string("message_send_sites=") + std::to_string(contract.message_send_sites) +
         ";runtime_link_required_sites=" + std::to_string(contract.runtime_link_required_sites) +
         ";runtime_link_elided_sites=" + std::to_string(contract.runtime_link_elided_sites) +
         ";runtime_dispatch_arg_slots=" + std::to_string(contract.runtime_dispatch_arg_slots) +
         ";runtime_dispatch_declaration_parameter_count=" +
         std::to_string(contract.runtime_dispatch_declaration_parameter_count) +
         ";runtime_dispatch_symbol=" + contract.runtime_dispatch_symbol +
         ";default_runtime_dispatch_symbol_binding=" +
         BoolToken(contract.default_runtime_dispatch_symbol_binding) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3RuntimeLinkHostLinkLaneContract;
}

// dispatch lowering ABI anchor: validation keeps the canonical runtime symbol,
// selector lookup/handle surface, i32 receiver/result ABI, and fixed four-slot
// padding model synchronized with the live emitted call path.
bool IsValidObjc3RuntimeDispatchLoweringAbiContract(
    const Objc3RuntimeDispatchLoweringAbiContract &contract) {
  if (contract.fixed_argument_slot_count > kObjc3RuntimeDispatchMaxArgs) {
    return false;
  }
  if (contract.runtime_dispatch_parameter_count !=
      contract.fixed_argument_slot_count + 2u) {
    return false;
  }
  if (contract.lowering_boundary_model !=
      kObjc3RuntimeDispatchLoweringAbiBoundaryModel) {
    return false;
  }
  if (contract.canonical_runtime_dispatch_symbol !=
      kObjc3RuntimeDispatchLoweringCanonicalEntrypointSymbol) {
    return false;
  }
  if (!IsValidRuntimeDispatchSymbol(contract.default_lowering_target_symbol)) {
    return false;
  }
  if (contract.default_lowering_target_symbol !=
      contract.canonical_runtime_dispatch_symbol) {
    return false;
  }
  if (contract.selector_lookup_symbol !=
      kObjc3RuntimeDispatchLoweringSelectorLookupSymbol) {
    return false;
  }
  if (contract.selector_handle_type !=
      kObjc3RuntimeDispatchLoweringSelectorHandleType) {
    return false;
  }
  if (contract.receiver_abi_type != kObjc3RuntimeDispatchLoweringReceiverAbiType ||
      contract.selector_abi_type != kObjc3RuntimeDispatchLoweringSelectorAbiType ||
      contract.argument_abi_type != kObjc3RuntimeDispatchLoweringArgumentAbiType ||
      contract.result_abi_type != kObjc3RuntimeDispatchLoweringResultAbiType) {
    return false;
  }
  if (contract.selector_operand_model !=
          kObjc3RuntimeDispatchLoweringSelectorOperandModel ||
      contract.selector_handle_model !=
          kObjc3RuntimeDispatchLoweringSelectorHandleModel ||
      contract.argument_padding_model !=
          kObjc3RuntimeDispatchLoweringArgumentPaddingModel ||
      contract.default_lowering_target_model !=
          kObjc3RuntimeDispatchLiveCutoverDefaultTargetModel ||
      contract.strict_dispatch_error_model !=
          kObjc3RuntimeDispatchLoweringStrictDispatchErrorModel ||
      contract.deferred_cases_model !=
          kObjc3RuntimeDispatchLiveCutoverDeferredCasesModel) {
    return false;
  }
  return contract.fail_closed;
}

std::string Objc3RuntimeDispatchLoweringAbiReplayKey(
    const Objc3RuntimeDispatchLoweringAbiContract &contract) {
  return std::string("message_send_sites=") +
         std::to_string(contract.message_send_sites) +
         ";fixed_argument_slot_count=" +
         std::to_string(contract.fixed_argument_slot_count) +
         ";runtime_dispatch_parameter_count=" +
         std::to_string(contract.runtime_dispatch_parameter_count) +
         ";lowering_boundary_model=" + contract.lowering_boundary_model +
         ";canonical_runtime_dispatch_symbol=" +
         contract.canonical_runtime_dispatch_symbol +
         ";default_lowering_target_symbol=" +
         contract.default_lowering_target_symbol +
         ";selector_lookup_symbol=" + contract.selector_lookup_symbol +
         ";selector_handle_type=" + contract.selector_handle_type +
         ";receiver_abi_type=" + contract.receiver_abi_type +
         ";selector_abi_type=" + contract.selector_abi_type +
         ";argument_abi_type=" + contract.argument_abi_type +
         ";result_abi_type=" + contract.result_abi_type +
         ";selector_operand_model=" + contract.selector_operand_model +
         ";selector_handle_model=" + contract.selector_handle_model +
         ";argument_padding_model=" + contract.argument_padding_model +
         ";default_lowering_target_model=" +
         contract.default_lowering_target_model +
         ";strict_dispatch_error_model=" +
         contract.strict_dispatch_error_model +
         ";deferred_cases_model=" + contract.deferred_cases_model +
         ";fail_closed=" + BoolToken(contract.fail_closed) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";contract_id=" + kObjc3RuntimeDispatchLoweringAbiContractId;
}

std::string Objc3RuntimeDispatchLoweringAbiBoundarySummary(
    const Objc3RuntimeDispatchLoweringAbiContract &contract) {
  return std::string("contract=") + kObjc3RuntimeDispatchLoweringAbiContractId +
         ";canonical_runtime_dispatch_symbol=" +
         contract.canonical_runtime_dispatch_symbol +
         ";default_lowering_target_symbol=" +
         contract.default_lowering_target_symbol +
         ";selector_lookup_symbol=" + contract.selector_lookup_symbol +
         ";selector_handle_type=" + contract.selector_handle_type +
         ";receiver_abi_type=" + contract.receiver_abi_type +
         ";argument_abi_type=" + contract.argument_abi_type +
         ";fixed_argument_slot_count=" +
         std::to_string(contract.fixed_argument_slot_count) +
         ";result_abi_type=" + contract.result_abi_type +
         ";default_lowering_target_model=" +
         contract.default_lowering_target_model +
         ";deferred_cases_model=" + contract.deferred_cases_model;
}

bool IsValidObjc3OwnershipQualifierLoweringContract(
    const Objc3OwnershipQualifierLoweringContract &contract) {
  // runtime-backed-object-ownership freeze anchor: these legacy
  // ownership replay surfaces remain part of the truthful runtime-backed
  // object ownership boundary until live ARC runtime behavior lands later in
  // They are preserved summary lanes, not proof that executable
  // function/method ownership qualifiers are already runnable.
  // retainable-object semantic-rule freeze anchor: ownership
  // qualifier replay stays authoritative for fail-closed executable storage
  // legality until live runtime-backed retain/release rules land.
  return contract.invalid_ownership_qualifier_sites <= contract.ownership_qualifier_sites &&
         contract.ownership_qualifier_sites <= contract.object_pointer_type_annotation_sites;
}

std::string Objc3OwnershipQualifierLoweringReplayKey(
    const Objc3OwnershipQualifierLoweringContract &contract) {
  return std::string("ownership_qualifier_sites=") + std::to_string(contract.ownership_qualifier_sites) +
         ";invalid_ownership_qualifier_sites=" + std::to_string(contract.invalid_ownership_qualifier_sites) +
         ";object_pointer_type_annotation_sites=" + std::to_string(contract.object_pointer_type_annotation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3OwnershipQualifierLoweringLaneContract;
}

bool IsValidObjc3RetainReleaseOperationLoweringContract(
    const Objc3RetainReleaseOperationLoweringContract &contract) {
  const std::size_t qualified_or_violation = contract.ownership_qualified_sites + contract.contract_violation_sites;
  return contract.retain_insertion_sites <= qualified_or_violation &&
         contract.release_insertion_sites <= qualified_or_violation &&
         contract.autorelease_insertion_sites <= qualified_or_violation;
}

std::string Objc3RetainReleaseOperationLoweringReplayKey(
    const Objc3RetainReleaseOperationLoweringContract &contract) {
  return std::string("ownership_qualified_sites=") + std::to_string(contract.ownership_qualified_sites) +
         ";retain_insertion_sites=" + std::to_string(contract.retain_insertion_sites) +
         ";release_insertion_sites=" + std::to_string(contract.release_insertion_sites) +
         ";autorelease_insertion_sites=" + std::to_string(contract.autorelease_insertion_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3RetainReleaseOperationLoweringLaneContract;
}

bool IsValidObjc3AutoreleasePoolScopeLoweringContract(
    const Objc3AutoreleasePoolScopeLoweringContract &contract) {
  return contract.scope_symbolized_sites <= contract.scope_sites &&
         contract.contract_violation_sites <= contract.scope_sites &&
         contract.scope_entry_transition_sites == contract.scope_sites &&
         contract.scope_exit_transition_sites == contract.scope_sites &&
         (contract.scope_sites > 0u || contract.max_scope_depth == 0u) &&
         contract.max_scope_depth <= static_cast<unsigned>(contract.scope_sites);
}

std::string Objc3AutoreleasePoolScopeLoweringReplayKey(
    const Objc3AutoreleasePoolScopeLoweringContract &contract) {
  return std::string("scope_sites=") + std::to_string(contract.scope_sites) +
         ";scope_symbolized_sites=" + std::to_string(contract.scope_symbolized_sites) +
         ";max_scope_depth=" + std::to_string(contract.max_scope_depth) +
         ";scope_entry_transition_sites=" + std::to_string(contract.scope_entry_transition_sites) +
         ";scope_exit_transition_sites=" + std::to_string(contract.scope_exit_transition_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3AutoreleasePoolScopeLoweringLaneContract;
}

bool IsValidObjc3WeakUnownedSemanticsLoweringContract(
    const Objc3WeakUnownedSemanticsLoweringContract &contract) {
  return contract.weak_reference_sites <= contract.ownership_candidate_sites &&
         contract.unowned_reference_sites <= contract.ownership_candidate_sites &&
         contract.unowned_safe_reference_sites <= contract.unowned_reference_sites &&
         contract.weak_unowned_conflict_sites <= contract.ownership_candidate_sites &&
         contract.contract_violation_sites <=
             contract.ownership_candidate_sites + contract.weak_unowned_conflict_sites;
}

std::string Objc3WeakUnownedSemanticsLoweringReplayKey(
    const Objc3WeakUnownedSemanticsLoweringContract &contract) {
  return std::string("ownership_candidate_sites=") + std::to_string(contract.ownership_candidate_sites) +
         ";weak_reference_sites=" + std::to_string(contract.weak_reference_sites) +
         ";unowned_reference_sites=" + std::to_string(contract.unowned_reference_sites) +
         ";unowned_safe_reference_sites=" + std::to_string(contract.unowned_safe_reference_sites) +
         ";weak_unowned_conflict_sites=" + std::to_string(contract.weak_unowned_conflict_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3WeakUnownedSemanticsLoweringLaneContract;
}

bool IsValidObjc3ArcDiagnosticsFixitLoweringContract(
    const Objc3ArcDiagnosticsFixitLoweringContract &contract) {
  return contract.ownership_arc_fixit_available_sites <=
             contract.ownership_arc_diagnostic_candidate_sites + contract.contract_violation_sites &&
         contract.ownership_arc_profiled_sites <=
             contract.ownership_arc_diagnostic_candidate_sites + contract.contract_violation_sites &&
         contract.ownership_arc_weak_unowned_conflict_diagnostic_sites <=
             contract.ownership_arc_diagnostic_candidate_sites + contract.contract_violation_sites &&
         contract.ownership_arc_empty_fixit_hint_sites <=
             contract.ownership_arc_fixit_available_sites + contract.contract_violation_sites;
}

std::string Objc3ArcDiagnosticsFixitLoweringReplayKey(
    const Objc3ArcDiagnosticsFixitLoweringContract &contract) {
  return std::string("ownership_arc_diagnostic_candidate_sites=") +
             std::to_string(contract.ownership_arc_diagnostic_candidate_sites) +
         ";ownership_arc_fixit_available_sites=" +
             std::to_string(contract.ownership_arc_fixit_available_sites) +
         ";ownership_arc_profiled_sites=" + std::to_string(contract.ownership_arc_profiled_sites) +
         ";ownership_arc_weak_unowned_conflict_diagnostic_sites=" +
             std::to_string(contract.ownership_arc_weak_unowned_conflict_diagnostic_sites) +
         ";ownership_arc_empty_fixit_hint_sites=" +
             std::to_string(contract.ownership_arc_empty_fixit_hint_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3ArcDiagnosticsFixitLoweringLaneContract;
}

bool IsValidObjc3BlockSourceModelCompletionContract(
    const Objc3BlockSourceModelCompletionContract &contract) {
  const bool explicit_parameter_count_valid =
      contract.explicit_typed_parameter_entries_total <=
      contract.signature_entries_total;
  const bool implicit_parameter_count_valid =
      contract.implicit_parameter_entries_total <=
      contract.signature_entries_total;
  const bool readonly_capture_count_valid =
      contract.byvalue_readonly_capture_entries_total <=
      contract.capture_inventory_entries_total;
  const bool invoke_surface_count_valid =
      contract.invoke_surface_entries_total >=
      contract.block_literal_sites * 2u;
  const bool non_normalized_sites_valid =
      contract.non_normalized_sites <= contract.block_literal_sites;
  return explicit_parameter_count_valid &&
         implicit_parameter_count_valid &&
         readonly_capture_count_valid &&
         invoke_surface_count_valid &&
         non_normalized_sites_valid;
}

std::string Objc3BlockSourceModelCompletionReplayKey(
    const Objc3BlockSourceModelCompletionContract &contract) {
  return "signature_entries=" +
         std::to_string(contract.signature_entries_total) +
         ";explicit_typed_parameters=" +
         std::to_string(contract.explicit_typed_parameter_entries_total) +
         ";capture_inventory_entries=" +
         std::to_string(contract.capture_inventory_entries_total) +
         ";byvalue_readonly_captures=" +
         std::to_string(contract.byvalue_readonly_capture_entries_total) +
         ";invoke_surface_entries=" +
         std::to_string(contract.invoke_surface_entries_total) +
         ";deterministic=" + (contract.deterministic ? "true" : "false") +
         ";lane_contract=" + kObjc3BlockSourceModelCompletionLaneContract;
}

bool IsValidObjc3BlockSourceStorageAnnotationContract(
    const Objc3BlockSourceStorageAnnotationContract &contract) {
  const std::size_t classified_sites =
      contract.expression_sites +
      contract.global_initializer_sites +
      contract.binding_initializer_sites +
      contract.assignment_value_sites +
      contract.return_value_sites +
      contract.call_argument_sites +
      contract.message_argument_sites;
  if (classified_sites != contract.block_literal_sites ||
      contract.non_normalized_sites > contract.block_literal_sites ||
      contract.contract_violation_sites > contract.block_literal_sites ||
      contract.copy_helper_intent_sites > contract.block_literal_sites ||
      contract.dispose_helper_intent_sites > contract.block_literal_sites ||
      contract.heap_candidate_sites > contract.block_literal_sites ||
      contract.mutated_capture_entries_total > contract.capture_entries_total ||
      contract.byref_capture_entries_total > contract.mutated_capture_entries_total) {
    return false;
  }
  if (contract.copy_helper_intent_sites != contract.dispose_helper_intent_sites) {
    return false;
  }
  if (contract.block_literal_sites == 0) {
    return contract.capture_entries_total == 0 &&
           contract.mutated_capture_entries_total == 0 &&
           contract.byref_capture_entries_total == 0;
  }
  if (contract.heap_candidate_sites !=
      contract.global_initializer_sites +
          contract.binding_initializer_sites +
          contract.assignment_value_sites +
          contract.return_value_sites +
          contract.call_argument_sites +
          contract.message_argument_sites) {
    return false;
  }
  if ((contract.non_normalized_sites > 0 ||
       contract.contract_violation_sites > 0) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3BlockSourceStorageAnnotationReplayKey(
    const Objc3BlockSourceStorageAnnotationContract &contract) {
  return "block_literal_sites=" +
         std::to_string(contract.block_literal_sites) +
         ";capture_entries_total=" +
         std::to_string(contract.capture_entries_total) +
         ";mutated_capture_entries_total=" +
         std::to_string(contract.mutated_capture_entries_total) +
         ";byref_capture_entries_total=" +
         std::to_string(contract.byref_capture_entries_total) +
         ";copy_helper_intent_sites=" +
         std::to_string(contract.copy_helper_intent_sites) +
         ";dispose_helper_intent_sites=" +
         std::to_string(contract.dispose_helper_intent_sites) +
         ";heap_candidate_sites=" +
         std::to_string(contract.heap_candidate_sites) +
         ";expression_sites=" +
         std::to_string(contract.expression_sites) +
         ";global_initializer_sites=" +
         std::to_string(contract.global_initializer_sites) +
         ";binding_initializer_sites=" +
         std::to_string(contract.binding_initializer_sites) +
         ";assignment_value_sites=" +
         std::to_string(contract.assignment_value_sites) +
         ";return_value_sites=" +
         std::to_string(contract.return_value_sites) +
         ";call_argument_sites=" +
         std::to_string(contract.call_argument_sites) +
         ";message_argument_sites=" +
         std::to_string(contract.message_argument_sites) +
         ";deterministic=" + (contract.deterministic ? "true" : "false") +
         ";lane_contract=" +
         kObjc3BlockSourceStorageAnnotationLaneContract;
}

bool IsValidObjc3BlockLiteralCaptureLoweringContract(
    const Objc3BlockLiteralCaptureLoweringContract &contract) {
  if (contract.block_empty_capture_sites > contract.block_literal_sites ||
      contract.block_nondeterministic_capture_sites > contract.block_literal_sites ||
      contract.block_non_normalized_sites > contract.block_literal_sites ||
      contract.contract_violation_sites > contract.block_literal_sites) {
    return false;
  }
  if (contract.block_literal_sites == 0) {
    return contract.block_parameter_entries == 0 && contract.block_capture_entries == 0 &&
           contract.block_body_statement_entries == 0;
  }
  if (contract.block_nondeterministic_capture_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3BlockLiteralCaptureLoweringReplayKey(
    const Objc3BlockLiteralCaptureLoweringContract &contract) {
  return std::string("block_literal_sites=") + std::to_string(contract.block_literal_sites) +
         ";block_parameter_entries=" + std::to_string(contract.block_parameter_entries) +
         ";block_capture_entries=" + std::to_string(contract.block_capture_entries) +
         ";block_body_statement_entries=" + std::to_string(contract.block_body_statement_entries) +
         ";block_empty_capture_sites=" + std::to_string(contract.block_empty_capture_sites) +
         ";block_nondeterministic_capture_sites=" +
             std::to_string(contract.block_nondeterministic_capture_sites) +
         ";block_non_normalized_sites=" + std::to_string(contract.block_non_normalized_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3BlockLiteralCaptureLoweringLaneContract;
}

bool IsValidObjc3BlockAbiInvokeTrampolineLoweringContract(
    const Objc3BlockAbiInvokeTrampolineLoweringContract &contract) {
  if (contract.descriptor_symbolized_sites > contract.block_literal_sites ||
      contract.invoke_trampoline_symbolized_sites > contract.block_literal_sites ||
      contract.missing_invoke_trampoline_sites > contract.block_literal_sites ||
      contract.non_normalized_layout_sites > contract.block_literal_sites ||
      contract.contract_violation_sites > contract.block_literal_sites) {
    return false;
  }
  if (contract.block_literal_sites == 0) {
    return contract.invoke_argument_slots_total == 0 &&
           contract.capture_word_count_total == 0 &&
           contract.parameter_entries_total == 0 &&
           contract.capture_entries_total == 0 &&
           contract.body_statement_entries_total == 0;
  }
  if (contract.invoke_trampoline_symbolized_sites + contract.missing_invoke_trampoline_sites !=
      contract.block_literal_sites) {
    return false;
  }
  if (contract.invoke_argument_slots_total != contract.parameter_entries_total ||
      contract.capture_word_count_total != contract.capture_entries_total) {
    return false;
  }
  if ((contract.missing_invoke_trampoline_sites > 0 || contract.non_normalized_layout_sites > 0) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3BlockAbiInvokeTrampolineLoweringReplayKey(
    const Objc3BlockAbiInvokeTrampolineLoweringContract &contract) {
  return std::string("block_literal_sites=") + std::to_string(contract.block_literal_sites) +
         ";invoke_argument_slots_total=" + std::to_string(contract.invoke_argument_slots_total) +
         ";capture_word_count_total=" + std::to_string(contract.capture_word_count_total) +
         ";parameter_entries_total=" + std::to_string(contract.parameter_entries_total) +
         ";capture_entries_total=" + std::to_string(contract.capture_entries_total) +
         ";body_statement_entries_total=" + std::to_string(contract.body_statement_entries_total) +
         ";descriptor_symbolized_sites=" + std::to_string(contract.descriptor_symbolized_sites) +
         ";invoke_trampoline_symbolized_sites=" +
             std::to_string(contract.invoke_trampoline_symbolized_sites) +
         ";missing_invoke_trampoline_sites=" + std::to_string(contract.missing_invoke_trampoline_sites) +
         ";non_normalized_layout_sites=" + std::to_string(contract.non_normalized_layout_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3BlockAbiInvokeTrampolineLoweringLaneContract;
}

bool IsValidObjc3BlockStorageEscapeLoweringContract(
    const Objc3BlockStorageEscapeLoweringContract &contract) {
  // capture-legality/escape/invocation implementation anchor:
  // truthful escape classification no longer pretends that every capture is
  // mutated or lowered through byref storage. Mutable/byref counts are now a
  // bounded subset of the total capture inventory while source-only native
  // fail-closed behavior remains unchanged.
  if (contract.requires_byref_cells_sites > contract.block_literal_sites ||
      contract.escape_analysis_enabled_sites > contract.block_literal_sites ||
      contract.escape_to_heap_sites > contract.block_literal_sites ||
      contract.escape_profile_normalized_sites > contract.block_literal_sites ||
      contract.byref_layout_symbolized_sites > contract.block_literal_sites ||
      contract.contract_violation_sites > contract.block_literal_sites) {
    return false;
  }
  if (contract.block_literal_sites == 0) {
    return contract.mutable_capture_count_total == 0 &&
           contract.byref_slot_count_total == 0 &&
           contract.parameter_entries_total == 0 &&
           contract.capture_entries_total == 0 &&
           contract.body_statement_entries_total == 0;
  }
  if (contract.mutable_capture_count_total > contract.capture_entries_total ||
      contract.byref_slot_count_total > contract.mutable_capture_count_total ||
      contract.escape_analysis_enabled_sites != contract.block_literal_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 || contract.escape_profile_normalized_sites !=
                                            contract.block_literal_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3BlockStorageEscapeLoweringReplayKey(
    const Objc3BlockStorageEscapeLoweringContract &contract) {
  return std::string("block_literal_sites=") + std::to_string(contract.block_literal_sites) +
         ";mutable_capture_count_total=" + std::to_string(contract.mutable_capture_count_total) +
         ";byref_slot_count_total=" + std::to_string(contract.byref_slot_count_total) +
         ";parameter_entries_total=" + std::to_string(contract.parameter_entries_total) +
         ";capture_entries_total=" + std::to_string(contract.capture_entries_total) +
         ";body_statement_entries_total=" + std::to_string(contract.body_statement_entries_total) +
         ";requires_byref_cells_sites=" + std::to_string(contract.requires_byref_cells_sites) +
         ";escape_analysis_enabled_sites=" + std::to_string(contract.escape_analysis_enabled_sites) +
         ";escape_to_heap_sites=" + std::to_string(contract.escape_to_heap_sites) +
         ";escape_profile_normalized_sites=" + std::to_string(contract.escape_profile_normalized_sites) +
         ";byref_layout_symbolized_sites=" + std::to_string(contract.byref_layout_symbolized_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3BlockStorageEscapeLoweringLaneContract;
}

bool IsValidObjc3BlockCopyDisposeLoweringContract(
    const Objc3BlockCopyDisposeLoweringContract &contract) {
  // capture-legality/escape/invocation implementation anchor:
  // copy/dispose helper intent is now driven by truthful mutable/byref
  // capture counts rather than by the older synthetic all-captures-need-
  // helpers model.
  // byref/copy-dispose/object-ownership anchor: helper eligibility
  // may now be promoted by owned object captures even when byref slot totals remain zero,
  // so this contract intentionally avoids pinning helper counts directly to
  // byref totals.
  if (contract.copy_helper_required_sites > contract.block_literal_sites ||
      contract.dispose_helper_required_sites > contract.block_literal_sites ||
      contract.profile_normalized_sites > contract.block_literal_sites ||
      contract.copy_helper_symbolized_sites > contract.block_literal_sites ||
      contract.dispose_helper_symbolized_sites > contract.block_literal_sites ||
      contract.contract_violation_sites > contract.block_literal_sites) {
    return false;
  }
  if (contract.block_literal_sites == 0) {
    return contract.mutable_capture_count_total == 0 &&
           contract.byref_slot_count_total == 0 &&
           contract.parameter_entries_total == 0 &&
           contract.capture_entries_total == 0 &&
           contract.body_statement_entries_total == 0;
  }
  if (contract.mutable_capture_count_total > contract.capture_entries_total ||
      contract.byref_slot_count_total > contract.mutable_capture_count_total ||
      contract.copy_helper_required_sites > contract.dispose_helper_required_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 || contract.profile_normalized_sites !=
                                            contract.block_literal_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3BlockCopyDisposeLoweringReplayKey(
    const Objc3BlockCopyDisposeLoweringContract &contract) {
  return std::string("block_literal_sites=") + std::to_string(contract.block_literal_sites) +
         ";mutable_capture_count_total=" + std::to_string(contract.mutable_capture_count_total) +
         ";byref_slot_count_total=" + std::to_string(contract.byref_slot_count_total) +
         ";parameter_entries_total=" + std::to_string(contract.parameter_entries_total) +
         ";capture_entries_total=" + std::to_string(contract.capture_entries_total) +
         ";body_statement_entries_total=" + std::to_string(contract.body_statement_entries_total) +
         ";copy_helper_required_sites=" + std::to_string(contract.copy_helper_required_sites) +
         ";dispose_helper_required_sites=" + std::to_string(contract.dispose_helper_required_sites) +
         ";profile_normalized_sites=" + std::to_string(contract.profile_normalized_sites) +
         ";copy_helper_symbolized_sites=" + std::to_string(contract.copy_helper_symbolized_sites) +
         ";dispose_helper_symbolized_sites=" + std::to_string(contract.dispose_helper_symbolized_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3BlockCopyDisposeLoweringLaneContract;
}

bool IsValidObjc3BlockDeterminismPerfBaselineLoweringContract(
    const Objc3BlockDeterminismPerfBaselineLoweringContract &contract) {
  if (contract.deterministic_capture_sites > contract.block_literal_sites ||
      contract.heavy_tier_sites > contract.block_literal_sites ||
      contract.normalized_profile_sites > contract.block_literal_sites ||
      contract.contract_violation_sites > contract.block_literal_sites) {
    return false;
  }
  if (contract.block_literal_sites == 0) {
    return contract.baseline_weight_total == 0 &&
           contract.parameter_entries_total == 0 &&
           contract.capture_entries_total == 0 &&
           contract.body_statement_entries_total == 0;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_profile_sites != contract.block_literal_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3BlockDeterminismPerfBaselineLoweringReplayKey(
    const Objc3BlockDeterminismPerfBaselineLoweringContract &contract) {
  return std::string("block_literal_sites=") + std::to_string(contract.block_literal_sites) +
         ";baseline_weight_total=" + std::to_string(contract.baseline_weight_total) +
         ";parameter_entries_total=" + std::to_string(contract.parameter_entries_total) +
         ";capture_entries_total=" + std::to_string(contract.capture_entries_total) +
         ";body_statement_entries_total=" + std::to_string(contract.body_statement_entries_total) +
         ";deterministic_capture_sites=" + std::to_string(contract.deterministic_capture_sites) +
         ";heavy_tier_sites=" + std::to_string(contract.heavy_tier_sites) +
         ";normalized_profile_sites=" + std::to_string(contract.normalized_profile_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3BlockDeterminismPerfBaselineLoweringLaneContract;
}

bool IsValidObjc3TypeSystemOptionalKeypathLoweringContract(
    const Objc3TypeSystemOptionalKeypathLoweringContract &contract) {
  if (contract.optional_binding_clause_sites > contract.optional_binding_sites ||
      contract.typed_keypath_self_root_sites >
          contract.typed_keypath_literal_sites ||
      contract.typed_keypath_class_root_sites >
          contract.typed_keypath_literal_sites) {
    return false;
  }
  if (contract.live_optional_lowering_sites !=
      contract.optional_binding_sites + contract.optional_send_sites +
          contract.nil_coalescing_sites) {
    return false;
  }
  if (contract.single_evaluation_nil_short_circuit_sites !=
      contract.optional_binding_clause_sites + contract.optional_send_sites +
          contract.nil_coalescing_sites) {
    return false;
  }
  if (contract.live_typed_keypath_artifact_sites +
          contract.deferred_typed_keypath_sites !=
      contract.typed_keypath_literal_sites) {
    return false;
  }
  if (contract.contract_violation_sites >
      contract.live_optional_lowering_sites +
          contract.live_typed_keypath_artifact_sites +
          contract.deferred_typed_keypath_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3TypeSystemOptionalKeypathLoweringReplayKey(
    const Objc3TypeSystemOptionalKeypathLoweringContract &contract) {
  return std::string("optional_binding_sites=") +
         std::to_string(contract.optional_binding_sites) +
         ";optional_binding_clause_sites=" +
         std::to_string(contract.optional_binding_clause_sites) +
         ";optional_send_sites=" +
         std::to_string(contract.optional_send_sites) +
         ";nil_coalescing_sites=" +
         std::to_string(contract.nil_coalescing_sites) +
         ";typed_keypath_literal_sites=" +
         std::to_string(contract.typed_keypath_literal_sites) +
         ";typed_keypath_self_root_sites=" +
         std::to_string(contract.typed_keypath_self_root_sites) +
         ";typed_keypath_class_root_sites=" +
         std::to_string(contract.typed_keypath_class_root_sites) +
         ";live_optional_lowering_sites=" +
         std::to_string(contract.live_optional_lowering_sites) +
         ";single_evaluation_nil_short_circuit_sites=" +
         std::to_string(contract.single_evaluation_nil_short_circuit_sites) +
         ";live_typed_keypath_artifact_sites=" +
         std::to_string(contract.live_typed_keypath_artifact_sites) +
         ";deferred_typed_keypath_sites=" +
         std::to_string(contract.deferred_typed_keypath_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3TypeSystemOptionalKeypathLoweringLaneContract;
}

bool IsValidObjc3ControlFlowControlFlowSafetyLoweringContract(
    const Objc3ControlFlowControlFlowSafetyLoweringContract &contract) {
  if (contract.guard_clause_sites < contract.guard_statement_sites) {
    return false;
  }
  if (contract.live_guard_short_circuit_sites +
          contract.fail_closed_guard_short_circuit_sites !=
      contract.guard_statement_sites) {
    return false;
  }
  if (contract.live_match_dispatch_sites +
          contract.fail_closed_match_dispatch_sites !=
      contract.match_statement_sites) {
    return false;
  }
  if (contract.live_defer_cleanup_sites +
          contract.fail_closed_defer_cleanup_sites !=
      contract.defer_statement_sites) {
    return false;
  }
  if (contract.deterministic_fail_closed_sites !=
      contract.fail_closed_guard_short_circuit_sites +
          contract.fail_closed_match_dispatch_sites +
          contract.fail_closed_defer_cleanup_sites) {
    return false;
  }
  if (contract.contract_violation_sites >
      contract.live_guard_short_circuit_sites +
          contract.live_match_dispatch_sites +
          contract.live_defer_cleanup_sites +
          contract.deterministic_fail_closed_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3ControlFlowControlFlowSafetyLoweringReplayKey(
    const Objc3ControlFlowControlFlowSafetyLoweringContract &contract) {
  return std::string("guard_statement_sites=") +
         std::to_string(contract.guard_statement_sites) +
         ";guard_clause_sites=" +
         std::to_string(contract.guard_clause_sites) +
         ";match_statement_sites=" +
         std::to_string(contract.match_statement_sites) +
         ";defer_statement_sites=" +
         std::to_string(contract.defer_statement_sites) +
         ";live_guard_short_circuit_sites=" +
         std::to_string(contract.live_guard_short_circuit_sites) +
         ";live_match_dispatch_sites=" +
         std::to_string(contract.live_match_dispatch_sites) +
         ";live_defer_cleanup_sites=" +
         std::to_string(contract.live_defer_cleanup_sites) +
         ";fail_closed_guard_short_circuit_sites=" +
         std::to_string(contract.fail_closed_guard_short_circuit_sites) +
         ";fail_closed_match_dispatch_sites=" +
         std::to_string(contract.fail_closed_match_dispatch_sites) +
         ";fail_closed_defer_cleanup_sites=" +
         std::to_string(contract.fail_closed_defer_cleanup_sites) +
         ";deterministic_fail_closed_sites=" +
         std::to_string(contract.deterministic_fail_closed_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3ControlFlowControlFlowSafetyLoweringLaneContract;
}

bool IsValidObjc3LightweightGenericsConstraintLoweringContract(
    const Objc3LightweightGenericsConstraintLoweringContract &contract) {
  if (contract.generic_suffix_sites > contract.generic_constraint_sites ||
      contract.object_pointer_type_sites > contract.generic_constraint_sites ||
      contract.terminated_generic_suffix_sites > contract.generic_suffix_sites ||
      contract.pointer_declarator_sites > contract.generic_constraint_sites ||
      contract.normalized_constraint_sites > contract.generic_constraint_sites ||
      contract.contract_violation_sites > contract.generic_constraint_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_constraint_sites != contract.generic_constraint_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3LightweightGenericsConstraintLoweringReplayKey(
    const Objc3LightweightGenericsConstraintLoweringContract &contract) {
  return std::string("generic_constraint_sites=") + std::to_string(contract.generic_constraint_sites) +
         ";generic_suffix_sites=" + std::to_string(contract.generic_suffix_sites) +
         ";object_pointer_type_sites=" + std::to_string(contract.object_pointer_type_sites) +
         ";terminated_generic_suffix_sites=" + std::to_string(contract.terminated_generic_suffix_sites) +
         ";pointer_declarator_sites=" + std::to_string(contract.pointer_declarator_sites) +
         ";normalized_constraint_sites=" + std::to_string(contract.normalized_constraint_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3LightweightGenericsConstraintLoweringLaneContract;
}

bool IsValidObjc3NullabilityFlowWarningPrecisionLoweringContract(
    const Objc3NullabilityFlowWarningPrecisionLoweringContract &contract) {
  if (contract.nullability_suffix_sites > contract.nullability_flow_sites ||
      contract.nullable_suffix_sites > contract.nullability_suffix_sites ||
      contract.nonnull_suffix_sites > contract.nullability_suffix_sites ||
      contract.object_pointer_type_sites < contract.nullability_suffix_sites ||
      contract.normalized_sites > contract.nullability_flow_sites ||
      contract.contract_violation_sites > contract.nullability_flow_sites) {
    return false;
  }
  if (contract.nullability_suffix_sites !=
      contract.nullable_suffix_sites + contract.nonnull_suffix_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites != contract.nullability_flow_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3NullabilityFlowWarningPrecisionLoweringReplayKey(
    const Objc3NullabilityFlowWarningPrecisionLoweringContract &contract) {
  return std::string("nullability_flow_sites=") + std::to_string(contract.nullability_flow_sites) +
         ";object_pointer_type_sites=" + std::to_string(contract.object_pointer_type_sites) +
         ";nullability_suffix_sites=" + std::to_string(contract.nullability_suffix_sites) +
         ";nullable_suffix_sites=" + std::to_string(contract.nullable_suffix_sites) +
         ";nonnull_suffix_sites=" + std::to_string(contract.nonnull_suffix_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3NullabilityFlowWarningPrecisionLoweringLaneContract;
}

bool IsValidObjc3ProtocolQualifiedObjectTypeLoweringContract(
    const Objc3ProtocolQualifiedObjectTypeLoweringContract &contract) {
  if (contract.terminated_protocol_composition_sites > contract.protocol_composition_sites ||
      contract.normalized_protocol_composition_sites > contract.protocol_qualified_object_type_sites ||
      contract.contract_violation_sites > contract.protocol_qualified_object_type_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_protocol_composition_sites != contract.protocol_qualified_object_type_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3ProtocolQualifiedObjectTypeLoweringReplayKey(
    const Objc3ProtocolQualifiedObjectTypeLoweringContract &contract) {
  return std::string("protocol_qualified_object_type_sites=") +
         std::to_string(contract.protocol_qualified_object_type_sites) +
         ";protocol_composition_sites=" + std::to_string(contract.protocol_composition_sites) +
         ";object_pointer_type_sites=" + std::to_string(contract.object_pointer_type_sites) +
         ";terminated_protocol_composition_sites=" + std::to_string(contract.terminated_protocol_composition_sites) +
         ";pointer_declarator_sites=" + std::to_string(contract.pointer_declarator_sites) +
         ";normalized_protocol_composition_sites=" +
         std::to_string(contract.normalized_protocol_composition_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3ProtocolQualifiedObjectTypeLoweringLaneContract;
}

bool IsValidObjc3VarianceBridgeCastLoweringContract(
    const Objc3VarianceBridgeCastLoweringContract &contract) {
  if (contract.protocol_composition_sites > contract.variance_bridge_cast_sites ||
      contract.ownership_qualifier_sites > contract.variance_bridge_cast_sites ||
      contract.object_pointer_type_sites < contract.protocol_composition_sites ||
      contract.pointer_declarator_sites > contract.variance_bridge_cast_sites ||
      contract.normalized_sites > contract.variance_bridge_cast_sites ||
      contract.contract_violation_sites > contract.variance_bridge_cast_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites != contract.variance_bridge_cast_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3VarianceBridgeCastLoweringReplayKey(
    const Objc3VarianceBridgeCastLoweringContract &contract) {
  return std::string("variance_bridge_cast_sites=") + std::to_string(contract.variance_bridge_cast_sites) +
         ";protocol_composition_sites=" + std::to_string(contract.protocol_composition_sites) +
         ";ownership_qualifier_sites=" + std::to_string(contract.ownership_qualifier_sites) +
         ";object_pointer_type_sites=" + std::to_string(contract.object_pointer_type_sites) +
         ";pointer_declarator_sites=" + std::to_string(contract.pointer_declarator_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3VarianceBridgeCastLoweringLaneContract;
}

bool IsValidObjc3GenericMetadataAbiLoweringContract(
    const Objc3GenericMetadataAbiLoweringContract &contract) {
  if (contract.generic_suffix_sites > contract.generic_metadata_abi_sites ||
      contract.protocol_composition_sites > contract.generic_metadata_abi_sites ||
      contract.ownership_qualifier_sites > contract.generic_metadata_abi_sites ||
      contract.object_pointer_type_sites < contract.protocol_composition_sites ||
      contract.pointer_declarator_sites > contract.generic_metadata_abi_sites ||
      contract.normalized_sites > contract.generic_metadata_abi_sites ||
      contract.contract_violation_sites > contract.generic_metadata_abi_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites != contract.generic_metadata_abi_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3GenericMetadataAbiLoweringReplayKey(
    const Objc3GenericMetadataAbiLoweringContract &contract) {
  return std::string("generic_metadata_abi_sites=") + std::to_string(contract.generic_metadata_abi_sites) +
         ";generic_suffix_sites=" + std::to_string(contract.generic_suffix_sites) +
         ";protocol_composition_sites=" + std::to_string(contract.protocol_composition_sites) +
         ";ownership_qualifier_sites=" + std::to_string(contract.ownership_qualifier_sites) +
         ";object_pointer_type_sites=" + std::to_string(contract.object_pointer_type_sites) +
         ";pointer_declarator_sites=" + std::to_string(contract.pointer_declarator_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3GenericMetadataAbiLoweringLaneContract;
}

bool IsValidObjc3ModuleImportGraphLoweringContract(
    const Objc3ModuleImportGraphLoweringContract &contract) {
  if (contract.import_edge_candidate_sites > contract.module_import_graph_sites ||
      contract.namespace_segment_sites > contract.module_import_graph_sites ||
      contract.object_pointer_type_sites < contract.import_edge_candidate_sites ||
      contract.pointer_declarator_sites > contract.module_import_graph_sites ||
      contract.normalized_sites > contract.module_import_graph_sites ||
      contract.contract_violation_sites > contract.module_import_graph_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites != contract.module_import_graph_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3ModuleImportGraphLoweringReplayKey(
    const Objc3ModuleImportGraphLoweringContract &contract) {
  return std::string("module_import_graph_sites=") + std::to_string(contract.module_import_graph_sites) +
         ";import_edge_candidate_sites=" + std::to_string(contract.import_edge_candidate_sites) +
         ";namespace_segment_sites=" + std::to_string(contract.namespace_segment_sites) +
         ";object_pointer_type_sites=" + std::to_string(contract.object_pointer_type_sites) +
         ";pointer_declarator_sites=" + std::to_string(contract.pointer_declarator_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3ModuleImportGraphLoweringLaneContract;
}

bool IsValidObjc3NamespaceCollisionShadowingLoweringContract(
    const Objc3NamespaceCollisionShadowingLoweringContract &contract) {
  if (contract.namespace_segment_sites > contract.namespace_collision_shadowing_sites ||
      contract.import_edge_candidate_sites > contract.namespace_collision_shadowing_sites ||
      contract.object_pointer_type_sites < contract.import_edge_candidate_sites ||
      contract.pointer_declarator_sites > contract.namespace_collision_shadowing_sites ||
      contract.normalized_sites > contract.namespace_collision_shadowing_sites ||
      contract.contract_violation_sites > contract.namespace_collision_shadowing_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites != contract.namespace_collision_shadowing_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3NamespaceCollisionShadowingLoweringReplayKey(
    const Objc3NamespaceCollisionShadowingLoweringContract &contract) {
  return std::string("namespace_collision_shadowing_sites=") +
             std::to_string(contract.namespace_collision_shadowing_sites) +
         ";namespace_segment_sites=" + std::to_string(contract.namespace_segment_sites) +
         ";import_edge_candidate_sites=" +
             std::to_string(contract.import_edge_candidate_sites) +
         ";object_pointer_type_sites=" +
             std::to_string(contract.object_pointer_type_sites) +
         ";pointer_declarator_sites=" +
             std::to_string(contract.pointer_declarator_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";contract_violation_sites=" +
             std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3NamespaceCollisionShadowingLoweringLaneContract;
}

bool IsValidObjc3PublicPrivateApiPartitionLoweringContract(
    const Objc3PublicPrivateApiPartitionLoweringContract &contract) {
  if (contract.namespace_segment_sites >
          contract.public_private_api_partition_sites ||
      contract.import_edge_candidate_sites >
          contract.public_private_api_partition_sites ||
      contract.object_pointer_type_sites <
          contract.import_edge_candidate_sites ||
      contract.pointer_declarator_sites >
          contract.public_private_api_partition_sites ||
      contract.normalized_sites > contract.public_private_api_partition_sites ||
      contract.contract_violation_sites >
          contract.public_private_api_partition_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites != contract.public_private_api_partition_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3PublicPrivateApiPartitionLoweringReplayKey(
    const Objc3PublicPrivateApiPartitionLoweringContract &contract) {
  return std::string("public_private_api_partition_sites=") +
             std::to_string(contract.public_private_api_partition_sites) +
         ";namespace_segment_sites=" +
         std::to_string(contract.namespace_segment_sites) +
         ";import_edge_candidate_sites=" +
         std::to_string(contract.import_edge_candidate_sites) +
         ";object_pointer_type_sites=" +
         std::to_string(contract.object_pointer_type_sites) +
         ";pointer_declarator_sites=" +
         std::to_string(contract.pointer_declarator_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3PublicPrivateApiPartitionLoweringLaneContract;
}

bool IsValidObjc3IncrementalModuleCacheInvalidationLoweringContract(
    const Objc3IncrementalModuleCacheInvalidationLoweringContract &contract) {
  if (contract.namespace_segment_sites >
          contract.incremental_module_cache_invalidation_sites ||
      contract.import_edge_candidate_sites >
          contract.incremental_module_cache_invalidation_sites ||
      contract.object_pointer_type_sites < contract.import_edge_candidate_sites ||
      contract.pointer_declarator_sites >
          contract.incremental_module_cache_invalidation_sites ||
      contract.normalized_sites >
          contract.incremental_module_cache_invalidation_sites ||
      contract.cache_invalidation_candidate_sites >
          contract.incremental_module_cache_invalidation_sites ||
      contract.contract_violation_sites >
          contract.incremental_module_cache_invalidation_sites) {
    return false;
  }
  if (contract.normalized_sites +
          contract.cache_invalidation_candidate_sites >
      contract.incremental_module_cache_invalidation_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites !=
           contract.incremental_module_cache_invalidation_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3IncrementalModuleCacheInvalidationLoweringReplayKey(
    const Objc3IncrementalModuleCacheInvalidationLoweringContract &contract) {
  return std::string("incremental_module_cache_invalidation_sites=") +
             std::to_string(contract.incremental_module_cache_invalidation_sites) +
         ";namespace_segment_sites=" +
         std::to_string(contract.namespace_segment_sites) +
         ";import_edge_candidate_sites=" +
         std::to_string(contract.import_edge_candidate_sites) +
         ";object_pointer_type_sites=" +
         std::to_string(contract.object_pointer_type_sites) +
         ";pointer_declarator_sites=" +
         std::to_string(contract.pointer_declarator_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";cache_invalidation_candidate_sites=" +
         std::to_string(contract.cache_invalidation_candidate_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3IncrementalModuleCacheInvalidationLoweringLaneContract;
}

bool IsValidObjc3CrossModuleConformanceLoweringContract(
    const Objc3CrossModuleConformanceLoweringContract &contract) {
  if (contract.namespace_segment_sites >
          contract.cross_module_conformance_sites ||
      contract.import_edge_candidate_sites >
          contract.cross_module_conformance_sites ||
      contract.object_pointer_type_sites < contract.import_edge_candidate_sites ||
      contract.pointer_declarator_sites >
          contract.cross_module_conformance_sites ||
      contract.normalized_sites > contract.cross_module_conformance_sites ||
      contract.cache_invalidation_candidate_sites >
          contract.cross_module_conformance_sites ||
      contract.contract_violation_sites >
          contract.cross_module_conformance_sites) {
    return false;
  }
  if (contract.normalized_sites +
          contract.cache_invalidation_candidate_sites >
      contract.cross_module_conformance_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites != contract.cross_module_conformance_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3CrossModuleConformanceLoweringReplayKey(
    const Objc3CrossModuleConformanceLoweringContract &contract) {
  return std::string("cross_module_conformance_sites=") +
             std::to_string(contract.cross_module_conformance_sites) +
         ";namespace_segment_sites=" +
         std::to_string(contract.namespace_segment_sites) +
         ";import_edge_candidate_sites=" +
         std::to_string(contract.import_edge_candidate_sites) +
         ";object_pointer_type_sites=" +
         std::to_string(contract.object_pointer_type_sites) +
         ";pointer_declarator_sites=" +
         std::to_string(contract.pointer_declarator_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";cache_invalidation_candidate_sites=" +
         std::to_string(contract.cache_invalidation_candidate_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3CrossModuleConformanceLoweringLaneContract;
}

bool IsValidObjc3ThrowsPropagationLoweringContract(
    const Objc3ThrowsPropagationLoweringContract &contract) {
  if (contract.namespace_segment_sites > contract.throws_propagation_sites ||
      contract.import_edge_candidate_sites > contract.throws_propagation_sites ||
      contract.object_pointer_type_sites < contract.import_edge_candidate_sites ||
      contract.pointer_declarator_sites > contract.throws_propagation_sites ||
      contract.normalized_sites > contract.throws_propagation_sites ||
      contract.cache_invalidation_candidate_sites >
          contract.throws_propagation_sites ||
      contract.contract_violation_sites > contract.throws_propagation_sites) {
    return false;
  }
  if (contract.normalized_sites +
          contract.cache_invalidation_candidate_sites >
      contract.throws_propagation_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites != contract.throws_propagation_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3ThrowsPropagationLoweringReplayKey(
    const Objc3ThrowsPropagationLoweringContract &contract) {
  return std::string("throws_propagation_sites=") +
             std::to_string(contract.throws_propagation_sites) +
         ";namespace_segment_sites=" +
         std::to_string(contract.namespace_segment_sites) +
         ";import_edge_candidate_sites=" +
         std::to_string(contract.import_edge_candidate_sites) +
         ";object_pointer_type_sites=" +
         std::to_string(contract.object_pointer_type_sites) +
         ";pointer_declarator_sites=" +
         std::to_string(contract.pointer_declarator_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";cache_invalidation_candidate_sites=" +
         std::to_string(contract.cache_invalidation_candidate_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3ThrowsPropagationLoweringLaneContract;
}

bool IsValidObjc3ResultLikeLoweringContract(
    const Objc3ResultLikeLoweringContract &contract) {
  if (contract.result_success_sites > contract.result_like_sites ||
      contract.result_failure_sites > contract.result_like_sites ||
      contract.result_branch_sites > contract.result_like_sites ||
      contract.result_payload_sites > contract.result_like_sites ||
      contract.normalized_sites > contract.result_like_sites ||
      contract.branch_merge_sites > contract.result_like_sites ||
      contract.contract_violation_sites > contract.result_like_sites) {
    return false;
  }
  if (contract.normalized_sites + contract.branch_merge_sites !=
      contract.result_like_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3ResultLikeLoweringReplayKey(
    const Objc3ResultLikeLoweringContract &contract) {
  return std::string("result_like_sites=") +
             std::to_string(contract.result_like_sites) +
         ";result_success_sites=" +
         std::to_string(contract.result_success_sites) +
         ";result_failure_sites=" +
         std::to_string(contract.result_failure_sites) +
         ";result_branch_sites=" +
         std::to_string(contract.result_branch_sites) +
         ";result_payload_sites=" +
         std::to_string(contract.result_payload_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";branch_merge_sites=" + std::to_string(contract.branch_merge_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3ResultLikeLoweringLaneContract;
}

bool IsValidObjc3NSErrorBridgingLoweringContract(
    const Objc3NSErrorBridgingLoweringContract &contract) {
  if (contract.ns_error_parameter_sites > contract.ns_error_bridging_sites ||
      contract.ns_error_out_parameter_sites > contract.ns_error_parameter_sites ||
      contract.ns_error_bridge_path_sites > contract.ns_error_out_parameter_sites ||
      contract.ns_error_bridge_path_sites > contract.failable_call_sites ||
      contract.failable_call_sites > contract.ns_error_bridging_sites ||
      contract.normalized_sites > contract.ns_error_bridging_sites ||
      contract.bridge_boundary_sites > contract.ns_error_bridging_sites ||
      contract.contract_violation_sites > contract.ns_error_bridging_sites) {
    return false;
  }
  if (contract.normalized_sites + contract.bridge_boundary_sites !=
      contract.ns_error_bridging_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3NSErrorBridgingLoweringReplayKey(
    const Objc3NSErrorBridgingLoweringContract &contract) {
  return std::string("ns_error_bridging_sites=") +
             std::to_string(contract.ns_error_bridging_sites) +
         ";ns_error_parameter_sites=" +
         std::to_string(contract.ns_error_parameter_sites) +
         ";ns_error_out_parameter_sites=" +
         std::to_string(contract.ns_error_out_parameter_sites) +
         ";ns_error_bridge_path_sites=" +
         std::to_string(contract.ns_error_bridge_path_sites) +
         ";failable_call_sites=" +
         std::to_string(contract.failable_call_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";bridge_boundary_sites=" + std::to_string(contract.bridge_boundary_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3NSErrorBridgingLoweringLaneContract;
}

bool IsValidObjc3UnwindCleanupLoweringContract(
    const Objc3UnwindCleanupLoweringContract &contract) {
  if (contract.unwind_edge_sites > contract.unwind_cleanup_sites ||
      contract.cleanup_scope_sites > contract.unwind_cleanup_sites ||
      contract.cleanup_emit_sites > contract.cleanup_scope_sites ||
      contract.landing_pad_sites > contract.unwind_cleanup_sites ||
      contract.cleanup_resume_sites > contract.unwind_cleanup_sites ||
      contract.normalized_sites > contract.unwind_cleanup_sites ||
      contract.guard_blocked_sites > contract.unwind_cleanup_sites ||
      contract.contract_violation_sites > contract.unwind_cleanup_sites) {
    return false;
  }
  if (contract.landing_pad_sites + contract.cleanup_resume_sites >
      contract.unwind_cleanup_sites) {
    return false;
  }
  if (contract.normalized_sites + contract.guard_blocked_sites !=
      contract.unwind_cleanup_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3UnwindCleanupLoweringReplayKey(
    const Objc3UnwindCleanupLoweringContract &contract) {
  return std::string("unwind_cleanup_sites=") +
             std::to_string(contract.unwind_cleanup_sites) +
         ";unwind_edge_sites=" + std::to_string(contract.unwind_edge_sites) +
         ";cleanup_scope_sites=" +
         std::to_string(contract.cleanup_scope_sites) +
         ";cleanup_emit_sites=" + std::to_string(contract.cleanup_emit_sites) +
         ";landing_pad_sites=" + std::to_string(contract.landing_pad_sites) +
         ";cleanup_resume_sites=" +
         std::to_string(contract.cleanup_resume_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";guard_blocked_sites=" +
         std::to_string(contract.guard_blocked_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3UnwindCleanupLoweringLaneContract;
}
