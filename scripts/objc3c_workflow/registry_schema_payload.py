"""Payload builders for workflow registry schema index data."""

from __future__ import annotations

from dataclasses import asdict

from .registry_schema_catalog import WORKFLOW_SCHEMA_SPECS
from .registry_schema_constants import (
    REGISTRY_SCHEMA_CATALOG_OWNER_SURFACE,
    REGISTRY_SCHEMA_CONSTANTS_OWNER_SURFACE,
    REGISTRY_SCHEMA_INDEX_FACADE_SURFACE,
    REGISTRY_SCHEMA_MODEL_OWNER_SURFACE,
    REGISTRY_SCHEMA_PAYLOAD_OWNER_SURFACE,
    REGISTRY_STORE_OWNER_SURFACE,
    REGISTRY_VIEW_OWNER_SURFACE,
    WORKFLOW_SCHEMA_INDEX_OWNED_FIELDS,
    WORKFLOW_SCHEMA_INDEX_SCHEMA_ID,
    WORKFLOW_SCHEMA_INDEX_SCHEMA_PATH,
)
from .registry_store import registry_store_contract
from .registry_views import registry_view_contract


def workflow_schema_index_entries() -> list[dict[str, object]]:
    return [asdict(spec) for spec in WORKFLOW_SCHEMA_SPECS]


def capability_truth_schema_ids() -> list[str]:
    return [
        spec.schema_id
        for spec in WORKFLOW_SCHEMA_SPECS
        if spec.capability_truth_scope.startswith("capability-")
    ]


def workflow_schema_index_payload() -> dict[str, object]:
    return {
        "schema_version": WORKFLOW_SCHEMA_INDEX_SCHEMA_ID,
        "schema_id": WORKFLOW_SCHEMA_INDEX_SCHEMA_ID,
        "schema_path": WORKFLOW_SCHEMA_INDEX_SCHEMA_PATH,
        "owner_surface": REGISTRY_SCHEMA_PAYLOAD_OWNER_SURFACE,
        "owned_fields": list(WORKFLOW_SCHEMA_INDEX_OWNED_FIELDS),
        "constants_owner_surface": REGISTRY_SCHEMA_CONSTANTS_OWNER_SURFACE,
        "model_owner_surface": REGISTRY_SCHEMA_MODEL_OWNER_SURFACE,
        "catalog_owner_surface": REGISTRY_SCHEMA_CATALOG_OWNER_SURFACE,
        "index_facade_surface": REGISTRY_SCHEMA_INDEX_FACADE_SURFACE,
        "schema_owner_surfaces": {
            "constants": REGISTRY_SCHEMA_CONSTANTS_OWNER_SURFACE,
            "model": REGISTRY_SCHEMA_MODEL_OWNER_SURFACE,
            "catalog": REGISTRY_SCHEMA_CATALOG_OWNER_SURFACE,
            "payload": REGISTRY_SCHEMA_PAYLOAD_OWNER_SURFACE,
            "index_facade": REGISTRY_SCHEMA_INDEX_FACADE_SURFACE,
            "registry_store": REGISTRY_STORE_OWNER_SURFACE,
            "registry_view": REGISTRY_VIEW_OWNER_SURFACE,
        },
        "registry_store_contract": registry_store_contract(),
        "registry_view_contract": registry_view_contract(),
        "capability_truth_schema_ids": capability_truth_schema_ids(),
        "capability_truth": {
            "scope": "workflow-schema-index",
            "machine_readable": True,
            "owner_surface": REGISTRY_SCHEMA_PAYLOAD_OWNER_SURFACE,
            "owned_fields": list(WORKFLOW_SCHEMA_INDEX_OWNED_FIELDS),
            "schema_ids": capability_truth_schema_ids(),
        },
        "schemas": workflow_schema_index_entries(),
    }
