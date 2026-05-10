#pragma once

#include <cstddef>
#include <string>

#include "ir/objc3_ir_frontend_metadata_error_propagation.h"
#include "ir/objc3_ir_frontend_metadata_error_runtime_bridge.h"

struct Objc3IRFrontendErrorHandlingMetadata
    : Objc3IRFrontendErrorPropagationMetadata,
      Objc3IRFrontendErrorRuntimeBridgeMetadata {
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
