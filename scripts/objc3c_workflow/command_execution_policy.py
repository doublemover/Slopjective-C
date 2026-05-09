"""Owned execution policy for objc3c workflow subprocess commands."""

from __future__ import annotations

from collections.abc import Sequence
from dataclasses import dataclass
from pathlib import Path

from .environment import ROOT

WORKFLOW_COMMAND_EXECUTION_OWNER = "objc3c-workflow-command-execution"
WORKFLOW_COMMAND_CWD_OWNER = "objc3c-workflow-root"
WORKFLOW_COMMAND_OUTPUT_POLICY_OWNER = "objc3c-workflow-command-output-policy"


@dataclass(frozen=True)
class WorkflowCommandExecutionPolicy:
    cwd: Path
    capture_output: bool
    owner: str = WORKFLOW_COMMAND_EXECUTION_OWNER
    cwd_owner: str = WORKFLOW_COMMAND_CWD_OWNER
    output_policy_owner: str = WORKFLOW_COMMAND_OUTPUT_POLICY_OWNER

    def subprocess_kwargs(self) -> dict[str, object]:
        return {
            "cwd": self.cwd,
            "capture_output": self.capture_output,
        }


WORKFLOW_COMMAND_EXECUTION_POLICY = WorkflowCommandExecutionPolicy(
    cwd=ROOT,
    capture_output=False,
)


def command_for_execution(command: Sequence[str]) -> tuple[str, ...]:
    return tuple(str(part) for part in command)


def workflow_command_subprocess_kwargs() -> dict[str, object]:
    return WORKFLOW_COMMAND_EXECUTION_POLICY.subprocess_kwargs()


__all__ = [
    "WORKFLOW_COMMAND_CWD_OWNER",
    "WORKFLOW_COMMAND_EXECUTION_OWNER",
    "WORKFLOW_COMMAND_EXECUTION_POLICY",
    "WORKFLOW_COMMAND_OUTPUT_POLICY_OWNER",
    "WorkflowCommandExecutionPolicy",
    "command_for_execution",
    "workflow_command_subprocess_kwargs",
]
