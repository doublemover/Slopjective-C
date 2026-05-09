"""Registered workflow action acceptance checks."""

from __future__ import annotations

from collections.abc import Sequence

from scripts.objc3c_workflow.action_handler_lookup import registered_action_handler
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
    handler = registered_action_handler(action)
    if spec is None or handler is None:
        return unknown_action(action)
    if rest and not spec.pass_through_args:
        return rejected_extra_args(action, len(rest))
    return accepted_action(action, len(rest))


__all__ = ["resolve_registered_action"]
