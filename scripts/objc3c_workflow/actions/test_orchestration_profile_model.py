"""Shared profile model for public test orchestration actions."""

from __future__ import annotations

from collections.abc import Sequence
from dataclasses import dataclass

WorkflowStep = tuple[str, Sequence[str]]


@dataclass(frozen=True)
class TestOrchestrationStep:
    action: str
    command: Sequence[str]
    source_owner: str
    command_owner: str
    hard_blocking_decision_owner: str

    def materialize(self) -> WorkflowStep:
        return self.action, list(self.command)

    def owner_payload(self) -> dict[str, object]:
        return {
            "action": self.action,
            "source_owner": self.source_owner,
            "command_owner": self.command_owner,
            "hard_blocking_decision_owner": self.hard_blocking_decision_owner,
        }


@dataclass(frozen=True)
class TestOrchestrationProfile:
    action: str
    profile_owner: str
    source_owner: str
    command_owner: str
    report_owner: str
    hard_blocking_decision_owner: str
    steps: tuple[TestOrchestrationStep, ...]

    def materialize(self) -> list[WorkflowStep]:
        return [step.materialize() for step in self.steps]

    def owner_payload(self) -> dict[str, object]:
        return {
            "action": self.action,
            "profile_owner": self.profile_owner,
            "source_owner": self.source_owner,
            "command_owner": self.command_owner,
            "report_owner": self.report_owner,
            "hard_blocking_decision_owner": self.hard_blocking_decision_owner,
            "steps": [step.owner_payload() for step in self.steps],
        }


def workflow_step(
    action: str,
    command: Sequence[str],
    *,
    source_owner: str,
    command_owner: str = "test_orchestration_commands",
    hard_blocking_decision_owner: str = "test_orchestration_composites",
) -> TestOrchestrationStep:
    return TestOrchestrationStep(
        action=action,
        command=command,
        source_owner=source_owner,
        command_owner=command_owner,
        hard_blocking_decision_owner=hard_blocking_decision_owner,
    )
