"""Registry-level payload ownership for workflow action listings."""

from __future__ import annotations

from collections.abc import Sequence

from .action_payload_builder import build_action_payload
from .action_registry_bridge_payload import registry_bridge_fields
from .action_registry_capability_truth import registry_capability_truth_fields
from .action_spec import ActionSpec
from .environment import WORKFLOW_RUNNER_MODE, WORKFLOW_RUNNER_SURFACE
from .registry_schema_index import (
    ACTION_REGISTRY_SCHEMA_ID,
    ACTION_REGISTRY_SCHEMA_PATH,
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
        **registry_bridge_fields(),
        "schema_index": workflow_schema_index_payload(),
        **registry_capability_truth_fields(),
        "actions": [build_action_payload(spec) for spec in specs],
    }
