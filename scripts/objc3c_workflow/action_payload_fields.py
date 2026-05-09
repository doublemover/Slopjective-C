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
from .registry_schema_index import (
    ACTION_PAYLOAD_SCHEMA_REF,
    ACTION_REGISTRY_SCHEMA_ID,
    ACTION_REGISTRY_SCHEMA_PATH,
    capability_truth_schema_ids,
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


def action_schema_fields() -> dict[str, object]:
    return {
        "payload_schema_ref": ACTION_PAYLOAD_SCHEMA_REF,
        "registry_schema_id": ACTION_REGISTRY_SCHEMA_ID,
        "registry_schema_path": ACTION_REGISTRY_SCHEMA_PATH,
    }


def action_capability_truth_fields(action: str) -> dict[str, object]:
    return {
        "capability_truth": {
            "scope": "workflow-action",
            "action": action,
            "machine_readable": True,
            "owner_surface": "scripts/objc3c_workflow/action_payload_fields.py",
            "schema_ids": capability_truth_schema_ids(),
        },
    }


def shared_action_payload_fields(action: str) -> dict[str, object]:
    return {
        **public_action_fields(action),
        **action_schema_fields(),
        **action_capability_truth_fields(action),
    }


def build_action_payload(spec: ActionSpec) -> dict[str, object]:
    payload = asdict(spec)
    payload.update(shared_action_payload_fields(spec.action))
    return payload
