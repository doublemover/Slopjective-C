#include "lower/objc3_lowering_contract.h"

#include "lower/metadata/lowering_metadata_helpers.h"

#include <string>

bool IsValidObjc3ThrowsPropagationLoweringContract(
    const Objc3ThrowsPropagationLoweringContract &contract) {
  if (contract.namespace_segment_sites > contract.throws_propagation_sites ||
      contract.import_edge_candidate_sites > contract.throws_propagation_sites ||
      contract.object_pointer_type_sites < contract.import_edge_candidate_sites ||
      contract.pointer_declarator_sites > contract.throws_propagation_sites ||
      contract.normalized_sites > contract.throws_propagation_sites ||
      contract.cache_invalidation_candidate_sites >
          contract.throws_propagation_sites ||
      contract.contract_violation_sites > contract.throws_propagation_sites) {
    return false;
  }
  if (contract.normalized_sites +
          contract.cache_invalidation_candidate_sites >
      contract.throws_propagation_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites != contract.throws_propagation_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3ThrowsPropagationLoweringReplayKey(
    const Objc3ThrowsPropagationLoweringContract &contract) {
  return std::string("throws_propagation_sites=") +
             std::to_string(contract.throws_propagation_sites) +
         ";namespace_segment_sites=" +
         std::to_string(contract.namespace_segment_sites) +
         ";import_edge_candidate_sites=" +
         std::to_string(contract.import_edge_candidate_sites) +
         ";object_pointer_type_sites=" +
         std::to_string(contract.object_pointer_type_sites) +
         ";pointer_declarator_sites=" +
         std::to_string(contract.pointer_declarator_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";cache_invalidation_candidate_sites=" +
         std::to_string(contract.cache_invalidation_candidate_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3ThrowsPropagationLoweringLaneContract;
}

bool IsValidObjc3ResultLikeLoweringContract(
    const Objc3ResultLikeLoweringContract &contract) {
  if (contract.result_success_sites > contract.result_like_sites ||
      contract.result_failure_sites > contract.result_like_sites ||
      contract.result_branch_sites > contract.result_like_sites ||
      contract.result_payload_sites > contract.result_like_sites ||
      contract.normalized_sites > contract.result_like_sites ||
      contract.branch_merge_sites > contract.result_like_sites ||
      contract.contract_violation_sites > contract.result_like_sites) {
    return false;
  }
  if (contract.normalized_sites + contract.branch_merge_sites !=
      contract.result_like_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3ResultLikeLoweringReplayKey(
    const Objc3ResultLikeLoweringContract &contract) {
  return std::string("result_like_sites=") +
             std::to_string(contract.result_like_sites) +
         ";result_success_sites=" +
         std::to_string(contract.result_success_sites) +
         ";result_failure_sites=" +
         std::to_string(contract.result_failure_sites) +
         ";result_branch_sites=" +
         std::to_string(contract.result_branch_sites) +
         ";result_payload_sites=" +
         std::to_string(contract.result_payload_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";branch_merge_sites=" + std::to_string(contract.branch_merge_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3ResultLikeLoweringLaneContract;
}

bool IsValidObjc3NSErrorBridgingLoweringContract(
    const Objc3NSErrorBridgingLoweringContract &contract) {
  if (contract.ns_error_parameter_sites > contract.ns_error_bridging_sites ||
      contract.ns_error_out_parameter_sites > contract.ns_error_parameter_sites ||
      contract.ns_error_bridge_path_sites > contract.ns_error_out_parameter_sites ||
      contract.ns_error_bridge_path_sites > contract.failable_call_sites ||
      contract.failable_call_sites > contract.ns_error_bridging_sites ||
      contract.normalized_sites > contract.ns_error_bridging_sites ||
      contract.bridge_boundary_sites > contract.ns_error_bridging_sites ||
      contract.contract_violation_sites > contract.ns_error_bridging_sites) {
    return false;
  }
  if (contract.normalized_sites + contract.bridge_boundary_sites !=
      contract.ns_error_bridging_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3NSErrorBridgingLoweringReplayKey(
    const Objc3NSErrorBridgingLoweringContract &contract) {
  return std::string("ns_error_bridging_sites=") +
             std::to_string(contract.ns_error_bridging_sites) +
         ";ns_error_parameter_sites=" +
         std::to_string(contract.ns_error_parameter_sites) +
         ";ns_error_out_parameter_sites=" +
         std::to_string(contract.ns_error_out_parameter_sites) +
         ";ns_error_bridge_path_sites=" +
         std::to_string(contract.ns_error_bridge_path_sites) +
         ";failable_call_sites=" +
         std::to_string(contract.failable_call_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";bridge_boundary_sites=" + std::to_string(contract.bridge_boundary_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3NSErrorBridgingLoweringLaneContract;
}

bool IsValidObjc3UnwindCleanupLoweringContract(
    const Objc3UnwindCleanupLoweringContract &contract) {
  if (contract.unwind_edge_sites > contract.unwind_cleanup_sites ||
      contract.cleanup_scope_sites > contract.unwind_cleanup_sites ||
      contract.cleanup_emit_sites > contract.cleanup_scope_sites ||
      contract.landing_pad_sites > contract.unwind_cleanup_sites ||
      contract.cleanup_resume_sites > contract.unwind_cleanup_sites ||
      contract.normalized_sites > contract.unwind_cleanup_sites ||
      contract.guard_blocked_sites > contract.unwind_cleanup_sites ||
      contract.contract_violation_sites > contract.unwind_cleanup_sites) {
    return false;
  }
  if (contract.landing_pad_sites + contract.cleanup_resume_sites >
      contract.unwind_cleanup_sites) {
    return false;
  }
  if (contract.normalized_sites + contract.guard_blocked_sites !=
      contract.unwind_cleanup_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3UnwindCleanupLoweringReplayKey(
    const Objc3UnwindCleanupLoweringContract &contract) {
  return std::string("unwind_cleanup_sites=") +
             std::to_string(contract.unwind_cleanup_sites) +
         ";unwind_edge_sites=" + std::to_string(contract.unwind_edge_sites) +
         ";cleanup_scope_sites=" +
         std::to_string(contract.cleanup_scope_sites) +
         ";cleanup_emit_sites=" + std::to_string(contract.cleanup_emit_sites) +
         ";landing_pad_sites=" + std::to_string(contract.landing_pad_sites) +
         ";cleanup_resume_sites=" +
         std::to_string(contract.cleanup_resume_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";guard_blocked_sites=" +
         std::to_string(contract.guard_blocked_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3UnwindCleanupLoweringLaneContract;
}
