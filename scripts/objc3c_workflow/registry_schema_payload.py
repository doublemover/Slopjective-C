"""Payload builders for workflow registry schema index data."""

from __future__ import annotations

from dataclasses import asdict

from .registry_schema_catalog import WORKFLOW_SCHEMA_SPECS
from .registry_schema_constants import (
    WORKFLOW_SCHEMA_INDEX_SCHEMA_ID,
    WORKFLOW_SCHEMA_INDEX_SCHEMA_PATH,
)


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
        "owner_surface": "scripts/objc3c_workflow/registry_schema_index.py",
        "capability_truth_schema_ids": capability_truth_schema_ids(),
        "schemas": workflow_schema_index_entries(),
    }
