"""Machine-readable schema index for workflow registry payloads."""

from __future__ import annotations

from dataclasses import asdict, dataclass


ACTION_REGISTRY_SCHEMA_ID = "objc3c-workflow-action-registry-v1"
ACTION_REGISTRY_SCHEMA_PATH = "scripts/objc3c_workflow/schemas/action-registry-v1.schema.json"
WORKFLOW_REPORT_SCHEMA_ID = "objc3c-workflow-report-v1"
WORKFLOW_REPORT_SCHEMA_PATH = "scripts/objc3c_workflow/schemas/workflow-report-v1.schema.json"
WORKFLOW_SCHEMA_INDEX_SCHEMA_ID = "objc3c-workflow-schema-index-v1"
WORKFLOW_SCHEMA_INDEX_SCHEMA_PATH = "scripts/objc3c_workflow/schemas/schema-index-v1.schema.json"
ACTION_PAYLOAD_SCHEMA_REF = f"{ACTION_REGISTRY_SCHEMA_ID}#/$defs/action"


@dataclass(frozen=True)
class WorkflowSchemaSpec:
    schema_id: str
    schema_path: str
    payload_surface: str
    owner_surface: str
    capability_truth_scope: str
    public_contract: bool


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
