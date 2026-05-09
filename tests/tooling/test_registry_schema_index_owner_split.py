from __future__ import annotations

import importlib
from pathlib import Path

from scripts.objc3c_workflow.registry_schema_catalog import WORKFLOW_SCHEMA_SPECS
from scripts.objc3c_workflow.registry_schema_constants import (
    ACTION_PAYLOAD_SCHEMA_REF,
    ACTION_REGISTRY_SCHEMA_ID,
)
from scripts.objc3c_workflow.registry_schema_index import (
    ACTION_PAYLOAD_SCHEMA_REF as PUBLIC_ACTION_PAYLOAD_SCHEMA_REF,
)
from scripts.objc3c_workflow.registry_schema_index import (
    WORKFLOW_SCHEMA_SPECS as PUBLIC_WORKFLOW_SCHEMA_SPECS,
)
from scripts.objc3c_workflow.registry_schema_model import WorkflowSchemaSpec
from scripts.objc3c_workflow.registry_schema_payload import (
    capability_truth_schema_ids,
    workflow_schema_index_payload,
)

ROOT = Path(__file__).resolve().parents[2]
WORKFLOW_ROOT = ROOT / "scripts" / "objc3c_workflow"

OWNER_MODULES = (
    "registry_schema_constants",
    "registry_schema_model",
    "registry_schema_catalog",
    "registry_schema_payload",
)


def test_registry_schema_index_is_public_facade() -> None:
    facade_text = (WORKFLOW_ROOT / "registry_schema_index.py").read_text(
        encoding="utf-8"
    )

    for module_name in OWNER_MODULES:
        assert importlib.import_module(f"scripts.objc3c_workflow.{module_name}")
        assert f"from .{module_name} import" in facade_text
    assert "@dataclass" not in facade_text
    assert "objc3c-capability-matrix-v1" not in facade_text


def test_registry_schema_public_exports_preserve_catalog_contract() -> None:
    assert PUBLIC_WORKFLOW_SCHEMA_SPECS is WORKFLOW_SCHEMA_SPECS
    assert PUBLIC_ACTION_PAYLOAD_SCHEMA_REF == ACTION_PAYLOAD_SCHEMA_REF
    assert ACTION_PAYLOAD_SCHEMA_REF == f"{ACTION_REGISTRY_SCHEMA_ID}#/$defs/action"
    assert all(isinstance(spec, WorkflowSchemaSpec) for spec in WORKFLOW_SCHEMA_SPECS)


def test_registry_schema_payload_preserves_capability_truth_ids() -> None:
    payload = workflow_schema_index_payload()

    assert payload["capability_truth_schema_ids"] == capability_truth_schema_ids()
    assert payload["capability_truth_schema_ids"] == [
        "objc3c-capability-matrix-v1",
        "objc3c-capability-evidence-map-v1",
    ]
    assert len(payload["schemas"]) == len(WORKFLOW_SCHEMA_SPECS)
