#include "artifacts/objc3_frontend_artifact_property_atomicity_manifest.h"

#include <ostream>

#include "artifacts/objc3_frontend_artifact_storage_accessor_manifest_contracts.h"
#include "ast/objc3_ast_contracts.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"
#include "runtime/metadata/selector_metadata_registration_descriptor_surfaces.h"
#include "runtime/metadata/selector_metadata_registration_manifest.h"

namespace objc3::artifacts::frontend {

void WriteRuntimePropertyAtomicitySynthesisReflectionSourceSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure) {
  manifest << "  \"runtime_property_atomicity_synthesis_reflection_source_surface\":{\"contract_id\":\""
           << kObjc3RuntimePropertyAtomicitySynthesisReflectionSourceSurfaceContractId
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
           << "\",\"property_storage_source_surface_contract_id\":\""
           << kObjc3RuntimePropertyIvarStorageAccessorSourceSurfaceContractId
           << "\",\"property_metadata_reflection_contract_id\":\""
           << kStorageAccessorRuntimePropertyMetadataReflectionContractId
           << "\",\"runtime_backed_object_ownership_attribute_surface_contract_id\":\""
           << kStorageAccessorRuntimeBackedObjectOwnershipAttributeSurfaceContractId
           << "\",\"atomic_modifier_field\":\"Objc3PropertyDecl.is_atomic\""
           << ",\"nonatomic_modifier_field\":\"Objc3PropertyDecl.is_nonatomic\""
           << ",\"atomicity_conflict_field\":\"Objc3PropertyDecl.has_atomicity_conflict\""
           << ",\"property_attribute_profile_field\":\"Objc3PropertyDecl.property_attribute_profile\""
           << ",\"reflection_attribute_profile_field\":\"objc3_runtime_property_entry_snapshot.property_attribute_profile\""
           << ",\"ast_source_path\":\"native/objc3c/src/ast/objc3_ast.h\""
           << ",\"sema_source_path\":\"native/objc3c/src/sema/objc3_semantic_passes.cpp\""
           << ",\"sema_pass_manager_source_path\":\"native/objc3c/src/sema/objc3_sema_pass_manager.cpp\""
           << ",\"frontend_pipeline_source_path\":\"native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp\""
           << ",\"frontend_artifacts_source_path\":\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"runtime_internal_header_path\":\"native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h\""
           << ",\"runtime_source_path\":\"native/objc3c/src/runtime/objc3_runtime.cpp\""
           << ",\"source_surface_model\":\""
           << kObjc3RuntimePropertyAtomicitySynthesisReflectionSourceSurfaceModel
           << "\",\"atomicity_fail_closed_model\":\""
           << kObjc3ExecutablePropertyOwnershipAtomicityInteractionModel
           << "\",\"reflection_boundary_model\":\""
           << kObjc3RuntimePropertyAtomicityReflectionBoundaryModel
           << "\",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/property_atomic_ownership_negative.objc3\""
           << ",\"tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/property_accessor_selector_compatibility_negative.objc3\""
           << ",\"tests/tooling/fixtures/native/property_reflection_attribute_compatibility_negative.objc3\""
           << ",\"tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3\"]"
           << ",\"authoritative_probe_paths\":[\"tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp\""
           << ",\"tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp\""
           << ",\"tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp\"]"
           << ",\"explicit_non_goals\":[\"no-public-atomic-property-runtime-abi-widening\""
           << ",\"no-runtime-managed-atomic-storage-semantics-before-lane-b-and-lane-d-implementation\""
           << ",\"no-milestone-specific-scaffolding\"]"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
}

}  // namespace objc3::artifacts::frontend
