"""Raw action-catalog access facade for workflow registry views."""

from __future__ import annotations

from .registry_iteration import (
    catalog_action_items,
    catalog_action_names,
    catalog_action_specs,
)
from .registry_lookup import (
    catalog_action_spec,
    catalog_has_action,
    require_catalog_action_spec,
)
from .registry_metrics import catalog_action_count
from .registry_schema_constants import (
    REGISTRY_STORE_OWNER_SURFACE,
    REGISTRY_VIEW_OWNER_SURFACE,
)

REGISTRY_STORE_CONTRACT_ID = "objc3c-workflow-registry-store-v1"
REGISTRY_STORE_OWNED_FUNCTIONS = (
    "catalog_action_count",
    "catalog_action_items",
    "catalog_action_names",
    "catalog_action_spec",
    "catalog_action_specs",
    "catalog_has_action",
    "require_catalog_action_spec",
)


def registry_store_contract() -> dict[str, object]:
    return {
        "contract_id": REGISTRY_STORE_CONTRACT_ID,
        "owner_surface": REGISTRY_STORE_OWNER_SURFACE,
        "owned_functions": list(REGISTRY_STORE_OWNED_FUNCTIONS),
        "view_surface": REGISTRY_VIEW_OWNER_SURFACE,
        "catalog_surface": "scripts/objc3c_workflow/action_catalog.py",
        "public_contract": True,
    }


__all__ = [
    "REGISTRY_STORE_CONTRACT_ID",
    "REGISTRY_STORE_OWNER_SURFACE",
    "REGISTRY_STORE_OWNED_FUNCTIONS",
    "catalog_action_count",
    "catalog_action_items",
    "catalog_action_names",
    "catalog_action_spec",
    "catalog_action_specs",
    "catalog_has_action",
    "registry_store_contract",
    "require_catalog_action_spec",
]
