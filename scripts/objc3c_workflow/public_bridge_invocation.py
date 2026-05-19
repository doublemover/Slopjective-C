"""Public bridge invocation rendering."""

from __future__ import annotations

from .public_bridge_constants import (
    WORKFLOW_PUBLIC_ACTION_TOKEN,
    WORKFLOW_PUBLIC_COMMAND_TEMPLATE,
)


def public_action_invocation(action: str) -> str:
    return WORKFLOW_PUBLIC_COMMAND_TEMPLATE.replace(WORKFLOW_PUBLIC_ACTION_TOKEN, action)


def public_action_invocation_template() -> str:
    return WORKFLOW_PUBLIC_COMMAND_TEMPLATE


__all__ = ["public_action_invocation", "public_action_invocation_template"]
