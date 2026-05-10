#include "artifacts/objc3_frontend_artifact_runtime_object_realized_dispatch_manifest.h"

#include <ostream>

#include "artifacts/objc3_frontend_runtime_metadata_section_artifacts.h"
#include "ast/objc3_ast_contracts.h"
#include "lower/contracts/message_send_selector_lowering_contracts.h"
#include "lower/contracts/ownership_runtime_semantics_contracts.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"
#include "runtime/metadata/selector_metadata.h"

namespace objc3::artifacts::frontend {

void WriteRuntimeClassMetaclassProtocolRealizationSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api) {
  manifest << "  \"runtime_class_metaclass_protocol_realization_surface\":{\"contract_id\":\""
           << kObjc3RuntimeClassMetaclassProtocolRealizationSurfaceContractId
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
           << "\",\"runtime_realization_lookup_semantics_surface_contract_id\":\""
           << kObjc3RuntimeRealizationLookupSemanticsSurfaceContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"registration_entrypoint_symbol\":\""
           << runtime_translation_unit_registration_manifest
                  .registration_entrypoint_symbol
           << "\",\"realized_class_graph_snapshot_symbol\":\"objc3_runtime_copy_realized_class_graph_state_for_testing\""
           << ",\"realized_class_entry_snapshot_symbol\":\"objc3_runtime_copy_realized_class_entry_for_testing\""
           << ",\"protocol_conformance_query_symbol\":\"objc3_runtime_copy_protocol_conformance_query_for_testing\""
           << ",\"class_realization_model\":\"registration-installs-runtime-backed-class-records-before-live-dispatch-and-reflection\""
           << ",\"metaclass_lineage_model\":\"realized-class-entries-publish-stable-class-metaclass-superclass-and-super-metaclass-owner-identities\""
           << ",\"protocol_conformance_model\":\"realized-class-entries-and-runtime-conformance-queries-publish-direct-and-attached-protocol-conformance\""
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
}

void WriteRuntimeCategoryAttachmentMergedDispatchSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api) {
  manifest << "  \"runtime_category_attachment_merged_dispatch_surface\":{\"contract_id\":\""
           << kObjc3RuntimeCategoryAttachmentMergedDispatchSurfaceContractId
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
           << "\",\"runtime_category_attachment_protocol_conformance_contract_id\":\""
           << kObjc3RuntimeCategoryAttachmentProtocolConformanceContractId
           << "\",\"runtime_realization_lookup_semantics_surface_contract_id\":\""
           << kObjc3RuntimeRealizationLookupSemanticsSurfaceContractId
           << "\",\"runtime_class_metaclass_protocol_realization_surface_contract_id\":\""
           << kObjc3RuntimeClassMetaclassProtocolRealizationSurfaceContractId
           << "\",\"public_header_path\":\""
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
           << "\",\"realized_class_graph_snapshot_symbol\":\"objc3_runtime_copy_realized_class_graph_state_for_testing\""
           << ",\"realized_class_entry_snapshot_symbol\":\"objc3_runtime_copy_realized_class_entry_for_testing\""
           << ",\"protocol_conformance_query_symbol\":\"objc3_runtime_copy_protocol_conformance_query_for_testing\""
           << ",\"method_cache_state_snapshot_symbol\":\"objc3_runtime_copy_method_cache_state_for_testing\""
           << ",\"method_cache_entry_snapshot_symbol\":\"objc3_runtime_copy_method_cache_entry_for_testing\""
           << ",\"category_attachment_model\":\"registration-attaches-category-owned-instance-and-protocol-members-onto-live-realized-classes-before-dispatch\""
           << ",\"merged_dispatch_resolution_model\":\"attached-category-implementations-override-base-class-instance-lookup-before-superclass-and-protocol-strict-error\""
           << ",\"attached_protocol_visibility_model\":\"attached-categories-publish-owner-and-name-through-realized-class-entries-and-protocol-conformance-queries\""
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
}

}  // namespace objc3::artifacts::frontend
