#include "artifacts/objc3_frontend_artifact_dispatch_accessor_lowering_manifest.h"

#include <ostream>

#include "artifacts/objc3_frontend_artifact_dispatch_accessor_manifest_contracts.h"
#include "artifacts/objc3_frontend_artifact_storage_accessor_manifest_contracts.h"
#include "artifacts/objc3_frontend_runtime_metadata_section_artifacts.h"
#include "ast/objc3_ast_contracts.h"
#include "ast/objc3_ast_contracts_source_property_metadata.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"
#include "runtime/metadata/runtime_metadata_section_surfaces.h"
#include "runtime/metadata/selector_metadata_registration_manifest.h"

namespace objc3::artifacts::frontend {

void WriteDispatchAndSynthesizedAccessorLoweringSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3DispatchAndSynthesizedAccessorLoweringFields
        &dispatch_and_synthesized_accessor_lowering_fields,
    const Objc3AccessorStorageLoweringMetadataSummary
        &accessor_storage_lowering_metadata_summary,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication) {
  manifest << "  \"dispatch_and_synthesized_accessor_lowering_surface\":{\"contract_id\":"
           << "\"" << kDispatchAccessorLoweringSurfaceContractId << "\""
           << ",\"compile_manifest_artifact\":\""
           << runtime_state_publication_emit_prefix << ".manifest.json"
           << "\",\"registration_manifest_artifact\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"object_artifact\":\""
           << runtime_state_publication_emit_prefix << ".obj"
           << "\",\"backend_artifact\":\""
           << runtime_state_publication_emit_prefix << ".ll\""
           << ",\"runtime_dispatch_symbol\":\""
           << dispatch_and_synthesized_accessor_lowering_fields
                  .runtime_dispatch_symbol
           << "\",\"runtime_dispatch_arg_slots\":"
           << dispatch_and_synthesized_accessor_lowering_fields
                  .runtime_dispatch_arg_slots
           << ",\"runtime_dispatch_declaration_parameter_count\":"
           << dispatch_and_synthesized_accessor_lowering_fields
                  .runtime_dispatch_declaration_parameter_count
           << ",\"runtime_dispatch_symbol_matches_lowering\":"
           << (dispatch_and_synthesized_accessor_lowering_fields
                       .runtime_dispatch_symbol_matches_lowering
                   ? "true"
                   : "false")
           << ",\"live_runtime_dispatch_sites\":"
           << dispatch_and_synthesized_accessor_lowering_fields
                  .live_runtime_dispatch_sites
           << ",\"direct_dispatch_sites\":"
           << dispatch_and_synthesized_accessor_lowering_fields
                  .direct_dispatch_sites
           << ",\"message_send_sites\":"
           << dispatch_and_synthesized_accessor_lowering_fields.message_send_sites
           << ",\"property_synthesis_sites\":"
           << dispatch_and_synthesized_accessor_lowering_fields
                  .property_synthesis_sites
           << ",\"property_synthesis_explicit_ivar_bindings\":"
           << dispatch_and_synthesized_accessor_lowering_fields
                  .property_synthesis_explicit_ivar_bindings
           << ",\"property_synthesis_default_ivar_bindings\":"
           << dispatch_and_synthesized_accessor_lowering_fields
                  .property_synthesis_default_ivar_bindings
           << ",\"interface_owned_property_synthesis_sites\":"
           << dispatch_and_synthesized_accessor_lowering_fields
                  .interface_owned_property_synthesis_sites
           << ",\"implementation_property_redeclaration_sites\":"
           << dispatch_and_synthesized_accessor_lowering_fields
                  .implementation_property_redeclaration_sites
           << ",\"ivar_binding_resolved\":"
           << dispatch_and_synthesized_accessor_lowering_fields
                  .ivar_binding_resolved
           << ",\"runtime_property_ivar_storage_accessor_source_surface_contract_id\":\""
           << kObjc3RuntimePropertyIvarStorageAccessorSourceSurfaceContractId
           << "\",\"storage_accessor_runtime_abi_surface_contract_id\":\""
           << kObjc3RuntimeStorageAccessorAbiSurfaceContractId
           << "\""
           << ",\"accessor_storage_lowering_metadata_model\":\""
           << kStorageAccessorLoweringMetadataModel
           << "\",\"accessor_storage_lowering_helper_selection_model\":\""
           << kStorageAccessorLoweringHelperSelectionModel
           << "\",\"lowering_contract_source_path\":\"native/objc3c/src/lower/objc3_lowering_contract.h\""
           << ",\"ir_emitter_source_path\":\"native/objc3c/src/ir/objc3_ir_emitter.cpp\""
           << ",\"frontend_artifacts_source_path\":\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"runtime_source_path\":\"native/objc3c/src/runtime/objc3_runtime.cpp\""
           << ",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/property_synthesis_default_ivar_binding_no_redeclaration.objc3\""
           << ",\"tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_property_interaction_positive.objc3\"]"
           << ",\"authoritative_probe_paths\":[\"tests/tooling/runtime/synthesized_accessor_probe.cpp\""
           << ",\"tests/tooling/runtime/property_layout_runtime_probe.cpp\""
           << ",\"tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp\""
           << ",\"tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp\""
           << ",\"tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp\""
           << ",\"tests/tooling/runtime/arc_debug_instrumentation_probe.cpp\"]"
           << ",\"explicit_non_goals\":[\"no-public-runtime-abi-widening\""
           << ",\"no-milestone-specific-scaffolding\""
           << ",\"no-sidecar-only-lowering-proof\"]"
           << ",\"current_property_read_symbol\":\""
           << kStorageAccessorReadCurrentPropertyI32Symbol
           << "\",\"current_property_write_symbol\":\""
           << kStorageAccessorWriteCurrentPropertyI32Symbol
           << "\",\"current_property_exchange_symbol\":\""
           << kStorageAccessorExchangeCurrentPropertyI32Symbol
           << "\",\"weak_current_property_load_symbol\":\""
           << kStorageAccessorLoadWeakCurrentPropertyI32Symbol
           << "\",\"weak_current_property_store_symbol\":\""
           << kStorageAccessorStoreWeakCurrentPropertyI32Symbol
           << "\",\"synthesized_accessor_owner_entries\":"
           << accessor_storage_lowering_metadata_summary
                  .synthesized_accessor_owner_entries
           << ",\"synthesized_getter_entries\":"
           << accessor_storage_lowering_metadata_summary
                  .synthesized_getter_entries
           << ",\"synthesized_setter_entries\":"
           << accessor_storage_lowering_metadata_summary
                  .synthesized_setter_entries
           << ",\"current_property_read_entries\":"
           << accessor_storage_lowering_metadata_summary
                  .current_property_read_entries
           << ",\"current_property_write_entries\":"
           << accessor_storage_lowering_metadata_summary
                  .current_property_write_entries
           << ",\"current_property_exchange_entries\":"
           << accessor_storage_lowering_metadata_summary
                  .current_property_exchange_entries
           << ",\"weak_current_property_load_entries\":"
           << accessor_storage_lowering_metadata_summary
                  .weak_current_property_load_entries
           << ",\"weak_current_property_store_entries\":"
           << accessor_storage_lowering_metadata_summary
                  .weak_current_property_store_entries
           << ",\"property_descriptor_count\":"
           << runtime_metadata_section_publication.property_descriptor_count
           << ",\"ivar_descriptor_count\":"
           << runtime_metadata_section_publication.ivar_descriptor_count
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << ",\"deterministic_handoff\":"
           << (dispatch_and_synthesized_accessor_lowering_fields
                       .deterministic_handoff
                   ? "true"
                   : "false")
           << "},\n";
}

}  // namespace objc3::artifacts::frontend
