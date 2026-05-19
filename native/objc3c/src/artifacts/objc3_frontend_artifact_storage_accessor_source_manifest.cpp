#include "artifacts/objc3_frontend_artifact_storage_accessor_source_manifest.h"

#include <ostream>

#include "artifacts/objc3_frontend_artifact_storage_accessor_manifest_contracts.h"
#include "ast/objc3_ast_contracts_source_property_metadata.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"
#include "runtime/metadata/selector_metadata_registration_descriptor_surfaces.h"
#include "runtime/metadata/selector_metadata_registration_manifest.h"

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
           << kStorageAccessorDispatchAndSynthesizedAccessorLoweringSurfaceContractId
           << "\",\"executable_property_accessor_layout_lowering_contract_id\":\""
           << kStorageAccessorExecutablePropertyAccessorLayoutLoweringContractId
           << "\",\"executable_ivar_layout_emission_contract_id\":\""
           << kStorageAccessorExecutableIvarLayoutEmissionContractId
           << "\",\"executable_synthesized_accessor_property_lowering_contract_id\":\""
           << kStorageAccessorExecutableSynthesizedAccessorPropertyLoweringContractId
           << "\",\"storage_accessor_runtime_abi_surface_contract_id\":\""
           << kObjc3RuntimeStorageAccessorAbiSurfaceContractId
           << "\""
           << ",\"accessor_storage_lowering_metadata_model\":\""
           << kStorageAccessorLoweringMetadataModel
           << "\",\"accessor_storage_lowering_helper_selection_model\":\""
           << kStorageAccessorLoweringHelperSelectionModel
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

}  // namespace objc3::artifacts::frontend
