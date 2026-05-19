#include "sema/objc3_sema_pass_block_equivalence.h"

#include <algorithm>

// executable-block-source-closure anchor: sema parity for block source closure
// remains summary-based at this freeze point because runnable block realization
// has not started yet.
bool IsEquivalentBlockLiteralCaptureSemanticsSummary(
    const Objc3BlockLiteralCaptureSemanticsSummary &lhs,
    const Objc3BlockLiteralCaptureSemanticsSummary &rhs) {
  return lhs.block_literal_sites == rhs.block_literal_sites &&
         lhs.block_parameter_entries == rhs.block_parameter_entries &&
         lhs.block_capture_entries == rhs.block_capture_entries &&
         lhs.block_body_statement_entries == rhs.block_body_statement_entries &&
         lhs.block_empty_capture_sites == rhs.block_empty_capture_sites &&
         lhs.block_nondeterministic_capture_sites ==
             rhs.block_nondeterministic_capture_sites &&
         lhs.block_non_normalized_sites == rhs.block_non_normalized_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentBlockAbiInvokeTrampolineSemanticsSummary(
    const Objc3BlockAbiInvokeTrampolineSemanticsSummary &lhs,
    const Objc3BlockAbiInvokeTrampolineSemanticsSummary &rhs) {
  return lhs.block_literal_sites == rhs.block_literal_sites &&
         lhs.invoke_argument_slots_total == rhs.invoke_argument_slots_total &&
         lhs.capture_word_count_total == rhs.capture_word_count_total &&
         lhs.parameter_entries_total == rhs.parameter_entries_total &&
         lhs.capture_entries_total == rhs.capture_entries_total &&
         lhs.body_statement_entries_total == rhs.body_statement_entries_total &&
         lhs.descriptor_symbolized_sites == rhs.descriptor_symbolized_sites &&
         lhs.invoke_trampoline_symbolized_sites ==
             rhs.invoke_trampoline_symbolized_sites &&
         lhs.missing_invoke_trampoline_sites ==
             rhs.missing_invoke_trampoline_sites &&
         lhs.non_normalized_layout_sites == rhs.non_normalized_layout_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentBlockStorageEscapeSemanticsSummary(
    const Objc3BlockStorageEscapeSemanticsSummary &lhs,
    const Objc3BlockStorageEscapeSemanticsSummary &rhs) {
  return lhs.block_literal_sites == rhs.block_literal_sites &&
         lhs.mutable_capture_count_total == rhs.mutable_capture_count_total &&
         lhs.byref_slot_count_total == rhs.byref_slot_count_total &&
         lhs.parameter_entries_total == rhs.parameter_entries_total &&
         lhs.capture_entries_total == rhs.capture_entries_total &&
         lhs.body_statement_entries_total == rhs.body_statement_entries_total &&
         lhs.requires_byref_cells_sites == rhs.requires_byref_cells_sites &&
         lhs.escape_analysis_enabled_sites == rhs.escape_analysis_enabled_sites &&
         lhs.escape_to_heap_sites == rhs.escape_to_heap_sites &&
         lhs.escape_profile_normalized_sites ==
             rhs.escape_profile_normalized_sites &&
         lhs.byref_layout_symbolized_sites == rhs.byref_layout_symbolized_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentBlockCopyDisposeSemanticsSummary(
    const Objc3BlockCopyDisposeSemanticsSummary &lhs,
    const Objc3BlockCopyDisposeSemanticsSummary &rhs) {
  return lhs.block_literal_sites == rhs.block_literal_sites &&
         lhs.mutable_capture_count_total == rhs.mutable_capture_count_total &&
         lhs.byref_slot_count_total == rhs.byref_slot_count_total &&
         lhs.parameter_entries_total == rhs.parameter_entries_total &&
         lhs.capture_entries_total == rhs.capture_entries_total &&
         lhs.body_statement_entries_total == rhs.body_statement_entries_total &&
         lhs.copy_helper_required_sites == rhs.copy_helper_required_sites &&
         lhs.dispose_helper_required_sites == rhs.dispose_helper_required_sites &&
         lhs.profile_normalized_sites == rhs.profile_normalized_sites &&
         lhs.copy_helper_symbolized_sites == rhs.copy_helper_symbolized_sites &&
         lhs.dispose_helper_symbolized_sites ==
             rhs.dispose_helper_symbolized_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentBlockDeterminismPerfBaselineSummary(
    const Objc3BlockDeterminismPerfBaselineSummary &lhs,
    const Objc3BlockDeterminismPerfBaselineSummary &rhs) {
  return lhs.block_literal_sites == rhs.block_literal_sites &&
         lhs.baseline_weight_total == rhs.baseline_weight_total &&
         lhs.parameter_entries_total == rhs.parameter_entries_total &&
         lhs.capture_entries_total == rhs.capture_entries_total &&
         lhs.body_statement_entries_total == rhs.body_statement_entries_total &&
         lhs.deterministic_capture_sites == rhs.deterministic_capture_sites &&
         lhs.heavy_tier_sites == rhs.heavy_tier_sites &&
         lhs.normalized_profile_sites == rhs.normalized_profile_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

static bool IsEquivalentBlockDeterminismPerfBaselineSiteMetadata(
    const Objc3BlockDeterminismPerfBaselineSiteMetadata &lhs,
    const Objc3BlockDeterminismPerfBaselineSiteMetadata &rhs) {
  return lhs.parameter_count == rhs.parameter_count &&
         lhs.capture_count == rhs.capture_count &&
         lhs.body_statement_count == rhs.body_statement_count &&
         lhs.baseline_weight == rhs.baseline_weight &&
         lhs.capture_set_deterministic == rhs.capture_set_deterministic &&
         lhs.baseline_profile_is_normalized == rhs.baseline_profile_is_normalized &&
         lhs.baseline_profile == rhs.baseline_profile && lhs.line == rhs.line &&
         lhs.column == rhs.column;
}

bool AreEquivalentBlockDeterminismPerfBaselineSites(
    const std::vector<Objc3BlockDeterminismPerfBaselineSiteMetadata> &lhs,
    const std::vector<Objc3BlockDeterminismPerfBaselineSiteMetadata> &rhs) {
  return lhs.size() == rhs.size() &&
         std::equal(lhs.begin(),
                    lhs.end(),
                    rhs.begin(),
                    IsEquivalentBlockDeterminismPerfBaselineSiteMetadata);
}
