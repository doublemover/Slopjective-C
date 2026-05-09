"""Per-action payload field ownership for the workflow registry."""

from __future__ import annotations

from dataclasses import asdict

from .action_audience import action_audience
from .action_spec import ActionSpec
from .environment import WORKFLOW_RUNNER_MODE, WORKFLOW_RUNNER_SURFACE
from .public_bridge import (
    PUBLIC_ENTRYPOINT_KIND,
    WORKFLOW_BRIDGE_SCRIPT,
    public_action_invocation,
)


def action_category(action: str) -> str:
    return action.split("-", 1)[0]


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


def build_action_payload(spec: ActionSpec) -> dict[str, object]:
    payload = asdict(spec)
    payload.update(public_action_fields(spec.action))
    return payload
