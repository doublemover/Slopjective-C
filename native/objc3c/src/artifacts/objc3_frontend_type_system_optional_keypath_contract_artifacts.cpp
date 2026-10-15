#include "artifacts/objc3_frontend_type_system_contract_artifacts.h"

#include <string>

#include "artifacts/objc3_frontend_type_system_contract_artifact_replay_fields.h"

namespace objc3::artifacts::frontend {

Objc3FrontendTypeSystemOptionalKeypathLoweringContractRecord
BuildFrontendTypeSystemOptionalKeypathLoweringContract(
    const Objc3FrontendTypeSystemSemanticModelRecord &summary) {
  Objc3FrontendTypeSystemOptionalKeypathLoweringContractRecord contract;
  contract.optional_binding_sites = summary.optional_binding_sites;
  contract.optional_binding_clause_sites =
      summary.optional_binding_clause_sites;
  contract.optional_send_sites = summary.optional_send_sites;
  contract.nil_coalescing_sites = summary.nil_coalescing_sites;
  contract.typed_keypath_literal_sites = summary.typed_keypath_literal_sites;
  contract.typed_keypath_self_root_sites =
      summary.typed_keypath_self_root_sites;
  contract.typed_keypath_class_root_sites =
      summary.typed_keypath_class_root_sites;
  contract.live_optional_lowering_sites =
      summary.optional_binding_sites + summary.optional_send_sites +
      summary.nil_coalescing_sites;
  contract.single_evaluation_nil_short_circuit_sites =
      summary.optional_binding_clause_sites + summary.optional_send_sites +
      summary.nil_coalescing_sites;
  contract.live_typed_keypath_artifact_sites =
      summary.typed_keypath_literal_sites;
  contract.deferred_typed_keypath_sites = 0;
  contract.typed_keypath_descriptor_publication_sites =
      contract.live_typed_keypath_artifact_sites;
  contract.typed_keypath_source_map_evidence_sites =
      contract.live_typed_keypath_artifact_sites;
  contract.typed_keypath_runtime_handle_evidence_sites =
      contract.live_typed_keypath_artifact_sites;
  contract.contract_violation_sites =
      summary.optional_binding_contract_violation_sites +
      summary.optional_send_contract_violation_sites +
      summary.optional_flow_contract_violation_sites;
  contract.deterministic =
      summary.deterministic && contract.contract_violation_sites == 0 &&
      contract.live_typed_keypath_artifact_sites +
              contract.deferred_typed_keypath_sites ==
          contract.typed_keypath_literal_sites;
  return contract;
}

bool IsValidFrontendTypeSystemOptionalKeypathLoweringContract(
    const Objc3FrontendTypeSystemOptionalKeypathLoweringContractRecord
        &contract) {
  if (contract.optional_binding_clause_sites > contract.optional_binding_sites ||
      contract.typed_keypath_self_root_sites >
          contract.typed_keypath_literal_sites ||
      contract.typed_keypath_class_root_sites >
          contract.typed_keypath_literal_sites) {
    return false;
  }
  if (contract.live_optional_lowering_sites !=
      contract.optional_binding_sites + contract.optional_send_sites +
          contract.nil_coalescing_sites) {
    return false;
  }
  if (contract.single_evaluation_nil_short_circuit_sites !=
      contract.optional_binding_clause_sites + contract.optional_send_sites +
          contract.nil_coalescing_sites) {
    return false;
  }
  if (contract.live_typed_keypath_artifact_sites +
          contract.deferred_typed_keypath_sites !=
      contract.typed_keypath_literal_sites) {
    return false;
  }
  if (contract.typed_keypath_descriptor_publication_sites !=
          contract.live_typed_keypath_artifact_sites ||
      contract.typed_keypath_source_map_evidence_sites !=
          contract.live_typed_keypath_artifact_sites ||
      contract.typed_keypath_runtime_handle_evidence_sites !=
          contract.live_typed_keypath_artifact_sites) {
    return false;
  }
  if (contract.contract_violation_sites >
      contract.live_optional_lowering_sites +
          contract.live_typed_keypath_artifact_sites +
          contract.deferred_typed_keypath_sites) {
    return false;
  }
  return contract.contract_violation_sites == 0 || !contract.deterministic;
}

std::string FrontendTypeSystemOptionalKeypathLoweringReplayKey(
    const Objc3FrontendTypeSystemOptionalKeypathLoweringContractRecord
        &contract) {
  return std::string("optional_binding_sites=") +
         std::to_string(contract.optional_binding_sites) +
         ";optional_binding_clause_sites=" +
         std::to_string(contract.optional_binding_clause_sites) +
         ";optional_send_sites=" +
         std::to_string(contract.optional_send_sites) +
         ";nil_coalescing_sites=" +
         std::to_string(contract.nil_coalescing_sites) +
         ";typed_keypath_literal_sites=" +
         std::to_string(contract.typed_keypath_literal_sites) +
         ";typed_keypath_self_root_sites=" +
         std::to_string(contract.typed_keypath_self_root_sites) +
         ";typed_keypath_class_root_sites=" +
         std::to_string(contract.typed_keypath_class_root_sites) +
         ";live_optional_lowering_sites=" +
         std::to_string(contract.live_optional_lowering_sites) +
         ";single_evaluation_nil_short_circuit_sites=" +
         std::to_string(contract.single_evaluation_nil_short_circuit_sites) +
         ";live_typed_keypath_artifact_sites=" +
         std::to_string(contract.live_typed_keypath_artifact_sites) +
         ";deferred_typed_keypath_sites=" +
         std::to_string(contract.deferred_typed_keypath_sites) +
         ";typed_keypath_descriptor_publication_sites=" +
         std::to_string(contract.typed_keypath_descriptor_publication_sites) +
         ";typed_keypath_source_map_evidence_sites=" +
         std::to_string(contract.typed_keypath_source_map_evidence_sites) +
         ";typed_keypath_runtime_handle_evidence_sites=" +
         std::to_string(contract.typed_keypath_runtime_handle_evidence_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" +
         type_system_contract_artifacts::BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3FrontendTypeSystemOptionalKeypathLoweringLaneContract;
}

}  // namespace objc3::artifacts::frontend
