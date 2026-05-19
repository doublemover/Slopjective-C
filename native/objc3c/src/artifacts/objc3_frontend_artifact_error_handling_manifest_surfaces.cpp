#include "artifacts/objc3_frontend_artifact_error_handling_manifest_surfaces.h"

#include <ostream>

#include "artifacts/evidence/error_handling_replay_evidence.h"
#include "artifacts/objc3_frontend_error_semantic_artifacts.h"
#include "lower/contracts/error_handling_result_bridging_contracts.h"
#include "lower/contracts/error_handling_throws_unwind_contracts.h"

namespace objc3::artifacts::frontend {

void WriteErrorHandlingManifestSurfaces(
    std::ostream &manifest,
    const Objc3ThrowsPropagationLoweringContract
        &throws_propagation_lowering_contract,
    const std::string &throws_propagation_lowering_replay_key,
    const Objc3ResultLikeLoweringContract &result_like_lowering_contract,
    const std::string &result_like_lowering_replay_key,
    const Objc3NSErrorBridgingLoweringContract
        &ns_error_bridging_lowering_contract,
    const std::string &ns_error_bridging_lowering_replay_key,
    const Objc3UnwindCleanupLoweringContract &unwind_cleanup_lowering_contract,
    const std::string &unwind_cleanup_lowering_replay_key,
    bool deterministic_error_handling_throws_abi_propagation_lowering,
    const objc3::artifacts::evidence::
        ErrorHandlingResultAndBridgingArtifactReplayEvidence
            &error_handling_result_and_bridging_artifact_replay_summary) {
  manifest
      << ",\"objc_throws_propagation_lowering_surface\":{\"throws_propagation_sites\":"
      << throws_propagation_lowering_contract.throws_propagation_sites
      << ",\"namespace_segment_sites\":"
      << throws_propagation_lowering_contract.namespace_segment_sites
      << ",\"import_edge_candidate_sites\":"
      << throws_propagation_lowering_contract.import_edge_candidate_sites
      << ",\"object_pointer_type_sites\":"
      << throws_propagation_lowering_contract.object_pointer_type_sites
      << ",\"pointer_declarator_sites\":"
      << throws_propagation_lowering_contract.pointer_declarator_sites
      << ",\"normalized_sites\":"
      << throws_propagation_lowering_contract.normalized_sites
      << ",\"cache_invalidation_candidate_sites\":"
      << throws_propagation_lowering_contract
             .cache_invalidation_candidate_sites
      << ",\"contract_violation_sites\":"
      << throws_propagation_lowering_contract.contract_violation_sites
      << ",\"replay_key\":\""
      << throws_propagation_lowering_replay_key
      << "\",\"deterministic_handoff\":"
      << (throws_propagation_lowering_contract.deterministic ? "true"
                                                             : "false")
      << "}"
      << ",\"objc_result_like_lowering_surface\":{\"result_like_sites\":"
      << result_like_lowering_contract.result_like_sites
      << ",\"result_success_sites\":"
      << result_like_lowering_contract.result_success_sites
      << ",\"result_failure_sites\":"
      << result_like_lowering_contract.result_failure_sites
      << ",\"result_branch_sites\":"
      << result_like_lowering_contract.result_branch_sites
      << ",\"result_payload_sites\":"
      << result_like_lowering_contract.result_payload_sites
      << ",\"normalized_sites\":"
      << result_like_lowering_contract.normalized_sites
      << ",\"branch_merge_sites\":"
      << result_like_lowering_contract.branch_merge_sites
      << ",\"contract_violation_sites\":"
      << result_like_lowering_contract.contract_violation_sites
      << ",\"replay_key\":\""
      << result_like_lowering_replay_key
      << "\",\"deterministic_handoff\":"
      << (result_like_lowering_contract.deterministic ? "true" : "false")
      << "}"
      << ",\"objc_ns_error_bridging_lowering_surface\":{\"ns_error_bridging_sites\":"
      << ns_error_bridging_lowering_contract.ns_error_bridging_sites
      << ",\"ns_error_parameter_sites\":"
      << ns_error_bridging_lowering_contract.ns_error_parameter_sites
      << ",\"ns_error_out_parameter_sites\":"
      << ns_error_bridging_lowering_contract.ns_error_out_parameter_sites
      << ",\"ns_error_bridge_path_sites\":"
      << ns_error_bridging_lowering_contract.ns_error_bridge_path_sites
      << ",\"failable_call_sites\":"
      << ns_error_bridging_lowering_contract.failable_call_sites
      << ",\"normalized_sites\":"
      << ns_error_bridging_lowering_contract.normalized_sites
      << ",\"bridge_boundary_sites\":"
      << ns_error_bridging_lowering_contract.bridge_boundary_sites
      << ",\"contract_violation_sites\":"
      << ns_error_bridging_lowering_contract.contract_violation_sites
      << ",\"replay_key\":\""
      << ns_error_bridging_lowering_replay_key
      << "\",\"deterministic_handoff\":"
      << (ns_error_bridging_lowering_contract.deterministic ? "true"
                                                            : "false")
      << "}"
      << ",\"objc_unwind_cleanup_lowering_surface\":{\"unwind_cleanup_sites\":"
      << unwind_cleanup_lowering_contract.unwind_cleanup_sites
      << ",\"unwind_edge_sites\":"
      << unwind_cleanup_lowering_contract.unwind_edge_sites
      << ",\"cleanup_scope_sites\":"
      << unwind_cleanup_lowering_contract.cleanup_scope_sites
      << ",\"cleanup_emit_sites\":"
      << unwind_cleanup_lowering_contract.cleanup_emit_sites
      << ",\"landing_pad_sites\":"
      << unwind_cleanup_lowering_contract.landing_pad_sites
      << ",\"cleanup_resume_sites\":"
      << unwind_cleanup_lowering_contract.cleanup_resume_sites
      << ",\"normalized_sites\":"
      << unwind_cleanup_lowering_contract.normalized_sites
      << ",\"guard_blocked_sites\":"
      << unwind_cleanup_lowering_contract.guard_blocked_sites
      << ",\"contract_violation_sites\":"
      << unwind_cleanup_lowering_contract.contract_violation_sites
      << ",\"replay_key\":\""
      << unwind_cleanup_lowering_replay_key
      << "\",\"deterministic_handoff\":"
      << (unwind_cleanup_lowering_contract.deterministic ? "true" : "false")
      << "}"
      << ",\"objc_error_handling_throws_abi_propagation_lowering\":{\"contract_id\":\""
      << kObjc3ErrorHandlingThrowsAbiPropagationLoweringContractId
      << "\",\"source_model\":\""
      << kObjc3ErrorHandlingThrowsAbiPropagationLoweringSourceModel
      << "\",\"abi_model\":\""
      << kObjc3ErrorHandlingThrowsAbiPropagationLoweringAbiModel
      << "\",\"throws_replay_key\":\""
      << throws_propagation_lowering_replay_key
      << "\",\"result_like_replay_key\":\""
      << result_like_lowering_replay_key
      << "\",\"ns_error_replay_key\":\""
      << ns_error_bridging_lowering_replay_key
      << "\",\"unwind_replay_key\":\""
      << unwind_cleanup_lowering_replay_key
      << "\",\"deterministic_handoff\":"
      << (deterministic_error_handling_throws_abi_propagation_lowering
              ? "true"
              : "false")
      << ",\"ready_for_runtime_execution\":true"
      << ",\"fail_closed_model\":\""
      << kObjc3ErrorHandlingThrowsAbiPropagationLoweringFailClosedModel
      << "\",\"next_issue\":\"objc3c.errors.resultbridging.artifactsurface.v1\"}"
      << ",\"objc_error_handling_result_and_bridging_artifact_replay\":"
      << objc3::artifacts::evidence::
             BuildErrorHandlingResultAndBridgingArtifactReplayJson(
                 error_handling_result_and_bridging_artifact_replay_summary);
}

}  // namespace objc3::artifacts::frontend
