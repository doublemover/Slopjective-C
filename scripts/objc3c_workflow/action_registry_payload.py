"""Registry-level payload ownership for workflow action listings."""

from __future__ import annotations

from collections.abc import Sequence

from .action_payload_builder import build_action_payload
from .action_spec import ActionSpec
from .environment import WORKFLOW_RUNNER_MODE, WORKFLOW_RUNNER_SURFACE
from .public_bridge import PACKAGE_BRIDGES
from .registry_schema_index import (
    ACTION_REGISTRY_SCHEMA_ID,
    ACTION_REGISTRY_SCHEMA_PATH,
    capability_truth_schema_ids,
    workflow_schema_index_payload,
)


def build_registry_payload(specs: Sequence[ActionSpec]) -> dict[str, object]:
    action_count = len(specs)
    return {
        "schema_version": ACTION_REGISTRY_SCHEMA_ID,
        "schema_id": ACTION_REGISTRY_SCHEMA_ID,
        "schema_path": ACTION_REGISTRY_SCHEMA_PATH,
        "mode": WORKFLOW_RUNNER_MODE,
        "runner_path": WORKFLOW_RUNNER_SURFACE,
        "action_count": action_count,
        "public_action_count": action_count,
        "internal_action_count": 0,
        "package_bridge_count": len(PACKAGE_BRIDGES),
        "package_bridges": list(PACKAGE_BRIDGES),
        "single_package_bridge_only": True,
        "schema_index": workflow_schema_index_payload(),
        "capability_truth": {
            "scope": "workflow-action-registry",
            "machine_readable": True,
            "owner_surface": "scripts/objc3c_workflow/action_registry_payload.py",
            "schema_ids": capability_truth_schema_ids(),
            "action_payload_schema_ref": f"{ACTION_REGISTRY_SCHEMA_ID}#/$defs/action",
        },
        "actions": [build_action_payload(spec) for spec in specs],
    }
