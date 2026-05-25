#include "artifacts/objc3_frontend_artifact_executable_synthesized_accessor_manifest.h"
#include "artifacts/identity/artifact_identity.h"
#include <ostream>
#include "artifacts/objc3_frontend_artifact_executable_accessor_layout_manifest_contracts.h"
#include "artifacts/objc3_frontend_runtime_metadata_section_artifacts.h"
#include "ast/objc3_ast_contracts.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"
#include "runtime/metadata/selector_metadata_registration_manifest.h"


namespace objc3::artifacts::frontend {

void WriteExecutableSynthesizedAccessorPropertyLoweringSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3ExecutableAccessorLayoutLoweringSummary
        &executable_accessor_layout_lowering_summary,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication) {
  manifest << "  \"executable_synthesized_accessor_property_lowering_surface\":{\"contract_id\":\""
           << kExecutableAccessorLayoutSynthesizedAccessorPropertyLoweringContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"object_artifact\":\""
           << objc3::artifacts::identity::BuildObjc3NativeObjectArtifactName(runtime_state_publication_emit_prefix)
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"executable_property_accessor_layout_lowering_surface_contract_id\":\""
           << kExecutableAccessorLayoutPropertyLoweringContractId
           << "\",\"dispatch_and_synthesized_accessor_lowering_surface_contract_id\":\""
           << kExecutableAccessorLayoutDispatchAndSynthesizedAccessorLoweringSurfaceContractId
           << "\",\"source_model\":\""
           << kExecutableAccessorLayoutSynthesizedAccessorSourceModel
           << "\",\"storage_model\":\""
           << kExecutableAccessorLayoutSynthesizedAccessorStorageModel
           << "\",\"property_descriptor_model\":\""
           << kExecutableAccessorLayoutSynthesizedAccessorPropertyDescriptorModel
           << "\",\"fail_closed_model\":\""
           << kExecutableAccessorLayoutSynthesizedAccessorFailClosedModel
           << "\",\"lowering_contract_source_path\":\"native/objc3c/src/lower/objc3_lowering_contract.h\""
           << ",\"ir_emitter_source_path\":\"native/objc3c/src/ir/objc3_ir_emitter.cpp\""
           << ",\"frontend_artifacts_source_path\":\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"runtime_source_path\":\"native/objc3c/src/runtime/objc3_runtime.cpp\""
           << ",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/property_synthesis_default_ivar_binding_no_redeclaration.objc3\""
           << ",\"tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_property_interaction_positive.objc3\"]"
           << ",\"authoritative_probe_paths\":[\"tests/tooling/runtime/synthesized_accessor_probe.cpp\""
           << ",\"tests/tooling/runtime/property_layout_runtime_probe.cpp\""
           << ",\"tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp\""
           << ",\"tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp\""
           << ",\"tests/tooling/runtime/arc_debug_instrumentation_probe.cpp\"]"
           << ",\"explicit_non_goals\":[\"no-public-runtime-abi-widening\""
           << ",\"no-milestone-specific-scaffolding\""
           << ",\"no-storage-global-retired-routes-or-sidecar-body-proof\"]"
           << ",\"implementation_owned_property_entries\":"
           << executable_accessor_layout_lowering_summary
                  .implementation_owned_property_entries
           << ",\"synthesized_getter_entries\":"
           << executable_accessor_layout_lowering_summary.synthesized_getter_entries
           << ",\"synthesized_setter_entries\":"
           << executable_accessor_layout_lowering_summary.synthesized_setter_entries
           << ",\"synthesized_accessor_entries\":"
           << executable_accessor_layout_lowering_summary
                  .synthesized_accessor_entries
           << ",\"property_descriptor_entries\":"
           << runtime_metadata_section_publication.property_descriptor_count
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
}

}  // namespace objc3::artifacts::frontend
