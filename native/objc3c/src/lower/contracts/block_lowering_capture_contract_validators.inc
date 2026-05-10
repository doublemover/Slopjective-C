bool IsValidObjc3BlockLiteralCaptureLoweringContract(
    const Objc3BlockLiteralCaptureLoweringContract &contract) {
  if (contract.block_empty_capture_sites > contract.block_literal_sites ||
      contract.block_nondeterministic_capture_sites >
          contract.block_literal_sites ||
      contract.block_non_normalized_sites > contract.block_literal_sites ||
      contract.contract_violation_sites > contract.block_literal_sites) {
    return false;
  }
  if (contract.block_literal_sites == 0) {
    return contract.block_parameter_entries == 0 &&
           contract.block_capture_entries == 0 &&
           contract.block_body_statement_entries == 0;
  }
  if (contract.block_nondeterministic_capture_sites > 0 &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3BlockLiteralCaptureLoweringReplayKey(
    const Objc3BlockLiteralCaptureLoweringContract &contract) {
  return std::string("block_literal_sites=") +
         std::to_string(contract.block_literal_sites) +
         ";block_parameter_entries=" +
         std::to_string(contract.block_parameter_entries) +
         ";block_capture_entries=" +
         std::to_string(contract.block_capture_entries) +
         ";block_body_statement_entries=" +
         std::to_string(contract.block_body_statement_entries) +
         ";block_empty_capture_sites=" +
         std::to_string(contract.block_empty_capture_sites) +
         ";block_nondeterministic_capture_sites=" +
         std::to_string(contract.block_nondeterministic_capture_sites) +
         ";block_non_normalized_sites=" +
         std::to_string(contract.block_non_normalized_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3BlockLiteralCaptureLoweringLaneContract;
}

bool IsValidObjc3BlockAbiInvokeTrampolineLoweringContract(
    const Objc3BlockAbiInvokeTrampolineLoweringContract &contract) {
  if (contract.descriptor_symbolized_sites > contract.block_literal_sites ||
      contract.invoke_trampoline_symbolized_sites >
          contract.block_literal_sites ||
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
  if (contract.invoke_trampoline_symbolized_sites +
          contract.missing_invoke_trampoline_sites !=
      contract.block_literal_sites) {
    return false;
  }
  if (contract.invoke_argument_slots_total !=
          contract.parameter_entries_total ||
      contract.capture_word_count_total != contract.capture_entries_total) {
    return false;
  }
  if ((contract.missing_invoke_trampoline_sites > 0 ||
       contract.non_normalized_layout_sites > 0) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3BlockAbiInvokeTrampolineLoweringReplayKey(
    const Objc3BlockAbiInvokeTrampolineLoweringContract &contract) {
  return std::string("block_literal_sites=") +
         std::to_string(contract.block_literal_sites) +
         ";invoke_argument_slots_total=" +
         std::to_string(contract.invoke_argument_slots_total) +
         ";capture_word_count_total=" +
         std::to_string(contract.capture_word_count_total) +
         ";parameter_entries_total=" +
         std::to_string(contract.parameter_entries_total) +
         ";capture_entries_total=" +
         std::to_string(contract.capture_entries_total) +
         ";body_statement_entries_total=" +
         std::to_string(contract.body_statement_entries_total) +
         ";descriptor_symbolized_sites=" +
         std::to_string(contract.descriptor_symbolized_sites) +
         ";invoke_trampoline_symbolized_sites=" +
         std::to_string(contract.invoke_trampoline_symbolized_sites) +
         ";missing_invoke_trampoline_sites=" +
         std::to_string(contract.missing_invoke_trampoline_sites) +
         ";non_normalized_layout_sites=" +
         std::to_string(contract.non_normalized_layout_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3BlockAbiInvokeTrampolineLoweringLaneContract;
}
