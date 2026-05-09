"""Machine-readable action payloads for the objc3c workflow registry."""

from __future__ import annotations

from dataclasses import asdict

from .action_spec import ActionSpec
from .environment import WORKFLOW_RUNNER_MODE, WORKFLOW_RUNNER_SURFACE
from .registry import ACTION_SPECS


def enrich_action_payload(spec: ActionSpec) -> dict[str, object]:
    payload = asdict(spec)
    payload["mode"] = WORKFLOW_RUNNER_MODE
    payload["runner_path"] = WORKFLOW_RUNNER_SURFACE
    payload["category"] = spec.action.split("-", 1)[0]
    return payload


def list_actions_payload() -> dict[str, object]:
    return {
        "mode": WORKFLOW_RUNNER_MODE,
        "runner_path": WORKFLOW_RUNNER_SURFACE,
        "action_count": len(ACTION_SPECS),
        "public_action_count": len(ACTION_SPECS),
        "internal_action_count": 0,
        "package_bridge_count": 1,
        "actions": [enrich_action_payload(spec) for spec in ACTION_SPECS.values()],
    }


def describe_action_payload(action: str) -> dict[str, object]:
    return enrich_action_payload(ACTION_SPECS[action])
