"""Action registry and handler consistency for workflow dispatch."""

from __future__ import annotations

from collections.abc import Sequence

from scripts.objc3c_workflow.action_handlers import ACTION_HANDLERS
from scripts.objc3c_workflow.command_result import (
    WorkflowCommandResult,
    accepted_action,
    rejected_extra_args,
    unknown_action,
)
from scripts.objc3c_workflow.registry_views import action_names, action_spec


def missing_action_handlers() -> list[str]:
    return sorted(set(action_names()) - set(ACTION_HANDLERS))


def orphan_action_handlers() -> list[str]:
    return sorted(set(ACTION_HANDLERS) - set(action_names()))


def action_handler_registry_is_complete() -> bool:
    return not missing_action_handlers() and not orphan_action_handlers()


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
