"""Schema reference fields for workflow action payloads."""

from __future__ import annotations

from .registry_schema_index import (
    ACTION_PAYLOAD_SCHEMA_REF,
    ACTION_REGISTRY_SCHEMA_ID,
    ACTION_REGISTRY_SCHEMA_PATH,
    REGISTRY_SCHEMA_INDEX_FACADE_SURFACE,
    REGISTRY_SCHEMA_PAYLOAD_OWNER_SURFACE,
    WORKFLOW_SCHEMA_INDEX_SCHEMA_ID,
)


def action_schema_fields() -> dict[str, object]:
    return {
        "payload_schema_ref": ACTION_PAYLOAD_SCHEMA_REF,
        "registry_schema_id": ACTION_REGISTRY_SCHEMA_ID,
        "registry_schema_path": ACTION_REGISTRY_SCHEMA_PATH,
        "registry_schema_owner_surface": REGISTRY_SCHEMA_PAYLOAD_OWNER_SURFACE,
        "registry_schema_index_schema_id": WORKFLOW_SCHEMA_INDEX_SCHEMA_ID,
        "registry_schema_index_owner_surface": REGISTRY_SCHEMA_INDEX_FACADE_SURFACE,
    }


__all__ = ["action_schema_fields"]
