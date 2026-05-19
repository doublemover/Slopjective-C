"""Owned acceptance policy for registered workflow actions."""

from __future__ import annotations

from collections.abc import Sequence
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from scripts.objc3c_workflow.action_metadata_lookup import WorkflowActionMetadata

ACTION_ACCEPTANCE_POLICY_OWNER = "objc3c-workflow-action-acceptance-policy"
ACTION_ACCEPTANCE_EXTRA_ARG_OWNER = "objc3c-workflow-action-extra-argument-policy"
ACTION_ACCEPTANCE_UNKNOWN_OWNER = "objc3c-workflow-action-unknown-policy"


def action_is_registered(metadata: WorkflowActionMetadata) -> bool:
    return metadata.registered


def action_rejects_extra_args(
    metadata: WorkflowActionMetadata,
    rest: Sequence[str],
) -> bool:
    return bool(rest) and not metadata.pass_through_args


def action_arg_count(rest: Sequence[str]) -> int:
    return len(rest)


__all__ = [
    "ACTION_ACCEPTANCE_EXTRA_ARG_OWNER",
    "ACTION_ACCEPTANCE_POLICY_OWNER",
    "ACTION_ACCEPTANCE_UNKNOWN_OWNER",
    "action_arg_count",
    "action_is_registered",
    "action_rejects_extra_args",
]
