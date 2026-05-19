#pragma once

#include <cstddef>
#include <string>

struct Objc3IRFrontendBlockExecutionMetadata {
  std::string lowering_block_literal_capture_replay_key;
  std::size_t block_literal_capture_lowering_block_literal_sites = 0;
  std::size_t block_literal_capture_lowering_block_parameter_entries = 0;
  std::size_t block_literal_capture_lowering_block_capture_entries = 0;
  std::size_t block_literal_capture_lowering_block_body_statement_entries = 0;
  std::size_t block_literal_capture_lowering_block_empty_capture_sites = 0;
  std::size_t
      block_literal_capture_lowering_block_nondeterministic_capture_sites = 0;
  std::size_t block_literal_capture_lowering_block_non_normalized_sites = 0;
  std::size_t block_literal_capture_lowering_contract_violation_sites = 0;
  bool deterministic_block_literal_capture_lowering_handoff = false;
  std::string lowering_block_abi_invoke_trampoline_replay_key;
  std::size_t block_abi_invoke_trampoline_lowering_block_literal_sites = 0;
  std::size_t
      block_abi_invoke_trampoline_lowering_invoke_argument_slots_total = 0;
  std::size_t block_abi_invoke_trampoline_lowering_capture_word_count_total = 0;
  std::size_t block_abi_invoke_trampoline_lowering_parameter_entries_total = 0;
  std::size_t block_abi_invoke_trampoline_lowering_capture_entries_total = 0;
  std::size_t
      block_abi_invoke_trampoline_lowering_body_statement_entries_total = 0;
  std::size_t
      block_abi_invoke_trampoline_lowering_descriptor_symbolized_sites = 0;
  std::size_t block_abi_invoke_trampoline_lowering_invoke_symbolized_sites = 0;
  std::size_t block_abi_invoke_trampoline_lowering_missing_invoke_sites = 0;
  std::size_t
      block_abi_invoke_trampoline_lowering_non_normalized_layout_sites = 0;
  std::size_t block_abi_invoke_trampoline_lowering_contract_violation_sites = 0;
  bool deterministic_block_abi_invoke_trampoline_lowering_handoff = false;
  std::string lowering_block_storage_escape_replay_key;
  std::size_t block_storage_escape_lowering_block_literal_sites = 0;
  std::size_t block_storage_escape_lowering_mutable_capture_count_total = 0;
  std::size_t block_storage_escape_lowering_byref_slot_count_total = 0;
  std::size_t block_storage_escape_lowering_parameter_entries_total = 0;
  std::size_t block_storage_escape_lowering_capture_entries_total = 0;
  std::size_t block_storage_escape_lowering_body_statement_entries_total = 0;
  std::size_t block_storage_escape_lowering_requires_byref_cells_sites = 0;
  std::size_t block_storage_escape_lowering_escape_analysis_enabled_sites = 0;
  std::size_t block_storage_escape_lowering_escape_to_heap_sites = 0;
  std::size_t block_storage_escape_lowering_escape_profile_normalized_sites = 0;
  std::size_t block_storage_escape_lowering_byref_layout_symbolized_sites = 0;
  std::size_t block_storage_escape_lowering_contract_violation_sites = 0;
  bool deterministic_block_storage_escape_lowering_handoff = false;
  std::string lowering_block_copy_dispose_replay_key;
  std::size_t block_copy_dispose_lowering_block_literal_sites = 0;
  std::size_t block_copy_dispose_lowering_mutable_capture_count_total = 0;
  std::size_t block_copy_dispose_lowering_byref_slot_count_total = 0;
  std::size_t block_copy_dispose_lowering_parameter_entries_total = 0;
  std::size_t block_copy_dispose_lowering_capture_entries_total = 0;
  std::size_t block_copy_dispose_lowering_body_statement_entries_total = 0;
  std::size_t block_copy_dispose_lowering_copy_helper_required_sites = 0;
  std::size_t block_copy_dispose_lowering_dispose_helper_required_sites = 0;
  std::size_t block_copy_dispose_lowering_profile_normalized_sites = 0;
  std::size_t block_copy_dispose_lowering_copy_helper_symbolized_sites = 0;
  std::size_t block_copy_dispose_lowering_dispose_helper_symbolized_sites = 0;
  std::size_t block_copy_dispose_lowering_contract_violation_sites = 0;
  bool deterministic_block_copy_dispose_lowering_handoff = false;
  std::string lowering_block_determinism_perf_baseline_replay_key;
  std::size_t
      block_determinism_perf_baseline_lowering_block_literal_sites = 0;
  std::size_t
      block_determinism_perf_baseline_lowering_baseline_weight_total = 0;
  std::size_t
      block_determinism_perf_baseline_lowering_parameter_entries_total = 0;
  std::size_t
      block_determinism_perf_baseline_lowering_capture_entries_total = 0;
  std::size_t
      block_determinism_perf_baseline_lowering_body_statement_entries_total = 0;
  std::size_t
      block_determinism_perf_baseline_lowering_deterministic_capture_sites = 0;
  std::size_t block_determinism_perf_baseline_lowering_heavy_tier_sites = 0;
  std::size_t
      block_determinism_perf_baseline_lowering_normalized_profile_sites = 0;
  std::size_t
      block_determinism_perf_baseline_lowering_contract_violation_sites = 0;
  bool deterministic_block_determinism_perf_baseline_lowering_handoff = false;
};
