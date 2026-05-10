#include "artifacts/objc3_frontend_artifact_runtime_concurrency_manifest.h"

#include <ostream>

#include "ast/objc3_ast_contracts.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"

namespace objc3::artifacts::frontend {

void WriteRuntimeUnifiedConcurrencySourceSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure) {
  manifest << "  \"runtime_unified_concurrency_source_surface\":{\"contract_id\":\""
           << kObjc3RuntimeUnifiedConcurrencySourceSurfaceContractId
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
           << "\",\"source_contract_ids\":[\"objc3c.concurrency.async.source.closure.v1\""
           << ",\"objc3c.concurrency.actor.member.isolation.source.closure.v1\""
           << ",\"objc3c.concurrency.task.group.cancellation.source.closure.v1\""
           << ",\"objc3c.concurrency.async.effect.suspension.semantic.model.v1\""
           << ",\"objc3c.concurrency.task.executor.cancellation.semantic.model.v1\""
           << ",\"objc3c.concurrency.actor.isolation.sendable.semantic.model.v1\"]"
           << ",\"authoritative_code_paths\":[\"native/objc3c/src/ast/objc3_ast.h\""
           << ",\"native/objc3c/src/sema/objc3_semantic_passes.cpp\""
           << ",\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"native/objc3c/src/ir/objc3_ir_emitter.cpp\""
           << ",\"native/objc3c/src/runtime/public/objc3_runtime_api.h\""
           << ",\"native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h\""
           << ",\"native/objc3c/src/runtime/objc3_runtime.cpp\"]"
           << ",\"authoritative_source_fields\":[\"frontend.pipeline.semantic_surface.objc_concurrency_async_source_closure\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_actor_member_and_isolation_source_closure\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_task_group_and_cancellation_source_closure\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_async_effect_and_suspension_semantic_model\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_task_executor_and_cancellation_semantic_model\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_actor_isolation_and_sendable_semantic_model\"]"
           << ",\"source_surface_model\":\"unified-concurrency-source-surface-freezes-live-async-actor-task-source-and-sema-boundaries-before-lowering-runtime-and-public-abi-expansion\""
           << ",\"public_runtime_abi_boundary\":[\""
           << kObjc3RuntimeSupportLibraryRegisterImageSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol << "\",\""
           << kObjc3RuntimeSupportLibraryResetForTestingSymbol << "\"]"
           << ",\"private_concurrency_runtime_boundary\":[\"objc3_runtime_allocate_async_continuation_i32\""
           << ",\"objc3_runtime_handoff_async_continuation_to_executor_i32\""
           << ",\"objc3_runtime_resume_async_continuation_i32\""
           << ",\"objc3_runtime_spawn_task_i32\""
           << ",\"objc3_runtime_enter_task_group_scope_i32\""
           << ",\"objc3_runtime_add_task_group_task_i32\""
           << ",\"objc3_runtime_wait_task_group_next_i32\""
           << ",\"objc3_runtime_cancel_task_group_i32\""
           << ",\"objc3_runtime_task_is_cancelled_i32\""
           << ",\"objc3_runtime_task_on_cancel_i32\""
           << ",\"objc3_runtime_actor_enter_isolation_thunk_i32\""
           << ",\"objc3_runtime_actor_enter_nonisolated_i32\""
           << ",\"objc3_runtime_actor_hop_to_executor_i32\""
           << ",\"objc3_runtime_actor_record_replay_proof_i32\""
           << ",\"objc3_runtime_actor_record_race_guard_i32\""
           << ",\"objc3_runtime_actor_bind_executor_i32\""
           << ",\"objc3_runtime_actor_mailbox_enqueue_i32\""
           << ",\"objc3_runtime_actor_mailbox_drain_next_i32\""
           << ",\"objc3_runtime_copy_async_continuation_state_for_testing\""
           << ",\"objc3_runtime_copy_task_runtime_state_for_testing\""
           << ",\"objc3_runtime_copy_actor_runtime_state_for_testing\"]"
           << ",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/async_await_executor_source_closure_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/actor_member_isolation_surface_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/task_executor_cancellation_source_closure_positive.objc3\"]"
           << ",\"authoritative_probe_paths\":[\"tests/tooling/runtime/continuation_runtime_helper_probe.cpp\""
           << ",\"tests/tooling/runtime/task_runtime_lowering_probe.cpp\""
           << ",\"tests/tooling/runtime/actor_lowering_runtime_probe.cpp\"]"
           << ",\"explicit_non_goals\":[\"no-public-runtime-abi-widening\""
           << ",\"no-milestone-specific-scaffolding\""
           << ",\"no-sidecar-only-concurrency-proof\"]"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
}

void WriteRuntimeAsyncTaskActorNormalizationCompletionSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure) {
  manifest << "  \"runtime_async_task_actor_normalization_completion_surface\":{\"contract_id\":\""
           << kObjc3RuntimeAsyncTaskActorNormalizationCompletionSurfaceContractId
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
           << "\",\"source_surface_contract_id\":\""
           << kObjc3RuntimeUnifiedConcurrencySourceSurfaceContractId
           << "\",\"normalized_semantic_contract_ids\":[\"objc3c.concurrency.async.effect.suspension.semantic.model.v1\""
           << ",\"objc3c.concurrency.task.executor.cancellation.semantic.model.v1\""
           << ",\"objc3c.concurrency.actor.isolation.sendable.semantic.model.v1\"]"
           << ",\"lowering_contract_ids\":[\"objc3c.concurrency.continuation.abi.async.lowering.contract.v1\""
           << ",\"objc3c.concurrency.task.runtime.lowering.contract.v1\""
           << ",\"objc3c.concurrency.actor.lowering.and.metadata.contract.v1\"]"
           << ",\"lowering_lane_contract_ids\":[\"objc3c.async.continuation.lowering.v1\""
           << ",\"objc3c.await.lowering.suspension.state.lowering.v1\""
           << ",\"objc3c.task.runtime.interop.cancellation.lowering.v1\""
           << ",\"objc3c.concurrency.replay.race.guard.lowering.v1\""
           << ",\"objc3c.actor.lowering.metadata.contract.v1\""
           << ",\"objc3c.actor.isolation.sendability.lowering.v1\"]"
           << ",\"authoritative_code_paths\":[\"native/objc3c/src/ast/objc3_ast.h\""
           << ",\"native/objc3c/src/sema/objc3_semantic_passes.cpp\""
           << ",\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"native/objc3c/src/ir/objc3_ir_emitter.cpp\""
           << ",\"native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h\""
           << ",\"native/objc3c/src/runtime/objc3_runtime.cpp\"]"
           << ",\"authoritative_surface_fields\":[\"frontend.pipeline.semantic_surface.objc_concurrency_async_effect_and_suspension_semantic_model\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_task_executor_and_cancellation_semantic_model\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_actor_isolation_and_sendable_semantic_model\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_continuation_abi_and_async_lowering_contract\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_task_runtime_lowering_contract\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_actor_lowering_and_metadata_contract\"]"
           << ",\"normalization_completion_model\":\"normalized-async-task-actor-sema-and-lowering-packets-freeze-the-live-boundary-before-runtime-abi-and-runnable-execution-closure\""
           << ",\"public_runtime_abi_boundary\":[\""
           << kObjc3RuntimeSupportLibraryRegisterImageSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol << "\",\""
           << kObjc3RuntimeSupportLibraryResetForTestingSymbol << "\"]"
           << ",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/async_lowering_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/actor_isolation_sendable_semantic_model_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/task_executor_cancellation_semantic_model_positive.objc3\"]"
           << ",\"authoritative_probe_paths\":[\"tests/tooling/runtime/continuation_runtime_helper_probe.cpp\""
           << ",\"tests/tooling/runtime/task_runtime_lowering_probe.cpp\""
           << ",\"tests/tooling/runtime/actor_lowering_runtime_probe.cpp\"]"
           << ",\"explicit_non_goals\":[\"no-public-runtime-abi-widening\""
           << ",\"no-runnable-task-or-actor-execution-claim\""
           << ",\"no-milestone-specific-scaffolding\"]"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << "},\n";
}

void WriteRuntimeUnifiedConcurrencyLoweringMetadataSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure) {
  manifest << "  \"runtime_unified_concurrency_lowering_metadata_surface\":{\"contract_id\":\""
           << kObjc3RuntimeUnifiedConcurrencyLoweringMetadataSurfaceContractId
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
           << "\",\"source_surface_contract_id\":\""
           << kObjc3RuntimeUnifiedConcurrencySourceSurfaceContractId
           << "\",\"normalization_completion_surface_contract_id\":\""
           << kObjc3RuntimeAsyncTaskActorNormalizationCompletionSurfaceContractId
           << "\",\"lowering_contract_ids\":[\"objc3c.concurrency.continuation.abi.async.lowering.contract.v1\""
           << ",\"objc3c.concurrency.task.runtime.lowering.contract.v1\""
           << ",\"objc3c.concurrency.actor.lowering.and.metadata.contract.v1\"]"
           << ",\"lowering_detail_contract_ids\":[\"objc3c.concurrency.async.direct.call.lowering.v1\""
           << ",\"objc3c.concurrency.task.runtime.abi.completion.v1\""
           << ",\"objc3c.concurrency.actor.isolation.sendability.enforcement.v1\"]"
           << ",\"lowering_lane_contract_ids\":[\"objc3c.async.continuation.lowering.v1\""
           << ",\"objc3c.await.lowering.suspension.state.lowering.v1\""
           << ",\"objc3c.task.runtime.interop.cancellation.lowering.v1\""
           << ",\"objc3c.concurrency.replay.race.guard.lowering.v1\""
           << ",\"objc3c.actor.lowering.metadata.contract.v1\""
           << ",\"objc3c.actor.isolation.sendability.lowering.v1\"]"
           << ",\"authoritative_code_paths\":[\"native/objc3c/src/ast/objc3_ast.h\""
           << ",\"native/objc3c/src/sema/objc3_semantic_passes.cpp\""
           << ",\"native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp\""
           << ",\"native/objc3c/src/ir/objc3_ir_emitter.cpp\"]"
           << ",\"authoritative_surface_fields\":[\"frontend.pipeline.semantic_surface.objc_concurrency_continuation_abi_and_async_lowering_contract\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_async_function_await_and_continuation_lowering\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_task_runtime_lowering_contract\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_task_group_and_runtime_abi_completion\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_actor_lowering_and_metadata_contract\""
           << ",\"frontend.pipeline.semantic_surface.objc_concurrency_actor_isolation_and_sendability_enforcement\"]"
           << ",\"lowering_metadata_surface_model\":\"unified-concurrency-lowering-and-metadata-surface-freezes-live-async-task-actor-lowering-packets-and-emitted-metadata-boundaries-before-runtime-abi-and-runnable-execution-closure\""
           << ",\"public_runtime_abi_boundary\":[\""
           << kObjc3RuntimeSupportLibraryRegisterImageSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol << "\",\""
           << kObjc3RuntimeSupportLibraryResetForTestingSymbol << "\"]"
           << ",\"authoritative_fixture_paths\":[\"tests/tooling/fixtures/native/async_lowering_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/task_runtime_async_entry_lowering_positive.objc3\""
           << ",\"tests/tooling/fixtures/native/actor_lowering_metadata_positive.objc3\"]"
           << ",\"authoritative_probe_paths\":[\"tests/tooling/runtime/continuation_runtime_helper_probe.cpp\""
           << ",\"tests/tooling/runtime/task_runtime_lowering_probe.cpp\""
           << ",\"tests/tooling/runtime/actor_lowering_runtime_probe.cpp\"]"
           << ",\"explicit_non_goals\":[\"no-public-runtime-abi-widening\""
           << ",\"no-runnable-task-or-actor-execution-claim\""
           << ",\"no-milestone-specific-scaffolding\"]"
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << "},\n";
}

}  // namespace objc3::artifacts::frontend
