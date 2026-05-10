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
  return explicit_parameter_count_valid && implicit_parameter_count_valid &&
         readonly_capture_count_valid && invoke_surface_count_valid &&
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
      contract.expression_sites + contract.global_initializer_sites +
      contract.binding_initializer_sites + contract.assignment_value_sites +
      contract.return_value_sites + contract.call_argument_sites +
      contract.message_argument_sites;
  if (classified_sites != contract.block_literal_sites ||
      contract.non_normalized_sites > contract.block_literal_sites ||
      contract.contract_violation_sites > contract.block_literal_sites ||
      contract.copy_helper_intent_sites > contract.block_literal_sites ||
      contract.dispose_helper_intent_sites > contract.block_literal_sites ||
      contract.heap_candidate_sites > contract.block_literal_sites ||
      contract.mutated_capture_entries_total > contract.capture_entries_total ||
      contract.byref_capture_entries_total >
          contract.mutated_capture_entries_total) {
    return false;
  }
  if (contract.copy_helper_intent_sites !=
      contract.dispose_helper_intent_sites) {
    return false;
  }
  if (contract.block_literal_sites == 0) {
    return contract.capture_entries_total == 0 &&
           contract.mutated_capture_entries_total == 0 &&
           contract.byref_capture_entries_total == 0;
  }
  if (contract.heap_candidate_sites !=
      contract.global_initializer_sites + contract.binding_initializer_sites +
          contract.assignment_value_sites + contract.return_value_sites +
          contract.call_argument_sites + contract.message_argument_sites) {
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
         ";expression_sites=" + std::to_string(contract.expression_sites) +
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
