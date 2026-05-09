"""Concurrency runtime acceptance linked runtime cases."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.native_build import compile_fixture_outputs
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_key_value_output
from objc3c_runtime_acceptance.probes import run_probe

from ..core import (
    ACTOR_RUNTIME_ABI_PROBE,
    CONTINUATION_RUNTIME_ABI_PROBE,
    LIVE_ACTOR_RUNTIME_FIXTURE,
    LIVE_ACTOR_RUNTIME_PROBE,
    LIVE_CONTINUATION_RUNTIME_FIXTURE,
    LIVE_CONTINUATION_RUNTIME_PROBE,
    LIVE_TASK_RUNTIME_FIXTURE,
    LIVE_TASK_RUNTIME_PROBE,
    ROOT,
    RUNTIME_UNIFIED_CONCURRENCY_RUNTIME_ABI_SURFACE_CONTRACT_ID,
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
