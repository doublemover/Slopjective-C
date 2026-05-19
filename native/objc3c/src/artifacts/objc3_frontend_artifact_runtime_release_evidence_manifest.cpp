#include "artifacts/objc3_frontend_artifact_runtime_release_evidence_manifest.h"

#include <ostream>

#include "ast/objc3_ast_contracts.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"

namespace objc3::artifacts::frontend {

void WriteRuntimeCurrentReleaseEvidenceOwnerPayloadSurface(
    std::ostream &manifest,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api) {
  manifest << "  \"runtime_current_release_evidence_owner_payload_surface\":{\"contract_id\":\""
           << kObjc3RuntimeCurrentReleaseEvidenceOwnerPayloadSurfaceContractId
           << "\",\"runtime_release_candidate_claim_abi_surface_contract_id\":\""
           << kObjc3RuntimeReleaseCandidateClaimAbiSurfaceContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"private_release_candidate_evidence_testing_boundary\":[\""
           << kObjc3RuntimeReleaseCandidateEvidenceSnapshotSymbol << "\"]"
           << ",\"release_candidate_evidence_snapshot_symbol\":\""
           << kObjc3RuntimeReleaseCandidateEvidenceSnapshotSymbol
           << "\",\"release_candidate_evidence_snapshot_type\":\""
           << kObjc3RuntimeReleaseCandidateEvidenceSnapshotType
           << "\",\"validation_artifact_name\":\"module.objc3-conformance-validation.json\""
           << ",\"release_evidence_operation_artifact_name\":\"module.objc3-release-evidence-operation.json\""
           << ",\"dashboard_status_artifact_name\":\"module.objc3-dashboard-status.json\""
           << ",\"advanced_feature_gate_artifact_name\":\"module.objc3-advanced-feature-gate.json\""
           << ",\"release_candidate_matrix_artifact_name\":\"module.objc3-release-candidate-matrix.json\""
           << ",\"authoritative_probe_paths\":[\""
           << kObjc3RuntimeReleaseCandidateEvidenceProbePath
           << "\"],\"current_release_evidence_owner_payload_model\":\""
           << kObjc3RuntimeCurrentReleaseEvidenceOwnerPayloadModel
           << "\""
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
}

}  // namespace objc3::artifacts::frontend
