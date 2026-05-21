"""Stdlib runtime-backed helper acceptance cases."""

from __future__ import annotations

from pathlib import Path

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.expectation_matching import expect_equal
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_json_output
from objc3c_runtime_acceptance.probes import run_probe

from ..paths import ROOT


def check_stdlib_core_runtime_probe_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "stdlib-core-runtime-probe"
    probe = ROOT / "tests" / "tooling" / "runtime" / "stdlib_core_runtime_probe.cpp"
    exe_path = case_dir / "stdlib_core_runtime_probe.exe"
    compile_probe(clangxx, probe, exe_path, [])
    payload = parse_json_output(run_probe(exe_path), "stdlib core runtime probe")
    expect_equal(payload.get("total_call_count"), 20, "stdlib core runtime calls drifted")
    expect_equal(payload.get("revision_call_count"), 2, "stdlib revision helper calls drifted")
    expect_equal(
        payload.get("capability_call_count"),
        5,
        "stdlib capability helper calls drifted",
    )
    expect_equal(payload.get("option_call_count"), 4, "stdlib option helper calls drifted")
    expect_equal(payload.get("count_call_count"), 2, "stdlib count helper calls drifted")
    expect_equal(payload.get("prefix_call_count"), 3, "stdlib prefix helper calls drifted")
    expect_equal(payload.get("map_call_count"), 4, "stdlib map helper calls drifted")
    expect_equal(payload.get("last_result"), 19, "stdlib last helper result drifted")
    return CaseResult(
        case_id="stdlib-core-runtime-probe",
        probe="tests/tooling/runtime/stdlib_core_runtime_probe.cpp",
        fixture="stdlib/modules/objc3.core/module.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "kind": "stdlib-core-runtime-backed-helper-probe",
            "runtime_abi": [
                "objc3_runtime_stdlib_core_language_revision_i32",
                "objc3_runtime_stdlib_core_profile_revision_i32",
                "objc3_runtime_stdlib_core_has_capability_i32",
                "objc3_runtime_stdlib_core_option_has_value_i32",
                "objc3_runtime_stdlib_core_option_unwrap_or_i32",
                "objc3_runtime_stdlib_core_count_i32",
                "objc3_runtime_stdlib_core_prefix_count_i32",
                "objc3_runtime_stdlib_core_map_entry_present_i32",
                "objc3_runtime_stdlib_core_map_entry_value_or_i32",
            ],
            "total_call_count": payload.get("total_call_count"),
            "revision_call_count": payload.get("revision_call_count"),
            "capability_call_count": payload.get("capability_call_count"),
            "option_call_count": payload.get("option_call_count"),
            "count_call_count": payload.get("count_call_count"),
            "prefix_call_count": payload.get("prefix_call_count"),
            "map_call_count": payload.get("map_call_count"),
            "last_result": payload.get("last_result"),
        },
    )


def check_stdlib_foundation_next_runtime_probe_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / "stdlib-foundation-next-runtime-probe"
    probe = ROOT / "tests" / "tooling" / "runtime" / "stdlib_foundation_next_runtime_probe.cpp"
    exe_path = case_dir / "stdlib_foundation_next_runtime_probe.exe"
    compile_probe(clangxx, probe, exe_path, [])
    payload = parse_json_output(
        run_probe(exe_path), "stdlib foundation-next runtime probe"
    )
    expect_equal(payload.get("text_total_call_count"), 14, "stdlib text calls drifted")
    expect_equal(payload.get("text_record_count"), 3, "stdlib text record count drifted")
    expect_equal(
        payload.get("collections_total_call_count"),
        44,
        "stdlib collections calls drifted",
    )
    expect_equal(payload.get("array_record_count"), 1, "stdlib array record count drifted")
    expect_equal(payload.get("map_record_count"), 1, "stdlib map record count drifted")
    expect_equal(payload.get("set_record_count"), 1, "stdlib set record count drifted")
    expect_equal(payload.get("slice_record_count"), 1, "stdlib slice record count drifted")
    expect_equal(
        payload.get("iterator_record_count"),
        3,
        "stdlib iterator record count drifted",
    )
    expect_equal(
        payload.get("last_collection_status"),
        30633,
        "stdlib collection invalid-count status drifted",
    )
    return CaseResult(
        case_id="stdlib-foundation-next-runtime-probe",
        probe="tests/tooling/runtime/stdlib_foundation_next_runtime_probe.cpp",
        fixture="stdlib/modules/objc3.text/module.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "kind": "stdlib-foundation-next-runtime-backed-text-collections-probe",
            "runtime_abi": [
                "objc3_runtime_stdlib_text_utf8_literal_i32",
                "objc3_runtime_stdlib_text_concat_i32",
                "objc3_runtime_stdlib_collections_array3_i32",
                "objc3_runtime_stdlib_collections_array_get_or_i32",
                "objc3_runtime_stdlib_collections_map_lookup_or_i32",
                "objc3_runtime_stdlib_collections_set3_i32",
                "objc3_runtime_stdlib_collections_array_slice_i32",
                "objc3_runtime_stdlib_collections_iterator_next_or_i32",
            ],
            "text_record_count": payload.get("text_record_count"),
            "array_record_count": payload.get("array_record_count"),
            "map_record_count": payload.get("map_record_count"),
            "set_record_count": payload.get("set_record_count"),
            "slice_record_count": payload.get("slice_record_count"),
            "iterator_record_count": payload.get("iterator_record_count"),
        },
    )


def check_stdlib_concurrency_runtime_probe_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / "stdlib-concurrency-runtime-probe"
    probe = ROOT / "tests" / "tooling" / "runtime" / "stdlib_concurrency_runtime_probe.cpp"
    exe_path = case_dir / "stdlib_concurrency_runtime_probe.exe"
    compile_probe(clangxx, probe, exe_path, [])
    payload = parse_json_output(run_probe(exe_path), "stdlib concurrency runtime probe")
    expect_equal(payload.get("spawn_call_count"), 3, "stdlib task spawn calls drifted")
    expect_equal(payload.get("scope_call_count"), 1, "stdlib task group scope calls drifted")
    expect_equal(payload.get("add_task_call_count"), 2, "stdlib task group add calls drifted")
    expect_equal(payload.get("wait_next_call_count"), 2, "stdlib task group wait calls drifted")
    expect_equal(payload.get("cancel_all_call_count"), 1, "stdlib cancellation calls drifted")
    expect_equal(payload.get("executor_hop_call_count"), 1, "stdlib executor hop calls drifted")
    expect_equal(payload.get("scheduler_enqueue_count"), 3, "stdlib scheduler enqueue count drifted")
    expect_equal(payload.get("scheduler_dequeue_count"), 2, "stdlib scheduler dequeue count drifted")
    expect_equal(payload.get("scheduler_sequence"), 5, "stdlib scheduler sequence drifted")
    expect_equal(payload.get("last_queue_drain_result"), 26, "stdlib task-group drain order drifted")
    expect_equal(
        payload.get("actor_bind_executor_call_count"),
        1,
        "stdlib actor mailbox executor binding calls drifted",
    )
    expect_equal(
        payload.get("actor_mailbox_enqueue_call_count"),
        1,
        "stdlib actor mailbox enqueue calls drifted",
    )
    expect_equal(
        payload.get("actor_mailbox_drain_call_count"),
        1,
        "stdlib actor mailbox drain calls drifted",
    )
    expect_equal(
        payload.get("actor_executor_binding_count"),
        1,
        "stdlib actor executor binding count drifted",
    )
    expect_equal(
        payload.get("actor_last_bound_executor_tag"),
        4,
        "stdlib actor executor binding tag drifted",
    )
    expect_equal(
        payload.get("actor_last_mailbox_drained_value"),
        13,
        "stdlib actor mailbox drained value drifted",
    )
    expect_equal(
        payload.get("actor_mailbox_identity_guard_passed"),
        1,
        "stdlib actor mailbox identity guard drifted",
    )
    expect_equal(
        payload.get("actor_executor_binding_guard_passed"),
        1,
        "stdlib actor executor binding guard drifted",
    )
    return CaseResult(
        case_id="stdlib-concurrency-runtime-probe",
        probe="tests/tooling/runtime/stdlib_concurrency_runtime_probe.cpp",
        fixture="stdlib/modules/objc3.concurrency/module.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "kind": "stdlib-concurrency-runtime-backed-public-helper-probe",
            "runtime_abi": [
                "objc3_runtime_spawn_task_i32",
                "objc3_runtime_enter_task_group_scope_i32",
                "objc3_runtime_add_task_group_task_i32",
                "objc3_runtime_wait_task_group_next_i32",
                "objc3_runtime_cancel_task_group_i32",
                "objc3_runtime_task_is_cancelled_i32",
                "objc3_runtime_task_on_cancel_i32",
                "objc3_runtime_executor_hop_i32",
                "objc3_runtime_actor_bind_executor_i32",
                "objc3_runtime_actor_mailbox_enqueue_i32",
                "objc3_runtime_actor_mailbox_drain_next_i32",
            ],
            "spawn_call_count": payload.get("spawn_call_count"),
            "scope_call_count": payload.get("scope_call_count"),
            "add_task_call_count": payload.get("add_task_call_count"),
            "wait_next_call_count": payload.get("wait_next_call_count"),
            "cancel_all_call_count": payload.get("cancel_all_call_count"),
            "executor_hop_call_count": payload.get("executor_hop_call_count"),
            "scheduler_enqueue_count": payload.get("scheduler_enqueue_count"),
            "scheduler_dequeue_count": payload.get("scheduler_dequeue_count"),
            "scheduler_sequence": payload.get("scheduler_sequence"),
            "last_queue_drain_result": payload.get("last_queue_drain_result"),
            "actor_bind_executor_call_count": payload.get(
                "actor_bind_executor_call_count"
            ),
            "actor_mailbox_enqueue_call_count": payload.get(
                "actor_mailbox_enqueue_call_count"
            ),
            "actor_mailbox_drain_call_count": payload.get(
                "actor_mailbox_drain_call_count"
            ),
            "actor_executor_binding_count": payload.get(
                "actor_executor_binding_count"
            ),
            "actor_last_mailbox_drained_value": payload.get(
                "actor_last_mailbox_drained_value"
            ),
        },
    )


__all__ = [
    "check_stdlib_core_runtime_probe_case",
    "check_stdlib_foundation_next_runtime_probe_case",
    "check_stdlib_concurrency_runtime_probe_case",
]
