"""Packaged concurrency scenario catalog."""

from __future__ import annotations

from dataclasses import dataclass

try:
    from objc3c_runtime_acceptance.domains.concurrency_runtime_probe_assertions import (
        EXPECTED_LIVE_RUNTIME_PAYLOADS,
    )
except ModuleNotFoundError:  # pragma: no cover - package-path pytest compatibility
    from scripts.objc3c_runtime_acceptance.domains.concurrency_runtime_probe_assertions import (
        EXPECTED_LIVE_RUNTIME_PAYLOADS,
    )


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
        expected_payload=EXPECTED_LIVE_RUNTIME_PAYLOADS["continuation"],
    ),
    ConcurrencyScenario(
        scenario_id="task",
        fixture_manifest_key="task_runtime_fixture",
        probe_manifest_key="task_runtime_probe",
        probe_label="packaged concurrency task probe",
        probe_executable_name="probe.exe",
        expected_payload=EXPECTED_LIVE_RUNTIME_PAYLOADS["task"],
    ),
    ConcurrencyScenario(
        scenario_id="actor",
        fixture_manifest_key="actor_runtime_fixture",
        probe_manifest_key="actor_runtime_probe",
        probe_label="packaged concurrency actor probe",
        probe_executable_name="probe.exe",
        expected_payload=EXPECTED_LIVE_RUNTIME_PAYLOADS["actor"],
    ),
)


__all__ = ["CONCURRENCY_SCENARIOS", "ConcurrencyScenario"]
