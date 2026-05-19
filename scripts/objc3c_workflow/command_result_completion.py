"""Command result factories for completed action execution."""

from __future__ import annotations

from .command_result_model import WorkflowCommandResult
from .command_result_status import completion_status


def completed_action(action: str, exit_code: int, arg_count: int) -> WorkflowCommandResult:
    return WorkflowCommandResult(
        action=action,
        status=completion_status(exit_code),
        exit_code=exit_code,
        pass_through_arg_count=arg_count,
    )


__all__ = ["completed_action"]
