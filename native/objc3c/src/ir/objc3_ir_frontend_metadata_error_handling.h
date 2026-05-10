#pragma once

#include <cstddef>
#include <string>

struct Objc3IRFrontendErrorHandlingMetadata {
  std::string lowering_error_handling_throws_abi_propagation_replay_key;
  std::string lowering_throws_propagation_replay_key;
  std::string lowering_result_like_replay_key;
  bool deterministic_result_like_lowering_handoff = false;
  std::size_t throws_propagation_lowering_sites = 0;
  std::size_t throws_propagation_lowering_namespace_segment_sites = 0;
  std::size_t throws_propagation_lowering_import_edge_candidate_sites = 0;
  std::size_t throws_propagation_lowering_object_pointer_type_sites = 0;
  std::size_t throws_propagation_lowering_pointer_declarator_sites = 0;
  std::size_t throws_propagation_lowering_normalized_sites = 0;
  std::size_t throws_propagation_lowering_cache_invalidation_candidate_sites = 0;
  std::size_t throws_propagation_lowering_contract_violation_sites = 0;
  bool deterministic_throws_propagation_lowering_handoff = false;
  std::string lowering_ns_error_bridging_replay_key;
  std::size_t ns_error_bridging_lowering_sites = 0;
  std::size_t ns_error_bridging_lowering_ns_error_parameter_sites = 0;
  std::size_t ns_error_bridging_lowering_ns_error_out_parameter_sites = 0;
  std::size_t ns_error_bridging_lowering_ns_error_bridge_path_sites = 0;
  std::size_t ns_error_bridging_lowering_failable_call_sites = 0;
  std::size_t ns_error_bridging_lowering_normalized_sites = 0;
  std::size_t ns_error_bridging_lowering_bridge_boundary_sites = 0;
  std::size_t ns_error_bridging_lowering_contract_violation_sites = 0;
  bool deterministic_ns_error_bridging_lowering_handoff = false;
  std::string lowering_unwind_cleanup_replay_key;
  std::size_t unwind_cleanup_lowering_sites = 0;
  std::size_t unwind_cleanup_lowering_unwind_edge_sites = 0;
  std::size_t unwind_cleanup_lowering_cleanup_scope_sites = 0;
  std::size_t unwind_cleanup_lowering_cleanup_emit_sites = 0;
  std::size_t unwind_cleanup_lowering_landing_pad_sites = 0;
  std::size_t unwind_cleanup_lowering_cleanup_resume_sites = 0;
  std::size_t unwind_cleanup_lowering_normalized_sites = 0;
  std::size_t unwind_cleanup_lowering_guard_blocked_sites = 0;
  std::size_t unwind_cleanup_lowering_contract_violation_sites = 0;
  bool deterministic_unwind_cleanup_lowering_handoff = false;
  std::string lowering_error_handling_result_and_bridging_artifact_replay_key;
  std::size_t imported_error_handling_result_and_bridging_artifact_modules = 0;
  bool error_handling_result_and_bridging_binary_artifact_replay_ready = false;
  bool error_handling_result_and_bridging_runtime_import_artifact_ready = false;
  bool error_handling_result_and_bridging_separate_compilation_replay_ready = false;
  bool deterministic_error_handling_result_and_bridging_artifact_replay_handoff = false;
  std::string lowering_error_diagnostics_recovery_replay_key;
  std::size_t error_diagnostics_recovery_lowering_sites = 0;
  std::size_t error_diagnostics_recovery_lowering_parser_diagnostic_sites = 0;
  std::size_t error_diagnostics_recovery_lowering_semantic_diagnostic_sites = 0;
  std::size_t error_diagnostics_recovery_lowering_fixit_hint_sites = 0;
  std::size_t error_diagnostics_recovery_lowering_recovery_candidate_sites = 0;
  std::size_t error_diagnostics_recovery_lowering_recovery_applied_sites = 0;
  std::size_t error_diagnostics_recovery_lowering_normalized_sites = 0;
  std::size_t error_diagnostics_recovery_lowering_guard_blocked_sites = 0;
  std::size_t error_diagnostics_recovery_lowering_contract_violation_sites = 0;
  bool deterministic_error_diagnostics_recovery_lowering_handoff = false;
};
