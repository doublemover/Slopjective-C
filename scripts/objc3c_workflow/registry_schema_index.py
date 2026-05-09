"""Machine-readable schema index for workflow registry payloads."""

from __future__ import annotations

from .registry_schema_catalog import WORKFLOW_SCHEMA_SPECS, workflow_schema_specs
from .registry_schema_constants import (
    ACTION_PAYLOAD_SCHEMA_REF,
    ACTION_REGISTRY_SCHEMA_ID,
    ACTION_REGISTRY_SCHEMA_PATH,
    WORKFLOW_REPORT_SCHEMA_ID,
    WORKFLOW_REPORT_SCHEMA_PATH,
    WORKFLOW_SCHEMA_INDEX_SCHEMA_ID,
    WORKFLOW_SCHEMA_INDEX_SCHEMA_PATH,
)
from .registry_schema_model import WorkflowSchemaSpec
from .registry_schema_payload import (
    capability_truth_schema_ids,
    workflow_schema_index_entries,
    workflow_schema_index_payload,
)

__all__ = [
    "ACTION_PAYLOAD_SCHEMA_REF",
    "ACTION_REGISTRY_SCHEMA_ID",
    "ACTION_REGISTRY_SCHEMA_PATH",
    "WORKFLOW_REPORT_SCHEMA_ID",
    "WORKFLOW_REPORT_SCHEMA_PATH",
    "WORKFLOW_SCHEMA_INDEX_SCHEMA_ID",
    "WORKFLOW_SCHEMA_INDEX_SCHEMA_PATH",
    "WORKFLOW_SCHEMA_SPECS",
    "WorkflowSchemaSpec",
    "capability_truth_schema_ids",
    "workflow_schema_index_entries",
    "workflow_schema_index_payload",
    "workflow_schema_specs",
]
