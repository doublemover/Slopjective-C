"""Shared profile model for public test orchestration actions."""

from __future__ import annotations

from collections.abc import Sequence
from dataclasses import dataclass

WorkflowStep = tuple[str, Sequence[str]]


@dataclass(frozen=True)
class TestOrchestrationProfile:
    action: str
    steps: tuple[WorkflowStep, ...]

    def materialize(self) -> list[WorkflowStep]:
        return [(action, list(command)) for action, command in self.steps]
