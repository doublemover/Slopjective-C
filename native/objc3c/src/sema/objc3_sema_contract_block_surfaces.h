#pragma once

#include <cstddef>
#include <string>

struct Objc3BlockLiteralCaptureSiteMetadata {
  std::size_t parameter_count = 0;
  std::size_t capture_count = 0;
  std::size_t body_statement_count = 0;
  bool capture_set_deterministic = false;
  bool literal_is_normalized = false;
  bool has_count_mismatch = false;
  std::string capture_profile;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3BlockLiteralCaptureSemanticsSummary {
  std::size_t block_literal_sites = 0;
  std::size_t block_parameter_entries = 0;
  std::size_t block_capture_entries = 0;
  std::size_t block_body_statement_entries = 0;
  std::size_t block_empty_capture_sites = 0;
  std::size_t block_nondeterministic_capture_sites = 0;
  std::size_t block_non_normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockAbiInvokeTrampolineSiteMetadata {
  std::size_t invoke_argument_slots = 0;
  std::size_t capture_word_count = 0;
  std::size_t parameter_count = 0;
  std::size_t capture_count = 0;
  std::size_t body_statement_count = 0;
  bool has_invoke_trampoline = false;
  bool layout_is_normalized = false;
  bool has_count_mismatch = false;
  std::string layout_profile;
  std::string descriptor_symbol;
  std::string invoke_trampoline_symbol;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3BlockAbiInvokeTrampolineSemanticsSummary {
  std::size_t block_literal_sites = 0;
  std::size_t invoke_argument_slots_total = 0;
  std::size_t capture_word_count_total = 0;
  std::size_t parameter_entries_total = 0;
  std::size_t capture_entries_total = 0;
  std::size_t body_statement_entries_total = 0;
  std::size_t descriptor_symbolized_sites = 0;
  std::size_t invoke_trampoline_symbolized_sites = 0;
  std::size_t missing_invoke_trampoline_sites = 0;
  std::size_t non_normalized_layout_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockStorageEscapeSiteMetadata {
  std::size_t mutable_capture_count = 0;
  std::size_t byref_slot_count = 0;
  std::size_t parameter_count = 0;
  std::size_t capture_count = 0;
  std::size_t body_statement_count = 0;
  bool requires_byref_cells = false;
  bool escape_analysis_enabled = false;
  bool escape_to_heap = false;
  bool escape_profile_is_normalized = false;
  bool has_count_mismatch = false;
  std::string escape_profile;
  std::string byref_layout_symbol;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3BlockStorageEscapeSemanticsSummary {
  std::size_t block_literal_sites = 0;
  std::size_t mutable_capture_count_total = 0;
  std::size_t byref_slot_count_total = 0;
  std::size_t parameter_entries_total = 0;
  std::size_t capture_entries_total = 0;
  std::size_t body_statement_entries_total = 0;
  std::size_t requires_byref_cells_sites = 0;
  std::size_t escape_analysis_enabled_sites = 0;
  std::size_t escape_to_heap_sites = 0;
  std::size_t escape_profile_normalized_sites = 0;
  std::size_t byref_layout_symbolized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockCopyDisposeSiteMetadata {
  std::size_t mutable_capture_count = 0;
  std::size_t byref_slot_count = 0;
  std::size_t parameter_count = 0;
  std::size_t capture_count = 0;
  std::size_t body_statement_count = 0;
  std::size_t owned_object_capture_count = 0;
  std::size_t weak_object_capture_count = 0;
  std::size_t unowned_object_capture_count = 0;
  std::size_t explicit_move_capture_count = 0;
  bool copy_helper_required = false;
  bool dispose_helper_required = false;
  bool copy_dispose_profile_is_normalized = false;
  bool ownership_helper_eligibility_is_normalized = false;
  bool has_count_mismatch = false;
  std::string copy_dispose_profile;
  std::string copy_helper_symbol;
  std::string dispose_helper_symbol;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3BlockCopyDisposeSemanticsSummary {
  std::size_t block_literal_sites = 0;
  std::size_t mutable_capture_count_total = 0;
  std::size_t byref_slot_count_total = 0;
  std::size_t parameter_entries_total = 0;
  std::size_t capture_entries_total = 0;
  std::size_t body_statement_entries_total = 0;
  std::size_t copy_helper_required_sites = 0;
  std::size_t dispose_helper_required_sites = 0;
  std::size_t profile_normalized_sites = 0;
  std::size_t copy_helper_symbolized_sites = 0;
  std::size_t dispose_helper_symbolized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockDeterminismPerfBaselineSiteMetadata {
  std::size_t parameter_count = 0;
  std::size_t capture_count = 0;
  std::size_t body_statement_count = 0;
  std::size_t baseline_weight = 0;
  bool capture_set_deterministic = false;
  bool baseline_profile_is_normalized = false;
  std::string baseline_profile;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3BlockDeterminismPerfBaselineSummary {
  std::size_t block_literal_sites = 0;
  std::size_t baseline_weight_total = 0;
  std::size_t parameter_entries_total = 0;
  std::size_t capture_entries_total = 0;
  std::size_t body_statement_entries_total = 0;
  std::size_t deterministic_capture_sites = 0;
  std::size_t heavy_tier_sites = 0;
  std::size_t normalized_profile_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};
