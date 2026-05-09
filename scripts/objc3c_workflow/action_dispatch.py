"""Action dispatch and execution-result handling for the objc3c workflow CLI."""

from __future__ import annotations

from scripts.objc3c_workflow.action_resolution import (
    execute_registered_handler,
    resolve_registered_action,
)
from scripts.objc3c_workflow.action_payloads import (
    describe_action_payload,
    list_actions_payload,
)
from scripts.objc3c_workflow.command_result import (
    WorkflowCommandResult,
    completed_action,
    emit_result_error,
)


def execute_registered_action_with_metadata(action: str, rest: list[str]) -> WorkflowCommandResult:
    result = resolve_registered_action(action, rest)
    if not result.accepted:
        return result
    exit_code = execute_registered_handler(action, rest)
    return completed_action(action, exit_code, len(rest))


def execute_registered_action(action: str, rest: list[str]) -> int:
    result = execute_registered_action_with_metadata(action, rest)
    if not result.accepted:
        emit_result_error(result)
    return result.exit_code
