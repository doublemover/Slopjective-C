"""Registered workflow action execution."""

from __future__ import annotations

from scripts.objc3c_workflow.action_acceptance import resolve_registered_action
from scripts.objc3c_workflow.action_execution_policy import (
    completion_arg_count,
    handler_args,
    should_execute_handler,
)
from scripts.objc3c_workflow.action_handler_lookup import require_registered_action_handler
from scripts.objc3c_workflow.command_result_completion import completed_action
from scripts.objc3c_workflow.command_result_model import WorkflowCommandResult


def execute_registered_handler(action: str, rest: list[str]) -> int:
    return require_registered_action_handler(action)(handler_args(rest))


def execute_registered_action_with_metadata(action: str, rest: list[str]) -> WorkflowCommandResult:
    result = resolve_registered_action(action, rest)
    if not should_execute_handler(result):
        return result
    exit_code = execute_registered_handler(action, rest)
    return completed_action(action, exit_code, completion_arg_count(rest))


__all__ = ["execute_registered_action_with_metadata", "execute_registered_handler"]
