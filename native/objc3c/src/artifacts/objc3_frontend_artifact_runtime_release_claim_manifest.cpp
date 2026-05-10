#include "artifacts/objc3_frontend_artifact_runtime_release_claim_manifest.h"

#include <ostream>

#include "ast/objc3_ast_contracts.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"

namespace objc3::artifacts::frontend {

void WriteRuntimeReleaseCandidateClaimAbiSurface(
    std::ostream &manifest,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api) {
  manifest << "  \"runtime_release_candidate_claim_abi_surface\":{\"contract_id\":\""
           << kObjc3RuntimeReleaseCandidateClaimAbiSurfaceContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"claimability_semantics_release_policy_surface_contract_id\":\"objc3c.runtime.claimability.semantics.release.policy.surface.v1\""
           << ",\"final_claim_publication_deprecated_path_shutdown_surface_contract_id\":\"objc3c.runtime.final.claim.publication.deprecated.path.shutdown.surface.v1\""
           << ",\"public_runtime_abi_boundary\":[\""
           << kObjc3RuntimeSupportLibraryRegisterImageSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol << "\",\""
           << kObjc3RuntimeSupportLibraryResetForTestingSymbol << "\"]"
           << ",\"private_release_candidate_claim_testing_boundary\":[\""
           << kObjc3RuntimeReleaseCandidateClaimSnapshotSymbol << "\"]"
           << ",\"release_candidate_claim_snapshot_symbol\":\""
           << kObjc3RuntimeReleaseCandidateClaimSnapshotSymbol
           << "\",\"release_candidate_claim_snapshot_type\":\""
           << kObjc3RuntimeReleaseCandidateClaimSnapshotType
           << "\",\"conformance_publication_contract_id\":\"objc3c.driver.conformance.report.publication.v1\""
           << ",\"conformance_claim_operations_contract_id\":\"objc3c.toolchain.conformance.claim.operations.v1\""
           << ",\"release_evidence_operation_contract_id\":\"objc3c.tooling.release.evidence.toolchain.operations.v1\""
           << ",\"dashboard_status_publication_contract_id\":\"objc3c.tooling.dashboard.status.publication.v1\""
           << ",\"release_candidate_matrix_contract_id\":\"objc3c.tooling.release.candidate.execution.matrix.v1\""
           << ",\"claimed_profile_ids\":[\"core\",\"strict\",\"strict-concurrency\",\"strict-system\"]"
           << ",\"targeted_profile_ids\":[\"strict\",\"strict-concurrency\",\"strict-system\"]"
           << ",\"authoritative_probe_path\":\""
           << kObjc3RuntimeReleaseCandidateClaimProbePath
           << "\",\"runtime_claim_boundary_model\":\""
           << kObjc3RuntimeReleaseCandidateClaimBoundaryModel

           << "\""
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
}

}  // namespace objc3::artifacts::frontend
