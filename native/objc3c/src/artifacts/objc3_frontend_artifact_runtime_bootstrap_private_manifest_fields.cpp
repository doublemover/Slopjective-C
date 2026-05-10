#include "artifacts/objc3_frontend_artifact_runtime_bootstrap_private_manifest_fields.h"

#include <ostream>

#include "ast/objc3_ast_contracts.h"
#include "io/objc3_json.h"
#include "lower/contracts/runtime_bootstrap_image_root_contracts.h"
#include "lower/contracts/runtime_bootstrap_link_discovery_contracts.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"
#include "runtime/metadata/selector_metadata.h"

namespace objc3::artifacts::frontend {

void WriteRuntimeBootstrapPrivateManifestFields(
    std::ostream &manifest,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api,
    const Objc3RuntimeBootstrapLoweringSummary &runtime_bootstrap_lowering,
    const Objc3RuntimeBootstrapFailureRestartSemanticsSummary
        &runtime_bootstrap_failure_restart_semantics,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure,
    const std::string &translation_unit_identity_key) {
  manifest << ",\"objc_runtime_bootstrap_registrar_contract\":{"
           << "\"contract_id\":\""
           << objc3::io::EscapeJsonString(
                  kObjc3RuntimeBootstrapRegistrarContractId)
           << "\",\"surface_path\":\""
           << objc3::io::EscapeJsonString(
                  kObjc3RuntimeBootstrapRegistrarSurfacePath)
           << "\",\"bootstrap_api_contract_id\":\""
           << objc3::io::EscapeJsonString(runtime_bootstrap_api.contract_id)
           << "\",\"bootstrap_lowering_contract_id\":\""
           << objc3::io::EscapeJsonString(runtime_bootstrap_lowering.contract_id)
           << "\",\"internal_header_path\":\""
           << objc3::io::EscapeJsonString(
                  kObjc3RuntimeBootstrapInternalHeaderPath)
           << "\",\"stage_registration_table_symbol\":\""
           << objc3::io::EscapeJsonString(
                  kObjc3RuntimeBootstrapStageRegistrationTableSymbol)
           << "\",\"image_walk_snapshot_symbol\":\""
           << objc3::io::EscapeJsonString(
                  kObjc3RuntimeBootstrapImageWalkSnapshotSymbol)
           << "\",\"image_walk_model\":\""
           << objc3::io::EscapeJsonString(kObjc3RuntimeBootstrapImageWalkModel)
           << "\",\"discovery_root_validation_model\":\""
           << objc3::io::EscapeJsonString(
                  kObjc3RuntimeBootstrapDiscoveryRootValidationModel)
           << "\",\"selector_pool_interning_model\":\""
           << objc3::io::EscapeJsonString(
                  kObjc3RuntimeBootstrapSelectorPoolInterningModel)
           << "\",\"realization_staging_model\":\""
           << objc3::io::EscapeJsonString(
                  kObjc3RuntimeBootstrapRealizationStagingModel)
           << "\",\"fail_closed\":true"
           << ",\"ready\":"
           << ((IsReadyObjc3RuntimeBootstrapApiSummary(runtime_bootstrap_api) &&
                IsReadyObjc3RuntimeBootstrapLoweringSummary(
                    runtime_bootstrap_lowering))
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_runtime_bootstrap_reset_contract\":{"
           << "\"contract_id\":\""
           << objc3::io::EscapeJsonString(kObjc3RuntimeBootstrapResetContractId)
           << "\",\"surface_path\":\""
           << objc3::io::EscapeJsonString(kObjc3RuntimeBootstrapResetSurfacePath)
           << "\",\"bootstrap_api_contract_id\":\""
           << objc3::io::EscapeJsonString(runtime_bootstrap_api.contract_id)
           << "\",\"bootstrap_registrar_contract_id\":\""
           << objc3::io::EscapeJsonString(
                  kObjc3RuntimeBootstrapRegistrarContractId)
           << "\",\"internal_header_path\":\""
           << objc3::io::EscapeJsonString(
                  kObjc3RuntimeBootstrapInternalHeaderPath)
           << "\",\"replay_registered_images_symbol\":\""
           << objc3::io::EscapeJsonString(
                  kObjc3RuntimeBootstrapReplayRegisteredImagesSymbol)
           << "\",\"reset_replay_state_snapshot_symbol\":\""
           << objc3::io::EscapeJsonString(
                  kObjc3RuntimeBootstrapResetReplayStateSnapshotSymbol)
           << "\",\"reset_lifecycle_model\":\""
           << objc3::io::EscapeJsonString(
                  kObjc3RuntimeBootstrapResetLifecycleModel)
           << "\",\"replay_order_model\":\""
           << objc3::io::EscapeJsonString(kObjc3RuntimeBootstrapReplayOrderModel)
           << "\",\"image_local_init_state_reset_model\":\""
           << objc3::io::EscapeJsonString(
                  kObjc3RuntimeBootstrapImageLocalInitStateResetModel)
           << "\",\"bootstrap_catalog_retention_model\":\""
           << objc3::io::EscapeJsonString(
                  kObjc3RuntimeBootstrapCatalogRetentionModel)
           << "\",\"fail_closed\":true"
           << ",\"ready\":"
           << ((IsReadyObjc3RuntimeBootstrapApiSummary(runtime_bootstrap_api) &&
                IsReadyObjc3RuntimeBootstrapLoweringSummary(
                    runtime_bootstrap_lowering))
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_runtime_bootstrap_archive_static_link_replay_corpus\":{"
           << "\"contract_id\":\""
           << objc3::io::EscapeJsonString(
                  kObjc3RuntimeBootstrapArchiveStaticLinkReplayCorpusContractId)
           << "\",\"archive_static_link_discovery_contract_id\":\""
           << objc3::io::EscapeJsonString(
                  kObjc3RuntimeArchiveStaticLinkDiscoveryContractId)
           << "\",\"bootstrap_failure_restart_contract_id\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics.contract_id)
           << "\",\"bootstrap_lowering_contract_id\":\""
           << objc3::io::EscapeJsonString(runtime_bootstrap_lowering.contract_id)
           << "\",\"registration_descriptor_lowering_contract_id\":\""
           << objc3::io::EscapeJsonString(
                  kObjc3RuntimeBootstrapRegistrationDescriptorImageRootLoweringContractId)
           << "\",\"corpus_model\":\""
           << objc3::io::EscapeJsonString(
                  kObjc3RuntimeBootstrapArchiveStaticLinkReplayCorpusModel)
           << "\",\"binary_proof_model\":\""
           << objc3::io::EscapeJsonString(
                  kObjc3RuntimeBootstrapArchiveStaticLinkReplayCorpusBinaryProofModel)
           << "\",\"merge_model\":\""
           << objc3::io::EscapeJsonString(kObjc3RuntimeArchiveStaticLinkMergeModel)
           << "\",\"translation_unit_identity_key\":\""
           << objc3::io::EscapeJsonString(translation_unit_identity_key)
           << "\",\"registration_descriptor_identifier\":\""
           << objc3::io::EscapeJsonString(
                  runtime_registration_descriptor_frontend_closure
                      .registration_descriptor_identifier)
           << "\",\"image_root_identifier\":\""
           << objc3::io::EscapeJsonString(
                  runtime_registration_descriptor_frontend_closure
                      .image_root_identifier)
           << "\",\"replay_registered_images_symbol\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .replay_registered_images_symbol)
           << "\",\"reset_replay_state_snapshot_symbol\":\""
           << objc3::io::EscapeJsonString(
                  runtime_bootstrap_failure_restart_semantics
                      .reset_replay_state_snapshot_symbol)
           << "\",\"ready\":"
           << ((IsReadyObjc3RuntimeBootstrapFailureRestartSemanticsSummary(
                    runtime_bootstrap_failure_restart_semantics) &&
                IsReadyObjc3RuntimeBootstrapLoweringSummary(
                    runtime_bootstrap_lowering))
                   ? "true"
                   : "false")
           << "}";
}

}  // namespace objc3::artifacts::frontend
