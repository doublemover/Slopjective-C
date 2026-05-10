#pragma once

#include <cstddef>
#include <string>

struct Objc3IRFrontendOwnershipMetadata {
  std::string lowering_ownership_qualifier_replay_key;
  std::size_t ownership_qualifier_lowering_ownership_qualifier_sites = 0;
  std::size_t ownership_qualifier_lowering_invalid_ownership_qualifier_sites = 0;
  std::size_t ownership_qualifier_lowering_object_pointer_type_annotation_sites = 0;
  bool deterministic_ownership_qualifier_lowering_handoff = false;
  std::string lowering_retain_release_operation_replay_key;
  std::size_t retain_release_operation_lowering_ownership_qualified_sites = 0;
  std::size_t retain_release_operation_lowering_retain_insertion_sites = 0;
  std::size_t retain_release_operation_lowering_release_insertion_sites = 0;
  std::size_t retain_release_operation_lowering_autorelease_insertion_sites = 0;
  std::size_t retain_release_operation_lowering_contract_violation_sites = 0;
  bool deterministic_retain_release_operation_lowering_handoff = false;
  std::string lowering_autoreleasepool_scope_replay_key;
  std::size_t autoreleasepool_scope_lowering_scope_sites = 0;
  std::size_t autoreleasepool_scope_lowering_scope_symbolized_sites = 0;
  unsigned autoreleasepool_scope_lowering_max_scope_depth = 0;
  std::size_t autoreleasepool_scope_lowering_scope_entry_transition_sites = 0;
  std::size_t autoreleasepool_scope_lowering_scope_exit_transition_sites = 0;
  std::size_t autoreleasepool_scope_lowering_contract_violation_sites = 0;
  bool deterministic_autoreleasepool_scope_lowering_handoff = false;
  std::string lowering_weak_unowned_semantics_replay_key;
  std::size_t weak_unowned_semantics_lowering_ownership_candidate_sites = 0;
  std::size_t weak_unowned_semantics_lowering_weak_reference_sites = 0;
  std::size_t weak_unowned_semantics_lowering_unowned_reference_sites = 0;
  std::size_t weak_unowned_semantics_lowering_unowned_safe_reference_sites = 0;
  std::size_t weak_unowned_semantics_lowering_conflict_sites = 0;
  std::size_t weak_unowned_semantics_lowering_contract_violation_sites = 0;
  bool deterministic_weak_unowned_semantics_lowering_handoff = false;
  std::string lowering_arc_diagnostics_fixit_replay_key;
  std::size_t arc_diagnostics_fixit_lowering_ownership_arc_diagnostic_candidate_sites = 0;
  std::size_t arc_diagnostics_fixit_lowering_ownership_arc_fixit_available_sites = 0;
  std::size_t arc_diagnostics_fixit_lowering_ownership_arc_profiled_sites = 0;
  std::size_t arc_diagnostics_fixit_lowering_ownership_arc_weak_unowned_conflict_diagnostic_sites = 0;
  std::size_t arc_diagnostics_fixit_lowering_ownership_arc_empty_fixit_hint_sites = 0;
  std::size_t arc_diagnostics_fixit_lowering_contract_violation_sites = 0;
  bool deterministic_arc_diagnostics_fixit_lowering_handoff = false;
};
