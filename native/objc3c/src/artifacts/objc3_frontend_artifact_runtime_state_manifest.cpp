#include "artifacts/objc3_frontend_artifact_runtime_state_manifest.h"

#include <ostream>

#include "ast/objc3_ast_contracts.h"
#include "runtime/metadata/selector_metadata.h"

namespace objc3::artifacts::frontend {

void WriteRuntimeStatePublicationSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeBootstrapSemanticsSummary &runtime_bootstrap_semantics) {
  manifest << "  \"runtime_state_publication_surface\":{\"contract_id\":\""
           << kObjc3RuntimeStatePublicationSurfaceContractId
           << "\",\"publication_surface_kind\":"
           << "\"compile-manifest-plus-registration-manifest\""
           << ",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"runtime_support_library_archive_relative_path\":\""
           << runtime_translation_unit_registration_manifest
                  .runtime_support_library_archive_relative_path
           << "\",\"registration_entrypoint_symbol\":\""
           << runtime_translation_unit_registration_manifest
                  .registration_entrypoint_symbol
           << "\",\"runtime_state_snapshot_symbol\":\""
           << runtime_bootstrap_semantics.runtime_state_snapshot_symbol
           << "\",\"public_runtime_abi_boundary\":[\""
           << kObjc3RuntimeSupportLibraryRegisterImageSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol << "\",\""
           << kObjc3RuntimeSupportLibraryResetForTestingSymbol
           << "\"],\"class_descriptor_count\":"
           << runtime_translation_unit_registration_manifest.class_descriptor_count
           << ",\"protocol_descriptor_count\":"
           << runtime_translation_unit_registration_manifest.protocol_descriptor_count
           << ",\"category_descriptor_count\":"
           << runtime_translation_unit_registration_manifest.category_descriptor_count
           << ",\"property_descriptor_count\":"
           << runtime_translation_unit_registration_manifest.property_descriptor_count
           << ",\"ivar_descriptor_count\":"
           << runtime_translation_unit_registration_manifest.ivar_descriptor_count
           << ",\"total_descriptor_count\":"
           << runtime_translation_unit_registration_manifest.total_descriptor_count
           << ",\"publication_requires_coupled_registration_manifest\":true"
           << ",\"publication_requires_real_compile_output\":true"
           << "},\n";
}

}  // namespace objc3::artifacts::frontend
