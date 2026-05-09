"""Registered action resolution for workflow dispatch."""

from __future__ import annotations

from collections.abc import Sequence

from scripts.objc3c_workflow.action_handlers import ACTION_HANDLERS
from scripts.objc3c_workflow.command_result_acceptance import (
    accepted_action,
    rejected_extra_args,
    unknown_action,
)
from scripts.objc3c_workflow.command_result_model import WorkflowCommandResult
from scripts.objc3c_workflow.registry_views import action_spec


def resolve_registered_action(
    action: str,
    rest: Sequence[str],
) -> WorkflowCommandResult:
    spec = action_spec(action)
    handler = ACTION_HANDLERS.get(action)
    if spec is None or handler is None:
        return unknown_action(action)
    if rest and not spec.pass_through_args:
        return rejected_extra_args(action, len(rest))
    return accepted_action(action, len(rest))


def execute_registered_handler(action: str, rest: list[str]) -> int:
    return ACTION_HANDLERS[action](rest)
