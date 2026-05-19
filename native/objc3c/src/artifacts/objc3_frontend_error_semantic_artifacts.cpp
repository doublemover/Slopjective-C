#include "artifacts/objc3_frontend_error_semantic_artifacts.h"

#include <algorithm>
#include <cstddef>

#include "artifacts/objc3_frontend_artifact_semantic_contract_records.h"
#include "lower/contracts/error_handling_result_bridging_contracts.h"
#include "lower/contracts/error_handling_throws_unwind_contracts.h"
#include "pipeline/objc3_frontend_types.h"
#include "sema/objc3_sema_parity_contract_surface.h"

namespace objc3::artifacts::frontend {

Objc3ThrowsPropagationLoweringContract
BuildThrowsPropagationLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3ThrowsPropagationLoweringContract contract;
  const std::size_t raw_sites =
      sema_parity_surface.throws_propagation_sites_total;
  const std::size_t raw_namespace_segment_sites =
      sema_parity_surface.throws_propagation_namespace_segment_sites_total;
  const std::size_t raw_import_edge_sites =
      sema_parity_surface.throws_propagation_import_edge_candidate_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface.throws_propagation_object_pointer_type_sites_total;
  const std::size_t raw_pointer_declarator_sites =
      sema_parity_surface.throws_propagation_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.throws_propagation_normalized_sites_total;
  const std::size_t raw_cache_candidate_sites =
      sema_parity_surface
          .throws_propagation_cache_invalidation_candidate_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.throws_propagation_contract_violation_sites_total;

  contract.throws_propagation_sites =
      std::max({raw_sites, raw_namespace_segment_sites, raw_import_edge_sites,
                raw_pointer_declarator_sites, raw_normalized_sites,
                raw_cache_candidate_sites, raw_violation_sites});
  contract.namespace_segment_sites =
      std::min(raw_namespace_segment_sites, contract.throws_propagation_sites);
  contract.import_edge_candidate_sites =
      std::min(raw_import_edge_sites, contract.throws_propagation_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_pointer_sites, contract.import_edge_candidate_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_declarator_sites, contract.throws_propagation_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.throws_propagation_sites);
  const std::size_t normalized_budget =
      (contract.throws_propagation_sites >= contract.normalized_sites)
          ? (contract.throws_propagation_sites - contract.normalized_sites)
          : 0;
  contract.cache_invalidation_candidate_sites =
      std::min(raw_cache_candidate_sites, normalized_budget);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.throws_propagation_sites);
  contract.deterministic =
      sema_parity_surface.throws_propagation_summary.deterministic &&
      sema_parity_surface.deterministic_throws_propagation_handoff &&
      contract.contract_violation_sites == 0 &&
      contract.normalized_sites == contract.throws_propagation_sites;
  return contract;
}

Objc3ResultLikeLoweringContract BuildResultLikeLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3ResultLikeLoweringContract contract;
  const std::size_t raw_sites =
      sema_parity_surface.result_like_lowering_sites_total;
  const std::size_t raw_success_sites =
      sema_parity_surface.result_like_lowering_result_success_sites_total;
  const std::size_t raw_failure_sites =
      sema_parity_surface.result_like_lowering_result_failure_sites_total;
  const std::size_t raw_branch_sites =
      sema_parity_surface.result_like_lowering_result_branch_sites_total;
  const std::size_t raw_payload_sites =
      sema_parity_surface.result_like_lowering_result_payload_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.result_like_lowering_normalized_sites_total;
  const std::size_t raw_branch_merge_sites =
      sema_parity_surface.result_like_lowering_branch_merge_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.result_like_lowering_contract_violation_sites_total;

  contract.result_like_sites =
      std::max({raw_sites, raw_success_sites, raw_failure_sites,
                raw_branch_sites, raw_payload_sites, raw_normalized_sites,
                raw_branch_merge_sites, raw_violation_sites});
  contract.result_success_sites =
      std::min(raw_success_sites, contract.result_like_sites);
  contract.result_failure_sites =
      std::min(raw_failure_sites, contract.result_like_sites);
  contract.result_branch_sites =
      std::min(raw_branch_sites, contract.result_like_sites);
  contract.result_payload_sites =
      std::min(raw_payload_sites, contract.result_like_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.result_like_sites);
  const std::size_t normalized_budget =
      (contract.result_like_sites >= contract.normalized_sites)
          ? (contract.result_like_sites - contract.normalized_sites)
          : 0;
  contract.branch_merge_sites =
      std::min(raw_branch_merge_sites, normalized_budget);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.result_like_sites);
  contract.deterministic =
      sema_parity_surface.deterministic_result_like_lowering_handoff &&
      contract.contract_violation_sites == 0 &&
      contract.normalized_sites + contract.branch_merge_sites ==
          contract.result_like_sites;
  return contract;
}

Objc3NSErrorBridgingLoweringContract BuildNSErrorBridgingLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3NSErrorBridgingLoweringContract contract;
  const std::size_t raw_sites =
      sema_parity_surface.ns_error_bridging_sites_total;
  const std::size_t raw_parameter_sites =
      sema_parity_surface.ns_error_bridging_ns_error_parameter_sites_total;
  const std::size_t raw_out_parameter_sites =
      sema_parity_surface.ns_error_bridging_ns_error_out_parameter_sites_total;
  const std::size_t raw_bridge_path_sites =
      sema_parity_surface.ns_error_bridging_ns_error_bridge_path_sites_total;
  const std::size_t raw_failable_call_sites =
      sema_parity_surface.ns_error_bridging_failable_call_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.ns_error_bridging_normalized_sites_total;
  const std::size_t raw_bridge_boundary_sites =
      sema_parity_surface.ns_error_bridging_bridge_boundary_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.ns_error_bridging_contract_violation_sites_total;

  contract.ns_error_bridging_sites =
      std::max({raw_sites, raw_parameter_sites, raw_out_parameter_sites,
                raw_bridge_path_sites, raw_failable_call_sites,
                raw_normalized_sites, raw_bridge_boundary_sites,
                raw_violation_sites});
  contract.ns_error_parameter_sites =
      std::min(raw_parameter_sites, contract.ns_error_bridging_sites);
  contract.ns_error_out_parameter_sites =
      std::min(raw_out_parameter_sites, contract.ns_error_parameter_sites);
  contract.ns_error_bridge_path_sites =
      std::min(raw_bridge_path_sites, contract.ns_error_out_parameter_sites);
  contract.failable_call_sites =
      std::min(raw_failable_call_sites, contract.ns_error_bridging_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.ns_error_bridging_sites);
  const std::size_t normalized_budget =
      (contract.ns_error_bridging_sites >= contract.normalized_sites)
          ? (contract.ns_error_bridging_sites - contract.normalized_sites)
          : 0;
  contract.bridge_boundary_sites =
      std::min(raw_bridge_boundary_sites, normalized_budget);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.ns_error_bridging_sites);
  contract.deterministic =
      sema_parity_surface.deterministic_ns_error_bridging_handoff &&
      contract.contract_violation_sites == 0 &&
      contract.normalized_sites + contract.bridge_boundary_sites ==
          contract.ns_error_bridging_sites;
  return contract;
}

Objc3UnwindCleanupLoweringContract BuildUnwindCleanupLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3UnwindCleanupLoweringContract contract;
  const std::size_t raw_sites =
      sema_parity_surface.unwind_cleanup_sites_total;
  const std::size_t raw_exceptional_exit_sites =
      sema_parity_surface.unwind_cleanup_exceptional_exit_sites_total;
  const std::size_t raw_action_sites =
      sema_parity_surface.unwind_cleanup_action_sites_total;
  const std::size_t raw_scope_sites =
      sema_parity_surface.unwind_cleanup_scope_sites_total;
  const std::size_t raw_resume_sites =
      sema_parity_surface.unwind_cleanup_resume_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.unwind_cleanup_normalized_sites_total;
  const std::size_t raw_fail_closed_sites =
      sema_parity_surface.unwind_cleanup_fail_closed_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.unwind_cleanup_contract_violation_sites_total;

  contract.unwind_cleanup_sites =
      std::max({raw_sites, raw_exceptional_exit_sites, raw_action_sites,
                raw_scope_sites, raw_resume_sites, raw_normalized_sites,
                raw_fail_closed_sites, raw_violation_sites});
  contract.unwind_edge_sites =
      std::min(raw_exceptional_exit_sites, contract.unwind_cleanup_sites);
  contract.cleanup_scope_sites =
      std::min(raw_scope_sites, contract.unwind_cleanup_sites);
  contract.cleanup_emit_sites =
      std::min(raw_action_sites, contract.cleanup_scope_sites);
  contract.landing_pad_sites = 0;
  contract.cleanup_resume_sites =
      std::min(raw_resume_sites, contract.unwind_cleanup_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.unwind_cleanup_sites);
  const std::size_t normalized_budget =
      (contract.unwind_cleanup_sites >= contract.normalized_sites)
          ? (contract.unwind_cleanup_sites - contract.normalized_sites)
          : 0;
  contract.guard_blocked_sites =
      std::min(raw_fail_closed_sites, normalized_budget);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.unwind_cleanup_sites);
  contract.deterministic =
      sema_parity_surface.deterministic_unwind_cleanup_handoff &&
      contract.contract_violation_sites == 0 &&
      contract.normalized_sites + contract.guard_blocked_sites ==
          contract.unwind_cleanup_sites;
  return contract;
}

}  // namespace objc3::artifacts::frontend
