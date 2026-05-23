from __future__ import annotations

import importlib
import json
from pathlib import Path

from scripts.objc3c_workflow.action_payload_schema_fields import action_schema_fields
from scripts.objc3c_workflow.registry_schema_catalog import WORKFLOW_SCHEMA_SPECS
from scripts.objc3c_workflow.registry_schema_constants import (
    ACTION_PAYLOAD_SCHEMA_REF,
    ACTION_REGISTRY_SCHEMA_ID,
    REGISTRY_SCHEMA_CATALOG_OWNER_SURFACE,
    REGISTRY_SCHEMA_INDEX_FACADE_SURFACE,
    REGISTRY_SCHEMA_MODEL_OWNER_SURFACE,
    REGISTRY_SCHEMA_PAYLOAD_OWNER_SURFACE,
    REGISTRY_STORE_OWNER_SURFACE,
    REGISTRY_VIEW_OWNER_SURFACE,
    WORKFLOW_SCHEMA_INDEX_OWNED_FIELDS,
    WORKFLOW_SCHEMA_INDEX_SCHEMA_ID,
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
from scripts.objc3c_workflow.registry_store import (
    REGISTRY_STORE_CONTRACT_ID,
    REGISTRY_STORE_OWNED_FUNCTIONS,
    registry_store_contract,
)
from scripts.objc3c_workflow.registry_views import (
    REGISTRY_VIEW_CONTRACT_ID,
    REGISTRY_VIEW_OWNED_FUNCTIONS,
    registry_view_contract,
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
        "objc3c-umbrella-readiness-v1",
    ]
    assert payload["owner_surface"] == REGISTRY_SCHEMA_PAYLOAD_OWNER_SURFACE
    assert payload["model_owner_surface"] == REGISTRY_SCHEMA_MODEL_OWNER_SURFACE
    assert payload["catalog_owner_surface"] == REGISTRY_SCHEMA_CATALOG_OWNER_SURFACE
    assert payload["index_facade_surface"] == REGISTRY_SCHEMA_INDEX_FACADE_SURFACE
    assert payload["owned_fields"] == list(WORKFLOW_SCHEMA_INDEX_OWNED_FIELDS)
    assert payload["schema_owner_surfaces"] == {
        "constants": "scripts/objc3c_workflow/registry_schema_constants.py",
        "model": REGISTRY_SCHEMA_MODEL_OWNER_SURFACE,
        "catalog": REGISTRY_SCHEMA_CATALOG_OWNER_SURFACE,
        "payload": REGISTRY_SCHEMA_PAYLOAD_OWNER_SURFACE,
        "index_facade": REGISTRY_SCHEMA_INDEX_FACADE_SURFACE,
        "registry_store": REGISTRY_STORE_OWNER_SURFACE,
        "registry_view": REGISTRY_VIEW_OWNER_SURFACE,
    }
    assert payload["registry_store_contract"] == registry_store_contract()
    assert payload["registry_view_contract"] == registry_view_contract()
    assert payload["capability_truth"]["owned_fields"] == list(
        WORKFLOW_SCHEMA_INDEX_OWNED_FIELDS
    )
    assert len(payload["schemas"]) == len(WORKFLOW_SCHEMA_SPECS)


def test_registry_schema_specs_publish_owner_boundaries() -> None:
    for spec in WORKFLOW_SCHEMA_SPECS:
        assert spec.schema_owner_surface == REGISTRY_SCHEMA_PAYLOAD_OWNER_SURFACE
        assert spec.schema_index_schema_id == WORKFLOW_SCHEMA_INDEX_SCHEMA_ID
        assert spec.schema_index_owner_surface == REGISTRY_SCHEMA_INDEX_FACADE_SURFACE
        assert spec.registry_store_owner_surface == REGISTRY_STORE_OWNER_SURFACE
        assert spec.registry_view_owner_surface == REGISTRY_VIEW_OWNER_SURFACE


def test_action_schema_fields_publish_schema_index_ownership() -> None:
    fields = action_schema_fields()

    assert fields["payload_schema_ref"] == ACTION_PAYLOAD_SCHEMA_REF
    assert fields["registry_schema_id"] == ACTION_REGISTRY_SCHEMA_ID
    assert fields["registry_schema_owner_surface"] == REGISTRY_SCHEMA_PAYLOAD_OWNER_SURFACE
    assert fields["registry_schema_index_schema_id"] == WORKFLOW_SCHEMA_INDEX_SCHEMA_ID
    assert fields["registry_schema_index_owner_surface"] == (
        REGISTRY_SCHEMA_INDEX_FACADE_SURFACE
    )


def test_registry_store_and_view_contracts_are_owned() -> None:
    store_contract = registry_store_contract()
    view_contract = registry_view_contract()

    assert store_contract["contract_id"] == REGISTRY_STORE_CONTRACT_ID
    assert store_contract["owner_surface"] == REGISTRY_STORE_OWNER_SURFACE
    assert store_contract["owned_functions"] == list(REGISTRY_STORE_OWNED_FUNCTIONS)
    assert store_contract["view_surface"] == REGISTRY_VIEW_OWNER_SURFACE
    assert view_contract["contract_id"] == REGISTRY_VIEW_CONTRACT_ID
    assert view_contract["owner_surface"] == REGISTRY_VIEW_OWNER_SURFACE
    assert view_contract["owned_functions"] == list(REGISTRY_VIEW_OWNED_FUNCTIONS)
    assert view_contract["store_surface"] == REGISTRY_STORE_OWNER_SURFACE


def test_checked_in_schema_files_lock_owner_boundary_fields() -> None:
    schema_index_schema = json.loads(
        (WORKFLOW_ROOT / "schemas" / "schema-index-v1.schema.json").read_text(
            encoding="utf-8"
        )
    )
    action_registry_schema = json.loads(
        (WORKFLOW_ROOT / "schemas" / "action-registry-v1.schema.json").read_text(
            encoding="utf-8"
        )
    )

    assert "schema_owner_surfaces" in schema_index_schema["required"]
    assert "registry_store_contract" in schema_index_schema["required"]
    assert "registry_view_contract" in schema_index_schema["required"]
    schema_entry_required = schema_index_schema["properties"]["schemas"]["items"][
        "required"
    ]
    assert "schema_owner_surface" in schema_entry_required
    assert "registry_store_owner_surface" in schema_entry_required
    action_required = action_registry_schema["$defs"]["action"]["required"]
    assert "registry_schema_owner_surface" in action_required
    assert "registry_schema_index_schema_id" in action_required
