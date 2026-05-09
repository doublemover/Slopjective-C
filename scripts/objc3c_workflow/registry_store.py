"""Raw action-catalog access for workflow registry views."""

from __future__ import annotations

from .action_catalog import ACTION_SPECS
from .action_spec import ActionSpec


def catalog_action_spec(action: str) -> ActionSpec | None:
    return ACTION_SPECS.get(action)


def require_catalog_action_spec(action: str) -> ActionSpec:
    return ACTION_SPECS[action]


def catalog_has_action(action: str) -> bool:
    return action in ACTION_SPECS


def catalog_action_names() -> list[str]:
    return list(ACTION_SPECS)


def catalog_action_specs() -> list[ActionSpec]:
    return list(ACTION_SPECS.values())


def catalog_action_items() -> list[tuple[str, ActionSpec]]:
    return list(ACTION_SPECS.items())


def catalog_action_count() -> int:
    return len(ACTION_SPECS)
