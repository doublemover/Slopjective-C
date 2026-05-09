#include "lower/objc3_lowering_contract.h"

#include "lower/metadata/lowering_metadata_helpers.h"

#include <string>

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

