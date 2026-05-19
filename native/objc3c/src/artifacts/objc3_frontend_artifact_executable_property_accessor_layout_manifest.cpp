#include "artifacts/objc3_frontend_artifact_executable_property_accessor_layout_manifest.h"

#include <ostream>

#include "artifacts/objc3_frontend_artifact_executable_accessor_layout_manifest_contracts.h"
#include "artifacts/objc3_frontend_runtime_metadata_section_artifacts.h"
#include "ast/objc3_ast_contracts.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"
#include "runtime/metadata/selector_metadata_registration_manifest.h"

namespace objc3::artifacts::frontend {

void WriteExecutablePropertyAccessorLayoutLoweringSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3ExecutableAccessorLayoutLoweringSummary
        &executable_accessor_layout_lowering_summary,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication) {
  manifest << "  \"executable_property_accessor_layout_lowering_surface\":{\"contract_id\":\""
           << kExecutableAccessorLayoutPropertyLoweringContractId
           << "\",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll"
           << "\",\"runtime_property_ivar_storage_accessor_source_surface_contract_id\":\""
           << kObjc3RuntimePropertyIvarStorageAccessorSourceSurfaceContractId
           << "\",\"dispatch_and_synthesized_accessor_lowering_surface_contract_id\":\""
           << kExecutableAccessorLayoutDispatchAndSynthesizedAccessorLoweringSurfaceContractId
           << "\",\"property_table_model\":\""
           << kExecutableAccessorLayoutPropertyTableModel
           << "\",\"ivar_layout_model\":\""
           << kExecutableAccessorLayoutIvarLayoutModel
           << "\",\"accessor_binding_model\":\""
           << kExecutableAccessorLayoutAccessorBindingModel
           << "\",\"scope_model\":\""
           << kExecutableAccessorLayoutScopeModel
           << "\",\"fail_closed_model\":\""
           << kExecutableAccessorLayoutFailClosedModel
           << "\",\"lowering_contract_source_path\":\"native/objc3c/src/lower/objc3_lowering_contract.h\""
           << ",\"ir_emitter_source_path\":\"native/objc3c/src/ir/objc3_ir_emitter.cpp\""
           << ",\"frontend_artifacts_source_path\":\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
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
           << ",\"no-layout-or-accessor-body-rederivation-outside-the-live-lowering-path\"]"
           << ",\"property_metadata_entries\":"
           << executable_accessor_layout_lowering_summary.property_metadata_entries
           << ",\"ivar_metadata_entries\":"
           << executable_accessor_layout_lowering_summary.ivar_metadata_entries
           << ",\"property_descriptor_entries\":"
           << runtime_metadata_section_publication.property_descriptor_count
           << ",\"ivar_descriptor_entries\":"
           << runtime_metadata_section_publication.ivar_descriptor_count
           << ",\"property_attribute_profile_entries\":"
           << executable_accessor_layout_lowering_summary
                  .property_attribute_profile_entries
           << ",\"accessor_ownership_profile_entries\":"
           << executable_accessor_layout_lowering_summary
                  .accessor_ownership_profile_entries
           << ",\"synthesized_binding_entries\":"
           << executable_accessor_layout_lowering_summary
                  .synthesized_binding_entries
           << ",\"ivar_layout_entries\":"
           << executable_accessor_layout_lowering_summary.ivar_layout_entries
           << ",\"ivar_layout_owner_entries\":"
           << executable_accessor_layout_lowering_summary
                  .ivar_layout_owner_entries
           << ",\"descriptor_counts_match_source_graph\":"
           << ((executable_accessor_layout_lowering_summary.property_metadata_entries ==
                        runtime_metadata_section_publication.property_descriptor_count &&
                executable_accessor_layout_lowering_summary.ivar_metadata_entries ==
                        runtime_metadata_section_publication.ivar_descriptor_count)
                   ? "true"
                   : "false")
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
}

}  // namespace objc3::artifacts::frontend
