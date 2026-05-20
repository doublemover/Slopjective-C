"""Unified concurrency source surface builder."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.concurrency_surface_support import (
    authoritative_case_ids,
)

from ..c_api import (
    PRIVATE_UNIFIED_CONCURRENCY_RUNTIME_ABI_BOUNDARY,
    PUBLIC_RUNTIME_ABI_BOUNDARY,
)
from ..runtime_contract_concurrency import (
    RUNTIME_UNIFIED_CONCURRENCY_SOURCE_SURFACE_CONTRACT_ID,
)


def build_runtime_unified_concurrency_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
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
        "private_concurrency_runtime_boundary": list(
            PRIVATE_UNIFIED_CONCURRENCY_RUNTIME_ABI_BOUNDARY
        ),
        "authoritative_case_ids": authoritative_case_ids(
            results,
            {"unified-concurrency-runtime-architecture"},
        ),
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


__all__ = ["build_runtime_unified_concurrency_source_surface"]
