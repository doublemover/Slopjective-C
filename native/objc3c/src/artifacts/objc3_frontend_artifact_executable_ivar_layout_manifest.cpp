#include "artifacts/objc3_frontend_artifact_executable_ivar_layout_manifest.h"

#include <ostream>

#include "artifacts/objc3_frontend_artifact_executable_accessor_layout_manifest_contracts.h"
#include "artifacts/objc3_frontend_runtime_metadata_section_artifacts.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"

namespace objc3::artifacts::frontend {

void WriteExecutableIvarLayoutEmissionSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3ExecutableAccessorLayoutLoweringSummary
        &executable_accessor_layout_lowering_summary,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication) {
  manifest << "  \"executable_ivar_layout_emission_surface\":{\"contract_id\":\""
           << kExecutableAccessorLayoutIvarEmissionContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"executable_property_accessor_layout_lowering_surface_contract_id\":\""
           << kExecutableAccessorLayoutPropertyLoweringContractId
           << "\",\"descriptor_model\":\""
           << kExecutableAccessorLayoutIvarDescriptorModel
           << "\",\"offset_global_model\":\""
           << kExecutableAccessorLayoutIvarOffsetGlobalModel
           << "\",\"layout_table_model\":\""
           << kExecutableAccessorLayoutIvarLayoutTableModel
           << "\",\"scope_model\":\""
           << kExecutableAccessorLayoutIvarEmissionScopeModel
           << "\",\"fail_closed_model\":\""
           << kExecutableAccessorLayoutIvarEmissionFailClosedModel
           << "\",\"lowering_contract_source_path\":\"native/objc3c/src/lower/objc3_lowering_contract.h\""
           << ",\"ir_emitter_source_path\":\"native/objc3c/src/ir/objc3_ir_emitter.cpp\""
           << ",\"frontend_artifacts_source_path\":\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/property_ivar_source_model_completion_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3\"]"
           << ",\"authoritative_probe_paths\":[\"tests/tooling/runtime/property_layout_runtime_probe.cpp\""
           << ",\"tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp\"]"
           << ",\"explicit_non_goals\":[\"no-public-runtime-abi-widening\""
           << ",\"no-milestone-specific-scaffolding\""
           << ",\"no-runtime-layout-rederivation\"]"
           << ",\"offset_global_entries\":"
           << executable_accessor_layout_lowering_summary.ivar_layout_entries
           << ",\"layout_table_entries\":"
           << executable_accessor_layout_lowering_summary.ivar_layout_owner_entries
           << ",\"layout_owner_entries\":"
           << executable_accessor_layout_lowering_summary.ivar_layout_owner_entries
           << ",\"ivar_descriptor_entries\":"
           << runtime_metadata_section_publication.ivar_descriptor_count
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
}

}  // namespace objc3::artifacts::frontend
