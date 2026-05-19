"""Capability-truth fields for workflow action payloads."""

from __future__ import annotations

from .action_payload_field_owners import (
    ACTION_PAYLOAD_CAPABILITY_TRUTH_OWNER_SURFACE,
)
from .registry_schema_index import capability_truth_schema_ids


def action_capability_truth_fields(action: str) -> dict[str, object]:
    return {
        "capability_truth": {
            "scope": "workflow-action",
            "action": action,
            "machine_readable": True,
            "owner_surface": ACTION_PAYLOAD_CAPABILITY_TRUTH_OWNER_SURFACE,
            "owned_fields": ["capability_truth"],
            "schema_ids": capability_truth_schema_ids(),
        },
    }


__all__ = ["action_capability_truth_fields"]
