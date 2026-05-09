"""Capability truth fields for workflow registry payloads."""

from __future__ import annotations

from .registry_schema_index import (
    ACTION_PAYLOAD_SCHEMA_REF,
    capability_truth_schema_ids,
)


def registry_capability_truth_fields() -> dict[str, object]:
    return {
        "capability_truth": {
            "scope": "workflow-action-registry",
            "machine_readable": True,
            "owner_surface": "scripts/objc3c_workflow/action_registry_payload.py",
            "schema_ids": capability_truth_schema_ids(),
            "action_payload_schema_ref": ACTION_PAYLOAD_SCHEMA_REF,
        },
    }
