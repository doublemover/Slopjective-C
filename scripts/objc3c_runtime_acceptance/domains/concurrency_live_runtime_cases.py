"""Concurrency runtime acceptance live runtime implementation cases."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.concurrency_runtime_probe_assertions import (
    EXPECTED_LIVE_RUNTIME_PAYLOADS,
    expect_probe_payload_fields,
)
from objc3c_runtime_acceptance.fixture_compilation import compile_fixture_outputs
from objc3c_runtime_acceptance.paths import ROOT
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_key_value_output
from objc3c_runtime_acceptance.probes import run_probe

from ..runtime_contract_concurrency import (
    LIVE_ACTOR_RUNTIME_FIXTURE,
    LIVE_ACTOR_RUNTIME_PROBE,
    LIVE_CONTINUATION_RUNTIME_FIXTURE,
    LIVE_CONTINUATION_RUNTIME_PROBE,
    LIVE_TASK_RUNTIME_FIXTURE,
    LIVE_TASK_RUNTIME_PROBE,
    RUNTIME_UNIFIED_CONCURRENCY_RUNTIME_ABI_SURFACE_CONTRACT_ID,
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
    expect_probe_payload_fields(
        continuation_payload,
        EXPECTED_LIVE_RUNTIME_PAYLOADS["continuation"],
        "live unified concurrency continuation runtime probe",
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
    expect_probe_payload_fields(
        task_payload,
        EXPECTED_LIVE_RUNTIME_PAYLOADS["task"],
        "live unified concurrency task runtime probe",
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
    expect_probe_payload_fields(
        actor_payload,
        EXPECTED_LIVE_RUNTIME_PAYLOADS["actor"],
        "live unified concurrency actor runtime probe",
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


__all__ = ["check_live_unified_concurrency_runtime_implementation_case"]
