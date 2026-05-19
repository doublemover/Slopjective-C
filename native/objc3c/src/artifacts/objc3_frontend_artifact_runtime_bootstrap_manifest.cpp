#include "artifacts/objc3_frontend_artifact_runtime_bootstrap_manifest.h"

#include <ostream>

#include "ast/objc3_ast_contracts_runtime_bootstrap_support_bootstrap_api.h"
#include "ast/objc3_ast_contracts_runtime_bootstrap_support_registrar_reset.h"
#include "ast/objc3_ast_contracts_runtime_bootstrap_support_source_surfaces.h"
#include "lower/contracts/runtime_bootstrap_link_discovery_contracts.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"
#include "runtime/metadata/selector_metadata_bootstrap_legality_surfaces.h"
#include "runtime/metadata/selector_metadata_bootstrap_surfaces.h"
#include "runtime/metadata/selector_metadata_registration_descriptor_surfaces.h"
#include "runtime/metadata/selector_metadata_registration_manifest.h"

namespace objc3::artifacts::frontend {

void WriteRuntimeBootstrapRegistrationSourceSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure,
    const Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary
        &runtime_registration_descriptor_image_root_source_surface,
    const Objc3RuntimeBootstrapLoweringSummary &runtime_bootstrap_lowering,
    const Objc3RuntimeBootstrapLegalitySemanticsSummary
        &runtime_bootstrap_legality_semantics) {
  manifest << "  \"runtime_bootstrap_registration_source_surface\":{\"contract_id\":\""
           << kObjc3RuntimeBootstrapRegistrationSourceSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"source_surface_contract_id\":\""
           << runtime_registration_descriptor_image_root_source_surface.contract_id
           << "\",\"frontend_closure_contract_id\":\""
           << runtime_registration_descriptor_frontend_closure.contract_id
           << "\",\"registration_manifest_contract_id\":\""
           << runtime_translation_unit_registration_manifest.contract_id
           << "\",\"bootstrap_lowering_contract_id\":\""
           << runtime_bootstrap_lowering.contract_id
           << "\",\"runtime_support_library_archive_relative_path\":\""
           << runtime_translation_unit_registration_manifest
                  .runtime_support_library_archive_relative_path
           << "\",\"registration_descriptor_identifier\":\""
           << runtime_registration_descriptor_frontend_closure
                  .registration_descriptor_identifier
           << "\",\"image_root_identifier\":\""
           << runtime_registration_descriptor_frontend_closure
                  .image_root_identifier
           << "\",\"registration_entrypoint_symbol\":\""
           << runtime_translation_unit_registration_manifest
                  .registration_entrypoint_symbol
           << "\",\"constructor_root_symbol\":\""
           << runtime_translation_unit_registration_manifest.constructor_root_symbol
           << "\",\"translation_unit_identity_key\":\""
           << runtime_bootstrap_legality_semantics.translation_unit_identity_key
           << "\",\"translation_unit_registration_order_ordinal\":"
           << runtime_translation_unit_registration_manifest
                  .translation_unit_registration_order_ordinal
           << ",\"requires_coupled_registration_descriptor_artifact\":true"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << "},\n";
}

void WriteRuntimeBootstrapLoweringRegistrationArtifactSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure,
    const Objc3RuntimeBootstrapLoweringSummary &runtime_bootstrap_lowering,
    const Objc3RuntimeBootstrapSemanticsSummary &runtime_bootstrap_semantics) {
  manifest << "  \"runtime_bootstrap_lowering_registration_artifact_surface\":{\"contract_id\":\""
           << kObjc3RuntimeBootstrapLoweringRegistrationArtifactSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"bootstrap_lowering_contract_id\":\""
           << runtime_bootstrap_lowering.contract_id
           << "\",\"registration_manifest_contract_id\":\""
           << runtime_translation_unit_registration_manifest.contract_id
           << "\",\"bootstrap_semantics_contract_id\":\""
           << runtime_bootstrap_semantics.contract_id
           << "\",\"registration_descriptor_frontend_closure_contract_id\":\""
           << runtime_registration_descriptor_frontend_closure.contract_id
           << "\",\"runtime_support_library_archive_relative_path\":\""
           << runtime_translation_unit_registration_manifest
                  .runtime_support_library_archive_relative_path
           << "\",\"constructor_root_symbol\":\""
           << runtime_bootstrap_lowering.constructor_root_symbol
           << "\",\"init_stub_symbol_prefix\":\""
           << runtime_bootstrap_lowering.constructor_init_stub_symbol_prefix
           << "\",\"registration_table_symbol_prefix\":\""
           << runtime_bootstrap_lowering.registration_table_symbol_prefix
           << "\",\"image_local_init_state_symbol_prefix\":\""
           << runtime_bootstrap_lowering.image_local_init_state_symbol_prefix
           << "\",\"registration_entrypoint_symbol\":\""
           << runtime_bootstrap_lowering.registration_entrypoint_symbol
           << "\",\"global_ctor_list_model\":\""
           << runtime_bootstrap_lowering.global_ctor_list_model
           << "\",\"registration_table_layout_model\":\""
           << runtime_bootstrap_lowering.registration_table_layout_model
           << "\",\"image_local_initialization_model\":\""
           << runtime_bootstrap_lowering.image_local_initialization_model
           << "\",\"registration_table_abi_version\":"
           << runtime_bootstrap_lowering.registration_table_abi_version
           << ",\"registration_table_pointer_field_count\":"
           << runtime_bootstrap_lowering
                  .registration_table_pointer_field_count
           << ",\"constructor_root_emission_state\":\""
           << runtime_bootstrap_lowering.constructor_root_emission_state
           << "\",\"init_stub_emission_state\":\""
           << runtime_bootstrap_lowering.init_stub_emission_state
           << "\",\"registration_table_emission_state\":\""
           << runtime_bootstrap_lowering.registration_table_emission_state
           << "\",\"lowered_registration_descriptor_fields\":["
           << "\"constructor_init_stub_symbol\","
           << "\"bootstrap_registration_table_symbol\","
           << "\"bootstrap_image_local_init_state_symbol\","
           << "\"bootstrap_registration_table_layout_model\","
           << "\"bootstrap_image_local_initialization_model\","
           << "\"bootstrap_registration_table_abi_version\","
           << "\"bootstrap_registration_table_pointer_field_count\","
           << "\"translation_unit_registration_order_ordinal\""
           << "],\"loader_table_ir_proof_fields\":["
           << "\"constructor_root_symbol\","
           << "\"constructor_init_stub_symbol\","
           << "\"bootstrap_registration_table_symbol\","
           << "\"bootstrap_image_local_init_state_symbol\","
           << "\"translation_unit_registration_order_ordinal\""
           << "]"
           << ",\"bootstrap_ir_materialization_landed\":"
           << (runtime_bootstrap_lowering.bootstrap_ir_materialization_landed
                   ? "true"
                   : "false")
           << ",\"image_local_initialization_landed\":"
           << (runtime_bootstrap_lowering.image_local_initialization_landed
                   ? "true"
                   : "false")
           << ",\"requires_coupled_registration_descriptor_artifact\":true"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_emitted_loader_table_ir\":true"
           << "},\n";
}

void WriteRuntimeMultiImageStartupOrderingSourceSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure,
    const Objc3RuntimeBootstrapLegalitySemanticsSummary
        &runtime_bootstrap_legality_semantics,
    const Objc3RuntimeBootstrapFailureRestartSemanticsSummary
        &runtime_bootstrap_failure_restart_semantics,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api,
    const Objc3RuntimeBootstrapSemanticsSummary &runtime_bootstrap_semantics) {
  manifest << "  \"runtime_multi_image_startup_ordering_source_surface\":{\"contract_id\":\""
           << kObjc3RuntimeMultiImageStartupOrderingSourceSurfaceContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"registration_descriptor_artifact\":\""
           << runtime_registration_descriptor_frontend_closure.artifact_relative_path
           << "\",\"bootstrap_legality_semantics_contract_id\":\""
           << runtime_bootstrap_legality_semantics.contract_id
           << "\",\"bootstrap_failure_restart_contract_id\":\""
           << runtime_bootstrap_failure_restart_semantics.contract_id
           << "\",\"bootstrap_api_contract_id\":\""
           << runtime_bootstrap_api.contract_id
           << "\",\"bootstrap_reset_contract_id\":\""
           << kObjc3RuntimeBootstrapResetContractId
           << "\",\"bootstrap_registrar_contract_id\":\""
           << kObjc3RuntimeBootstrapRegistrarContractId
           << "\",\"archive_static_link_replay_corpus_contract_id\":\""
           << kObjc3RuntimeBootstrapArchiveStaticLinkReplayCorpusContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"registration_entrypoint_symbol\":\""
           << runtime_bootstrap_api.registration_entrypoint_symbol
           << "\",\"state_snapshot_symbol\":\""
           << runtime_bootstrap_api.state_snapshot_symbol
           << "\",\"reset_for_testing_symbol\":\""
           << runtime_bootstrap_api.reset_for_testing_symbol
           << "\",\"replay_registered_images_symbol\":\""
           << runtime_bootstrap_failure_restart_semantics
                  .replay_registered_images_symbol
           << "\",\"reset_replay_state_snapshot_symbol\":\""
           << runtime_bootstrap_failure_restart_semantics
                  .reset_replay_state_snapshot_symbol
           << "\",\"translation_unit_identity_key\":\""
           << runtime_bootstrap_legality_semantics.translation_unit_identity_key
           << "\",\"translation_unit_registration_order_ordinal\":"
           << runtime_bootstrap_legality_semantics
                  .translation_unit_registration_order_ordinal
           << ",\"duplicate_registration_status_code\":"
           << runtime_bootstrap_semantics.duplicate_registration_status_code
           << ",\"out_of_order_status_code\":"
           << runtime_bootstrap_semantics.out_of_order_status_code
           << ",\"duplicate_install_diagnostic_model\":\""
           << kObjc3RuntimeBootstrapDuplicateInstallDiagnosticModel
           << "\",\"out_of_order_install_diagnostic_model\":\""
           << kObjc3RuntimeBootstrapOutOfOrderDiagnosticModel
           << "\",\"last_rejected_module_name_field\":\""
           << kObjc3RuntimeBootstrapRejectedModuleNameField
           << "\",\"last_rejected_translation_unit_identity_key_field\":\""
           << kObjc3RuntimeBootstrapRejectedTranslationUnitIdentityKeyField
           << "\",\"next_expected_registration_order_field\":\""
           << kObjc3RuntimeBootstrapNextExpectedRegistrationOrderField
           << "\",\"last_successful_registration_order_field\":\""
           << kObjc3RuntimeBootstrapLastSuccessfulRegistrationOrderField
           << "\",\"last_rejected_registration_order_field\":\""
           << kObjc3RuntimeBootstrapLastRejectedRegistrationOrderField
           << "\",\"requires_linked_runtime_probe\":true"
           << ",\"requires_real_compile_output\":true"
           << "},\n";
}

}  // namespace objc3::artifacts::frontend
