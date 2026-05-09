"""Concurrency runtime acceptance surface builders."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult

from ..core import (
    ACTOR_RUNTIME_ABI_PROBE,
    CONTINUATION_RUNTIME_ABI_PROBE,
    PRIVATE_UNIFIED_CONCURRENCY_RUNTIME_ABI_BOUNDARY,
    PUBLIC_RUNTIME_ABI_BOUNDARY,
    RUNTIME_ASYNC_TASK_ACTOR_NORMALIZATION_COMPLETION_SURFACE_CONTRACT_ID,
    RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
    RUNTIME_PUBLIC_HEADER_PATH,
    RUNTIME_UNIFIED_CONCURRENCY_LOWERING_METADATA_SURFACE_CONTRACT_ID,
    RUNTIME_UNIFIED_CONCURRENCY_RUNTIME_ABI_SURFACE_CONTRACT_ID,
    RUNTIME_UNIFIED_CONCURRENCY_SOURCE_SURFACE_CONTRACT_ID,
    TASK_RUNTIME_ABI_PROBE,
    UNIFIED_CONCURRENCY_ACTOR_RUNTIME_MODEL,
    UNIFIED_CONCURRENCY_CONTINUATION_RUNTIME_MODEL,
    UNIFIED_CONCURRENCY_RUNTIME_ABI_BOUNDARY_MODEL,
    UNIFIED_CONCURRENCY_RUNTIME_FAIL_CLOSED_MODEL,
    UNIFIED_CONCURRENCY_TASK_RUNTIME_MODEL,
)


def build_runtime_unified_concurrency_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"unified-concurrency-runtime-architecture"}
    ]
    return {
        "contract_id": RUNTIME_UNIFIED_CONCURRENCY_SOURCE_SURFACE_CONTRACT_ID,
        "compile_manifest_artifact": "<emit-prefix>.manifest.json",
        "registration_manifest_artifact": "<emit-prefix>.runtime-registration-manifest.json",
        "registration_descriptor_artifact": "<emit-prefix>.runtime-registration-descriptor.json",
        "object_artifact": "<emit-prefix>.obj",
        "backend_artifact": "<emit-prefix>.ll",
        "source_surface_model": (
            "unified-concurrency-source-surface-freezes-live-async-actor-task-source-and-sema-boundaries-before-lowering-runtime-and-public-abi-expansion"
        ),
        "source_contract_ids": [
            "objc3c.concurrency.async.source.closure.v1",
            "objc3c.concurrency.actor.member.isolation.source.closure.v1",
            "objc3c.concurrency.task.group.cancellation.source.closure.v1",
            "objc3c.concurrency.async.effect.suspension.semantic.model.v1",
            "objc3c.concurrency.task.executor.cancellation.semantic.model.v1",
            "objc3c.concurrency.actor.isolation.sendable.semantic.model.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
            "native/objc3c/src/runtime/public/objc3_runtime_api.h",
            "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h",
            "native/objc3c/src/runtime/objc3_runtime.cpp",
        ],
        "authoritative_source_fields": [
            "frontend.pipeline.semantic_surface.objc_concurrency_async_source_closure",
            "frontend.pipeline.semantic_surface.objc_concurrency_actor_member_and_isolation_source_closure",
            "frontend.pipeline.semantic_surface.objc_concurrency_task_group_and_cancellation_source_closure",
            "frontend.pipeline.semantic_surface.objc_concurrency_async_effect_and_suspension_semantic_model",
            "frontend.pipeline.semantic_surface.objc_concurrency_task_executor_and_cancellation_semantic_model",
            "frontend.pipeline.semantic_surface.objc_concurrency_actor_isolation_and_sendable_semantic_model",
        ],
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_concurrency_runtime_boundary": [
            "objc3_runtime_allocate_async_continuation_i32",
            "objc3_runtime_handoff_async_continuation_to_executor_i32",
            "objc3_runtime_resume_async_continuation_i32",
            "objc3_runtime_spawn_task_i32",
            "objc3_runtime_enter_task_group_scope_i32",
            "objc3_runtime_add_task_group_task_i32",
            "objc3_runtime_wait_task_group_next_i32",
            "objc3_runtime_cancel_task_group_i32",
            "objc3_runtime_task_is_cancelled_i32",
            "objc3_runtime_task_on_cancel_i32",
            "objc3_runtime_actor_enter_isolation_thunk_i32",
            "objc3_runtime_actor_enter_nonisolated_i32",
            "objc3_runtime_actor_hop_to_executor_i32",
            "objc3_runtime_actor_record_replay_proof_i32",
            "objc3_runtime_actor_record_race_guard_i32",
            "objc3_runtime_actor_bind_executor_i32",
            "objc3_runtime_actor_mailbox_enqueue_i32",
            "objc3_runtime_actor_mailbox_drain_next_i32",
            "objc3_runtime_copy_async_continuation_state_for_testing",
            "objc3_runtime_copy_task_runtime_state_for_testing",
            "objc3_runtime_copy_actor_runtime_state_for_testing",
        ],
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/async_await_executor_source_closure_positive.objc3",
            "tests/tooling/fixtures/native/actor_member_isolation_surface_positive.objc3",
            "tests/tooling/fixtures/native/task_executor_cancellation_source_closure_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/continuation_runtime_helper_probe.cpp",
            "tests/tooling/runtime/task_runtime_lowering_probe.cpp",
            "tests/tooling/runtime/actor_lowering_runtime_probe.cpp",
        ],
        "explicit_non_goals": [
            "no-public-runtime-abi-widening",
            "no-milestone-specific-scaffolding",
            "no-sidecar-only-concurrency-proof",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_async_task_actor_normalization_completion_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"async-task-actor-normalization-completion"}
    ]
    return {
        "contract_id": (
            RUNTIME_ASYNC_TASK_ACTOR_NORMALIZATION_COMPLETION_SURFACE_CONTRACT_ID
        ),
        "compile_manifest_artifact": "<emit-prefix>.manifest.json",
        "registration_manifest_artifact": "<emit-prefix>.runtime-registration-manifest.json",
        "registration_descriptor_artifact": "<emit-prefix>.runtime-registration-descriptor.json",
        "object_artifact": "<emit-prefix>.obj",
        "backend_artifact": "<emit-prefix>.ll",
        "source_surface_contract_id": RUNTIME_UNIFIED_CONCURRENCY_SOURCE_SURFACE_CONTRACT_ID,
        "normalized_semantic_contract_ids": [
            "objc3c.concurrency.async.effect.suspension.semantic.model.v1",
            "objc3c.concurrency.task.executor.cancellation.semantic.model.v1",
            "objc3c.concurrency.actor.isolation.sendable.semantic.model.v1",
        ],
        "lowering_contract_ids": [
            "objc3c.concurrency.continuation.abi.async.lowering.contract.v1",
            "objc3c.concurrency.task.runtime.lowering.contract.v1",
            "objc3c.concurrency.actor.lowering.and.metadata.contract.v1",
        ],
        "lowering_lane_contract_ids": [
            "objc3c.async.continuation.lowering.v1",
            "objc3c.await.lowering.suspension.state.lowering.v1",
            "objc3c.task.runtime.interop.cancellation.lowering.v1",
            "objc3c.concurrency.replay.race.guard.lowering.v1",
            "objc3c.actor.lowering.metadata.contract.v1",
            "objc3c.actor.isolation.sendability.lowering.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
            "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h",
            "native/objc3c/src/runtime/objc3_runtime.cpp",
        ],
        "authoritative_surface_fields": [
            "frontend.pipeline.semantic_surface.objc_concurrency_async_effect_and_suspension_semantic_model",
            "frontend.pipeline.semantic_surface.objc_concurrency_task_executor_and_cancellation_semantic_model",
            "frontend.pipeline.semantic_surface.objc_concurrency_actor_isolation_and_sendable_semantic_model",
            "frontend.pipeline.semantic_surface.objc_concurrency_continuation_abi_and_async_lowering_contract",
            "frontend.pipeline.semantic_surface.objc_concurrency_task_runtime_lowering_contract",
            "frontend.pipeline.semantic_surface.objc_concurrency_actor_lowering_and_metadata_contract",
        ],
        "normalization_completion_model": (
            "normalized-async-task-actor-sema-and-lowering-packets-freeze-the-live-boundary-before-runtime-abi-and-runnable-execution-closure"
        ),
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/async_lowering_positive.objc3",
            "tests/tooling/fixtures/native/actor_isolation_sendable_semantic_model_positive.objc3",
            "tests/tooling/fixtures/native/task_executor_cancellation_semantic_model_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/continuation_runtime_helper_probe.cpp",
            "tests/tooling/runtime/task_runtime_lowering_probe.cpp",
            "tests/tooling/runtime/actor_lowering_runtime_probe.cpp",
        ],
        "explicit_non_goals": [
            "no-public-runtime-abi-widening",
            "no-runnable-task-or-actor-execution-claim",
            "no-milestone-specific-scaffolding",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
    }


def build_runtime_unified_concurrency_lowering_metadata_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"unified-concurrency-lowering-metadata-surface"}
    ]
    return {
        "contract_id": RUNTIME_UNIFIED_CONCURRENCY_LOWERING_METADATA_SURFACE_CONTRACT_ID,
        "compile_manifest_artifact": "<emit-prefix>.manifest.json",
        "registration_manifest_artifact": "<emit-prefix>.runtime-registration-manifest.json",
        "registration_descriptor_artifact": "<emit-prefix>.runtime-registration-descriptor.json",
        "object_artifact": "<emit-prefix>.obj",
        "backend_artifact": "<emit-prefix>.ll",
        "source_surface_contract_id": RUNTIME_UNIFIED_CONCURRENCY_SOURCE_SURFACE_CONTRACT_ID,
        "normalization_completion_surface_contract_id": (
            RUNTIME_ASYNC_TASK_ACTOR_NORMALIZATION_COMPLETION_SURFACE_CONTRACT_ID
        ),
        "lowering_contract_ids": [
            "objc3c.concurrency.continuation.abi.async.lowering.contract.v1",
            "objc3c.concurrency.task.runtime.lowering.contract.v1",
            "objc3c.concurrency.actor.lowering.and.metadata.contract.v1",
        ],
        "lowering_detail_contract_ids": [
            "objc3c.concurrency.async.direct.call.lowering.v1",
            "objc3c.concurrency.task.runtime.abi.completion.v1",
            "objc3c.concurrency.actor.isolation.sendability.enforcement.v1",
        ],
        "lowering_lane_contract_ids": [
            "objc3c.async.continuation.lowering.v1",
            "objc3c.await.lowering.suspension.state.lowering.v1",
            "objc3c.task.runtime.interop.cancellation.lowering.v1",
            "objc3c.concurrency.replay.race.guard.lowering.v1",
            "objc3c.actor.lowering.metadata.contract.v1",
            "objc3c.actor.isolation.sendability.lowering.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
        ],
        "authoritative_surface_fields": [
            "frontend.pipeline.semantic_surface.objc_concurrency_continuation_abi_and_async_lowering_contract",
            "frontend.pipeline.semantic_surface.objc_concurrency_async_function_await_and_continuation_lowering",
            "frontend.pipeline.semantic_surface.objc_concurrency_task_runtime_lowering_contract",
            "frontend.pipeline.semantic_surface.objc_concurrency_task_group_and_runtime_abi_completion",
            "frontend.pipeline.semantic_surface.objc_concurrency_actor_lowering_and_metadata_contract",
            "frontend.pipeline.semantic_surface.objc_concurrency_actor_isolation_and_sendability_enforcement",
        ],
        "lowering_metadata_surface_model": (
            "unified-concurrency-lowering-and-metadata-surface-freezes-live-async-task-actor-lowering-packets-and-emitted-metadata-boundaries-before-runtime-abi-and-runnable-execution-closure"
        ),
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/async_lowering_positive.objc3",
            "tests/tooling/fixtures/native/task_runtime_async_entry_lowering_positive.objc3",
            "tests/tooling/fixtures/native/actor_lowering_metadata_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/continuation_runtime_helper_probe.cpp",
            "tests/tooling/runtime/task_runtime_lowering_probe.cpp",
            "tests/tooling/runtime/actor_lowering_runtime_probe.cpp",
        ],
        "explicit_non_goals": [
            "no-public-runtime-abi-widening",
            "no-runnable-task-or-actor-execution-claim",
            "no-milestone-specific-scaffolding",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
    }


def build_runtime_unified_concurrency_runtime_abi_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"unified-concurrency-runtime-abi"}
    ]
    return {
        "contract_id": RUNTIME_UNIFIED_CONCURRENCY_RUNTIME_ABI_SURFACE_CONTRACT_ID,
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "unified_concurrency_source_surface_contract_id": (
            RUNTIME_UNIFIED_CONCURRENCY_SOURCE_SURFACE_CONTRACT_ID
        ),
        "async_task_actor_normalization_completion_surface_contract_id": (
            RUNTIME_ASYNC_TASK_ACTOR_NORMALIZATION_COMPLETION_SURFACE_CONTRACT_ID
        ),
        "unified_concurrency_lowering_metadata_surface_contract_id": (
            RUNTIME_UNIFIED_CONCURRENCY_LOWERING_METADATA_SURFACE_CONTRACT_ID
        ),
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_unified_concurrency_runtime_abi_boundary": (
            PRIVATE_UNIFIED_CONCURRENCY_RUNTIME_ABI_BOUNDARY
        ),
        "async_continuation_state_snapshot_symbol": (
            "objc3_runtime_copy_async_continuation_state_for_testing"
        ),
        "task_runtime_state_snapshot_symbol": (
            "objc3_runtime_copy_task_runtime_state_for_testing"
        ),
        "actor_runtime_state_snapshot_symbol": (
            "objc3_runtime_copy_actor_runtime_state_for_testing"
        ),
        "runtime_abi_boundary_model": UNIFIED_CONCURRENCY_RUNTIME_ABI_BOUNDARY_MODEL,
        "continuation_runtime_model": UNIFIED_CONCURRENCY_CONTINUATION_RUNTIME_MODEL,
        "task_runtime_model": UNIFIED_CONCURRENCY_TASK_RUNTIME_MODEL,
        "actor_runtime_model": UNIFIED_CONCURRENCY_ACTOR_RUNTIME_MODEL,
        "fail_closed_model": UNIFIED_CONCURRENCY_RUNTIME_FAIL_CLOSED_MODEL,
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_probe_paths": [
            CONTINUATION_RUNTIME_ABI_PROBE,
            TASK_RUNTIME_ABI_PROBE,
            ACTOR_RUNTIME_ABI_PROBE,
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }
