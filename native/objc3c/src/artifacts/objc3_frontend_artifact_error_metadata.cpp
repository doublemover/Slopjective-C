#include "artifacts/objc3_frontend_artifact_error_metadata.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendErrorMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const std::string &throws_propagation_lowering_replay_key,
    const Objc3ThrowsPropagationLoweringContract
        &throws_propagation_lowering_contract,
    const std::string
        &error_handling_throws_abi_propagation_lowering_replay_key,
    const std::string &result_like_lowering_replay_key,
    const Objc3ResultLikeLoweringContract &result_like_lowering_contract,
    const std::string &ns_error_bridging_lowering_replay_key,
    const Objc3NSErrorBridgingLoweringContract
        &ns_error_bridging_lowering_contract,
    const std::string &unwind_cleanup_lowering_replay_key,
    const Objc3UnwindCleanupLoweringContract &unwind_cleanup_lowering_contract,
    const objc3::artifacts::evidence::
        ErrorHandlingResultAndBridgingArtifactReplayEvidence
            &error_handling_result_and_bridging_artifact_replay_summary) {
  ir_frontend_metadata.lowering_throws_propagation_replay_key =
      throws_propagation_lowering_replay_key;
  ir_frontend_metadata
      .lowering_error_handling_throws_abi_propagation_replay_key =
      error_handling_throws_abi_propagation_lowering_replay_key;
  ir_frontend_metadata.lowering_result_like_replay_key =
      result_like_lowering_replay_key;
  ir_frontend_metadata.deterministic_result_like_lowering_handoff =
      result_like_lowering_contract.deterministic;

  ir_frontend_metadata.throws_propagation_lowering_sites =
      throws_propagation_lowering_contract.throws_propagation_sites;
  ir_frontend_metadata.throws_propagation_lowering_namespace_segment_sites =
      throws_propagation_lowering_contract.namespace_segment_sites;
  ir_frontend_metadata.throws_propagation_lowering_import_edge_candidate_sites =
      throws_propagation_lowering_contract.import_edge_candidate_sites;
  ir_frontend_metadata.throws_propagation_lowering_object_pointer_type_sites =
      throws_propagation_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata.throws_propagation_lowering_pointer_declarator_sites =
      throws_propagation_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.throws_propagation_lowering_normalized_sites =
      throws_propagation_lowering_contract.normalized_sites;
  ir_frontend_metadata
      .throws_propagation_lowering_cache_invalidation_candidate_sites =
      throws_propagation_lowering_contract
          .cache_invalidation_candidate_sites;
  ir_frontend_metadata.throws_propagation_lowering_contract_violation_sites =
      throws_propagation_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_throws_propagation_lowering_handoff =
      throws_propagation_lowering_contract.deterministic;

  ir_frontend_metadata.lowering_ns_error_bridging_replay_key =
      ns_error_bridging_lowering_replay_key;
  ir_frontend_metadata.ns_error_bridging_lowering_sites =
      ns_error_bridging_lowering_contract.ns_error_bridging_sites;
  ir_frontend_metadata.ns_error_bridging_lowering_ns_error_parameter_sites =
      ns_error_bridging_lowering_contract.ns_error_parameter_sites;
  ir_frontend_metadata.ns_error_bridging_lowering_ns_error_out_parameter_sites =
      ns_error_bridging_lowering_contract.ns_error_out_parameter_sites;
  ir_frontend_metadata.ns_error_bridging_lowering_ns_error_bridge_path_sites =
      ns_error_bridging_lowering_contract.ns_error_bridge_path_sites;
  ir_frontend_metadata.ns_error_bridging_lowering_failable_call_sites =
      ns_error_bridging_lowering_contract.failable_call_sites;
  ir_frontend_metadata.ns_error_bridging_lowering_normalized_sites =
      ns_error_bridging_lowering_contract.normalized_sites;
  ir_frontend_metadata.ns_error_bridging_lowering_bridge_boundary_sites =
      ns_error_bridging_lowering_contract.bridge_boundary_sites;
  ir_frontend_metadata.ns_error_bridging_lowering_contract_violation_sites =
      ns_error_bridging_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_ns_error_bridging_lowering_handoff =
      ns_error_bridging_lowering_contract.deterministic;

  ir_frontend_metadata.lowering_unwind_cleanup_replay_key =
      unwind_cleanup_lowering_replay_key;
  ir_frontend_metadata.unwind_cleanup_lowering_sites =
      unwind_cleanup_lowering_contract.unwind_cleanup_sites;
  ir_frontend_metadata.unwind_cleanup_lowering_unwind_edge_sites =
      unwind_cleanup_lowering_contract.unwind_edge_sites;
  ir_frontend_metadata.unwind_cleanup_lowering_cleanup_scope_sites =
      unwind_cleanup_lowering_contract.cleanup_scope_sites;
  ir_frontend_metadata.unwind_cleanup_lowering_cleanup_emit_sites =
      unwind_cleanup_lowering_contract.cleanup_emit_sites;
  ir_frontend_metadata.unwind_cleanup_lowering_landing_pad_sites =
      unwind_cleanup_lowering_contract.landing_pad_sites;
  ir_frontend_metadata.unwind_cleanup_lowering_cleanup_resume_sites =
      unwind_cleanup_lowering_contract.cleanup_resume_sites;
  ir_frontend_metadata.unwind_cleanup_lowering_normalized_sites =
      unwind_cleanup_lowering_contract.normalized_sites;
  ir_frontend_metadata.unwind_cleanup_lowering_guard_blocked_sites =
      unwind_cleanup_lowering_contract.guard_blocked_sites;
  ir_frontend_metadata.unwind_cleanup_lowering_contract_violation_sites =
      unwind_cleanup_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_unwind_cleanup_lowering_handoff =
      unwind_cleanup_lowering_contract.deterministic;

  ir_frontend_metadata
      .lowering_error_handling_result_and_bridging_artifact_replay_key =
      error_handling_result_and_bridging_artifact_replay_summary.replay_key;
  ir_frontend_metadata
      .imported_error_handling_result_and_bridging_artifact_modules =
      error_handling_result_and_bridging_artifact_replay_summary
          .imported_module_names_lexicographic.size();
  ir_frontend_metadata
      .error_handling_result_and_bridging_binary_artifact_replay_ready =
      error_handling_result_and_bridging_artifact_replay_summary
          .binary_artifact_replay_ready;
  ir_frontend_metadata
      .error_handling_result_and_bridging_runtime_import_artifact_ready =
      error_handling_result_and_bridging_artifact_replay_summary
          .runtime_import_artifact_ready;
  ir_frontend_metadata
      .error_handling_result_and_bridging_separate_compilation_replay_ready =
      error_handling_result_and_bridging_artifact_replay_summary
          .separate_compilation_replay_ready;
  ir_frontend_metadata
      .deterministic_error_handling_result_and_bridging_artifact_replay_handoff =
      error_handling_result_and_bridging_artifact_replay_summary.deterministic;
}

}  // namespace objc3::artifacts::frontend
