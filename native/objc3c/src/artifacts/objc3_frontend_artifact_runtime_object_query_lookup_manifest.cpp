#include "artifacts/objc3_frontend_artifact_runtime_object_query_lookup_manifest.h"

#include <ostream>

#include "artifacts/objc3_frontend_runtime_metadata_section_artifacts.h"
#include "ast/objc3_ast_contracts.h"
#include "lower/contracts/message_send_selector_lowering_contracts.h"
#include "lower/contracts/ownership_runtime_semantics_contracts.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"
#include "runtime/metadata/selector_metadata.h"

namespace objc3::artifacts::frontend {

void WriteRuntimeObjectModelAbiQuerySurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api) {
  manifest << "  \"runtime_object_model_abi_query_surface\":{\"contract_id\":\""
           << kObjc3RuntimeObjectModelAbiQuerySurfaceContractId
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
           << "\",\"runtime_object_model_realization_source_surface_contract_id\":\""
           << kObjc3RuntimeObjectModelRealizationSourceSurfaceContractId
           << "\",\"runtime_realization_lowering_reflection_artifact_surface_contract_id\":\""
           << kObjc3RuntimeRealizationLoweringReflectionArtifactSurfaceContractId
           << "\",\"runtime_dispatch_table_reflection_record_lowering_surface_contract_id\":\""
           << kObjc3RuntimeDispatchTableReflectionRecordLoweringSurfaceContractId
           << "\",\"runtime_cross_module_realized_metadata_replay_preservation_surface_contract_id\":\""
           << kObjc3RuntimeCrossModuleRealizedMetadataReplayPreservationSurfaceContractId
           << "\",\"runtime_reflection_query_surface_contract_id\":\""
           << kObjc3RuntimeReflectionQuerySurfaceContractId
           << "\",\"runtime_realization_lookup_semantics_surface_contract_id\":\""
           << kObjc3RuntimeRealizationLookupSemanticsSurfaceContractId
           << "\",\"runtime_class_metaclass_protocol_realization_surface_contract_id\":\""
           << kObjc3RuntimeClassMetaclassProtocolRealizationSurfaceContractId
           << "\",\"runtime_category_attachment_merged_dispatch_surface_contract_id\":\""
           << kObjc3RuntimeCategoryAttachmentMergedDispatchSurfaceContractId
           << "\",\"runtime_reflection_visibility_coherence_diagnostics_surface_contract_id\":\""
           << kObjc3RuntimeReflectionVisibilityCoherenceDiagnosticsSurfaceContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"public_runtime_abi_boundary\":[\""
           << kObjc3RuntimeSupportLibraryRegisterImageSymbol
           << "\",\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol
           << "\",\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol
           << "\",\""
           << kObjc3RuntimeSupportLibraryResetForTestingSymbol
           << "\"]"
           << ",\"private_object_model_query_boundary\":[\"objc3_runtime_copy_realized_class_graph_state_for_testing\""
           << ",\"objc3_runtime_copy_realized_class_entry_for_testing\""
           << ",\"objc3_runtime_copy_property_registry_state_for_testing\""
           << ",\"objc3_runtime_copy_property_entry_for_testing\""
           << ",\"objc3_runtime_copy_protocol_conformance_query_for_testing\""
           << ",\"objc3_runtime_copy_selector_lookup_table_state_for_testing\""
           << ",\"objc3_runtime_copy_selector_lookup_entry_for_testing\""
           << ",\"objc3_runtime_copy_method_cache_state_for_testing\""
           << ",\"objc3_runtime_copy_method_cache_entry_for_testing\""
           << ",\"objc3_runtime_copy_dispatch_state_for_testing\"]"
           << ",\"object_model_query_boundary_model\":\""
           << kObjc3RuntimeObjectModelQueryBoundaryModel
           << "\""
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
}

void WriteRuntimeRealizationLookupReflectionImplementationSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure) {
  manifest << "  \"runtime_realization_lookup_reflection_implementation_surface\":{\"contract_id\":\""
           << kObjc3RuntimeRealizationLookupReflectionImplementationSurfaceContractId
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
           << "\",\"runtime_object_model_abi_query_surface_contract_id\":\""
           << kObjc3RuntimeObjectModelAbiQuerySurfaceContractId
           << "\",\"runtime_reflection_query_surface_contract_id\":\""
           << kObjc3RuntimeReflectionQuerySurfaceContractId
           << "\",\"runtime_realization_lookup_semantics_surface_contract_id\":\""
           << kObjc3RuntimeRealizationLookupSemanticsSurfaceContractId
           << "\",\"runtime_class_metaclass_protocol_realization_surface_contract_id\":\""
           << kObjc3RuntimeClassMetaclassProtocolRealizationSurfaceContractId
           << "\",\"runtime_category_attachment_merged_dispatch_surface_contract_id\":\""
           << kObjc3RuntimeCategoryAttachmentMergedDispatchSurfaceContractId
           << "\",\"runtime_reflection_visibility_coherence_diagnostics_surface_contract_id\":\""
           << kObjc3RuntimeReflectionVisibilityCoherenceDiagnosticsSurfaceContractId
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"object_model_query_state_snapshot_symbol\":\"objc3_runtime_copy_object_model_query_state_for_testing\""
           << ",\"realized_class_entry_snapshot_symbol\":\"objc3_runtime_copy_realized_class_entry_for_testing\""
           << ",\"property_registry_state_snapshot_symbol\":\"objc3_runtime_copy_property_registry_state_for_testing\""
           << ",\"property_entry_snapshot_symbol\":\"objc3_runtime_copy_property_entry_for_testing\""
           << ",\"protocol_conformance_query_symbol\":\"objc3_runtime_copy_protocol_conformance_query_for_testing\""
           << ",\"selector_lookup_table_state_snapshot_symbol\":\"objc3_runtime_copy_selector_lookup_table_state_for_testing\""
           << ",\"selector_lookup_entry_snapshot_symbol\":\"objc3_runtime_copy_selector_lookup_entry_for_testing\""
           << ",\"method_cache_state_snapshot_symbol\":\"objc3_runtime_copy_method_cache_state_for_testing\""
           << ",\"method_cache_entry_snapshot_symbol\":\"objc3_runtime_copy_method_cache_entry_for_testing\""
           << ",\"dispatch_state_snapshot_symbol\":\"objc3_runtime_copy_dispatch_state_for_testing\""
           << ",\"realization_lookup_reflection_implementation_model\":\""
           << kObjc3RuntimeRealizationLookupReflectionImplementationModel
           << "\""
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
}

void WriteRuntimeReflectionQuerySurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api) {
  manifest << "  \"runtime_reflection_query_surface\":{\"contract_id\":\""
           << kObjc3RuntimeReflectionQuerySurfaceContractId
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
           << "\",\"object_model_realization_source_surface_contract_id\":\""
           << kObjc3RuntimeObjectModelRealizationSourceSurfaceContractId
           << "\",\"dispatch_accessor_runtime_abi_surface_contract_id\":\"objc3c.runtime.dispatch_accessor.abi.surface.v1\""
           << ",\"property_metadata_reflection_contract_id\":\""
           << kObjc3RuntimePropertyMetadataReflectionContractId
           << "\",\"runtime_backed_object_ownership_attribute_surface_contract_id\":\""
           << kObjc3RuntimeBackedObjectOwnershipAttributeSurfaceContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"query_api_boundary_model\":\"private-testing-snapshots-over-runtime-owned-realized-class-property-and-protocol-metadata-with-no-public-reflection-abi\""
           << ",\"private_query_symbols\":[\"objc3_runtime_copy_realized_class_graph_state_for_testing\""
           << ",\"objc3_runtime_copy_realized_class_entry_for_testing\""
           << ",\"objc3_runtime_copy_property_registry_state_for_testing\""
           << ",\"objc3_runtime_copy_property_entry_for_testing\""
           << ",\"objc3_runtime_copy_protocol_conformance_query_for_testing\"]"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
}

void WriteRuntimeRealizationLookupSemanticsSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api) {
  manifest << "  \"runtime_realization_lookup_semantics_surface\":{\"contract_id\":\""
           << kObjc3RuntimeRealizationLookupSemanticsSurfaceContractId
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
           << "\",\"runtime_object_model_realization_source_surface_contract_id\":\""
           << kObjc3RuntimeObjectModelRealizationSourceSurfaceContractId
           << "\",\"runtime_reflection_query_surface_contract_id\":\""
           << kObjc3RuntimeReflectionQuerySurfaceContractId
           << "\",\"dispatch_accessor_runtime_abi_surface_contract_id\":\"objc3c.runtime.dispatch_accessor.abi.surface.v1\""
           << ",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"registration_entrypoint_symbol\":\""
           << runtime_translation_unit_registration_manifest
                  .registration_entrypoint_symbol
           << "\",\"selector_lookup_symbol\":\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol
           << "\",\"runtime_dispatch_symbol\":\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol
           << "\",\"selector_lookup_table_state_snapshot_symbol\":\"objc3_runtime_copy_selector_lookup_table_state_for_testing\""
           << ",\"selector_lookup_entry_snapshot_symbol\":\"objc3_runtime_copy_selector_lookup_entry_for_testing\""
           << ",\"method_cache_state_snapshot_symbol\":\"objc3_runtime_copy_method_cache_state_for_testing\""
           << ",\"method_cache_entry_snapshot_symbol\":\"objc3_runtime_copy_method_cache_entry_for_testing\""
           << ",\"realized_class_entry_snapshot_symbol\":\"objc3_runtime_copy_realized_class_entry_for_testing\""
           << ",\"protocol_conformance_query_symbol\":\"objc3_runtime_copy_protocol_conformance_query_for_testing\""
           << ",\"lookup_resolution_order_model\":\"seeded-cache-then-live-class-chain-then-attached-category-and-protocol-checks-then-strict-dispatch-error\""
           << ",\"selector_materialization_model\":\"metadata-selectors-materialized-at-registration-and-dynamic-misses-interned-at-first-lookup\""
           << ",\"unresolved_selector_behavior_model\":\"negative-cache-entry-preserved-and-typed-strict-dispatch-error-returned\""
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
}

void WriteRuntimeReflectionVisibilityCoherenceDiagnosticsSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api) {
  manifest << "  \"runtime_reflection_visibility_coherence_diagnostics_surface\":{\"contract_id\":\""
           << kObjc3RuntimeReflectionVisibilityCoherenceDiagnosticsSurfaceContractId
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
           << "\",\"runtime_reflection_query_surface_contract_id\":\""
           << kObjc3RuntimeReflectionQuerySurfaceContractId
           << "\",\"runtime_category_attachment_merged_dispatch_surface_contract_id\":\""
           << kObjc3RuntimeCategoryAttachmentMergedDispatchSurfaceContractId
           << "\",\"dispatch_accessor_runtime_abi_surface_contract_id\":\"objc3c.runtime.dispatch_accessor.abi.surface.v1\""
           << ",\"property_metadata_reflection_contract_id\":\""
           << kObjc3RuntimePropertyMetadataReflectionContractId
           << "\",\"runtime_backed_object_ownership_attribute_surface_contract_id\":\""
           << kObjc3RuntimeBackedObjectOwnershipAttributeSurfaceContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"property_registry_state_snapshot_symbol\":\"objc3_runtime_copy_property_registry_state_for_testing\""
           << ",\"property_entry_snapshot_symbol\":\"objc3_runtime_copy_property_entry_for_testing\""
           << ",\"realized_class_entry_snapshot_symbol\":\"objc3_runtime_copy_realized_class_entry_for_testing\""
           << ",\"protocol_conformance_query_symbol\":\"objc3_runtime_copy_protocol_conformance_query_for_testing\""
           << ",\"method_cache_state_snapshot_symbol\":\"objc3_runtime_copy_method_cache_state_for_testing\""
           << ",\"reflection_visibility_boundary_model\":\"private-testing-snapshots-remain-the-only-reflection-visibility-surface-and-publish-runtime-owned-class-property-and-protocol-state\""
           << ",\"fail_closed_lookup_diagnostic_model\":\"missing-class-and-property-lookups-publish-found-zero-without-mutating-property-registry-or-realized-class-state\""
           << ",\"runtime_coherence_diagnostic_model\":\"reflected-property-selectors-owner-identities-slot-layout-and-ownership-profiles-must-match-live-dispatch-realized-class-and-attached-protocol-state\""
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
}

}  // namespace objc3::artifacts::frontend
