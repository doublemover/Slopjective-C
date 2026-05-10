#include "artifacts/objc3_frontend_artifact_runtime_object_realization_manifest.h"

#include <ostream>

#include "artifacts/objc3_frontend_artifact_runtime_object_manifest_contracts.h"
#include "artifacts/objc3_frontend_runtime_metadata_section_artifacts.h"
#include "ast/objc3_ast_contracts.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"
#include "runtime/metadata/selector_metadata.h"

namespace objc3::artifacts::frontend {

void WriteRuntimeObjectModelRealizationSourceSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api) {
  manifest << "  \"runtime_object_model_realization_source_surface\":{\"contract_id\":\""
           << kObjc3RuntimeObjectModelRealizationSourceSurfaceContractId
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
           << "\",\"executable_realization_records_contract_id\":\""
           << kRuntimeObjectExecutableRealizationRecordsContractId
           << "\",\"runtime_class_realization_contract_id\":\""
           << kRuntimeObjectClassRealizationContractId
           << "\",\"runtime_metaclass_graph_contract_id\":\""
           << kRuntimeObjectMetaclassGraphRootClassContractId
           << "\",\"runtime_category_attachment_protocol_conformance_contract_id\":\""
           << kRuntimeObjectCategoryAttachmentProtocolConformanceContractId
           << "\",\"canonical_runnable_object_support_contract_id\":\""
           << kObjc3RuntimeCanonicalRunnableObjectSampleSupportContractId
           << "\",\"runtime_support_library_archive_relative_path\":\""
           << runtime_translation_unit_registration_manifest
                  .runtime_support_library_archive_relative_path
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
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
}

}  // namespace objc3::artifacts::frontend
