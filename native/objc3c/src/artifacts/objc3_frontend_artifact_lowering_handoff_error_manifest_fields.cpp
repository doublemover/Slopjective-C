#include "artifacts/objc3_frontend_artifact_lowering_handoff_error_manifest_fields.h"

#include <sstream>
#include <string>

#include "artifacts/objc3_frontend_artifact_error_lowering_plan.h"
#include "lower/contracts/error_handling_throws_unwind_contracts.h"

namespace objc3::artifacts::frontend {

void AppendObjc3FrontendArtifactLoweringHandoffErrorManifestFields(
    std::ostringstream &manifest,
    const Objc3FrontendArtifactErrorLoweringPlan &error_lowering_plan) {
  const Objc3ThrowsPropagationLoweringContract
      &throws_propagation_lowering_contract =
          error_lowering_plan.throws_propagation_lowering_contract;
  const std::string &throws_propagation_lowering_replay_key =
      error_lowering_plan.throws_propagation_lowering_replay_key;

  manifest << ",\"deterministic_throws_propagation_lowering_handoff\":"
           << (throws_propagation_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << ",\"throws_propagation_lowering_sites\":"
           << throws_propagation_lowering_contract.throws_propagation_sites
           << ",\"throws_propagation_lowering_namespace_segment_sites\":"
           << throws_propagation_lowering_contract.namespace_segment_sites
           << ",\"throws_propagation_lowering_import_edge_candidate_sites\":"
           << throws_propagation_lowering_contract.import_edge_candidate_sites
           << ",\"throws_propagation_lowering_object_pointer_type_sites\":"
           << throws_propagation_lowering_contract.object_pointer_type_sites
           << ",\"throws_propagation_lowering_pointer_declarator_sites\":"
           << throws_propagation_lowering_contract.pointer_declarator_sites
           << ",\"throws_propagation_lowering_normalized_sites\":"
           << throws_propagation_lowering_contract.normalized_sites
           << ",\"throws_propagation_lowering_cache_invalidation_candidate_sites\":"
           << throws_propagation_lowering_contract
                  .cache_invalidation_candidate_sites
           << ",\"throws_propagation_lowering_contract_violation_sites\":"
           << throws_propagation_lowering_contract.contract_violation_sites
           << ",\"lowering_throws_propagation_replay_key\":\""
           << throws_propagation_lowering_replay_key
           << "\"";
}

}  // namespace objc3::artifacts::frontend
