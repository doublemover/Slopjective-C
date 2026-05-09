"""Public invocation fields for workflow action payloads."""

from __future__ import annotations

from .action_audience import action_audience
from .action_payload_category import action_category
from .environment import WORKFLOW_RUNNER_MODE, WORKFLOW_RUNNER_SURFACE
from .public_bridge import (
    PUBLIC_ENTRYPOINT_KIND,
    WORKFLOW_BRIDGE_SCRIPT,
    public_action_invocation,
)


def public_action_fields(action: str) -> dict[str, object]:
    invocation = public_action_invocation(action)
    return {
        "mode": WORKFLOW_RUNNER_MODE,
        "runner_path": WORKFLOW_RUNNER_SURFACE,
        "category": action_category(action),
        "audience": action_audience(action),
        "package_bridge": WORKFLOW_BRIDGE_SCRIPT,
        "public_invocation": invocation,
        "public_command": invocation,
        "public_entrypoint": PUBLIC_ENTRYPOINT_KIND,
    }


__all__ = ["public_action_fields"]
