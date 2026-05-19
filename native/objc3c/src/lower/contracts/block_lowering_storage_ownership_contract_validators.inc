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
  if ((contract.contract_violation_sites > 0 ||
       contract.escape_profile_normalized_sites !=
           contract.block_literal_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3BlockStorageEscapeLoweringReplayKey(
    const Objc3BlockStorageEscapeLoweringContract &contract) {
  return std::string("block_literal_sites=") +
         std::to_string(contract.block_literal_sites) +
         ";mutable_capture_count_total=" +
         std::to_string(contract.mutable_capture_count_total) +
         ";byref_slot_count_total=" +
         std::to_string(contract.byref_slot_count_total) +
         ";parameter_entries_total=" +
         std::to_string(contract.parameter_entries_total) +
         ";capture_entries_total=" +
         std::to_string(contract.capture_entries_total) +
         ";body_statement_entries_total=" +
         std::to_string(contract.body_statement_entries_total) +
         ";requires_byref_cells_sites=" +
         std::to_string(contract.requires_byref_cells_sites) +
         ";escape_analysis_enabled_sites=" +
         std::to_string(contract.escape_analysis_enabled_sites) +
         ";escape_to_heap_sites=" +
         std::to_string(contract.escape_to_heap_sites) +
         ";escape_profile_normalized_sites=" +
         std::to_string(contract.escape_profile_normalized_sites) +
         ";byref_layout_symbolized_sites=" +
         std::to_string(contract.byref_layout_symbolized_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
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
  // may now be promoted by owned object captures even when byref slot totals
  // remain zero, so this contract intentionally avoids pinning helper counts
  // directly to byref totals.
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
      contract.copy_helper_required_sites >
          contract.dispose_helper_required_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.profile_normalized_sites != contract.block_literal_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3BlockCopyDisposeLoweringReplayKey(
    const Objc3BlockCopyDisposeLoweringContract &contract) {
  return std::string("block_literal_sites=") +
         std::to_string(contract.block_literal_sites) +
         ";mutable_capture_count_total=" +
         std::to_string(contract.mutable_capture_count_total) +
         ";byref_slot_count_total=" +
         std::to_string(contract.byref_slot_count_total) +
         ";parameter_entries_total=" +
         std::to_string(contract.parameter_entries_total) +
         ";capture_entries_total=" +
         std::to_string(contract.capture_entries_total) +
         ";body_statement_entries_total=" +
         std::to_string(contract.body_statement_entries_total) +
         ";copy_helper_required_sites=" +
         std::to_string(contract.copy_helper_required_sites) +
         ";dispose_helper_required_sites=" +
         std::to_string(contract.dispose_helper_required_sites) +
         ";profile_normalized_sites=" +
         std::to_string(contract.profile_normalized_sites) +
         ";copy_helper_symbolized_sites=" +
         std::to_string(contract.copy_helper_symbolized_sites) +
         ";dispose_helper_symbolized_sites=" +
         std::to_string(contract.dispose_helper_symbolized_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3BlockCopyDisposeLoweringLaneContract;
}
