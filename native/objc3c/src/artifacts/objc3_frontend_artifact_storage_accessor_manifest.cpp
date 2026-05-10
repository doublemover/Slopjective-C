#include "artifacts/objc3_frontend_artifact_storage_accessor_manifest.h"

#include <ostream>

#include "ast/objc3_ast_contracts.h"
#include "lower/contracts/object_model_lowering_contracts.h"
#include "lower/contracts/ownership_runtime_accessor_helper_contracts.h"
#include "lower/contracts/runtime_dispatch_abi_contracts.h"
#include "lower/contracts/runtime_property_layout_contracts.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"

namespace objc3::artifacts::frontend {

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

}  // namespace objc3::artifacts::frontend
