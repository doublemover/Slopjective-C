"""Action dispatch and execution-result handling for the objc3c workflow CLI."""

from __future__ import annotations

from scripts.objc3c_workflow.action_acceptance import resolve_registered_action
from scripts.objc3c_workflow.action_execution import execute_registered_action_with_metadata
from scripts.objc3c_workflow.action_execution_dispatch import execute_registered_action
from scripts.objc3c_workflow.action_payloads import (
    describe_action_payload,
    list_actions_payload,
)


__all__ = [
    "describe_action_payload",
    "execute_registered_action",
    "execute_registered_action_with_metadata",
    "list_actions_payload",
    "resolve_registered_action",
]
