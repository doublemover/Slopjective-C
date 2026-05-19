"""Command result factories for pre-execution action acceptance."""

from __future__ import annotations

from .command_result_diagnostics import (
    extra_arguments_result_message,
    unknown_action_result_message,
)
from .command_result_model import WorkflowCommandResult
from .command_result_policy import (
    COMMAND_RESULT_ACCEPTED_EXIT_CODE,
    COMMAND_RESULT_REJECTED_EXIT_CODE,
)
from .command_result_status import (
    STATUS_ACCEPTED,
    STATUS_EXTRA_ARGUMENTS_REJECTED,
    STATUS_UNKNOWN_ACTION,
)


def accepted_action(action: str, arg_count: int) -> WorkflowCommandResult:
    return WorkflowCommandResult(
        action=action,
        status=STATUS_ACCEPTED,
        exit_code=COMMAND_RESULT_ACCEPTED_EXIT_CODE,
        pass_through_arg_count=arg_count,
    )


def unknown_action(action: str) -> WorkflowCommandResult:
    return WorkflowCommandResult(
        action=action,
        status=STATUS_UNKNOWN_ACTION,
        exit_code=COMMAND_RESULT_REJECTED_EXIT_CODE,
        message=unknown_action_result_message(action),
        accepted=False,
    )


def rejected_extra_args(action: str, arg_count: int) -> WorkflowCommandResult:
    return WorkflowCommandResult(
        action=action,
        status=STATUS_EXTRA_ARGUMENTS_REJECTED,
        exit_code=COMMAND_RESULT_REJECTED_EXIT_CODE,
        message=extra_arguments_result_message(action),
        accepted=False,
        pass_through_arg_count=arg_count,
    )


__all__ = ["accepted_action", "rejected_extra_args", "unknown_action"]
