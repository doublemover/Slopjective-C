"""Machine-readable action payloads for the objc3c workflow registry."""

from __future__ import annotations

from dataclasses import asdict

from .action_spec import ActionSpec
from .environment import WORKFLOW_RUNNER_MODE, WORKFLOW_RUNNER_SURFACE
from .public_bridge import (
    PACKAGE_BRIDGES,
    PUBLIC_ENTRYPOINT_KIND,
    WORKFLOW_BRIDGE_SCRIPT,
    public_action_invocation,
)
from .action_catalog import ACTION_SPECS
from .registry_views import action_count


def enrich_action_payload(spec: ActionSpec) -> dict[str, object]:
    payload = asdict(spec)
    payload["mode"] = WORKFLOW_RUNNER_MODE
    payload["runner_path"] = WORKFLOW_RUNNER_SURFACE
    payload["category"] = spec.action.split("-", 1)[0]
    payload["package_bridge"] = WORKFLOW_BRIDGE_SCRIPT
    payload["public_invocation"] = public_action_invocation(spec.action)
    payload["public_entrypoint"] = PUBLIC_ENTRYPOINT_KIND
    return payload


def list_actions_payload() -> dict[str, object]:
    return {
        "mode": WORKFLOW_RUNNER_MODE,
        "runner_path": WORKFLOW_RUNNER_SURFACE,
        "action_count": action_count(),
        "public_action_count": action_count(),
        "internal_action_count": 0,
        "package_bridge_count": len(PACKAGE_BRIDGES),
        "package_bridges": list(PACKAGE_BRIDGES),
        "single_package_bridge_only": True,
        "actions": [enrich_action_payload(spec) for spec in ACTION_SPECS.values()],
    }


def describe_action_payload(action: str) -> dict[str, object]:
    return enrich_action_payload(ACTION_SPECS[action])
