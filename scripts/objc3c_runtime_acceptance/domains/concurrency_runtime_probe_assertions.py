"""Shared concurrency runtime probe payload assertions."""

from __future__ import annotations

from collections.abc import Mapping

from objc3c_runtime_acceptance.expectation_matching import expect

EXPECTED_RUNTIME_ABI_PAYLOADS = {
    "continuation": {
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
    },
    "task": {
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
    },
    "actor": {
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
    },
}

EXPECTED_LIVE_RUNTIME_PAYLOADS = {
    "continuation": {
        "runTask": 7,
        "loadValue": 7,
        "copy_status": 0,
        "allocation_call_count": 2,
        "handoff_call_count": 2,
        "resume_call_count": 2,
        "live_continuation_handle_count": 0,
        "last_handoff_executor_tag": 1,
        "last_resume_return_value": 7,
    },
    "task": EXPECTED_RUNTIME_ABI_PAYLOADS["task"],
    "actor": {
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
    },
}


def expect_probe_payload_fields(
    payload: Mapping[str, object],
    expected_payload: Mapping[str, object],
    probe_description: str,
) -> None:
    for field_name, expected_value in expected_payload.items():
        expect(
            payload.get(field_name) == expected_value,
            f"expected {probe_description} to preserve {field_name}",
        )


__all__ = [
    "EXPECTED_LIVE_RUNTIME_PAYLOADS",
    "EXPECTED_RUNTIME_ABI_PAYLOADS",
    "expect_probe_payload_fields",
]
