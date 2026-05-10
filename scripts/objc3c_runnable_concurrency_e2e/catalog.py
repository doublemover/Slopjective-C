"""Packaged concurrency scenario catalog."""

from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class ConcurrencyScenario:
    scenario_id: str
    fixture_manifest_key: str
    probe_manifest_key: str
    probe_label: str
    probe_executable_name: str
    expected_payload: dict[str, object]

    @property
    def compile_fixture_action(self) -> str:
        return f"compile-{self.scenario_id}-fixture"

    @property
    def compile_probe_action(self) -> str:
        return f"compile-packaged-{self.scenario_id}-probe"

    @property
    def run_probe_action(self) -> str:
        return f"run-packaged-{self.scenario_id}-probe"


CONCURRENCY_SCENARIOS = (
    ConcurrencyScenario(
        scenario_id="continuation",
        fixture_manifest_key="continuation_runtime_fixture",
        probe_manifest_key="continuation_runtime_probe",
        probe_label="packaged concurrency continuation probe",
        probe_executable_name="probe.exe",
        expected_payload={
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
    ),
    ConcurrencyScenario(
        scenario_id="task",
        fixture_manifest_key="task_runtime_fixture",
        probe_manifest_key="task_runtime_probe",
        probe_label="packaged concurrency task probe",
        probe_executable_name="probe.exe",
        expected_payload={
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
    ),
    ConcurrencyScenario(
        scenario_id="actor",
        fixture_manifest_key="actor_runtime_fixture",
        probe_manifest_key="actor_runtime_probe",
        probe_label="packaged concurrency actor probe",
        probe_executable_name="probe.exe",
        expected_payload={
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
    ),
)


__all__ = ["CONCURRENCY_SCENARIOS", "ConcurrencyScenario"]
