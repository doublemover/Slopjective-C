#include "artifacts/objc3_frontend_artifact_runtime_state_publication_manifest.h"

#include <ostream>

#include "ast/objc3_ast_contracts.h"
#include "artifacts/objc3_runtime_state_publication_paths.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"
#include "runtime/metadata/selector_metadata_registration_descriptor_surfaces.h"
#include "runtime/metadata/selector_metadata_registration_manifest.h"
#include "runtime/metadata/selector_metadata.h"

namespace objc3::artifacts::frontend {

void WriteRuntimeStatePublicationSurface(
    std::ostream &manifest,
    const RuntimeStatePublicationPaths &runtime_state_publication_paths,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeBootstrapSemanticsSummary &runtime_bootstrap_semantics) {
  const RuntimeStateObjectDebugIdentity &object_debug_identity =
      runtime_state_publication_paths.object_debug_identity;
  manifest << "  \"runtime_state_publication_surface\":{\"contract_id\":\""
           << kObjc3RuntimeStatePublicationSurfaceContractId
           << "\",\"publication_surface_kind\":"
           << "\"compile-manifest-plus-registration-manifest\""
           << ",\"compile_manifest_artifact\":\""
           << runtime_state_publication_paths.compile_manifest_artifact
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_paths.object_artifact
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_paths.backend_artifact
           << "\",\"object_debug_identity_surface\":{\"contract_id\":\""
           << object_debug_identity.contract_id
           << "\",\"platform_id\":\"" << object_debug_identity.platform_id
           << "\",\"host_os\":\"" << object_debug_identity.host_os
           << "\",\"host_arch\":\"" << object_debug_identity.host_arch
           << "\",\"target_triple\":\"" << object_debug_identity.target_triple
           << "\",\"object_format\":\"" << object_debug_identity.object_format
           << "\",\"package_object_format\":\""
           << object_debug_identity.package_object_format
           << "\",\"debug_format\":\"" << object_debug_identity.debug_format
           << "\",\"object_file_extension\":\""
           << object_debug_identity.object_file_extension
           << "\",\"host_promotion_state\":\""
           << object_debug_identity.host_promotion_state
           << "\",\"wrong_format_behavior\":\""
           << object_debug_identity.wrong_format_behavior
           << "\",\"wrong_arch_behavior\":\""
           << object_debug_identity.wrong_arch_behavior
           << "\",\"support_truth_policy\":\""
           << object_debug_identity.support_truth_policy
           << "\",\"unsupported_host_behavior\":\""
           << object_debug_identity.unsupported_host_behavior
           << "\",\"platform_identity_known\":"
           << (object_debug_identity.platform_identity_known ? "true" : "false")
           << ",\"object_artifact_publication_supported\":"
           << (object_debug_identity.object_artifact_publication_supported
                   ? "true"
                   : "false")
           << ",\"object_emission_alone_supports_platform\":"
           << (object_debug_identity.object_emission_alone_supports_platform
                   ? "true"
                   : "false")
           << "},\"runtime_support_library_archive_relative_path\":\""
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
