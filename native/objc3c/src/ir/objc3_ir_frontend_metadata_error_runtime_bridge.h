#pragma once

#include <cstddef>
#include <string>

struct Objc3IRFrontendErrorRuntimeBridgeMetadata {
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
};
