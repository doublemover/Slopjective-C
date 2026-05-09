"""Unified concurrency lowering metadata surface builder."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.concurrency_surface_support import (
    authoritative_case_ids,
)

from ..c_api import PUBLIC_RUNTIME_ABI_BOUNDARY
from ..runtime_contract_concurrency import (
    RUNTIME_ASYNC_TASK_ACTOR_NORMALIZATION_COMPLETION_SURFACE_CONTRACT_ID,
    RUNTIME_UNIFIED_CONCURRENCY_LOWERING_METADATA_SURFACE_CONTRACT_ID,
    RUNTIME_UNIFIED_CONCURRENCY_SOURCE_SURFACE_CONTRACT_ID,
)


def build_runtime_unified_concurrency_lowering_metadata_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
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
        "authoritative_case_ids": authoritative_case_ids(
            results,
            {"unified-concurrency-lowering-metadata-surface"},
        ),
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


__all__ = ["build_runtime_unified_concurrency_lowering_metadata_surface"]
