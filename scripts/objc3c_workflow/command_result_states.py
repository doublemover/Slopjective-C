"""Command result factories for workflow dispatch states."""

from __future__ import annotations

from .command_result_model import WorkflowCommandResult


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
