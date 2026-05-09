"""Workflow registry schema catalog."""

from __future__ import annotations

from .registry_schema_constants import (
    ACTION_REGISTRY_SCHEMA_ID,
    ACTION_REGISTRY_SCHEMA_PATH,
    WORKFLOW_REPORT_SCHEMA_ID,
    WORKFLOW_REPORT_SCHEMA_PATH,
    WORKFLOW_SCHEMA_INDEX_SCHEMA_ID,
    WORKFLOW_SCHEMA_INDEX_SCHEMA_PATH,
)
from .registry_schema_model import WorkflowSchemaSpec

WORKFLOW_SCHEMA_SPECS: tuple[WorkflowSchemaSpec, ...] = (
    WorkflowSchemaSpec(
        schema_id=ACTION_REGISTRY_SCHEMA_ID,
        schema_path=ACTION_REGISTRY_SCHEMA_PATH,
        payload_surface="npm run objc3c -- --list-json",
        owner_surface="scripts/objc3c_workflow/action_registry_payload.py",
        capability_truth_scope="workflow-action-registry",
        public_contract=True,
    ),
    WorkflowSchemaSpec(
        schema_id=WORKFLOW_REPORT_SCHEMA_ID,
        schema_path=WORKFLOW_REPORT_SCHEMA_PATH,
        payload_surface="tmp/reports/objc3c-public-workflow/*.json",
        owner_surface="scripts/objc3c_workflow/reports.py",
        capability_truth_scope="workflow-report",
        public_contract=True,
    ),
    WorkflowSchemaSpec(
        schema_id=WORKFLOW_SCHEMA_INDEX_SCHEMA_ID,
        schema_path=WORKFLOW_SCHEMA_INDEX_SCHEMA_PATH,
        payload_surface="embedded in npm run objc3c -- --list-json",
        owner_surface="scripts/objc3c_workflow/registry_schema_index.py",
        capability_truth_scope="workflow-schema-index",
        public_contract=True,
    ),
    WorkflowSchemaSpec(
        schema_id="objc3c-capability-matrix-v1",
        schema_path="schemas/objc3c-capability-matrix-v1.schema.json",
        payload_surface="docs/support/capability_matrix.json",
        owner_surface="schemas/objc3c-capability-matrix-v1.schema.json",
        capability_truth_scope="capability-matrix",
        public_contract=True,
    ),
    WorkflowSchemaSpec(
        schema_id="objc3c-capability-evidence-map-v1",
        schema_path="schemas/objc3c-capability-evidence-map-v1.schema.json",
        payload_surface="docs/support/evidence_map.json",
        owner_surface="schemas/objc3c-capability-evidence-map-v1.schema.json",
        capability_truth_scope="capability-evidence-map",
        public_contract=True,
    ),
)


def workflow_schema_specs() -> list[WorkflowSchemaSpec]:
    return list(WORKFLOW_SCHEMA_SPECS)
