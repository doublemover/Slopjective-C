"""Command result metadata for objc3c workflow dispatch."""

from __future__ import annotations

import sys
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


def accepted_action(action: str, arg_count: int) -> WorkflowCommandResult:
    return WorkflowCommandResult(
        action=action,
        status="accepted",
        exit_code=0,
        pass_through_arg_count=arg_count,
    )


def unknown_action(action: str) -> WorkflowCommandResult:
    return WorkflowCommandResult(
        action=action,
        status="unknown-action",
        exit_code=2,
        message=f"unknown action: {action}",
        accepted=False,
    )


def rejected_extra_args(action: str, arg_count: int) -> WorkflowCommandResult:
    return WorkflowCommandResult(
        action=action,
        status="extra-arguments-rejected",
        exit_code=2,
        message=f"action does not accept extra arguments: {action}",
        accepted=False,
        pass_through_arg_count=arg_count,
    )


def completed_action(action: str, exit_code: int, arg_count: int) -> WorkflowCommandResult:
    return WorkflowCommandResult(
        action=action,
        status="completed" if exit_code == 0 else "failed",
        exit_code=exit_code,
        pass_through_arg_count=arg_count,
    )


def emit_result_error(result: WorkflowCommandResult) -> None:
    if result.message:
        print(result.message, file=sys.stderr)
