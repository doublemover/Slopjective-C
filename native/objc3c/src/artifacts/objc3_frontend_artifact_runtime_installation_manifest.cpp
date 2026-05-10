#include "artifacts/objc3_frontend_artifact_runtime_installation_manifest.h"

#include <ostream>

#include "ast/objc3_ast_contracts.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"
#include "runtime/metadata/selector_metadata.h"

namespace objc3::artifacts::frontend {

void WriteRuntimeInstallationAbiSurface(
    std::ostream &manifest,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api) {
  manifest << "  \"runtime_installation_abi_surface\":{\"contract_id\":\""
           << kObjc3RuntimeInstallationAbiSurfaceContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"bootstrap_api_contract_id\":\""
           << runtime_bootstrap_api.contract_id
           << "\",\"bootstrap_reset_contract_id\":\""
           << kObjc3RuntimeBootstrapResetContractId
           << "\",\"bootstrap_registrar_contract_id\":\""
           << kObjc3RuntimeBootstrapRegistrarContractId
           << "\",\"public_installation_abi_boundary\":[\""
           << kObjc3RuntimeSupportLibraryRegisterImageSymbol << "\",\""
           << kObjc3RuntimeBootstrapStateSnapshotSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryResetForTestingSymbol
           << "\"],\"private_loader_testing_boundary\":[\""
           << kObjc3RuntimeBootstrapStageRegistrationTableSymbol << "\",\""
           << kObjc3RuntimeBootstrapImageWalkSnapshotSymbol << "\",\""
           << kObjc3RuntimeBootstrapReplayRegisteredImagesSymbol << "\",\""
           << kObjc3RuntimeBootstrapResetReplayStateSnapshotSymbol
           << "\"],\"installation_requires_coupled_registration_manifest\":true"
           << ",\"register_image_consumes_staged_registration_table_once\":true"
           << ",\"deterministic_reset_replay_supported\":true"
           << "},\n";
}

void WriteRuntimeLoaderLifecycleSurface(
    std::ostream &manifest,
    const Objc3RuntimeBootstrapSemanticsSummary &runtime_bootstrap_semantics) {
  manifest << "  \"runtime_loader_lifecycle_surface\":{\"contract_id\":\""
           << kObjc3RuntimeLoaderLifecycleSurfaceContractId
           << "\",\"runtime_installation_abi_surface_contract_id\":\""
           << kObjc3RuntimeInstallationAbiSurfaceContractId
           << "\",\"bootstrap_semantics_contract_id\":\""
           << runtime_bootstrap_semantics.contract_id
           << "\",\"bootstrap_reset_contract_id\":\""
           << kObjc3RuntimeBootstrapResetContractId
           << "\",\"bootstrap_registrar_contract_id\":\""
           << kObjc3RuntimeBootstrapRegistrarContractId
           << "\",\"authoritative_probe_path\":\""
           << kObjc3RuntimeInstallationLifecycleProbePath
           << "\",\"loader_testing_boundary_symbols\":[\""
           << kObjc3RuntimeBootstrapStageRegistrationTableSymbol << "\",\""
           << kObjc3RuntimeBootstrapImageWalkSnapshotSymbol << "\",\""
           << kObjc3RuntimeBootstrapReplayRegisteredImagesSymbol << "\",\""
           << kObjc3RuntimeBootstrapResetReplayStateSnapshotSymbol
           << "\"],\"lifecycle_phases\":[\"startup-installed-runtime-state\""
           << ",\"duplicate-registration-rejected-without-state-advance\""
           << ",\"out-of-order-registration-rejected-without-state-advance\""
           << ",\"invalid-anchor-root-rejected-without-state-advance\""
           << ",\"invalid-discovery-root-rejected-without-state-advance\""
           << ",\"reset-retained-bootstrap-catalog\""
           << ",\"replay-restored-installed-runtime-state\""
           << "],\"rejected_registration_status_codes\":{"
           << "\"duplicate_translation_unit_identity_key\":"
           << kObjc3RuntimeBootstrapDuplicateRegistrationStatusCode
           << ",\"out_of_order_registration\":"
           << kObjc3RuntimeBootstrapOutOfOrderStatusCode
           << ",\"invalid_registration_roots\":"
           << kObjc3RuntimeBootstrapInvalidRegistrationRootsStatusCode
           << "},\"retained_bootstrap_catalog_required\":true"
           << ",\"deterministic_replay_required\":true"
           << ",\"requires_linked_fixture_or_loader_retained_roots\":true"
           << "},\n";
}

}  // namespace objc3::artifacts::frontend
