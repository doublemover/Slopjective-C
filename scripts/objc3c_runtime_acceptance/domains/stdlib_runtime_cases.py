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
    expect_equal(payload.get("text_total_call_count"), 17, "stdlib text calls drifted")
    expect_equal(payload.get("text_record_count"), 4, "stdlib text record count drifted")
    expect_equal(
        payload.get("collections_total_call_count"),
        114,
        "stdlib collections calls drifted",
    )
    expect_equal(payload.get("array_record_count"), 4, "stdlib array record count drifted")
    expect_equal(
        payload.get("descriptor_record_count"),
        3,
        "stdlib collection descriptor record count drifted",
    )
    expect_equal(
        payload.get("descriptor_create_call_count"),
        4,
        "stdlib collection descriptor create calls drifted",
    )
    expect_equal(
        payload.get("descriptor_query_call_count"),
        4,
        "stdlib collection descriptor query calls drifted",
    )
    expect_equal(
        payload.get("descriptor_mismatch_failure_count"),
        2,
        "stdlib collection descriptor mismatch evidence drifted",
    )
    expect_equal(
        payload.get("last_descriptor_status"),
        30641,
        "stdlib collection last descriptor status drifted",
    )
    expect_equal(
        payload.get("last_descriptor_actual_kind"),
        4,
        "stdlib collection actual descriptor kind drifted",
    )
    expect_equal(
        payload.get("last_descriptor_expected_kind"),
        1,
        "stdlib collection expected descriptor kind drifted",
    )
    expect_equal(payload.get("map_record_count"), 3, "stdlib map record count drifted")
    expect_equal(
        payload.get("map_mutation_call_count"),
        6,
        "stdlib map mutation calls drifted",
    )
    expect_equal(
        payload.get("map_delete_remaining_count"),
        1,
        "stdlib map delete result drifted",
    )
    expect_equal(payload.get("set_record_count"), 2, "stdlib set record count drifted")
    expect_equal(
        payload.get("set_mutation_call_count"),
        3,
        "stdlib set mutation calls drifted",
    )
    expect_equal(
        payload.get("set_delete_remaining_count"),
        1,
        "stdlib set delete result drifted",
    )
    expect_equal(payload.get("slice_record_count"), 1, "stdlib slice record count drifted")
    expect_equal(
        payload.get("iterator_record_count"),
        8,
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
                "objc3_runtime_stdlib_text_utf8_storage_i32",
                "objc3_runtime_stdlib_text_concat_i32",
                "objc3_runtime_stdlib_text_builder_i32",
                "objc3_runtime_stdlib_text_builder_append_utf8_i32",
                "objc3_runtime_stdlib_text_builder_build_i32",
                "objc3_runtime_stdlib_text_scalar_iterator_i32",
                "objc3_runtime_stdlib_text_scalar_iterator_next_or_i32",
                "objc3_runtime_stdlib_collections_array3_i32",
                "objc3_runtime_stdlib_collections_array_storage_i32",
                "objc3_runtime_stdlib_collections_mutable_array_i32",
                "objc3_runtime_stdlib_collections_mutable_array_append_i32",
                "objc3_runtime_stdlib_collections_array_get_or_i32",
                "objc3_runtime_stdlib_collections_array_sum_i32",
                "objc3_runtime_stdlib_collections_map_lookup_or_i32",
                "objc3_runtime_stdlib_collections_map_insert_i32",
                "objc3_runtime_stdlib_collections_map_delete_i32",
                "objc3_runtime_stdlib_collections_map_key_iterator_i32",
                "objc3_runtime_stdlib_collections_map_value_iterator_i32",
                "objc3_runtime_stdlib_collections_set3_i32",
                "objc3_runtime_stdlib_collections_set_delete_i32",
                "objc3_runtime_stdlib_collections_set_iterator_i32",
                "objc3_runtime_stdlib_collections_array_slice_i32",
                "objc3_runtime_stdlib_collections_iterator_next_or_i32",
            ],
            "text_record_count": payload.get("text_record_count"),
            "array_record_count": payload.get("array_record_count"),
            "descriptor_record_count": payload.get("descriptor_record_count"),
            "descriptor_mismatch_failure_count": payload.get(
                "descriptor_mismatch_failure_count"
            ),
            "map_record_count": payload.get("map_record_count"),
            "map_mutation_call_count": payload.get("map_mutation_call_count"),
            "set_record_count": payload.get("set_record_count"),
            "set_mutation_call_count": payload.get("set_mutation_call_count"),
            "slice_record_count": payload.get("slice_record_count"),
            "iterator_record_count": payload.get("iterator_record_count"),
        },
    )


def check_stdlib_runtime_storage_substrate_probe_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / "stdlib-runtime-storage-substrate-probe"
    probe = ROOT / "tests" / "tooling" / "runtime" / "stdlib_runtime_storage_substrate_probe.cpp"
    exe_path = case_dir / "stdlib_runtime_storage_substrate_probe.exe"
    compile_probe(clangxx, probe, exe_path, [])
    payload = parse_json_output(run_probe(exe_path), "stdlib runtime storage substrate probe")
    expect_equal(
        payload.get("text_handle_generation"),
        3,
        "stdlib text handle generation drifted after reset",
    )
    expect_equal(
        payload.get("text_cross_kind_failures"),
        1,
        "stdlib text cross-kind failures drifted",
    )
    expect_equal(
        payload.get("text_stale_failures"),
        1,
        "stdlib text stale-handle failures drifted",
    )
    expect_equal(
        payload.get("text_stale_record_count"),
        1,
        "stdlib text stale-record accounting drifted",
    )
    expect_equal(
        payload.get("text_iterator_invalidations"),
        1,
        "stdlib text iterator invalidation count drifted",
    )
    expect_equal(
        payload.get("collections_handle_generation"),
        3,
        "stdlib collection handle generation drifted after reset",
    )
    expect_equal(
        payload.get("collections_cross_kind_failures"),
        1,
        "stdlib collection cross-kind failures drifted",
    )
    expect_equal(
        payload.get("collections_stale_failures"),
        1,
        "stdlib collection stale-handle failures drifted",
    )
    expect_equal(
        payload.get("collections_stale_record_count"),
        1,
        "stdlib collection stale-record accounting drifted",
    )
    expect_equal(
        payload.get("collections_mutation_generation"),
        3,
        "stdlib collection mutation generation drifted",
    )
    expect_equal(
        payload.get("collections_iterator_invalidations"),
        1,
        "stdlib collection iterator invalidation count drifted",
    )
    return CaseResult(
        case_id="stdlib-runtime-storage-substrate-probe",
        probe="tests/tooling/runtime/stdlib_runtime_storage_substrate_probe.cpp",
        fixture="tests/tooling/fixtures/native/execution/positive/stdlib_runtime_storage_substrate_handles.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "kind": "stdlib-shared-runtime-storage-substrate-probe",
            "runtime_abi": [
                "objc3_runtime_stdlib_text_utf8_storage_i32",
                "objc3_runtime_stdlib_text_builder_i32",
                "objc3_runtime_stdlib_text_builder_append_utf8_i32",
                "objc3_runtime_stdlib_text_builder_build_i32",
                "objc3_runtime_stdlib_text_scalar_iterator_i32",
                "objc3_runtime_stdlib_text_scalar_iterator_next_or_i32",
                "objc3_runtime_stdlib_collections_array_storage_i32",
                "objc3_runtime_stdlib_collections_mutable_array_i32",
                "objc3_runtime_stdlib_collections_mutable_array_append_i32",
            ],
            "text_handle_generation": payload.get("text_handle_generation"),
            "text_stale_record_count": payload.get("text_stale_record_count"),
            "collections_handle_generation": payload.get("collections_handle_generation"),
            "collections_mutation_generation": payload.get(
                "collections_mutation_generation"
            ),
            "collections_stale_record_count": payload.get(
                "collections_stale_record_count"
            ),
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
    expect_equal(
        payload.get("unsupported_task_kind_result"),
        -2,
        "stdlib unsupported task-kind rejection drifted",
    )
    expect_equal(
        payload.get("invalid_group_executor_result"),
        -1,
        "stdlib invalid task-group executor rejection drifted",
    )
    expect_equal(
        payload.get("invalid_actor_bind_result"),
        0,
        "stdlib invalid actor bind rejection drifted",
    )
    expect_equal(
        payload.get("actor_invalid_handle_failure_code"),
        2,
        "stdlib invalid actor handle diagnostic drifted",
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
            "unsupported_task_kind_result": payload.get(
                "unsupported_task_kind_result"
            ),
            "invalid_group_executor_result": payload.get(
                "invalid_group_executor_result"
            ),
            "invalid_actor_bind_result": payload.get("invalid_actor_bind_result"),
            "actor_invalid_handle_failure_code": payload.get(
                "actor_invalid_handle_failure_code"
            ),
        },
    )


__all__ = [
    "check_stdlib_core_runtime_probe_case",
    "check_stdlib_foundation_next_runtime_probe_case",
    "check_stdlib_runtime_storage_substrate_probe_case",
    "check_stdlib_concurrency_runtime_probe_case",
]
