#include "artifacts/objc3_frontend_artifact_runtime_object_visibility_diagnostics_manifest.h"
#include "artifacts/identity/artifact_identity.h"
#include <ostream>
#include "artifacts/objc3_frontend_artifact_runtime_object_manifest_contracts.h"
#include "ast/objc3_ast_contracts.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"
#include "runtime/metadata/selector_metadata_registration_descriptor_surfaces.h"
#include "runtime/metadata/selector_metadata_registration_manifest.h"


namespace objc3::artifacts::frontend {

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
           << objc3::artifacts::identity::BuildObjc3NativeObjectArtifactName(runtime_state_publication_emit_prefix)
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"runtime_reflection_query_surface_contract_id\":\""
           << kObjc3RuntimeReflectionQuerySurfaceContractId
           << "\",\"runtime_category_attachment_merged_dispatch_surface_contract_id\":\""
           << kObjc3RuntimeCategoryAttachmentMergedDispatchSurfaceContractId
           << "\",\"dispatch_accessor_runtime_abi_surface_contract_id\":\"objc3c.runtime.dispatch_accessor.abi.surface.v1\""
           << ",\"property_metadata_reflection_contract_id\":\""
           << kRuntimeObjectPropertyMetadataReflectionContractId
           << "\",\"runtime_backed_object_ownership_attribute_surface_contract_id\":\""
           << kRuntimeObjectBackedObjectOwnershipAttributeSurfaceContractId
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
