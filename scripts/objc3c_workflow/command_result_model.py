"""Command result model for workflow dispatch."""

from __future__ import annotations

from dataclasses import dataclass

from .environment import WORKFLOW_RUNNER_MODE, WORKFLOW_RUNNER_SURFACE


@dataclass(frozen=True)
class WorkflowCommandResult:
    action: str
    status: str
    exit_code: int
    message: str = ""
    accepted: bool = True
    pass_through_arg_count: int = 0
    mode: str = WORKFLOW_RUNNER_MODE
    runner_path: str = WORKFLOW_RUNNER_SURFACE

    def to_payload(self) -> dict[str, object]:
        return {
            "mode": self.mode,
            "runner_path": self.runner_path,
            "action": self.action,
            "status": self.status,
            "exit_code": self.exit_code,
            "accepted": self.accepted,
            "pass_through_arg_count": self.pass_through_arg_count,
            "message": self.message,
        }
