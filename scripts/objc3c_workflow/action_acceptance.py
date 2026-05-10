"""Registered workflow action acceptance checks."""

from __future__ import annotations

from collections.abc import Sequence

from scripts.objc3c_workflow.action_acceptance_policy import (
    action_arg_count,
    action_is_registered,
    action_rejects_extra_args,
)
from scripts.objc3c_workflow.action_metadata_lookup import workflow_action_metadata
from scripts.objc3c_workflow.command_result_acceptance import (
    accepted_action,
    rejected_extra_args,
    unknown_action,
)
from scripts.objc3c_workflow.command_result_model import WorkflowCommandResult


def resolve_registered_action(
    action: str,
    rest: Sequence[str],
) -> WorkflowCommandResult:
    metadata = workflow_action_metadata(action)
    if not action_is_registered(metadata):
        return unknown_action(action)
    if action_rejects_extra_args(metadata, rest):
        return rejected_extra_args(action, action_arg_count(rest))
    return accepted_action(action, action_arg_count(rest))


__all__ = ["resolve_registered_action"]
