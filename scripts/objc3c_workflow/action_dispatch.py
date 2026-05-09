"""Action dispatch and execution-result handling for the objc3c workflow CLI."""

from __future__ import annotations

from collections.abc import Sequence

from scripts.objc3c_workflow.action_handlers import ACTION_HANDLERS
from scripts.objc3c_workflow.action_payloads import (
    describe_action_payload,
    list_actions_payload,
)
from scripts.objc3c_workflow.command_result import (
    WorkflowCommandResult,
    accepted_action,
    completed_action,
    emit_result_error,
    rejected_extra_args,
    unknown_action,
)
from scripts.objc3c_workflow.registry_views import action_spec


def resolve_registered_action(action: str, rest: Sequence[str]) -> WorkflowCommandResult:
    spec = action_spec(action)
    handler = ACTION_HANDLERS.get(action)
    if spec is None or handler is None:
        return unknown_action(action)
    if rest and not spec.pass_through_args:
        return rejected_extra_args(action, len(rest))
    return accepted_action(action, len(rest))


def execute_registered_action_with_metadata(action: str, rest: list[str]) -> WorkflowCommandResult:
    result = resolve_registered_action(action, rest)
    if not result.accepted:
        return result
    exit_code = ACTION_HANDLERS[action](rest)
    return completed_action(action, exit_code, len(rest))


def execute_registered_action(action: str, rest: list[str]) -> int:
    result = execute_registered_action_with_metadata(action, rest)
    if not result.accepted:
        emit_result_error(result)
    return result.exit_code
