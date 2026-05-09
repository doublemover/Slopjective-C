"""Public bridge invocation rendering."""

from __future__ import annotations

from .public_bridge_constants import WORKFLOW_PUBLIC_COMMAND_PREFIX


def public_action_invocation(action: str) -> str:
    return f"{WORKFLOW_PUBLIC_COMMAND_PREFIX} {action}"


__all__ = ["public_action_invocation"]
