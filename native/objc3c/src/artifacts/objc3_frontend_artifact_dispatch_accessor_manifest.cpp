#include "artifacts/objc3_frontend_artifact_dispatch_accessor_manifest.h"

#include <ostream>

#include "artifacts/objc3_frontend_runtime_metadata_section_artifacts.h"
#include "ast/objc3_ast_contracts.h"
#include "lower/contracts/dispatch_surface_classification_contracts.h"
#include "lower/contracts/executable_property_layout_contracts.h"
#include "lower/contracts/message_send_selector_lowering_contracts.h"
#include "lower/contracts/object_model_lowering_contracts.h"
#include "lower/contracts/ownership_runtime_accessor_helper_contracts.h"
#include "lower/contracts/ownership_runtime_memory_management_contracts.h"
#include "lower/contracts/runtime_dispatch_abi_contracts.h"
#include "pipeline/objc3_frontend_types.h"
#include "runtime/metadata/class_metadata.h"
#include "runtime/metadata/property_metadata.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"
#include "runtime/metadata/selector_metadata.h"

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

void WriteDispatchAccessorRuntimeAbiSurface(
    std::ostream &manifest,
    const Objc3RuntimeLinkHostLinkContract &runtime_link_host_link_contract,
    const Objc3PropertySynthesisIvarBindingContract
        &property_synthesis_ivar_binding_contract,
    const Objc3DispatchSurfaceClassificationContract
        &dispatch_surface_classification_contract,
    const Objc3MessageSendSelectorLoweringContract
        &message_send_selector_lowering_contract) {
  manifest << "  \"dispatch_accessor_runtime_abi_surface\":{\"contract_id\":"
           << "\"objc3c.runtime.dispatch_accessor.abi.surface.v1\""
           << ",\"abi_boundary_model\":"
           << "\"public-dispatch-entrypoint-plus-private-testing-snapshot-and-property-helper-surface\""
           << ",\"public_header_path\":\"native/objc3c/src/runtime/public/objc3_runtime_api.h\""
           << ",\"private_header_path\":\"native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h\""
           << ",\"runtime_dispatch_symbol\":\""
           << runtime_link_host_link_contract.runtime_dispatch_symbol
           << "\",\"dispatch_state_snapshot_symbol\":\"objc3_runtime_copy_dispatch_state_for_testing\""
           << ",\"method_cache_state_snapshot_symbol\":\"objc3_runtime_copy_method_cache_state_for_testing\""
           << ",\"method_cache_entry_snapshot_symbol\":\"objc3_runtime_copy_method_cache_entry_for_testing\""
           << ",\"property_registry_state_snapshot_symbol\":\"objc3_runtime_copy_property_registry_state_for_testing\""
           << ",\"property_entry_snapshot_symbol\":\"objc3_runtime_copy_property_entry_for_testing\""
           << ",\"arc_debug_state_snapshot_symbol\":\"objc3_runtime_copy_arc_debug_state_for_testing\""
           << ",\"current_property_read_symbol\":\""
           << kObjc3RuntimeReadCurrentPropertyI32Symbol
           << "\",\"current_property_write_symbol\":\""
           << kObjc3RuntimeWriteCurrentPropertyI32Symbol
           << "\",\"current_property_exchange_symbol\":\""
           << kObjc3RuntimeExchangeCurrentPropertyI32Symbol
           << "\",\"bind_current_property_context_symbol\":\"objc3_runtime_bind_current_property_context_for_testing\""
           << ",\"clear_current_property_context_symbol\":\"objc3_runtime_clear_current_property_context_for_testing\""
           << ",\"weak_current_property_load_symbol\":\""
           << kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol
           << "\",\"weak_current_property_store_symbol\":\""
           << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol
           << "\",\"retain_symbol\":\"" << kObjc3RuntimeRetainI32Symbol
           << "\",\"release_symbol\":\"" << kObjc3RuntimeReleaseI32Symbol
           << "\",\"autorelease_symbol\":\""
           << kObjc3RuntimeAutoreleaseI32Symbol
           << "\",\"private_testing_surface_only\":true"
           << ",\"deterministic\":"
           << ((property_synthesis_ivar_binding_contract.deterministic &&
                dispatch_surface_classification_contract.deterministic &&
                message_send_selector_lowering_contract.deterministic &&
                runtime_link_host_link_contract.deterministic)
                   ? "true"
                   : "false")
           << "},\n";
}

void WriteStorageAccessorRuntimeAbiSurface(
    std::ostream &manifest,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api,
    const Objc3RuntimeLinkHostLinkContract &runtime_link_host_link_contract,
    const Objc3PropertySynthesisIvarBindingContract
        &property_synthesis_ivar_binding_contract) {
  manifest << "  \"storage_accessor_runtime_abi_surface\":{\"contract_id\":\""
           << kObjc3RuntimeStorageAccessorAbiSurfaceContractId
           << "\",\"abi_boundary_model\":\"private-bootstrap-internal-property-helper-and-reflection-snapshot-surface-without-public-header-widening\""
           << ",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"private_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"property_registry_state_snapshot_symbol\":\"objc3_runtime_copy_property_registry_state_for_testing\""
           << ",\"property_entry_snapshot_symbol\":\"objc3_runtime_copy_property_entry_for_testing\""
           << ",\"current_property_read_symbol\":\""
           << kObjc3RuntimeReadCurrentPropertyI32Symbol
           << "\",\"current_property_write_symbol\":\""
           << kObjc3RuntimeWriteCurrentPropertyI32Symbol
           << "\",\"current_property_exchange_symbol\":\""
           << kObjc3RuntimeExchangeCurrentPropertyI32Symbol
           << "\",\"bind_current_property_context_symbol\":\"objc3_runtime_bind_current_property_context_for_testing\""
           << ",\"clear_current_property_context_symbol\":\"objc3_runtime_clear_current_property_context_for_testing\""
           << ",\"weak_current_property_load_symbol\":\""
           << kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol
           << "\",\"weak_current_property_store_symbol\":\""
           << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol
           << "\",\"private_testing_surface_only\":true"
           << ",\"deterministic\":"
           << ((property_synthesis_ivar_binding_contract.deterministic &&
                runtime_link_host_link_contract.deterministic)
                   ? "true"
                   : "false")
           << "},\n";
}

void WriteRuntimePropertyIvarStorageAccessorSourceSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure) {
  manifest << "  \"runtime_property_ivar_storage_accessor_source_surface\":{\"contract_id\":\""
           << kObjc3RuntimePropertyIvarStorageAccessorSourceSurfaceContractId
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
           << "\",\"property_ast_anchor\":\""
           << kObjc3RuntimeMetadataPropertyAstAnchor
           << "\",\"ivar_ast_anchor\":\""
           << kObjc3RuntimeMetadataIvarAstAnchor
           << "\",\"source_closure_contract_id\":\""
           << kObjc3ExecutablePropertyIvarSourceClosureContractId
           << "\",\"source_model_completion_contract_id\":\""
           << kObjc3ExecutablePropertyIvarSourceModelCompletionContractId
           << "\",\"source_semantics_contract_id\":\""
           << kObjc3ExecutablePropertyIvarSemanticsContractId
           << "\",\"source_surface_model\":\""
           << kObjc3RuntimePropertyIvarStorageAccessorSourceSurfaceModel
           << "\",\"layout_model\":\""
           << kObjc3ExecutablePropertyIvarLayoutModel
           << "\",\"attribute_model\":\""
           << kObjc3ExecutablePropertyAttributeModel
           << "\",\"synthesis_semantics_model\":\""
           << kObjc3ExecutablePropertySynthesisSemanticsModel
           << "\",\"default_ivar_binding_resolution_model\":\""
           << kObjc3ExecutablePropertyDefaultIvarBindingResolutionModel
           << "\",\"accessor_semantics_model\":\""
           << kObjc3ExecutablePropertyAccessorSemanticsModel
           << "\",\"accessor_selector_uniqueness_model\":\""
           << kObjc3ExecutablePropertyAccessorSelectorUniquenessModel
           << "\",\"ownership_atomicity_interaction_model\":\""
           << kObjc3ExecutablePropertyOwnershipAtomicityInteractionModel
           << "\",\"storage_semantics_model\":\""
           << kObjc3ExecutablePropertyStorageSemanticsModel
           << "\",\"layout_init_order_field\":\"Objc3PropertyDecl.executable_ivar_init_order_index\""
           << ",\"layout_destroy_order_field\":\"Objc3PropertyDecl.executable_ivar_destroy_order_index\""
           << ",\"synthesizes_accessors_field\":\"Objc3RuntimeMetadataPropertySourceRecord.synthesizes_executable_accessors\""
           << ",\"getter_runtime_helper_field\":\"Objc3RuntimeMetadataPropertySourceRecord.getter_storage_runtime_helper_symbol\""
           << ",\"setter_runtime_helper_field\":\"Objc3RuntimeMetadataPropertySourceRecord.setter_storage_runtime_helper_symbol\""
           << ",\"dispatch_and_synthesized_accessor_lowering_surface_contract_id\":\""
           << kObjc3DispatchAndSynthesizedAccessorLoweringSurfaceContractId
           << "\",\"executable_property_accessor_layout_lowering_contract_id\":\""
           << kObjc3ExecutablePropertyAccessorLayoutLoweringContractId
           << "\",\"executable_ivar_layout_emission_contract_id\":\""
           << kObjc3ExecutableIvarLayoutEmissionContractId
           << "\",\"executable_synthesized_accessor_property_lowering_contract_id\":\""
           << kObjc3ExecutableSynthesizedAccessorPropertyLoweringContractId
           << "\",\"storage_accessor_runtime_abi_surface_contract_id\":\""
           << kObjc3RuntimeStorageAccessorAbiSurfaceContractId
           << "\""
           << ",\"accessor_storage_lowering_metadata_model\":\""
           << kObjc3AccessorStorageLoweringMetadataModel
           << "\",\"accessor_storage_lowering_helper_selection_model\":\""
           << kObjc3AccessorStorageLoweringHelperSelectionModel
           << "\",\"compatibility_semantics_model\":\""
           << kObjc3ExecutablePropertyCompatibilitySemanticsModel
           << "\",\"ast_source_path\":\"native/objc3c/src/ast/objc3_ast.h\""
           << ",\"lowering_contract_source_path\":\"native/objc3c/src/lower/objc3_lowering_contract.h\""
           << ",\"sema_source_path\":\"native/objc3c/src/sema/objc3_semantic_passes.cpp\""
           << ",\"frontend_pipeline_source_path\":\"native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp\""
           << ",\"ir_emitter_source_path\":\"native/objc3c/src/ir/objc3_ir_emitter.cpp\""
           << ",\"frontend_artifacts_source_path\":\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"runtime_source_path\":\"native/objc3c/src/runtime/objc3_runtime.cpp\""
           << ",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/property_accessor_selector_compatibility_negative.objc3\""
           << ",\"tests/tooling/fixtures/native/property_reflection_attribute_compatibility_negative.objc3\""
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
           << ",\"no-lowering-owned-storage-or-accessor-semantics-invention\"]"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
}

void WriteRuntimePropertyIvarAccessorReflectionImplementationSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api) {
  manifest << "  \"runtime_property_ivar_accessor_reflection_implementation_surface\":{\"contract_id\":\""
           << kObjc3RuntimePropertyIvarAccessorReflectionImplementationSurfaceContractId
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
           << "\",\"runtime_property_ivar_storage_accessor_source_surface_contract_id\":\""
           << kObjc3RuntimePropertyIvarStorageAccessorSourceSurfaceContractId
           << "\",\"storage_accessor_runtime_abi_surface_contract_id\":\""
           << kObjc3RuntimeStorageAccessorAbiSurfaceContractId
           << "\",\"property_metadata_reflection_contract_id\":\""
           << kObjc3RuntimePropertyMetadataReflectionContractId
           << "\",\"runtime_backed_object_ownership_attribute_surface_contract_id\":\""
           << kObjc3RuntimeBackedObjectOwnershipAttributeSurfaceContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"implementation_snapshot_symbol\":\"objc3_runtime_copy_storage_accessor_implementation_snapshot_for_testing\""
           << ",\"property_registry_state_snapshot_symbol\":\"objc3_runtime_copy_property_registry_state_for_testing\""
           << ",\"property_entry_snapshot_symbol\":\"objc3_runtime_copy_property_entry_for_testing\""
           << ",\"current_property_read_symbol\":\""
           << kObjc3RuntimeReadCurrentPropertyI32Symbol
           << "\",\"current_property_write_symbol\":\""
           << kObjc3RuntimeWriteCurrentPropertyI32Symbol
           << "\",\"current_property_exchange_symbol\":\""
           << kObjc3RuntimeExchangeCurrentPropertyI32Symbol
           << "\",\"bind_current_property_context_symbol\":\"objc3_runtime_bind_current_property_context_for_testing\""
           << ",\"clear_current_property_context_symbol\":\"objc3_runtime_clear_current_property_context_for_testing\""
           << ",\"weak_current_property_load_symbol\":\""
           << kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol
           << "\",\"weak_current_property_store_symbol\":\""
           << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol
           << "\",\"implementation_model\":\""
           << kObjc3RuntimePropertyIvarAccessorReflectionImplementationModel
           << "\",\"reflection_model\":\""
           << kObjc3RuntimePropertyIvarAccessorReflectionImplementationReflectionModel
           << "\",\"fail_closed_model\":\""
           << kObjc3RuntimePropertyIvarAccessorReflectionImplementationFailClosedModel
           << "\",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
}

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
           << kObjc3ExecutablePropertyAccessorLayoutLoweringContractId
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
           << kObjc3DispatchAndSynthesizedAccessorLoweringSurfaceContractId
           << "\",\"property_table_model\":\""
           << kObjc3ExecutablePropertyAccessorLayoutLoweringPropertyTableModel
           << "\",\"ivar_layout_model\":\""
           << kObjc3ExecutablePropertyAccessorLayoutLoweringIvarLayoutModel
           << "\",\"accessor_binding_model\":\""
           << kObjc3ExecutablePropertyAccessorLayoutLoweringAccessorBindingModel
           << "\",\"scope_model\":\""
           << kObjc3ExecutablePropertyAccessorLayoutLoweringScopeModel
           << "\",\"fail_closed_model\":\""
           << kObjc3ExecutablePropertyAccessorLayoutLoweringFailClosedModel
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
           << kObjc3ExecutableIvarLayoutEmissionContractId
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
           << kObjc3ExecutablePropertyAccessorLayoutLoweringContractId
           << "\",\"descriptor_model\":\""
           << kObjc3ExecutableIvarLayoutDescriptorModel
           << "\",\"offset_global_model\":\""
           << kObjc3ExecutableIvarOffsetGlobalModel
           << "\",\"layout_table_model\":\""
           << kObjc3ExecutableIvarLayoutTableModel
           << "\",\"scope_model\":\""
           << kObjc3ExecutableIvarLayoutEmissionScopeModel
           << "\",\"fail_closed_model\":\""
           << kObjc3ExecutableIvarLayoutEmissionFailClosedModel
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
           << kObjc3ExecutableSynthesizedAccessorPropertyLoweringContractId
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
           << kObjc3ExecutablePropertyAccessorLayoutLoweringContractId
           << "\",\"dispatch_and_synthesized_accessor_lowering_surface_contract_id\":\""
           << kObjc3DispatchAndSynthesizedAccessorLoweringSurfaceContractId
           << "\",\"source_model\":\""
           << kObjc3ExecutableSynthesizedAccessorPropertyLoweringSourceModel
           << "\",\"storage_model\":\""
           << kObjc3ExecutableSynthesizedAccessorPropertyLoweringStorageModel
           << "\",\"property_descriptor_model\":\""
           << kObjc3ExecutableSynthesizedAccessorPropertyLoweringPropertyDescriptorModel
           << "\",\"fail_closed_model\":\""
           << kObjc3ExecutableSynthesizedAccessorPropertyLoweringFailClosedModel
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
           << ",\"hard-cut-storage-global-body-proof\"]"
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
           << kObjc3RuntimePropertyMetadataReflectionContractId
           << "\",\"runtime_backed_object_ownership_attribute_surface_contract_id\":\""
           << kObjc3RuntimeBackedObjectOwnershipAttributeSurfaceContractId
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
