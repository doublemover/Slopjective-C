"""Owned execution policy for registered workflow action handlers."""

from __future__ import annotations

from collections.abc import Sequence

from scripts.objc3c_workflow.command_result_model import WorkflowCommandResult

ACTION_EXECUTION_POLICY_OWNER = "objc3c-workflow-action-execution-policy"
ACTION_EXECUTION_COMPLETION_OWNER = "objc3c-workflow-action-completion-policy"


def handler_args(rest: Sequence[str]) -> list[str]:
    return list(rest)


def should_execute_handler(result: WorkflowCommandResult) -> bool:
    return result.accepted


def completion_arg_count(rest: Sequence[str]) -> int:
    return len(rest)


__all__ = [
    "ACTION_EXECUTION_COMPLETION_OWNER",
    "ACTION_EXECUTION_POLICY_OWNER",
    "completion_arg_count",
    "handler_args",
    "should_execute_handler",
]
