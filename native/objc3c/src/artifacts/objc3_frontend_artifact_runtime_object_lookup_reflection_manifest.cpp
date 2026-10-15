#include "artifacts/objc3_frontend_artifact_runtime_object_lookup_reflection_manifest.h"
#include "artifacts/identity/artifact_identity.h"
#include <ostream>
#include "ast/objc3_ast_contracts.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"
#include "runtime/metadata/selector_metadata_registration_descriptor_surfaces.h"
#include "runtime/metadata/selector_metadata_registration_manifest.h"


namespace objc3::artifacts::frontend {

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
           << objc3::artifacts::identity::BuildObjc3NativeObjectArtifactName(runtime_state_publication_emit_prefix)
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
           << ",\"realized_class_graph_state_snapshot_symbol\":\"objc3_runtime_copy_realized_class_graph_state_for_testing\""
           << ",\"realized_class_entry_snapshot_symbol\":\"objc3_runtime_copy_realized_class_entry_for_testing\""
           << ",\"runtime_instance_entry_snapshot_symbol\":\"objc3_runtime_copy_instance_entry_for_testing\""
           << ",\"property_registry_state_snapshot_symbol\":\"objc3_runtime_copy_property_registry_state_for_testing\""
           << ",\"property_entry_snapshot_symbol\":\"objc3_runtime_copy_property_entry_for_testing\""
           << ",\"storage_accessor_snapshot_symbol\":\"objc3_runtime_copy_storage_accessor_implementation_snapshot_for_testing\""
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

}  // namespace objc3::artifacts::frontend
