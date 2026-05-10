#include "artifacts/objc3_frontend_artifact_dispatch_accessor_lowering_manifest.h"

#include <ostream>

#include "artifacts/objc3_frontend_runtime_metadata_section_artifacts.h"
#include "ast/objc3_ast_contracts.h"
#include "lower/contracts/dispatch_surface_classification_contracts.h"
#include "lower/contracts/message_send_selector_lowering_contracts.h"
#include "lower/contracts/object_model_lowering_contracts.h"
#include "lower/contracts/ownership_runtime_accessor_helper_contracts.h"
#include "lower/contracts/runtime_dispatch_abi_contracts.h"
#include "pipeline/objc3_frontend_types.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"

namespace objc3::artifacts::frontend {

void WriteDispatchAndSynthesizedAccessorLoweringSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeLinkHostLinkContract &runtime_link_host_link_contract,
    const Objc3FrontendOptions &options,
    const Objc3RuntimeSupportLibraryLinkWiringSummary
        &runtime_support_library_link_wiring,
    const Objc3DispatchSurfaceClassificationContract
        &dispatch_surface_classification_contract,
    const Objc3MessageSendSelectorLoweringContract
        &message_send_selector_lowering_contract,
    const Objc3PropertySynthesisIvarBindingContract
        &property_synthesis_ivar_binding_contract,
    const Objc3AccessorStorageLoweringMetadataSummary
        &accessor_storage_lowering_metadata_summary,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication) {
  manifest << "  \"dispatch_and_synthesized_accessor_lowering_surface\":{\"contract_id\":"
           << "\"" << kObjc3DispatchAndSynthesizedAccessorLoweringSurfaceContractId << "\""
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
           << runtime_link_host_link_contract.runtime_dispatch_symbol
           << "\",\"runtime_dispatch_arg_slots\":"
           << runtime_link_host_link_contract.runtime_dispatch_arg_slots
           << ",\"runtime_dispatch_declaration_parameter_count\":"
           << runtime_link_host_link_contract.runtime_dispatch_declaration_parameter_count
           << ",\"runtime_dispatch_symbol_matches_lowering\":"
           << ((runtime_link_host_link_contract.runtime_dispatch_symbol ==
                        options.lowering.runtime_dispatch_symbol &&
                runtime_link_host_link_contract.runtime_dispatch_symbol ==
                        runtime_support_library_link_wiring.runtime_dispatch_symbol)
                       ? "true"
                       : "false")
           << ",\"live_runtime_dispatch_sites\":"
           << (dispatch_surface_classification_contract.instance_dispatch_sites +
               dispatch_surface_classification_contract.class_dispatch_sites +
               dispatch_surface_classification_contract.super_dispatch_sites +
               dispatch_surface_classification_contract.dynamic_dispatch_sites)
           << ",\"direct_dispatch_sites\":"
           << dispatch_surface_classification_contract.direct_dispatch_sites
           << ",\"message_send_sites\":"
           << message_send_selector_lowering_contract.message_send_sites
           << ",\"property_synthesis_sites\":"
           << property_synthesis_ivar_binding_contract.property_synthesis_sites
           << ",\"property_synthesis_explicit_ivar_bindings\":"
           << property_synthesis_ivar_binding_contract.property_synthesis_explicit_ivar_bindings
           << ",\"property_synthesis_default_ivar_bindings\":"
           << property_synthesis_ivar_binding_contract.property_synthesis_default_ivar_bindings
           << ",\"interface_owned_property_synthesis_sites\":"
           << property_synthesis_ivar_binding_contract.interface_owned_property_synthesis_sites
           << ",\"implementation_property_redeclaration_sites\":"
           << property_synthesis_ivar_binding_contract.implementation_property_redeclaration_sites
           << ",\"ivar_binding_resolved\":"
           << property_synthesis_ivar_binding_contract.ivar_binding_resolved
           << ",\"runtime_property_ivar_storage_accessor_source_surface_contract_id\":\""
           << kObjc3RuntimePropertyIvarStorageAccessorSourceSurfaceContractId
           << "\",\"storage_accessor_runtime_abi_surface_contract_id\":\""
           << kObjc3RuntimeStorageAccessorAbiSurfaceContractId
           << "\""
           << ",\"accessor_storage_lowering_metadata_model\":\""
           << kObjc3AccessorStorageLoweringMetadataModel
           << "\",\"accessor_storage_lowering_helper_selection_model\":\""
           << kObjc3AccessorStorageLoweringHelperSelectionModel
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
           << kObjc3RuntimeReadCurrentPropertyI32Symbol
           << "\",\"current_property_write_symbol\":\""
           << kObjc3RuntimeWriteCurrentPropertyI32Symbol
           << "\",\"current_property_exchange_symbol\":\""
           << kObjc3RuntimeExchangeCurrentPropertyI32Symbol
           << "\",\"weak_current_property_load_symbol\":\""
           << kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol
           << "\",\"weak_current_property_store_symbol\":\""
           << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol
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
           << (property_synthesis_ivar_binding_contract.deterministic &&
                       dispatch_surface_classification_contract.deterministic &&
                       message_send_selector_lowering_contract.deterministic &&
                       runtime_link_host_link_contract.deterministic
                   ? "true"
                   : "false")
           << "},\n";
}

}  // namespace objc3::artifacts::frontend
