"""Concurrency runtime acceptance source-manifest cases."""

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

from ..runtime_contract_concurrency import RUNTIME_UNIFIED_CONCURRENCY_SOURCE_SURFACE_CONTRACT_ID


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


__all__ = ["check_unified_concurrency_runtime_architecture_case"]
