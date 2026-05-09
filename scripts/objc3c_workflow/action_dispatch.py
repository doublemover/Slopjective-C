"""Action dispatch and execution-result handling for the objc3c workflow CLI."""

from __future__ import annotations

from scripts.objc3c_workflow.action_acceptance import resolve_registered_action
from scripts.objc3c_workflow.action_execution import execute_registered_action_with_metadata
from scripts.objc3c_workflow.action_payloads import (
    describe_action_payload,
    list_actions_payload,
)
from scripts.objc3c_workflow.command_result_output import emit_result_error


def execute_registered_action(action: str, rest: list[str]) -> int:
    result = execute_registered_action_with_metadata(action, rest)
    if not result.accepted:
        emit_result_error(result)
    return result.exit_code


__all__ = [
    "describe_action_payload",
    "execute_registered_action",
    "execute_registered_action_with_metadata",
    "list_actions_payload",
    "resolve_registered_action",
]
