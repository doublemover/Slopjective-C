"""Read-only views over the objc3c workflow action registry."""

from __future__ import annotations

from collections.abc import Callable

from .action_spec import ActionSpec
from .registry_filters import filter_actions_by_category, filter_actions_matching
from .registry_store import (
    catalog_action_count,
    catalog_action_items,
    catalog_action_names,
    catalog_action_spec,
    catalog_action_specs,
    catalog_has_action,
    require_catalog_action_spec,
)
from .registry_schema_constants import (
    REGISTRY_STORE_OWNER_SURFACE,
    REGISTRY_VIEW_OWNER_SURFACE,
)

REGISTRY_VIEW_CONTRACT_ID = "objc3c-workflow-registry-view-v1"
REGISTRY_VIEW_OWNED_FUNCTIONS = (
    "action_count",
    "action_items",
    "action_names",
    "action_spec",
    "action_specs",
    "actions_by_category",
    "actions_matching",
    "has_action",
    "require_action_spec",
)


def registry_view_contract() -> dict[str, object]:
    return {
        "contract_id": REGISTRY_VIEW_CONTRACT_ID,
        "owner_surface": REGISTRY_VIEW_OWNER_SURFACE,
        "owned_functions": list(REGISTRY_VIEW_OWNED_FUNCTIONS),
        "store_surface": REGISTRY_STORE_OWNER_SURFACE,
        "public_contract": True,
    }


def action_spec(action: str) -> ActionSpec | None:
    return catalog_action_spec(action)


def require_action_spec(action: str) -> ActionSpec:
    return require_catalog_action_spec(action)


def has_action(action: str) -> bool:
    return catalog_has_action(action)


def action_names() -> list[str]:
    return catalog_action_names()


def action_specs() -> list[ActionSpec]:
    return catalog_action_specs()


def action_items() -> list[tuple[str, ActionSpec]]:
    return catalog_action_items()


def actions_by_category(category: str) -> list[str]:
    return filter_actions_by_category(action_items(), category)


def actions_matching(predicate: Callable[[str, ActionSpec], bool]) -> list[str]:
    return filter_actions_matching(action_items(), predicate)


def action_count() -> int:
    return catalog_action_count()


__all__ = [
    "REGISTRY_VIEW_CONTRACT_ID",
    "REGISTRY_VIEW_OWNER_SURFACE",
    "REGISTRY_VIEW_OWNED_FUNCTIONS",
    "action_count",
    "action_items",
    "action_names",
    "action_spec",
    "action_specs",
    "actions_by_category",
    "actions_matching",
    "has_action",
    "registry_view_contract",
    "require_action_spec",
]
