#include "artifacts/objc3_frontend_artifact_runtime_block_ownership_manifest.h"

#include <ostream>

#include "ast/objc3_ast.h"
#include "ast/objc3_ast_contracts.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"

namespace objc3::artifacts::frontend {

void WriteRuntimeOwnershipTransferCaptureFamilySourceSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure) {
  manifest << "  \"runtime_ownership_transfer_capture_family_source_surface\":{\"contract_id\":\""
           << kObjc3RuntimeOwnershipTransferCaptureFamilySourceSurfaceContractId
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
           << "\",\"source_surface_model\":\""
           << kObjc3RuntimeOwnershipTransferCaptureFamilySourceSurfaceModel
           << "\",\"ownership_resource_move_use_after_move_surface_path\":\"frontend.pipeline.semantic_surface.objc_ownership_resource_move_and_use_after_move_semantics\""
           << ",\"ownership_capture_list_retainable_family_surface_path\":\"frontend.pipeline.semantic_surface.objc_ownership_capture_list_and_retainable_family_legality_completion\""
           << ",\"block_capture_ownership_contract_id\":\""
           << Expr::kObjc3ExecutableBlockOwnershipSemanticsImplementationContractId
           << "\",\"arc_inference_lifetime_contract_id\":\""
           << Expr::kObjc3ArcInferenceLifetimeContractId
           << "\",\"arc_interaction_semantics_contract_id\":\""
           << Expr::kObjc3ArcInteractionSemanticsContractId
           << "\",\"block_capture_ownership_profile_field\":\"Expr.block_runtime_capture_ownership_profile\""
           << ",\"block_capture_owned_count_field\":\"Expr.block_runtime_owned_object_capture_count\""
           << ",\"block_capture_weak_count_field\":\"Expr.block_runtime_weak_object_capture_count\""
           << ",\"block_capture_unowned_count_field\":\"Expr.block_runtime_unowned_object_capture_count\""
           << ",\"cleanup_ownership_transfer_field\":\"cleanup_ownership_transfer_enforced\""
           << ",\"explicit_capture_ownership_mode_field\":\"explicit_capture_ownership_mode_enforced\""
           << ",\"retainable_family_conflict_field\":\"retainable_family_conflict_enforced\""
           << ",\"authoritative_code_paths\":[\"native/objc3c/src/ast/objc3_ast.h\""
           << ",\"native/objc3c/src/sema/objc3_semantic_passes.cpp\""
           << ",\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"native/objc3c/src/ir/objc3_ir_emitter.cpp\"]"
           << ",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/owned_object_capture_helper_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/owned_object_capture_runtime_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/nonowning_object_capture_helper_elided_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/nonowning_object_capture_runtime_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/weak_object_capture_mutation_negative.objc3\""
           << ",\"tests/tooling/fixtures/native/unowned_object_capture_mutation_negative.objc3\""
           << ",\"tests/tooling/fixtures/native/escaping_block_runtime_hook_owned_capture_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_inference_lifetime_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_cleanup_scope_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_implicit_cleanup_void_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/arc_autorelease_return_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/capture_list_and_retainable_family_legality_completion_positive.objc3\"]"
           << ",\"authoritative_probe_paths\":[\"tests/tooling/runtime/block_runtime_copy_dispose_invoke_probe.cpp\""
           << ",\"tests/tooling/runtime/block_runtime_byref_forwarding_probe.cpp\""
           << ",\"tests/tooling/runtime/arc_debug_instrumentation_probe.cpp\"]"
           << ",\"explicit_non_goals\":[\"no-parallel-semantics-path\""
           << ",\"no-milestone-specific-scaffolding\""
           << ",\"no-lowering-owned-reinterpretation-of-capture-family-truth\"]"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
}

}  // namespace objc3::artifacts::frontend
