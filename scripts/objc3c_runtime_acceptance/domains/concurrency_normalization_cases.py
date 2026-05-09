"""Concurrency runtime acceptance normalization cases."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.fixture_compilation import (
    compile_fixture_manifest_only,
    compile_fixture_outputs,
)
from objc3c_runtime_acceptance.paths import ROOT

from ..runtime_contract_concurrency import RUNTIME_ASYNC_TASK_ACTOR_NORMALIZATION_COMPLETION_SURFACE_CONTRACT_ID


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


__all__ = ["check_async_task_actor_normalization_completion_case"]
