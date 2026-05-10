#include "artifacts/objc3_frontend_artifact_runtime_block_manifest.h"

#include <ostream>

#include "ast/objc3_ast.h"
#include "ast/objc3_ast_contracts.h"
#include "lower/contracts/block_runtime_helper_contracts.h"
#include "lower/contracts/ownership_runtime_accessor_helper_contracts.h"
#include "lower/contracts/ownership_runtime_memory_management_contracts.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"
#include "runtime/metadata/selector_metadata.h"

namespace objc3::artifacts::frontend {

void WriteRuntimeBlockArcUnifiedSourceSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure) {
  manifest << "  \"runtime_block_arc_unified_source_surface\":{\"contract_id\":\""
           << kObjc3RuntimeBlockArcUnifiedSourceSurfaceContractId
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
           << "\",\"source_surface_model\":\""
           << kObjc3RuntimeBlockArcUnifiedSourceSurfaceModel
           << "\",\"source_contract_ids\":[\""
           << Expr::kObjc3ExecutableBlockSourceClosureContractId
           << "\",\""
           << Expr::kObjc3ExecutableBlockSourceModelCompletionContractId
           << "\",\""
           << Expr::kObjc3ExecutableBlockSourceStorageAnnotationContractId
           << "\",\""
           << Expr::kObjc3ExecutableBlockRuntimeSemanticRulesContractId
           << "\",\""
           << Expr::kObjc3ExecutableBlockCaptureLegalityImplementationContractId
           << "\",\""
           << Expr::kObjc3ExecutableBlockOwnershipSemanticsImplementationContractId
           << "\",\""
           << Expr::kObjc3ExecutableBlockObjectInvokeThunkLoweringContractId
           << "\",\""
           << Expr::kObjc3ExecutableBlockByrefHelperLoweringContractId
           << "\",\""
           << Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId
           << "\",\""
           << Expr::kObjc3ArcSourceModeBoundaryContractId
           << "\",\""
           << Expr::kObjc3ArcModeHandlingContractId
           << "\",\""
           << Expr::kObjc3ArcSemanticRulesContractId
           << "\",\""
           << Expr::kObjc3ArcInferenceLifetimeContractId
           << "\",\""
           << Expr::kObjc3ArcInteractionSemanticsContractId
           << "\"]"
           << ",\"authoritative_code_paths\":[\"native/objc3c/src/ast/objc3_ast.h\""
           << ",\"native/objc3c/src/sema/objc3_semantic_passes.cpp\""
           << ",\"native/objc3c/src/ir/objc3_ir_emitter.cpp\""
           << ",\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h\""
           << ",\"native/objc3c/src/runtime/objc3_runtime.cpp\"]"
           << ",\"authoritative_source_fields\":[\"frontend.pipeline.semantic_surface.objc_arc_diagnostics_fixit_lowering_surface\""
           << ",\"frontend.pipeline.semantic_surface.objc_block_literal_capture_lowering_surface\""
           << ",\"frontend.pipeline.semantic_surface.objc_block_source_model_completion_surface\""
           << ",\"frontend.pipeline.semantic_surface.objc_block_source_storage_annotation_surface\""
           << ",\"frontend.pipeline.semantic_surface.objc_block_abi_invoke_trampoline_lowering_surface\""
           << ",\"frontend.pipeline.semantic_surface.objc_block_storage_escape_lowering_surface\""
           << ",\"frontend.pipeline.semantic_surface.objc_block_copy_dispose_lowering_surface\""
           << ",\"llvm_ir_summary.executable_block_object_invoke_thunk_lowering\""
           << ",\"llvm_ir_summary.executable_block_byref_helper_lowering\""
           << ",\"llvm_ir_summary.executable_block_escape_runtime_hook_lowering\""
           << ",\"llvm_ir_summary.runtime_block_api_object_layout\""
           << ",\"llvm_ir_summary.runtime_block_allocation_copy_dispose_invoke_support\""
           << ",\"llvm_ir_summary.runtime_block_byref_forwarding_heap_promotion_ownership_interop\""
           << ",\"runtime_api.objc3_runtime_promote_block_i32\""
           << ",\"runtime_api.objc3_runtime_invoke_block_i32\""
           << ",\"runtime_api.objc3_runtime_copy_arc_debug_state_for_testing\""
           << ",\"runtime_api.objc3_runtime_copy_block_arc_runtime_abi_snapshot_for_testing\"]"
           << ",\"block_runtime_boundary_model\":\""
           << Expr::kObjc3ExecutableBlockByrefMutationOwnershipModel
           << "\""
           << ",\"arc_runtime_boundary_model\":\""
           << Expr::kObjc3ArcInteractionSemanticsSemanticModel
           << "\""
           << ",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/block_source_model_completion_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/block_source_storage_annotations_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/capture_legality_escape_invocation_bad_call.objc3\""
           << ",\"tests/tooling/fixtures/native/capture_legality_escape_invocation_missing_capture.objc3\""
           << ",\"tests/tooling/fixtures/native/byref_cell_copy_dispose_runtime_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/escaping_block_runtime_hook_argument_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/escaping_block_runtime_hook_return_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/escaping_block_runtime_hook_byref_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/escaping_block_runtime_hook_owned_capture_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/executable_block_object_invoke_thunk_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_mode_handling_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_inference_lifetime_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_cleanup_scope_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_implicit_cleanup_void_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_autorelease_return_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_property_interaction_positive.objc3\"]"
           << ",\"authoritative_probe_paths\":[\"tests/tooling/runtime/block_runtime_copy_dispose_invoke_probe.cpp\""
           << ",\"tests/tooling/runtime/block_runtime_byref_forwarding_probe.cpp\""
           << ",\"tests/tooling/runtime/arc_debug_instrumentation_probe.cpp\""
           << ",\"tests/tooling/runtime/block_arc_runtime_abi_probe.cpp\"]"
           << ",\"explicit_non_goals\":[\"no-public-block-object-abi-widening\""
           << ",\"no-milestone-specific-scaffolding\""
           << ",\"no-sidecar-only-block-or-arc-proof\"]"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
}

void WriteRuntimeBlockArcLoweringHelperSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure) {
  manifest << "  \"runtime_block_arc_lowering_helper_surface\":{\"contract_id\":\""
           << kObjc3RuntimeBlockArcLoweringHelperSurfaceContractId
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
           << "\",\"block_arc_unified_source_surface_contract_id\":\""
           << kObjc3RuntimeBlockArcUnifiedSourceSurfaceContractId
           << "\",\"runtime_block_arc_runtime_abi_surface_contract_id\":\""
           << kObjc3RuntimeBlockArcRuntimeAbiSurfaceContractId
           << "\",\"ownership_transfer_capture_family_source_surface_contract_id\":\""
           << kObjc3RuntimeOwnershipTransferCaptureFamilySourceSurfaceContractId
           << "\",\"block_object_invoke_thunk_lowering_contract_id\":\""
           << Expr::kObjc3ExecutableBlockObjectInvokeThunkLoweringContractId
           << "\",\"block_byref_helper_lowering_contract_id\":\""
           << Expr::kObjc3ExecutableBlockByrefHelperLoweringContractId
           << "\",\"block_escape_runtime_hook_lowering_contract_id\":\""
           << Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId
           << "\",\"arc_mode_handling_contract_id\":\""
           << Expr::kObjc3ArcModeHandlingContractId
           << "\",\"arc_semantic_rules_contract_id\":\""
           << Expr::kObjc3ArcSemanticRulesContractId
           << "\",\"arc_inference_lifetime_contract_id\":\""
           << Expr::kObjc3ArcInferenceLifetimeContractId
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"lowering_helper_surface_model\":\""
           << kObjc3RuntimeBlockArcLoweringHelperSurfaceModel
           << "\",\"semantic_surface_paths\":[\"frontend.pipeline.semantic_surface.objc_block_abi_invoke_trampoline_lowering_surface\""
           << ",\"frontend.pipeline.semantic_surface.objc_block_storage_escape_lowering_surface\""
           << ",\"frontend.pipeline.semantic_surface.objc_block_copy_dispose_lowering_surface\""
           << ",\"frontend.pipeline.semantic_surface.objc_arc_diagnostics_fixit_lowering_surface\"]"
           << ",\"manifest_lowering_paths\":[\"lowering_block_abi_invoke_trampoline\""
           << ",\"lowering_block_storage_escape\""
           << ",\"lowering_block_copy_dispose\"]"
           << ",\"llvm_ir_summary_paths\":[\"llvm_ir_summary.executable_block_object_invoke_thunk_lowering\""
           << ",\"llvm_ir_summary.executable_block_byref_helper_lowering\""
           << ",\"llvm_ir_summary.executable_block_escape_runtime_hook_lowering\""
           << ",\"llvm_ir_summary.arc_cleanup_weak_lifetime_hooks\""
           << ",\"llvm_ir_summary.arc_block_autorelease_return_lowering\"]"
           << ",\"runtime_api_paths\":[\"runtime_api.objc3_runtime_promote_block_i32\""
           << ",\"runtime_api.objc3_runtime_invoke_block_i32\""
           << ",\"runtime_api.objc3_runtime_copy_arc_debug_state_for_testing\""
           << ",\"runtime_api.objc3_runtime_copy_block_arc_runtime_abi_snapshot_for_testing\"]"
           << ",\"authoritative_code_paths\":[\"native/objc3c/src/ast/objc3_ast.h\""
           << ",\"native/objc3c/src/ir/objc3_ir_emitter.cpp\""
           << ",\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h\""
           << ",\"native/objc3c/src/runtime/objc3_runtime.cpp\"]"
           << ",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/byref_cell_copy_dispose_runtime_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/owned_object_capture_runtime_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/nonowning_object_capture_runtime_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/escaping_block_runtime_hook_argument_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/escaping_block_runtime_hook_return_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/escaping_block_runtime_hook_byref_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/escaping_block_runtime_hook_owned_capture_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/executable_block_object_invoke_thunk_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_mode_handling_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_inference_lifetime_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_cleanup_scope_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_implicit_cleanup_void_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_autorelease_return_positive.objc3\"]"
           << ",\"authoritative_probe_paths\":[\"tests/tooling/runtime/block_runtime_copy_dispose_invoke_probe.cpp\""
           << ",\"tests/tooling/runtime/block_runtime_byref_forwarding_probe.cpp\""
           << ",\"tests/tooling/runtime/arc_debug_instrumentation_probe.cpp\""
           << ",\"tests/tooling/runtime/block_arc_runtime_abi_probe.cpp\"]"
           << ",\"explicit_non_goals\":[\"no-cross-module-packaging-claims\""
           << ",\"no-public-block-abi-widening\""
           << ",\"no-milestone-specific-scaffolding\"]"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
}

}  // namespace objc3::artifacts::frontend
