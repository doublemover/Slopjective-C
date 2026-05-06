"""Concurrency runtime acceptance domain."""

from __future__ import annotations

import json
from pathlib import Path
from time import perf_counter
from typing import Any

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.native_build import (
    compile_fixture_manifest_only,
    compile_fixture_outputs,
    compile_fixture_with_args,
)
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_key_value_output
from objc3c_runtime_acceptance.probes import run_probe

from ..core import (
    ACTOR_RUNTIME_ABI_PROBE,
    CONCURRENCY_ACTOR_PRESERVATION_CONSUMER_FIXTURE,
    CONCURRENCY_ACTOR_PRESERVATION_PROVIDER_FIXTURE,
    CONTINUATION_RUNTIME_ABI_PROBE,
    LIVE_ACTOR_RUNTIME_FIXTURE,
    LIVE_ACTOR_RUNTIME_PROBE,
    LIVE_CONTINUATION_RUNTIME_FIXTURE,
    LIVE_CONTINUATION_RUNTIME_PROBE,
    LIVE_TASK_RUNTIME_FIXTURE,
    LIVE_TASK_RUNTIME_PROBE,
    PRIVATE_UNIFIED_CONCURRENCY_RUNTIME_ABI_BOUNDARY,
    PUBLIC_RUNTIME_ABI_BOUNDARY,
    ROOT,
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

_EXPORTED_CASE_NAMES = [
    "build_runtime_unified_concurrency_source_surface",
    "build_runtime_async_task_actor_normalization_completion_surface",
    "build_runtime_unified_concurrency_lowering_metadata_surface",
    "build_runtime_unified_concurrency_runtime_abi_surface",
    "check_unified_concurrency_runtime_architecture_case",
    "check_async_task_actor_normalization_completion_case",
    "check_unified_concurrency_lowering_metadata_surface_case",
    "check_unified_concurrency_runtime_abi_case",
    "check_live_unified_concurrency_runtime_implementation_case",
    "check_cross_module_concurrency_actor_artifact_preservation_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)

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

def check_unified_concurrency_runtime_architecture_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "unified-concurrency-runtime-architecture"
    fixtures: dict[str, tuple[Path, bool]] = {
        "async_source": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "async_await_executor_source_closure_positive.objc3",
            True,
        ),
        "actor_source": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "actor_member_isolation_surface_positive.objc3",
            True,
        ),
        "task_source": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "task_executor_cancellation_source_closure_positive.objc3",
            True,
        ),
    }
    expected_semantic_surfaces = {
        "async_source": (
            "objc_concurrency_async_source_closure",
            "objc3c.concurrency.async.source.closure.v1",
        ),
        "actor_source": (
            "objc_concurrency_actor_member_and_isolation_source_closure",
            "objc3c.concurrency.actor.member.isolation.source.closure.v1",
        ),
        "task_source": (
            "objc_concurrency_task_group_and_cancellation_source_closure",
            "objc3c.concurrency.task.group.cancellation.source.closure.v1",
        ),
    }
    surface_summaries: dict[str, Any] = {}

    for fixture_key, (fixture_path, requires_real_compile_output) in fixtures.items():
        compile_dir = case_dir / fixture_key / "compile"
        diagnostics_path = compile_dir / "module.diagnostics.txt"
        if requires_real_compile_output:
            _, _, manifest_path = compile_fixture_outputs(fixture_path, compile_dir)
        else:
            manifest_path, compile_result = compile_fixture_manifest_only(
                fixture_path, compile_dir
            )
            expect(
                compile_result.returncode != 0,
                "expected task source closure fixture to remain source-surface-only until later lowering work lands",
            )
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        semantic_surface_name, expected_contract_id = expected_semantic_surfaces[fixture_key]
        semantic_surface = (
            manifest.get("frontend", {})
            .get("pipeline", {})
            .get("semantic_surface", {})
            .get(semantic_surface_name, {})
        )
        expect(
            isinstance(semantic_surface, dict),
            f"expected {fixture_key} fixture to publish {semantic_surface_name}",
        )
        expect(
            semantic_surface.get("contract_id") == expected_contract_id,
            f"expected {fixture_key} fixture to preserve {expected_contract_id}",
        )
        expect(
            semantic_surface.get("deterministic_handoff") is True,
            f"expected {fixture_key} fixture to preserve deterministic_handoff",
        )
        expect(
            semantic_surface.get("ready_for_semantic_expansion") is True,
            f"expected {fixture_key} fixture to preserve ready_for_semantic_expansion",
        )
        surface_summaries[fixture_key] = {
            "fixture": str(fixture_path.relative_to(ROOT)).replace("\\", "/"),
            "manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "surface": semantic_surface_name,
            "contract_id": semantic_surface.get("contract_id"),
        }
        if not requires_real_compile_output and diagnostics_path.is_file():
            surface_summaries[fixture_key]["diagnostics"] = str(
                diagnostics_path.relative_to(ROOT)
            ).replace("\\", "/")
        if fixture_key == "async_source":
            top_level_surface = manifest.get("runtime_unified_concurrency_source_surface", {})
            expect(
                isinstance(top_level_surface, dict),
                "expected async concurrency fixture to publish runtime_unified_concurrency_source_surface",
            )
            expect(
                top_level_surface.get("contract_id")
                == RUNTIME_UNIFIED_CONCURRENCY_SOURCE_SURFACE_CONTRACT_ID,
                "expected concurrency runtime architecture fixture to preserve the unified source surface contract",
            )
            expect(
                top_level_surface.get("source_surface_model")
                == "unified-concurrency-source-surface-freezes-live-async-actor-task-source-and-sema-boundaries-before-lowering-runtime-and-public-abi-expansion",
                "expected concurrency runtime architecture fixture to preserve the unified source surface model",
            )
            expect(
                top_level_surface.get("requires_coupled_registration_manifest") is True
                and top_level_surface.get("requires_real_compile_output") is True
                and top_level_surface.get("requires_linked_runtime_probe") is True,
                "expected unified concurrency source surface to remain compile-coupled and probe-backed",
            )
            surface_summaries["runtime_surface"] = {
                "manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
                "contract_id": top_level_surface.get("contract_id"),
                "source_contract_ids": top_level_surface.get("source_contract_ids"),
            }

    return CaseResult(
        case_id="unified-concurrency-runtime-architecture",
        probe="compile-manifest-runtime-source-surface",
        fixture="tests/tooling/fixtures/native/async_await_executor_source_closure_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary=surface_summaries,
    )

def check_async_task_actor_normalization_completion_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "async-task-actor-normalization-completion"
    fixtures: dict[str, tuple[Path, bool, str, str]] = {
        "async_normalization": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "async_lowering_positive.objc3",
            True,
            "objc_concurrency_async_effect_and_suspension_semantic_model",
            "objc_concurrency_continuation_abi_and_async_lowering_contract",
        ),
        "actor_normalization": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "actor_isolation_sendable_semantic_model_positive.objc3",
            True,
            "objc_concurrency_actor_isolation_and_sendable_semantic_model",
            "objc_concurrency_actor_lowering_and_metadata_contract",
        ),
        "task_normalization": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "task_executor_cancellation_semantic_model_positive.objc3",
            True,
            "objc_concurrency_task_executor_and_cancellation_semantic_model",
            "objc_concurrency_task_runtime_lowering_contract",
        ),
    }
    expected_contract_ids = {
        "async_normalization": (
            "objc3c.concurrency.async.effect.suspension.semantic.model.v1",
            "objc3c.concurrency.continuation.abi.async.lowering.contract.v1",
        ),
        "actor_normalization": (
            "objc3c.concurrency.actor.isolation.sendable.semantic.model.v1",
            "objc3c.concurrency.actor.lowering.and.metadata.contract.v1",
        ),
        "task_normalization": (
            "objc3c.concurrency.task.executor.cancellation.semantic.model.v1",
            "objc3c.concurrency.task.runtime.lowering.contract.v1",
        ),
    }
    summary: dict[str, Any] = {}

    for fixture_key, (
        fixture_path,
        requires_real_compile_output,
        semantic_surface_name,
        lowering_surface_name,
    ) in fixtures.items():
        compile_dir = case_dir / fixture_key / "compile"
        diagnostics_path = compile_dir / "module.diagnostics.txt"
        if requires_real_compile_output:
            _, _, manifest_path = compile_fixture_outputs(fixture_path, compile_dir)
        else:
            manifest_path, compile_result = compile_fixture_manifest_only(
                fixture_path, compile_dir
            )
            expect(
                compile_result.returncode != 0,
                "expected task normalization fixture to remain manifest-backed until task lowering determinism lands",
            )
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        semantic_surface = (
            manifest.get("frontend", {})
            .get("pipeline", {})
            .get("semantic_surface", {})
            .get(semantic_surface_name, {})
        )
        lowering_surface = (
            manifest.get("frontend", {})
            .get("pipeline", {})
            .get("semantic_surface", {})
            .get(lowering_surface_name, {})
        )
        expected_semantic_contract_id, expected_lowering_contract_id = (
            expected_contract_ids[fixture_key]
        )
        expect(
            isinstance(semantic_surface, dict),
            f"expected {fixture_key} fixture to publish {semantic_surface_name}",
        )
        expect(
            isinstance(lowering_surface, dict),
            f"expected {fixture_key} fixture to publish {lowering_surface_name}",
        )
        expect(
            semantic_surface.get("contract_id") == expected_semantic_contract_id,
            f"expected {fixture_key} semantic model to preserve {expected_semantic_contract_id}",
        )
        expect(
            lowering_surface.get("contract_id") == expected_lowering_contract_id,
            f"expected {fixture_key} lowering surface to preserve {expected_lowering_contract_id}",
        )
        expect(
            lowering_surface.get("deterministic_handoff") is True
            and lowering_surface.get("ready_for_ir_emission") is True,
            f"expected {fixture_key} lowering surface to preserve deterministic IR handoff",
        )
        summary[fixture_key] = {
            "fixture": str(fixture_path.relative_to(ROOT)).replace("\\", "/"),
            "manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "semantic_surface": semantic_surface_name,
            "semantic_contract_id": semantic_surface.get("contract_id"),
            "lowering_surface": lowering_surface_name,
            "lowering_contract_id": lowering_surface.get("contract_id"),
        }
        if not requires_real_compile_output and diagnostics_path.is_file():
            summary[fixture_key]["diagnostics"] = str(
                diagnostics_path.relative_to(ROOT)
            ).replace("\\", "/")
        if fixture_key == "async_normalization":
            top_level_surface = manifest.get(
                "runtime_async_task_actor_normalization_completion_surface", {}
            )
            expect(
                isinstance(top_level_surface, dict),
                "expected async lowering fixture to publish runtime_async_task_actor_normalization_completion_surface",
            )
            expect(
                top_level_surface.get("contract_id")
                == RUNTIME_ASYNC_TASK_ACTOR_NORMALIZATION_COMPLETION_SURFACE_CONTRACT_ID,
                "expected async/task/actor normalization fixture to preserve the normalization completion surface contract",
            )
            expect(
                top_level_surface.get("normalization_completion_model")
                == "normalized-async-task-actor-sema-and-lowering-packets-freeze-the-live-boundary-before-runtime-abi-and-runnable-execution-closure",
                "expected async/task/actor normalization fixture to preserve the normalization completion model",
            )
            summary["runtime_surface"] = {
                "manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
                "contract_id": top_level_surface.get("contract_id"),
                "normalized_semantic_contract_ids": top_level_surface.get(
                    "normalized_semantic_contract_ids"
                ),
                "lowering_contract_ids": top_level_surface.get(
                    "lowering_contract_ids"
                ),
            }

    return CaseResult(
        case_id="async-task-actor-normalization-completion",
        probe="compile-manifest-normalization-surface",
        fixture="tests/tooling/fixtures/native/async_lowering_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary=summary,
    )

def check_unified_concurrency_lowering_metadata_surface_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "unified-concurrency-lowering-metadata-surface"
    fixtures: dict[
        str,
        tuple[
            Path,
            str,
            str,
            str,
            str,
            str,
            str,
        ],
    ] = {
        "async_lowering": (
            ROOT / "tests" / "tooling" / "fixtures" / "native" / "async_lowering_positive.objc3",
            "objc_concurrency_continuation_abi_and_async_lowering_contract",
            "objc3c.concurrency.continuation.abi.async.lowering.contract.v1",
            "objc_concurrency_async_function_await_and_continuation_lowering",
            "objc3c.concurrency.async.direct.call.lowering.v1",
            "deterministic_handoff",
            "ready_for_ir_emission",
        ),
        "task_lowering": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "task_runtime_async_entry_lowering_positive.objc3",
            "objc_concurrency_task_runtime_lowering_contract",
            "objc3c.concurrency.task.runtime.lowering.contract.v1",
            "objc_concurrency_task_group_and_runtime_abi_completion",
            "objc3c.concurrency.task.runtime.abi.completion.v1",
            "deterministic_handoff",
            "ready_for_ir_emission",
        ),
        "actor_lowering": (
            ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "actor_lowering_metadata_positive.objc3",
            "objc_concurrency_actor_lowering_and_metadata_contract",
            "objc3c.concurrency.actor.lowering.and.metadata.contract.v1",
            "objc_concurrency_actor_isolation_and_sendability_enforcement",
            "objc3c.concurrency.actor.isolation.sendability.enforcement.v1",
            "deterministic_handoff",
            "ready_for_ir_emission",
        ),
    }
    summary: dict[str, Any] = {}

    for (
        fixture_key,
        (
            fixture_path,
            lowering_surface_name,
            expected_lowering_contract_id,
            detail_surface_name,
            expected_detail_contract_id,
            lowering_determinism_field,
            lowering_readiness_field,
        ),
    ) in fixtures.items():
        compile_dir = case_dir / fixture_key / "compile"
        _, _, manifest_path = compile_fixture_outputs(fixture_path, compile_dir)
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        semantic_surface = manifest.get("frontend", {}).get("pipeline", {}).get(
            "semantic_surface", {}
        )
        lowering_surface = semantic_surface.get(lowering_surface_name, {})
        detail_surface = semantic_surface.get(detail_surface_name, {})
        expect(
            isinstance(lowering_surface, dict),
            f"expected {fixture_key} fixture to publish {lowering_surface_name}",
        )
        expect(
            isinstance(detail_surface, dict),
            f"expected {fixture_key} fixture to publish {detail_surface_name}",
        )
        expect(
            lowering_surface.get("contract_id") == expected_lowering_contract_id,
            f"expected {fixture_key} lowering fixture to preserve {expected_lowering_contract_id}",
        )
        expect(
            detail_surface.get("contract_id") == expected_detail_contract_id,
            f"expected {fixture_key} lowering fixture to preserve {expected_detail_contract_id}",
        )
        expect(
            lowering_surface.get(lowering_determinism_field) is True,
            f"expected {fixture_key} lowering surface to preserve {lowering_determinism_field}",
        )
        expect(
            lowering_surface.get(lowering_readiness_field) is True,
            f"expected {fixture_key} lowering surface to preserve {lowering_readiness_field}",
        )
        summary[fixture_key] = {
            "fixture": str(fixture_path.relative_to(ROOT)).replace("\\", "/"),
            "manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "lowering_surface": lowering_surface_name,
            "lowering_contract_id": lowering_surface.get("contract_id"),
            "detail_surface": detail_surface_name,
            "detail_contract_id": detail_surface.get("contract_id"),
        }
        if fixture_key == "async_lowering":
            top_level_surface = manifest.get(
                "runtime_unified_concurrency_lowering_metadata_surface", {}
            )
            expect(
                isinstance(top_level_surface, dict),
                "expected async lowering fixture to publish runtime_unified_concurrency_lowering_metadata_surface",
            )
            expect(
                top_level_surface.get("contract_id")
                == RUNTIME_UNIFIED_CONCURRENCY_LOWERING_METADATA_SURFACE_CONTRACT_ID,
                "expected unified concurrency lowering fixture to preserve the lowering metadata surface contract",
            )
            expect(
                top_level_surface.get("lowering_metadata_surface_model")
                == "unified-concurrency-lowering-and-metadata-surface-freezes-live-async-task-actor-lowering-packets-and-emitted-metadata-boundaries-before-runtime-abi-and-runnable-execution-closure",
                "expected unified concurrency lowering fixture to preserve the lowering metadata model",
            )
            summary["runtime_surface"] = {
                "manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
                "contract_id": top_level_surface.get("contract_id"),
                "lowering_contract_ids": top_level_surface.get("lowering_contract_ids"),
                "lowering_detail_contract_ids": top_level_surface.get(
                    "lowering_detail_contract_ids"
                ),
            }

    return CaseResult(
        case_id="unified-concurrency-lowering-metadata-surface",
        probe="compile-manifest-lowering-metadata-surface",
        fixture="tests/tooling/fixtures/native/async_lowering_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary=summary,
    )

def check_unified_concurrency_runtime_abi_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / "unified-concurrency-runtime-abi"

    continuation_probe = ROOT / Path(CONTINUATION_RUNTIME_ABI_PROBE)
    continuation_exe = case_dir / "continuation_runtime_abi_probe.exe"
    compile_probe(clangxx, continuation_probe, continuation_exe, [])
    continuation_payload = parse_key_value_output(
        run_probe(continuation_exe), "unified concurrency continuation runtime ABI probe"
    )
    for field_name, expected_value in {
        "handle": 1,
        "handed_off": 1,
        "resumed": 77,
        "copy_status": 0,
        "allocation_call_count": 1,
        "handoff_call_count": 1,
        "resume_call_count": 1,
        "live_continuation_handle_count": 0,
        "last_allocated_continuation_handle": 1,
        "last_allocated_resume_entry_tag": 41,
        "last_allocated_executor_tag": 9,
        "last_handoff_continuation_handle": 1,
        "last_handoff_executor_tag": 17,
        "last_resume_continuation_handle": 1,
        "last_resume_result_value": 77,
        "last_resume_return_value": 77,
    }.items():
        expect(
            continuation_payload.get(field_name) == expected_value,
            f"expected unified concurrency continuation runtime ABI probe to preserve {field_name}",
        )

    task_probe = ROOT / Path(TASK_RUNTIME_ABI_PROBE)
    task_exe = case_dir / "task_runtime_abi_probe.exe"
    compile_probe(clangxx, task_probe, task_exe, [])
    task_payload = parse_key_value_output(
        run_probe(task_exe), "unified concurrency task runtime ABI probe"
    )
    for field_name, expected_value in {
        "spawn_group": 111,
        "scope": 1,
        "add_task": 1,
        "cancelled": 0,
        "wait_next": 23,
        "hop": 23,
        "cancel_all": 31,
        "on_cancel": 41,
        "spawn_detached": 121,
        "copy_status": 0,
        "spawn_call_count": 2,
        "scope_call_count": 1,
        "add_task_call_count": 1,
        "wait_next_call_count": 1,
        "cancel_all_call_count": 1,
        "cancellation_poll_call_count": 1,
        "on_cancel_call_count": 1,
        "executor_hop_call_count": 1,
        "last_spawn_kind": 2,
        "last_spawn_executor_tag": 3,
        "last_wait_next_result": 23,
        "last_executor_hop_executor_tag": 2,
        "last_executor_hop_value": 23,
    }.items():
        expect(
            task_payload.get(field_name) == expected_value,
            f"expected unified concurrency task runtime ABI probe to preserve {field_name}",
        )

    actor_probe = ROOT / Path(ACTOR_RUNTIME_ABI_PROBE)
    actor_exe = case_dir / "actor_runtime_abi_probe.exe"
    compile_probe(clangxx, actor_probe, actor_exe, [])
    actor_payload = parse_key_value_output(
        run_probe(actor_exe), "unified concurrency actor runtime ABI probe"
    )
    for field_name, expected_value in {
        "copy_status": 0,
        "replay": 1,
        "guard": 1,
        "isolation": 1,
        "nonisolated": 5,
        "hopped": 17,
        "replay_proof_call_count": 1,
        "race_guard_call_count": 1,
        "isolation_thunk_call_count": 1,
        "nonisolated_entry_call_count": 1,
        "hop_to_executor_call_count": 1,
        "last_replay_proof_executor_tag": 1,
        "last_race_guard_executor_tag": 1,
        "last_isolation_executor_tag": 1,
        "last_nonisolated_value": 5,
        "last_nonisolated_executor_tag": 0,
        "last_hop_value": 17,
        "last_hop_executor_tag": 1,
        "last_hop_result": 17,
    }.items():
        expect(
            actor_payload.get(field_name) == expected_value,
            f"expected unified concurrency actor runtime ABI probe to preserve {field_name}",
        )

    return CaseResult(
        case_id="unified-concurrency-runtime-abi",
        probe=";".join(
            [
                CONTINUATION_RUNTIME_ABI_PROBE,
                TASK_RUNTIME_ABI_PROBE,
                ACTOR_RUNTIME_ABI_PROBE,
            ]
        ),
        fixture=None,
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "continuation_probe": CONTINUATION_RUNTIME_ABI_PROBE,
            "task_probe": TASK_RUNTIME_ABI_PROBE,
            "actor_probe": ACTOR_RUNTIME_ABI_PROBE,
            "async_continuation_state_snapshot_symbol": (
                "objc3_runtime_copy_async_continuation_state_for_testing"
            ),
            "task_runtime_state_snapshot_symbol": (
                "objc3_runtime_copy_task_runtime_state_for_testing"
            ),
            "actor_runtime_state_snapshot_symbol": (
                "objc3_runtime_copy_actor_runtime_state_for_testing"
            ),
        },
    )

def check_live_unified_concurrency_runtime_implementation_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / "live-unified-concurrency-runtime-implementation"

    continuation_obj, _, continuation_manifest_path = compile_fixture_outputs(
        ROOT / Path(LIVE_CONTINUATION_RUNTIME_FIXTURE), case_dir / "continuation" / "compile"
    )
    continuation_manifest = json.loads(
        continuation_manifest_path.read_text(encoding="utf-8")
    )
    expect(
        continuation_manifest.get("runtime_unified_concurrency_runtime_abi_surface", {}).get(
            "contract_id"
        )
        == RUNTIME_UNIFIED_CONCURRENCY_RUNTIME_ABI_SURFACE_CONTRACT_ID,
        "expected live continuation fixture to publish the unified concurrency runtime ABI surface",
    )
    continuation_exe = case_dir / "continuation" / "live_continuation_runtime_probe.exe"
    compile_probe(
        clangxx,
        ROOT / Path(LIVE_CONTINUATION_RUNTIME_PROBE),
        continuation_exe,
        [continuation_obj],
    )
    continuation_payload = parse_key_value_output(
        run_probe(continuation_exe), "live unified concurrency continuation runtime probe"
    )
    for field_name, expected_value in {
        "runTask": 7,
        "loadValue": 7,
        "copy_status": 0,
        "allocation_call_count": 2,
        "handoff_call_count": 2,
        "resume_call_count": 2,
        "live_continuation_handle_count": 0,
        "last_handoff_executor_tag": 1,
        "last_resume_return_value": 7,
    }.items():
        expect(
            continuation_payload.get(field_name) == expected_value,
            f"expected live unified concurrency continuation runtime probe to preserve {field_name}",
        )

    task_obj, _, task_manifest_path = compile_fixture_outputs(
        ROOT / Path(LIVE_TASK_RUNTIME_FIXTURE), case_dir / "task" / "compile"
    )
    task_manifest = json.loads(task_manifest_path.read_text(encoding="utf-8"))
    expect(
        task_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_concurrency_task_runtime_lowering_contract", {})
        .get("contract_id")
        == "objc3c.concurrency.task.runtime.lowering.contract.v1",
        "expected live task fixture to preserve the task runtime lowering contract",
    )
    task_exe = case_dir / "task" / "live_task_runtime_probe.exe"
    compile_probe(
        clangxx,
        ROOT / Path(LIVE_TASK_RUNTIME_PROBE),
        task_exe,
        [task_obj],
    )
    task_payload = parse_key_value_output(
        run_probe(task_exe), "live unified concurrency task runtime probe"
    )
    for field_name, expected_value in {
        "spawn_group": 111,
        "scope": 1,
        "add_task": 1,
        "cancelled": 0,
        "wait_next": 23,
        "hop": 23,
        "cancel_all": 31,
        "on_cancel": 41,
        "spawn_detached": 121,
        "copy_status": 0,
        "spawn_call_count": 2,
        "scope_call_count": 1,
        "add_task_call_count": 1,
        "wait_next_call_count": 1,
        "cancel_all_call_count": 1,
        "cancellation_poll_call_count": 1,
        "on_cancel_call_count": 1,
        "executor_hop_call_count": 1,
        "last_spawn_kind": 2,
        "last_spawn_executor_tag": 3,
        "last_wait_next_result": 23,
        "last_executor_hop_executor_tag": 2,
        "last_executor_hop_value": 23,
    }.items():
        expect(
            task_payload.get(field_name) == expected_value,
            f"expected live unified concurrency task runtime probe to preserve {field_name}",
        )

    actor_obj, _, actor_manifest_path = compile_fixture_outputs(
        ROOT / Path(LIVE_ACTOR_RUNTIME_FIXTURE), case_dir / "actor" / "compile"
    )
    actor_manifest = json.loads(actor_manifest_path.read_text(encoding="utf-8"))
    expect(
        actor_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_concurrency_actor_lowering_and_metadata_contract", {})
        .get("contract_id")
        == "objc3c.concurrency.actor.lowering.and.metadata.contract.v1",
        "expected live actor fixture to preserve the actor lowering metadata contract",
    )
    actor_exe = case_dir / "actor" / "live_actor_runtime_probe.exe"
    compile_probe(
        clangxx,
        ROOT / Path(LIVE_ACTOR_RUNTIME_PROBE),
        actor_exe,
        [actor_obj],
    )
    actor_payload = parse_key_value_output(
        run_probe(actor_exe), "live unified concurrency actor runtime probe"
    )
    for field_name, expected_value in {
        "copy_status": 0,
        "replay": 1,
        "guard": 1,
        "isolation": 1,
        "bound": 1,
        "enqueued": 23,
        "drained": 23,
        "replay_proof_call_count": 1,
        "race_guard_call_count": 1,
        "isolation_thunk_call_count": 1,
        "bind_executor_call_count": 1,
        "mailbox_enqueue_call_count": 1,
        "mailbox_drain_call_count": 1,
        "last_replay_proof_executor_tag": 1,
        "last_race_guard_executor_tag": 1,
        "last_isolation_executor_tag": 1,
        "last_bound_actor_handle": 41,
        "last_bound_executor_tag": 1,
        "last_mailbox_actor_handle": 41,
        "last_mailbox_enqueued_value": 23,
        "last_mailbox_executor_tag": 1,
        "last_mailbox_depth": 0,
        "last_mailbox_drained_value": 23,
    }.items():
        expect(
            actor_payload.get(field_name) == expected_value,
            f"expected live unified concurrency actor runtime probe to preserve {field_name}",
        )

    return CaseResult(
        case_id="live-unified-concurrency-runtime-implementation",
        probe=";".join(
            [
                LIVE_CONTINUATION_RUNTIME_PROBE,
                LIVE_TASK_RUNTIME_PROBE,
                LIVE_ACTOR_RUNTIME_PROBE,
            ]
        ),
        fixture=LIVE_CONTINUATION_RUNTIME_FIXTURE,
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "continuation_fixture": LIVE_CONTINUATION_RUNTIME_FIXTURE,
            "task_fixture": LIVE_TASK_RUNTIME_FIXTURE,
            "actor_fixture": LIVE_ACTOR_RUNTIME_FIXTURE,
            "continuation_probe": LIVE_CONTINUATION_RUNTIME_PROBE,
            "task_probe": LIVE_TASK_RUNTIME_PROBE,
            "actor_probe": LIVE_ACTOR_RUNTIME_PROBE,
        },
    )

def check_cross_module_concurrency_actor_artifact_preservation_case(
    run_dir: Path,
) -> CaseResult:
    case_started = perf_counter()
    case_dir = run_dir / "cross-module-concurrency-actor-artifact-preservation"
    provider_fixture = ROOT / Path(CONCURRENCY_ACTOR_PRESERVATION_PROVIDER_FIXTURE)
    consumer_fixture = ROOT / Path(CONCURRENCY_ACTOR_PRESERVATION_CONSUMER_FIXTURE)

    provider_compile_dir = case_dir / "provider"
    provider_compile_started = perf_counter()
    compile_fixture_with_args(
        provider_fixture,
        provider_compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    provider_compile_ms = int((perf_counter() - provider_compile_started) * 1000)
    provider_import_surface = provider_compile_dir / "module.runtime-import-surface.json"
    provider_import_payload = json.loads(
        provider_import_surface.read_text(encoding="utf-8")
    )
    provider_registration_manifest = json.loads(
        (provider_compile_dir / "module.runtime-registration-manifest.json").read_text(
            encoding="utf-8"
        )
    )
    provider_actor_surface = provider_import_payload.get(
        "objc_concurrency_actor_mailbox_and_isolation_runtime_import_surface", {}
    )
    expect(
        isinstance(provider_actor_surface, dict),
        "expected concurrency actor provider import surface to publish the actor mailbox preservation packet",
    )
    expected_provider_fields = {
        "contract_id": "objc3c.concurrency.actor.mailbox.isolation.import.surface.v1",
        "source_contract_id": "objc3c.concurrency.actor.lowering.and.metadata.contract.v1",
        "surface_path": "frontend.pipeline.semantic_surface.objc_concurrency_actor_mailbox_and_isolation_runtime_import_surface",
        "source_model": "runtime-import-surface-preserves-actor-lowering-and-isolation-replay-facts-for-cross-module-runtime-link-planning",
        "fail_closed_model": "missing-or-drifted-actor-mailbox-runtime-import-packets-disable-cross-module-actor-runtime-preservation-claims",
    }
    for field_name, expected_value in expected_provider_fields.items():
        expect(
            provider_actor_surface.get(field_name) == expected_value,
            f"expected concurrency actor provider import surface to preserve {field_name}",
        )
    expect(
        provider_actor_surface.get("actor_mailbox_runtime_ready") is True
        and provider_actor_surface.get("deterministic") is True,
        "expected concurrency actor provider import surface to be runtime-ready and deterministic",
    )
    for field_name in (
        "replay_key",
        "actor_lowering_replay_key",
        "actor_isolation_lowering_replay_key",
    ):
        expect(
            isinstance(provider_actor_surface.get(field_name), str)
            and provider_actor_surface.get(field_name) != "",
            f"expected concurrency actor provider import surface to publish {field_name}",
        )

    consumer_compile_dir = case_dir / "consumer"
    consumer_compile_started = perf_counter()
    compile_fixture_with_args(
        consumer_fixture,
        consumer_compile_dir,
        [
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(provider_import_surface),
        ],
    )
    consumer_compile_ms = int((perf_counter() - consumer_compile_started) * 1000)
    consumer_registration_manifest = json.loads(
        (consumer_compile_dir / "module.runtime-registration-manifest.json").read_text(
            encoding="utf-8"
        )
    )
    link_plan = json.loads(
        (consumer_compile_dir / "module.cross-module-runtime-link-plan.json").read_text(
            encoding="utf-8"
        )
    )

    expect(
        link_plan.get("concurrency_actor_imported_module_count") == 1,
        "expected cross-module link plan to publish one imported actor module",
    )
    expect(
        link_plan.get("concurrency_actor_imported_module_names_lexicographic")
        == [provider_import_payload.get("module_name")],
        "expected cross-module link plan to preserve the imported actor module names",
    )
    expect(
        link_plan.get("concurrency_actor_cross_module_isolation_ready") is True,
        "expected cross-module link plan to mark actor cross-module isolation ready",
    )
    expect(
        link_plan.get("expected_concurrency_actor_contract_id")
        == "objc3c.concurrency.actor.mailbox.isolation.import.surface.v1",
        "expected cross-module link plan to preserve the actor import contract id",
    )
    expect(
        link_plan.get("expected_concurrency_actor_source_contract_id")
        == "objc3c.concurrency.actor.lowering.and.metadata.contract.v1",
        "expected cross-module link plan to preserve the actor source contract id",
    )

    imported_modules = link_plan.get("imported_modules")
    expect(
        isinstance(imported_modules, list) and len(imported_modules) == 1,
        "expected cross-module actor link plan to publish exactly one imported module",
    )
    imported_module = imported_modules[0]
    expect(
        imported_module.get("module_name") == provider_import_payload.get("module_name"),
        "expected imported actor module name to match the provider import surface",
    )
    expect(
        imported_module.get("translation_unit_registration_order_ordinal") == 1,
        "expected imported actor module registration ordinal to remain one",
    )
    expect(
        imported_module.get("concurrency_actor_mailbox_runtime_import_present") is True
        and imported_module.get("concurrency_actor_mailbox_runtime_ready") is True
        and imported_module.get("concurrency_actor_mailbox_runtime_deterministic")
        is True,
        "expected imported actor module to preserve actor runtime import readiness",
    )
    for field_name, expected_value in (
        (
            "concurrency_actor_contract_id",
            "objc3c.concurrency.actor.mailbox.isolation.import.surface.v1",
        ),
        (
            "concurrency_actor_source_contract_id",
            "objc3c.concurrency.actor.lowering.and.metadata.contract.v1",
        ),
    ):
        expect(
            imported_module.get(field_name) == expected_value,
            f"expected imported actor module to preserve {field_name}",
        )
    for field_name in (
        "concurrency_actor_mailbox_runtime_replay_key",
        "concurrency_actor_lowering_replay_key",
        "concurrency_actor_isolation_lowering_replay_key",
    ):
        expect(
            isinstance(imported_module.get(field_name), str)
            and imported_module.get(field_name) != "",
            f"expected imported actor module to preserve {field_name}",
        )
    for field_name in (
        "class_descriptor_count",
        "protocol_descriptor_count",
        "category_descriptor_count",
        "property_descriptor_count",
        "ivar_descriptor_count",
        "total_descriptor_count",
    ):
        expect(
            imported_module.get(field_name) == provider_registration_manifest.get(field_name),
            f"expected imported actor module to preserve {field_name}",
        )

    local_module = link_plan.get("local_module", {})
    expect(
        local_module.get("module_name") == "M270D003Consumer"
        and local_module.get("translation_unit_registration_order_ordinal") == 2,
        "expected cross-module actor consumer to preserve the local module identity and registration ordinal",
    )
    for field_name in (
        "class_descriptor_count",
        "protocol_descriptor_count",
        "category_descriptor_count",
        "property_descriptor_count",
        "ivar_descriptor_count",
        "total_descriptor_count",
    ):
        expect(
            local_module.get(field_name) == consumer_registration_manifest.get(field_name),
            f"expected local actor module to preserve {field_name}",
        )

    case_total_ms = int((perf_counter() - case_started) * 1000)
    return CaseResult(
        case_id="cross-module-concurrency-actor-artifact-preservation",
        probe=None,
        fixture=CONCURRENCY_ACTOR_PRESERVATION_PROVIDER_FIXTURE,
        claim_class="compile-manifest-and-link-plan",
        passed=True,
        summary={
            "provider_fixture": CONCURRENCY_ACTOR_PRESERVATION_PROVIDER_FIXTURE,
            "consumer_fixture": CONCURRENCY_ACTOR_PRESERVATION_CONSUMER_FIXTURE,
            "provider_compile_ms": provider_compile_ms,
            "consumer_compile_ms": consumer_compile_ms,
            "case_total_ms": case_total_ms,
            "provider_module_name": provider_import_payload.get("module_name"),
            "consumer_module_name": local_module.get("module_name"),
            "imported_actor_registration_ordinal": imported_module.get(
                "translation_unit_registration_order_ordinal"
            ),
            "local_actor_registration_ordinal": local_module.get(
                "translation_unit_registration_order_ordinal"
            ),
        },
    )

__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
