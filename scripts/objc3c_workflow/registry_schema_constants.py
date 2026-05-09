"""Schema identifiers and paths for workflow registry payloads."""

from __future__ import annotations

ACTION_REGISTRY_SCHEMA_ID = "objc3c-workflow-action-registry-v1"
ACTION_REGISTRY_SCHEMA_PATH = (
    "scripts/objc3c_workflow/schemas/action-registry-v1.schema.json"
)
WORKFLOW_REPORT_SCHEMA_ID = "objc3c-workflow-report-v1"
WORKFLOW_REPORT_SCHEMA_PATH = (
    "scripts/objc3c_workflow/schemas/workflow-report-v1.schema.json"
)
WORKFLOW_SCHEMA_INDEX_SCHEMA_ID = "objc3c-workflow-schema-index-v1"
WORKFLOW_SCHEMA_INDEX_SCHEMA_PATH = (
    "scripts/objc3c_workflow/schemas/schema-index-v1.schema.json"
)
ACTION_PAYLOAD_SCHEMA_REF = f"{ACTION_REGISTRY_SCHEMA_ID}#/$defs/action"
REGISTRY_SCHEMA_CONSTANTS_OWNER_SURFACE = (
    "scripts/objc3c_workflow/registry_schema_constants.py"
)
REGISTRY_SCHEMA_MODEL_OWNER_SURFACE = "scripts/objc3c_workflow/registry_schema_model.py"
REGISTRY_SCHEMA_CATALOG_OWNER_SURFACE = (
    "scripts/objc3c_workflow/registry_schema_catalog.py"
)
REGISTRY_SCHEMA_PAYLOAD_OWNER_SURFACE = (
    "scripts/objc3c_workflow/registry_schema_payload.py"
)
REGISTRY_SCHEMA_INDEX_FACADE_SURFACE = (
    "scripts/objc3c_workflow/registry_schema_index.py"
)
