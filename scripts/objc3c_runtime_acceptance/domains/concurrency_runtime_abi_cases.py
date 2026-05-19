"""Concurrency runtime acceptance runtime ABI cases."""

from __future__ import annotations

from pathlib import Path

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.concurrency_runtime_probe_assertions import (
    EXPECTED_RUNTIME_ABI_PAYLOADS,
    expect_probe_payload_fields,
)
from objc3c_runtime_acceptance.paths import ROOT
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_key_value_output
from objc3c_runtime_acceptance.probes import run_probe

from ..runtime_contract_concurrency import (
    ACTOR_RUNTIME_ABI_PROBE,
    CONTINUATION_RUNTIME_ABI_PROBE,
    TASK_RUNTIME_ABI_PROBE,
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
    expect_probe_payload_fields(
        continuation_payload,
        EXPECTED_RUNTIME_ABI_PAYLOADS["continuation"],
        "unified concurrency continuation runtime ABI probe",
    )

    task_probe = ROOT / Path(TASK_RUNTIME_ABI_PROBE)
    task_exe = case_dir / "task_runtime_abi_probe.exe"
    compile_probe(clangxx, task_probe, task_exe, [])
    task_payload = parse_key_value_output(
        run_probe(task_exe), "unified concurrency task runtime ABI probe"
    )
    expect_probe_payload_fields(
        task_payload,
        EXPECTED_RUNTIME_ABI_PAYLOADS["task"],
        "unified concurrency task runtime ABI probe",
    )

    actor_probe = ROOT / Path(ACTOR_RUNTIME_ABI_PROBE)
    actor_exe = case_dir / "actor_runtime_abi_probe.exe"
    compile_probe(clangxx, actor_probe, actor_exe, [])
    actor_payload = parse_key_value_output(
        run_probe(actor_exe), "unified concurrency actor runtime ABI probe"
    )
    expect_probe_payload_fields(
        actor_payload,
        EXPECTED_RUNTIME_ABI_PAYLOADS["actor"],
        "unified concurrency actor runtime ABI probe",
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


__all__ = ["check_unified_concurrency_runtime_abi_case"]
