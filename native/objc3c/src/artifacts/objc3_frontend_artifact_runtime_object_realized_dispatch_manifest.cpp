#include "artifacts/objc3_frontend_artifact_runtime_object_realized_dispatch_manifest.h"
#include "artifacts/identity/artifact_identity.h"
#include <ostream>
#include "artifacts/objc3_frontend_runtime_metadata_section_artifacts.h"
#include "ast/objc3_ast_contracts.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"
#include "runtime/metadata/selector_metadata_registration_descriptor_surfaces.h"
#include "runtime/metadata/selector_metadata_registration_manifest.h"


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
           << objc3::artifacts::identity::BuildObjc3NativeObjectArtifactName(runtime_state_publication_emit_prefix)
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

}  // namespace objc3::artifacts::frontend
