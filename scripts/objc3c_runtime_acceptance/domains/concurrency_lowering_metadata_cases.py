"""Concurrency runtime acceptance lowering metadata cases."""

from __future__ import annotations

import json
import re
from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.fixture_compilation import compile_fixture_outputs
from objc3c_runtime_acceptance.paths import ROOT

from ..runtime_contract_concurrency import RUNTIME_UNIFIED_CONCURRENCY_LOWERING_METADATA_SURFACE_CONTRACT_ID


def _assert_return_await_cleanup_ordering(ir_text: str) -> list[str]:
    handoff_symbol = "objc3_runtime_handoff_async_continuation_to_executor_i32"
    allocate_symbol = "objc3_runtime_allocate_async_continuation_i32"
    resume_symbol = "objc3_runtime_resume_async_continuation_i32"
    push_symbol = "objc3_runtime_push_autoreleasepool_scope"
    pop_symbol = "objc3_runtime_pop_autoreleasepool_scope"
    function_blocks = re.findall(
        r"define\s+[^@]+@([^(]+)\([^)]*\)[^{]*\{\n(.*?)\n\}",
        ir_text,
        flags=re.DOTALL,
    )
    await_blocks = [
        (symbol, body) for symbol, body in function_blocks if handoff_symbol in body
    ]
    expect(
        len(await_blocks) == 2,
        "expected async cleanup fixture to emit exactly two return-await bodies",
    )
    asserted_symbols: list[str] = []
    for symbol, body in await_blocks:
        push = body.find(push_symbol)
        allocate = body.find(allocate_symbol)
        pop = body.find(pop_symbol)
        handoff = body.find(handoff_symbol)
        resume = body.find(resume_symbol)
        fetch_calls = [
            match.start()
            for match in re.finditer(r"call i32 @fetchValue\(", body)
        ]
        expect(
            push != -1
            and allocate != -1
            and pop != -1
            and handoff != -1
            and resume != -1
            and len(fetch_calls) >= 2,
            f"expected {symbol} return-await body to carry cleanup and continuation helpers",
        )
        expect(
            push
            < fetch_calls[0]
            < allocate
            < fetch_calls[1]
            < pop
            < handoff
            < resume,
            f"expected {symbol} to run defer/autoreleasepool cleanup before await handoff",
        )
        asserted_symbols.append(symbol)
    return sorted(asserted_symbols)


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

    cleanup_fixture_path = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "async_cleanup_integration_positive.objc3"
    )
    cleanup_compile_dir = case_dir / "async_cleanup" / "compile"
    _, cleanup_ll_path, cleanup_manifest_path = compile_fixture_outputs(
        cleanup_fixture_path, cleanup_compile_dir
    )
    cleanup_symbols = _assert_return_await_cleanup_ordering(
        cleanup_ll_path.read_text(encoding="utf-8")
    )
    summary["async_cleanup_ordering"] = {
        "fixture": str(cleanup_fixture_path.relative_to(ROOT)).replace("\\", "/"),
        "manifest": str(cleanup_manifest_path.relative_to(ROOT)).replace("\\", "/"),
        "ir": str(cleanup_ll_path.relative_to(ROOT)).replace("\\", "/"),
        "asserted_functions": cleanup_symbols,
        "ordering": "defer-and-autoreleasepool-cleanup-before-await-handoff",
    }

    return CaseResult(
        case_id="unified-concurrency-lowering-metadata-surface",
        probe="compile-manifest-lowering-metadata-surface",
        fixture="tests/tooling/fixtures/native/async_lowering_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary=summary,
    )


__all__ = ["check_unified_concurrency_lowering_metadata_surface_case"]
